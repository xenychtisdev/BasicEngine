
#pragma once

#include <Engine/Core/Export.hpp>
#include <Engine/Window/Window.hpp>
#include <Engine/Renderer/Renderer.hpp>

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
        void Update();
        void Shutdown();

        bool IsRunning() const;

        static const char* GetVersion();

    private:
        Window m_Window;
        Renderer m_Renderer;

        bool m_Initialized = false;
    };
}
