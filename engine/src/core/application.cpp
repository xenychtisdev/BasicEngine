#include "engine/core/application.hpp"

#include <chrono>

Application::~Application()
{
    Shutdown();
}

int Application::Run()
{
    if (!Initialize())
        return -1;

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    while (m_engine.IsRunning())
    {
        const auto currentTime = Clock::now();

        const float deltaTime = std::chrono::duration<float>(
            currentTime - previousTime
        ).count();

        previousTime = currentTime;
        
        Update(deltaTime);
    }

    Shutdown();
    return 0;
}

bool Application::Initialize()
{
    if (m_initialized)
        return true;

    if (!m_engine.Initialize())
        return false;

    m_initialized = true;
    return true;
}

void Application::Update(float deltaTime)
{
    m_engine.Update(deltaTime);
}

void Application::Shutdown()
{
    if (!m_initialized)
        return;

    m_engine.Shutdown();
    m_initialized = false;
}