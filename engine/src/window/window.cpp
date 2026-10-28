#include <Engine/Window/Window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine
{
    Window::Window(int width, int height, const char* title)
        : m_Width(width), m_Height(height), m_Title(title ? title : "")
    {
    }

    Window::~Window()
    {
        Destroy();
    }

    bool Window::Create()
    {
        if (!glfwInit())
            return false;

        m_Window = glfwCreateWindow(
            m_Width, m_Height, m_Title.c_str(), nullptr, nullptr
        );

        if (!m_Window)
        {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader(
                reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
            glfwTerminate();
            return false;
        }

        return true;
    }

    void Window::Destroy()
    {
        if (m_Window != nullptr)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    bool Window::ShouldClose() const
    {
        return !m_Window || glfwWindowShouldClose(m_Window);
    }

    void Window::SwapBuffers()
    {
        if (m_Window)
            glfwSwapBuffers(m_Window);
    }

    GLFWwindow* Window::GetNativeWindow() const
    {
        return m_Window;
    }
}