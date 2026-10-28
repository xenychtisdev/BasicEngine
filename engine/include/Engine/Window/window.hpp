
#pragma once

#include <Engine/Core/Export.hpp>

#include <string>

struct GLFWwindow;

namespace Engine
{
    class ENGINE_API Window
    {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool Create();
        void Destroy();

        void PollEvents();
        bool ShouldClose() const;
        void SwapBuffers();

        GLFWwindow* GetNativeWindow() const;

    private:
        int m_Width;
        int m_Height;
        std::string m_Title;

        GLFWwindow* m_Window = nullptr;
    };
}
