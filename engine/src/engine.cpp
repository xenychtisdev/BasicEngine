
#include <engine/engine.hpp>

#include <engine/math/transform.hpp>
#include <engine/scene/components/sprite.hpp>
#include <engine/scene/components/velocity.hpp>
#include <engine/scene/components/animation.hpp>
#include <engine/scene/components/attachedTo.hpp>

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

    namespace
    {
        constexpr char BaseShipTextureId[] = "ships.player.base";
        constexpr char BaseEngineTextureId[] = "engines.player.base";
        constexpr char BaseEngineEffectTextureId[] = "ship.engine.effects";
        constexpr char DemoTextureId[] = "demo.original";
    }
    
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

        if (!m_Textures.Load(
                BaseShipTextureId,
                "assets/textures/Foozle_2DS0011_Void_MainShip/Main Ship/Main Ship - Bases/PNGs/Main Ship - Base - Full health.png")
            || !m_Textures.Load(
                BaseEngineTextureId,
                "assets/textures/Foozle_2DS0011_Void_MainShip/Main Ship/Main Ship - Engines/PNGs/Main Ship - Engines - Base Engine.png")
            || !m_Textures.Load(
                BaseEngineEffectTextureId,
                "assets/textures/Foozle_2DS0011_Void_MainShip/Main Ship/Main Ship - Engine Effects/PNGs/Main Ship - Engines - Base Engine - Spritesheet.png")
            || !m_Textures.Load(
                DemoTextureId,
                "assets/textures/Foozle_2DS0011_Void_MainShip/Main Ship/Main Ship - Bases/PNGs/Main Ship - Base - Very damaged.png")
            || !RegisterGridClip(
                "ship.engine.idle",
                BaseEngineEffectTextureId,
                4, 2,
                3,
                0.15,
                0)
            || !RegisterGridClip(
                "ship.engine.powered",
                BaseEngineEffectTextureId,
                4, 2,
                4,
                0.08,
                4
            ))
        {
            m_Textures.Clear();
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
        registry.emplace<Sprite>(m_Player).textureId = BaseShipTextureId;

        m_PlayerEngine = registry.create();

        registry.emplace<Transform>(m_PlayerEngine);
        registry.emplace<Sprite>(m_PlayerEngine).textureId = BaseEngineTextureId;
        registry.emplace<AttachedTo>(
            m_PlayerEngine,
            AttachedTo(m_Player)
        );

        m_PlayerEngineEffect = registry.create();

        registry.emplace<Transform>(m_PlayerEngineEffect);
        registry.emplace<Sprite>(m_PlayerEngineEffect);
        registry.emplace<AttachedTo>(
            m_PlayerEngineEffect,
            AttachedTo(m_Player)
        );

        PlayAnimation(m_PlayerEngineEffect, "ship.engine.idle");

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
            sprite.textureId = DemoTextureId;
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
            
            const bool powered =
                glm::dot(direction, direction) > 0.0f;

            const std::string desiredClip =
                powered ? "ship.engine.powered" : "ship.engine.idle";

            const auto& playback =
                registry.get<Animation>(m_PlayerEngineEffect);

            if (playback.clipId != desiredClip)
            {
                PlayAnimation(m_PlayerEngineEffect, desiredClip);
            }
            
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

        const Texture2D* shipTexture = m_Textures.Find(BaseShipTextureId);

        UpdateAnimations(deltaTime);

        m_Renderer.BeginFrame();

        const auto drawSprite = [&](entt::entity entity)
        {
            if (!registry.valid(entity) ||
                !registry.all_of<Transform, Sprite>(entity))
            {
                return;
            }

            const auto& transform = registry.get<Transform>(entity);
            const auto& sprite = registry.get<Sprite>(entity);
            
            const Texture2D* texture = m_Textures.Find(sprite.textureId);

            if (texture == nullptr)
            {
                return;
            }

            glm::mat4 model = transform.GetMatrix();
            float opacity = sprite.opacity;

            if (const auto* attachment = registry.try_get<AttachedTo>(entity))
            {
                if (!registry.valid(attachment->parent))
                    return;
                
                const auto* parentTransform =
                    registry.try_get<Transform>(attachment->parent);

                if (!parentTransform)
                    return;

                model = parentTransform->GetMatrix() * model;

                if (const auto* parentSprite = 
                        registry.try_get<Sprite>(attachment->parent))
                {
                    opacity *= parentSprite->opacity;
                }
            }

            m_Renderer.DrawQuad(
                viewProjection * model,
                *texture,
                sprite.tint,
                opacity,
                sprite.uvRect
            );
        };

        auto sprites = registry.view<Transform, Sprite>();

        for (const auto& entity : sprites)
        {
            if (entity == m_Player ||
                entity == m_PlayerEngine ||
                entity == m_PlayerEngineEffect)
            {
                continue;
            }

            drawSprite(entity);
        }

        drawSprite(m_PlayerEngineEffect);
        drawSprite(m_PlayerEngine);
        drawSprite(m_Player);

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
        m_PlayerEngine = entt::null;
        m_PlayerEngineEffect = entt::null;

        m_Textures.Clear();

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

    bool Engine::RegisterGridClip(
        const std::string& clipId,
        const std::string& textureId,
        int columns,
        int rows,
        int frameCount,
        double frameDuration,
        int firstFrame
    )
    {
        const Texture2D* texture = m_Textures.Find(textureId);

        if (!texture ||
            columns <= 0 ||
            rows <= 0 ||
            frameCount <= 0 ||
            frameCount > static_cast<long long>(columns) * rows ||
            !std::isfinite(frameDuration) ||
            frameDuration <= 0.0 ||
            firstFrame < 0 ||
            frameCount <= 0 ||
            static_cast<long long>(firstFrame) + frameCount >
                static_cast<long long>(columns) * rows)
        {
            return false;
        }

        if (texture->GetWidth() % columns != 0 ||
            texture->GetHeight() % rows != 0)
        {
            return false;
        }
        AnimationClip clip;
        clip.textureId = textureId;
        clip.frameDuration = frameDuration;
        clip.frames.reserve(frameCount);

        for (int index = 0; index < frameCount; ++index)
        {
            const int cell = firstFrame + index;
            const int column = cell % columns;
            const int row = cell / columns;

            const float uMin =
                static_cast<float>(column) / columns;
            const float uMax =
                static_cast<float>(column + 1) / columns;

            // Cells are numbered from the top of the source image.
            //  Texture2D currently flips the image vertically on loading.
            const float vMin =
                1.0f - static_cast<float>(row + 1) / rows;
            const float vMax =
                1.0f - static_cast<float>(row) / rows;

            clip.frames.emplace_back(uMin, vMin, uMax, vMax);
        }

        return m_AnimationClips.emplace(
            clipId, std::move(clip)
        ).second;
    }

    bool Engine::PlayAnimation(
        entt::entity entity,
        const std::string& clipId,
        std::size_t startingFrame
    )
    {
        const auto found = m_AnimationClips.find(clipId);

        if (found == m_AnimationClips.end())
            return false;

        const auto& clip = found->second;

        auto& registry = m_Scene.GetRegistry();

        if (!registry.valid(entity) || !registry.all_of<Sprite>(entity))
            return false;

        Animation playback;
        playback.clipId = clipId;
        playback.currentFrame = startingFrame % clip.frames.size();

        registry.emplace_or_replace<Animation>(entity, playback);

        auto& sprite = registry.get<Sprite>(entity);
        sprite.textureId = clip.textureId;
        sprite.uvRect = clip.frames[playback.currentFrame];

        return true;
    }

    void Engine::UpdateAnimations(float deltaTime)
    {
        if (!std::isfinite(deltaTime) || deltaTime <= 0.0f)
            return;

        auto& registry = m_Scene.GetRegistry();
        auto animatedSprites = registry.view<Sprite, Animation>();

        for (const auto entity : animatedSprites)
        {
            auto& sprite = animatedSprites.get<Sprite>(entity);
            auto& animation = animatedSprites.get<Animation>(entity);

            const auto found =
                m_AnimationClips.find(animation.clipId);

            if (found == m_AnimationClips.end())
                continue;

            const auto& clip = found->second;

            // Reduce long frame delays to the equivalent loop position
            const double loopDuration =
                clip.frameDuration * clip.frames.size();

            animation.elapsed +=
                std::fmod(static_cast<double>(deltaTime), loopDuration);

            while (animation.elapsed >= clip.frameDuration)
            {
                animation.elapsed -= clip.frameDuration;

                animation.currentFrame =
                    (animation.currentFrame + 1) % clip.frames.size();
            }

            sprite.textureId = clip.textureId;
            sprite.uvRect = clip.frames[animation.currentFrame];
        }
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
        sprite.textureId = BaseShipTextureId;

        auto& velocity = registry.emplace<Velocity>(drifter);
        velocity.linear = {0.3f, 0.0f, 0.0f};

        // Temp, to test despawning
        registry.emplace<Drifter>(drifter);
    }
}
