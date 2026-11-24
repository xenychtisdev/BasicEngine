
#pragma once

#include <engine/core/Export.hpp>
#include <engine/window/window.hpp>
#include <engine/input/input.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/scene/scene.hpp>
#include <engine/assetHandler/textureStore.hpp>
#include <engine/assetHandler/animationClip.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>

namespace Engine
{
    class ENGINE_API Engine
    {
    public:
        Engine();
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        bool Initialize();
        void Update(float deltaTime);
        void Shutdown();

        bool IsRunning() const;

        static const char* GetVersion();

    private:
        void DrawControls();
        void DrawAppearanceControls();

        bool RegisterGridClip(
            const std::string& clipId,
            const std::string& textureId,
            int columns,
            int rows,
            int frameCount,
            double frameDuration,
            int firstFrame = 0
        );

        bool PlayAnimation(
            entt::entity entity,
            const std::string& clipId,
            std::size_t startingFrame = 0
        );
        void UpdateAnimations(float deltaTime);

        void SpawnDrifter(const glm::vec3& position);

    private:
        Window m_Window;
        Input m_Input;
        Renderer m_Renderer;
        TextureStore m_Textures;
        Scene m_Scene;

        entt::entity m_Player = entt::null;
        entt::entity m_PlayerEngine = entt::null;
        entt::entity m_PlayerEngineEffect = entt::null;

        glm::vec2 m_CameraPosition{0.0f, 0.0f};

        std::unordered_map<std::string, AnimationClip> m_AnimationClips;

        bool m_LeftMouseWasDown = false;

        bool m_Initialized = false;
    };
}
