#pragma once

#include <Engine/Engine.hpp>

class ENGINE_API Application
{
public:
    Application() = default;
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    int Run();

private:
    bool Initialize();
    void Update();
    void Shutdown();

    Engine::Engine m_engine;
    bool m_initialized = false;
};