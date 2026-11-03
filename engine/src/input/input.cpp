#include <engine/input/input.hpp>
#include <engine/window/window.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine
{
    namespace
    {
        int ToGLFWKey(Key key)
        {
            switch (key)
            {
                case Key::W:        return GLFW_KEY_W;
                case Key::A:        return GLFW_KEY_A;
                case Key::S:        return GLFW_KEY_S;
                case Key::D:        return GLFW_KEY_D;
                case Key::Q:        return GLFW_KEY_Q;
                case Key::E:        return GLFW_KEY_E;
                case Key::X:        return GLFW_KEY_X;
                case Key::Z:        return GLFW_KEY_Z;
                case Key::R:        return GLFW_KEY_R;
                case Key::Escape:   return GLFW_KEY_ESCAPE;
            }

            return GLFW_KEY_UNKNOWN;
        }
    }

    Input::Input(const Window& window)
    : m_Window(window)
    {
    }

    bool Input::IsKeyDown(Key key) const
    {
        GLFWwindow* window = m_Window.GetNativeWindow();
        const int nativeKey = ToGLFWKey(key);

        if (!window || nativeKey == GLFW_KEY_UNKNOWN)
            return false;

        return glfwGetKey(window, nativeKey) == GLFW_PRESS;
    }
}