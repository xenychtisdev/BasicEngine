
#pragma once

#include <engine/core/Export.hpp>
#include <engine/window/window.hpp>
#include <engine/input/input.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/scene/scene.hpp>

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

        void SpawnDrifter(const glm::vec3& position);

    private:
        Window m_Window;
        Input m_Input;
        Renderer m_Renderer;
        Scene m_Scene;

        entt::entity m_Player = entt::null;

        glm::vec2 m_CameraPosition{0.0f, 0.0f};

        bool m_LeftMouseWasDown = false;

        bool m_Initialized = false;
    };
}
