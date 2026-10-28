#include <Engine/Renderer/Renderer.hpp>

#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace Engine
{
    namespace
    {
        constexpr const char* VertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPosition;

            void main()
            {
                gl_Position = vec4(aPosition, 1.0);
            }
        )";

        constexpr const char* FragmentShaderSource = R"(
            #version 330 core

            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(0.2, 0.7, 1.0, 1.0);
            }
        )";

        GLuint CompileShader(GLenum type, const char* source)
        {
            GLuint shader = glCreateShader(type);

            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            GLint success = GL_FALSE;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success != GL_TRUE)
            {
                GLint length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

                if (length > 0)
                {
                    std::vector<char> log(
                        static_cast<std::size_t>(length)
                    );

                    glGetShaderInfoLog(
                        shader, length, nullptr, log.data()
                    );

                    std::cerr << "Shader compilation failed:\n"
                              << log.data() << '\n';
                }

                glDeleteShader(shader);
                return 0;
            }

            return shader;
        }

        GLuint CreateShaderProgram()
        {
            GLuint vertexShader =
                CompileShader(GL_VERTEX_SHADER, VertexShaderSource);

            if (vertexShader == 0)
                return 0;

            GLuint fragmentShader =
                CompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);

            if (fragmentShader == 0)
            {
                glDeleteShader(vertexShader);
                return 0;
            }

            GLuint program = glCreateProgram();

            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);

            GLint success = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (success != GL_TRUE)
            {
                GLint length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

                if (length > 0)
                {
                    std::vector<char> log(
                        static_cast<std::size_t>(length)
                    );

                    glGetProgramInfoLog(
                        program, length, nullptr, log.data()
                    );

                    std::cerr << "Shader linking failed:\n"
                              << log.data() << '\n';
                }

                glDeleteProgram(program);
                program = 0;
            }

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            return program;
        }
    }

    Renderer::~Renderer()
    {
        Shutdown();
    }

    bool Renderer::Initialize()
    {
        if (m_Initialized)
            return true;

        // A valid, current OpenGL context and initialized GLAD are required.
        m_ShaderProgram = CreateShaderProgram();

        if (m_ShaderProgram == 0)
        {
            Shutdown();
            return false;
        }

        constexpr float vertices[] =
        {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            nullptr
        );

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_Initialized = true;
        return true;
    }

    void Renderer::Shutdown()
    {
        // Requires a valid, current OpenGL context.
        if (m_VBO != 0)
        {
            glDeleteBuffers(1, &m_VBO);
            m_VBO = 0;
        }

        if (m_VAO != 0)
        {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }

        if (m_ShaderProgram != 0)
        {
            glDeleteProgram(m_ShaderProgram);
            m_ShaderProgram = 0;
        }

        m_Initialized = false;
    }

    void Renderer::BeginFrame()
    {
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::DrawTriangle()
    {
        if (!m_Initialized)
            return;

        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Renderer::EndFrame()
    {
        // Buffer swapping remains the Window's responsibility.
    }
}