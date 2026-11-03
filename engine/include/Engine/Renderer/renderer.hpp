#pragma once

#include <engine/core/Export.hpp>
#include <glm/mat4x4.hpp>

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
        void DrawTriangle(const glm::mat4& model);
        void EndFrame();

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_ShaderProgram = 0;

        int m_ModelLocation = -1;
        bool m_Initialized = false;
    };
}