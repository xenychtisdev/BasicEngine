#include "Engine/Core/Application.hpp"

Application::~Application()
{
    Shutdown();
}

int Application::Run()
{
    if (!Initialize())
        return -1;

    while (m_engine.IsRunning())
    {
        Update();
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

void Application::Update()
{
    m_engine.Update();
}

void Application::Shutdown()
{
    if (!m_initialized)
        return;

    m_engine.Shutdown();
    m_initialized = false;
}