
#include <engine/engine.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

//Temp
#include <algorithm>

namespace Engine
{
    Engine::Engine()
        : m_Window(640, 480, "My Engine"),
          m_Input(m_Window)
    {
    }

    Engine::~Engine()
    {
        Shutdown();
    }

    bool Engine::Initialize()
    {
        if (m_Initialized)
            return true;

        if (glfwInit() != GLFW_TRUE)
            return false;

        // Request an OpenGL 3.3 Core Profile context.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!m_Window.Create())
        {
            glfwTerminate();
            return false;
        }

        GLFWwindow* nativeWindow = m_Window.GetNativeWindow();

        if (nativeWindow == nullptr)
        {
            m_Window.Destroy();
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(nativeWindow);

        // Load OpenGL functions only after the context is current.
        if (!gladLoadGLLoader(
                reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            m_Window.Destroy();
            glfwTerminate();
            return false;
        }

        if (!m_Renderer.Initialize())
        {
            m_Renderer.Shutdown();
            m_Window.Destroy();
            glfwTerminate();
            return false;
        }

        // Initialize Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(
            m_Window.GetNativeWindow(),
            true
        );

        ImGui_ImplOpenGL3_Init("#version 330 core");

        m_QuadTransform = Transform{};

        m_Initialized = true;
        return true;
    }

    void Engine::Update(float deltaTime)
    {
        if (!m_Initialized)
            return;

        // Receive input, window events
        m_Window.PollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Gates keyboard movement while ImGui captures keyboard
        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            // Input-driven movement, update triangle's transform
            constexpr float moveSpeed = 0.5f;

            glm::vec3 direction(0.0f, 0.0f, 0.0f);

            if (m_Input.IsKeyDown(Key::D))
                direction.x += 1.0f;
                //m_QuadTransform.position.x += moveSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::A))
                direction.x -= 1.0f;
                //m_QuadTransform.position.x -= moveSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::W))
                direction.y += 1.0f;
                //m_QuadTransform.position.y += moveSpeed * deltaTime;
                
            if (m_Input.IsKeyDown(Key::S))
                direction.y -= 1.0f;
                //m_QuadTransform.position.y -= moveSpeed * deltaTime;

            //if (glm::length(direction) > 0.0f)
            if (glm::dot(direction, direction) > 1.0f)
                direction = glm::normalize(direction);

            m_QuadTransform.position +=
                direction * moveSpeed * deltaTime;
            
            float rotationSpeed = glm::radians(90.0f);
            
            if (m_Input.IsKeyDown(Key::Q))
                m_QuadTransform.rotationRadians += rotationSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::E))
                m_QuadTransform.rotationRadians -= rotationSpeed * deltaTime;
            
            constexpr float scaleSpeed = 0.5f;
            float scaleChange = 0.0f;

            if (m_Input.IsKeyDown(Key::X))
                scaleChange += scaleSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::Z))
                scaleChange -= scaleSpeed * deltaTime;

            const float newScale = std::clamp(
                m_QuadTransform.scale.x + scaleChange,
                0.05f,
                5.0f
            );

            m_QuadTransform.scale = glm::vec3{newScale, newScale, 1.0f};

            constexpr float fadeSpeed = 0.5f;

            if (m_Input.IsKeyDown(Key::C))
            {
                m_QuadOpacity -= fadeSpeed * deltaTime;
            }

            if (m_Input.IsKeyDown(Key::V))
            {
                m_QuadOpacity += fadeSpeed * deltaTime;
            }

            m_QuadOpacity = std::clamp(m_QuadOpacity, 0.0f, 1.0f);

            if (m_Input.IsKeyDown(Key::R))
            {
                m_QuadTransform = Transform{};
                m_QuadOpacity = 1.0f;
            }
        }

        // Draw using the current transform
        m_Renderer.BeginFrame();
        m_Renderer.DrawQuad(
            m_QuadTransform.GetMatrix(),
            m_QuadTint,
            m_QuadOpacity
        );
        m_Renderer.EndFrame();

        // Draw UI on top of scene
        DrawControls();
        DrawAppearanceControls();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present the complete frame
        m_Window.SwapBuffers();
    }

    bool Engine::IsRunning() const
    {
        return m_Initialized && !m_Window.ShouldClose();
    }

    void Engine::Shutdown()
    {
        if (!m_Initialized)
            return;

        // Release ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        m_Renderer.Shutdown();

        m_Window.Destroy();

        // The OpenGL context must remain valid while resources are released.
        glfwTerminate();

        m_Initialized = false;
    }

    const char* Engine::GetVersion()
    {
        return "0.0.0";
    }

    void Engine::DrawControls()
    {
        // UI positioned 10 pixels from the top-left corner
        ImGui::SetNextWindowPos(
            ImVec2(10.0f, 10.0f),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowBgAlpha(0.75f);

        const ImGuiWindowFlags flags = 
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs;

        if (ImGui::Begin("Controls", nullptr, flags))
        {
            ImGui::TextUnformatted("Controls");
            ImGui::Separator();
            ImGui::TextUnformatted("WASD    Move");
            ImGui::TextUnformatted("Q / E   Rotate");
            ImGui::TextUnformatted("Z / X   Shrink / Grow");
            ImGui::TextUnformatted("C / V   Fade out / Fade in");
            ImGui::TextUnformatted("R       Reset");
        }

        ImGui::End();
    }

    void Engine::DrawAppearanceControls()
    {
        if (ImGui::Begin("Appearance"))
        {
            float tint[] =
            {
                m_QuadTint.r,
                m_QuadTint.g,
                m_QuadTint.b
            };

            if (ImGui::ColorEdit3("Tint", tint))
            {
                m_QuadTint = glm::vec3{
                    tint[0], tint[1], tint[2]
                };
            }

            ImGui::SliderFloat(
                "Opacity", &m_QuadOpacity, 0.0f, 1.0f,
                "%.2f", ImGuiSliderFlags_AlwaysClamp
            );

            if (ImGui::Button("Reset appearance"))
            {
                m_QuadTint = glm::vec3{1.0f, 1.0f, 1.0f};
                m_QuadOpacity = 1.0f;
            }
        }

        ImGui::End();
    }
}
