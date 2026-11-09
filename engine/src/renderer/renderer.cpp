#include <engine/renderer/renderer.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

namespace Engine
{
    namespace
    {
        constexpr const char* VertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPosition;
            layout (location = 1) in vec2 aTexCoord;

            uniform mat4 u_ClipFromLocal;

            out vec2 v_TexCoord;

            void main()
            {
                v_TexCoord = aTexCoord;
                gl_Position = u_ClipFromLocal * vec4(aPosition, 1.0);
            }
        )";

        constexpr const char* FragmentShaderSource = R"(
            #version 330 core

            in vec2 v_TexCoord;

            uniform sampler2D u_Texture;
            uniform vec3 u_Tint;
            uniform float u_Opacity;
            
            out vec4 FragColor;

            void main()
            {
                vec4 color = texture(u_Texture, v_TexCoord);

                FragColor = vec4(
                    color.rgb * u_Tint,
                    color.a * u_Opacity
                );
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

        m_ModelLocation =
            glGetUniformLocation(m_ShaderProgram, "u_ClipFromLocal");

        m_TextureLocation =
            glGetUniformLocation(m_ShaderProgram, "u_Texture");

        m_TintLocation =
            glGetUniformLocation(m_ShaderProgram, "u_Tint");

        m_OpacityLocation =
            glGetUniformLocation(m_ShaderProgram, "u_Opacity");

        if (m_ModelLocation == -1 || 
            m_TextureLocation == -1 ||
            m_OpacityLocation == -1 ||
            m_TintLocation == -1
        )
        {
            std::cerr << "Could not find quad shader uniforms. \n";
            Shutdown();
            return false;
        }

        constexpr float vertices[] =
        {
            // Position             // UV
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // 0: bottom-left
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // 1: bottom-right
             0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // 2: top-right
             -0.5f, 0.5f, 0.0f,     0.0f, 1.0f  // 3: top-left
        };

        constexpr unsigned int indices[] = 
        {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Upload indices. This binding is stored in the VAO.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
        );

        // Position: first three floats.
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            nullptr
        );
        glEnableVertexAttribArray(0);

        // Texture coordinates: last two floats.
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float))
        );
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!m_Texture.LoadFromFile("assets/textures/sprite.png"))
        {
            Shutdown();
            return false;
        }

        // The sampler reads from texture unit zero.
        glUseProgram(m_ShaderProgram);
        glUniform1i(m_TextureLocation, 0);
        glUseProgram(0);

        m_Initialized = true;
        return true;
    }

    void Renderer::Shutdown()
    {
        // All OpenGL resources must be released before the context closes
        m_Texture.Shutdown();
        
        // Requires a valid, current OpenGL context.
        if (m_EBO != 0)
        {
            glDeleteBuffers(1, &m_EBO);
            m_EBO = 0;
        }
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

        m_ModelLocation = -1;
        m_TextureLocation = -1;
        m_TintLocation = -1;
        m_OpacityLocation = -1;

        m_Initialized = false;
    }

    void Renderer::BeginFrame()
    {
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::DrawQuad(
        const glm::mat4& model,
        const glm::vec3& tint,
        float opacity
    )
    {
        if (!m_Initialized)
            return;

        glUseProgram(m_ShaderProgram);

        glUniform3f(
            m_TintLocation,
            tint.r,
            tint.g,
            tint.b
        );

        glUniform1f(
            m_OpacityLocation,
            std::clamp(opacity, 0.0f, 1.0f)
        );

        glUniformMatrix4fv(
            m_ModelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        m_Texture.Bind(0);

        glBindVertexArray(m_VAO);
        
        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            nullptr
        );

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    void Renderer::EndFrame()
    {
        // Buffer swapping remains the Window's responsibility.
    }
}