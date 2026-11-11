
#include <engine/engine.hpp>

#include <engine/math/transform.hpp>
#include <engine/scene/components/sprite.hpp>
#include <engine/scene/components/velocity.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

//Temp
#include <iostream>
#include <algorithm>
#include <vector>

namespace Engine
{
    // Temporary, to test entity despawning functionality
    struct Drifter {};
    
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

        auto& registry = m_Scene.GetRegistry();

        m_Player = registry.create();
        registry.emplace<Transform>(m_Player);
        registry.emplace<Sprite>(m_Player);

        const glm::vec3 landmarkPositions[] = 
        {
            {-0.85f, -0.55f, 0.0f},
            { 0.85f,  0.55f, 0.0f},
            { 2.25f,  0.10f, 0.0f}
        };

        for (const auto& position : landmarkPositions)
        {
            const auto entity = registry.create();

            auto& transform = registry.emplace<Transform>(entity);
            transform.position = position;
            transform.scale = {0.25f, 0.25f, 1.0f};

            auto& sprite = registry.emplace<Sprite>(entity);
            sprite.tint = {0.5f, 0.8f, 1.0f};
        }

        m_Initialized = true;
        return true;
    }

    void Engine::Update(float deltaTime)
    {
        if (!m_Initialized)
            return;

        // Receive input, window events
        m_Window.PollEvents();

        // Orthographic Projection
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window.GetNativeWindow(), &width, &height);

        if (width <= 0 || height <= 0)
            return;

        glViewport(0, 0, width, height);

        const float aspect =
            static_cast<float>(width) / static_cast<float>(height);

        const glm::mat4 projection = glm::ortho(
            -aspect, aspect,    // LR
            -1.0f, 1.0f         // TB
        );

        auto& registry = m_Scene.GetRegistry();
        auto& playerTransform = registry.get<Transform>(m_Player);
        auto& playerSprite = registry.get<Sprite>(m_Player);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Gates keyboard movement while ImGui captures keyboard
        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            // Input-driven movement, update triangle's transform
            constexpr float baseMoveSpeed = 0.5f;   // World units / s
            constexpr float defaultScale = 1.0f;    // Match reset scale
            constexpr float sprintMultiplier = 3.0f;

            const float sizeMultiplier =
                playerTransform.scale.x / defaultScale;

            const bool sprinting = m_Input.IsKeyDown(Key::LeftShift);
            const float moveSpeed = baseMoveSpeed * sizeMultiplier
                * (sprinting ? sprintMultiplier : 1.0f);

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

            playerTransform.position +=
                direction * moveSpeed * deltaTime;
            
            float rotationSpeed = glm::radians(90.0f);
            
            if (m_Input.IsKeyDown(Key::Q))
                playerTransform.rotationRadians += rotationSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::E))
                playerTransform.rotationRadians -= rotationSpeed * deltaTime;
            
            constexpr float scaleSpeed = 0.5f;
            float scaleChange = 0.0f;

            if (m_Input.IsKeyDown(Key::X))
                scaleChange += scaleSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::Z))
                scaleChange -= scaleSpeed * deltaTime;

            const float newScale = std::clamp(
                playerTransform.scale.x + scaleChange,
                0.05f,
                5.0f
            );

            playerTransform.scale = glm::vec3{newScale, newScale, 1.0f};

            constexpr float fadeSpeed = 0.5f;

            if (m_Input.IsKeyDown(Key::C))
            {
                playerSprite.opacity -= fadeSpeed * deltaTime;
            }

            if (m_Input.IsKeyDown(Key::V))
            {
                playerSprite.opacity += fadeSpeed * deltaTime;
            }

            playerSprite.opacity =
                std::clamp(playerSprite.opacity, 0.0f, 1.0f);

            if (m_Input.IsKeyDown(Key::R))
            {
                playerTransform = Transform{};
                playerSprite.opacity = 1.0f;

                const auto drifters = registry.view<Drifter>();
                const std::vector<entt::entity> toDestroy(
                    drifters.begin(), drifters.end()
                );

                for (const auto entity : toDestroy)
                {
                    registry.destroy(entity);
                }
            }

            constexpr float cameraSpeed = 1.0f; // World units per sec.
            const float cameraStep = cameraSpeed * deltaTime;

            if (m_Input.IsKeyDown(Key::Up))
            {
                m_CameraPosition.y += cameraStep;
            }

            if (m_Input.IsKeyDown(Key::Down))
            {
                m_CameraPosition.y -= cameraStep;
            }
            
            if (m_Input.IsKeyDown(Key::Left))
            {
                m_CameraPosition.x -= cameraStep;
            }
            
            if (m_Input.IsKeyDown(Key::Right))
            {
                m_CameraPosition.x += cameraStep;
            }
        }

        const glm::mat4 view = glm::translate(
            glm::mat4{1.0f},
            glm::vec3{-m_CameraPosition.x, -m_CameraPosition.y, 0.0f}
        );

        const glm::mat4 viewProjection = projection * view;

        GLFWwindow* window = m_Window.GetNativeWindow();

        const bool leftMouseDown =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        const bool clicked = leftMouseDown && !m_LeftMouseWasDown;

        m_LeftMouseWasDown = leftMouseDown;

        if (clicked && !ImGui::GetIO().WantCaptureMouse)
        {
            double mouseX = 0.0;
            double mouseY = 0.0;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            if (windowWidth > 0 && windowHeight > 0)
            {
                const float ndcX = static_cast<float>(
                    2.0 * mouseX / windowWidth - 1.0
                );
                const float ndcY = static_cast<float>(
                    1.0 - 2.0 * mouseY / windowHeight
                );

                const glm::vec4 world = glm::inverse(viewProjection) *
                    glm::vec4{ndcX, ndcY, 0.0f, 1.0f};

                SpawnDrifter(glm::vec3{
                    world.x / world.w,
                    world.y / world.w,
                    0.0f
                });
            }
        }

        auto movingEntities =
            registry.view<Transform, const Velocity>();

        for (const auto entity : movingEntities)
        {
            auto& transform =
                movingEntities.get<Transform>(entity);
            const auto& velocity =
                movingEntities.get<const Velocity>(entity);

            transform.position += velocity.linear * deltaTime;
        }

        m_Renderer.BeginFrame();

        auto sprites = registry.view<Transform, Sprite>();

        for (const auto& entity : sprites)
        {
            if (entity == m_Player)
                continue;

            const auto& transform = sprites.get<Transform>(entity);
            const auto& sprite = sprites.get<Sprite>(entity);
            
            m_Renderer.DrawQuad(
                viewProjection * transform.GetMatrix(),
                sprite.tint,
                sprite.opacity
            );
        }

        m_Renderer.DrawQuad(
            viewProjection * playerTransform.GetMatrix(),
            playerSprite.tint,
            playerSprite.opacity
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

        m_Scene.GetRegistry().clear();
        m_Player = entt::null;

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
            ImGui::TextUnformatted("Shift   Sprint");
            ImGui::TextUnformatted("Q / E   Rotate");
            ImGui::TextUnformatted("Z / X   Shrink / Grow");
            ImGui::TextUnformatted("C / V   Fade out / Fade in");
            ImGui::TextUnformatted("L Click Spawn Drifter");
            ImGui::TextUnformatted("R       Reset");
        }

        ImGui::End();
    }

    void Engine::DrawAppearanceControls()
    {
        auto& playerSprite =
            m_Scene.GetRegistry().get<Sprite>(m_Player);
        
        if (ImGui::Begin("Appearance"))
        {
            float tint[] =
            {
                playerSprite.tint.r,
                playerSprite.tint.g,
                playerSprite.tint.b
            };

            if (ImGui::ColorEdit3("Tint", tint))
            {
                playerSprite.tint = glm::vec3{
                    tint[0], tint[1], tint[2]
                };
            }

            ImGui::SliderFloat(
                "Opacity", &playerSprite.opacity, 0.0f, 1.0f,
                "%.2f", ImGuiSliderFlags_AlwaysClamp
            );

            if (ImGui::Button("Reset appearance"))
            {
                playerSprite.tint = glm::vec3{1.0f, 1.0f, 1.0f};
                playerSprite.opacity = 1.0f;
            }
        }

        ImGui::End();
    }

    void Engine::SpawnDrifter(const glm::vec3& position)
    {
        auto& registry = m_Scene.GetRegistry();
        const auto drifter = registry.create();

        auto& transform = registry.emplace<Transform>(drifter);
        transform.position = position;
        transform.scale = {0.25f, 0.25f, 1.0f};

        auto& sprite = registry.emplace<Sprite>(drifter);
        sprite.tint = {1.0f, 0.7f, 0.3f};

        auto& velocity = registry.emplace<Velocity>(drifter);
        velocity.linear = {0.3f, 0.0f, 0.0f};

        // Temp, to test despawning
        registry.emplace<Drifter>(drifter);
    }
}
