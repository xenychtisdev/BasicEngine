
#pragma once

#include <engine/core/Export.hpp>
#include <engine/window/window.hpp>
#include <engine/input/input.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/math/transform.hpp>

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

    private:
        Window m_Window;
        Input m_Input;
        Renderer m_Renderer;

        Transform m_QuadTransform;
        glm::vec3 m_QuadTint{1.0f, 1.0f, 1.0f};
        float m_QuadOpacity = 1.0f;

        bool m_Initialized = false;
    };
}
