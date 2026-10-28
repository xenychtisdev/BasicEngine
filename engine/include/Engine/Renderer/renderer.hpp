#pragma once

#include <Engine/Core/Export.hpp>

namespace Engine
{
    class ENGINE_API Renderer
    {
    public:
        Renderer() = default;
        ~Renderer();

        // OpenGL resources should not be copied.
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        bool Initialize();
        void Shutdown();

        void BeginFrame();
        void DrawTriangle();
        void EndFrame();

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_ShaderProgram = 0;

        bool m_Initialized = false;
    };
}