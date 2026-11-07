#include <engine/renderer/texture2d.hpp>

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

namespace Engine
{
    Texture2D::~Texture2D()
    {
        Shutdown();
    }

    bool Texture2D::LoadFromFile(const std::string& path)
    {
        
        // Match UV's where (0, 0) represents the bottom-left
        stbi_set_flip_vertically_on_load(true);

        int width = 0;
        int height = 0;
        int sourceChannels = 0;

        // Always request 4 channels - RGBA
        unsigned char* pixels = stbi_load(
            path.c_str(),
            &width,
            &height,
            &sourceChannels,
            STBI_rgb_alpha
        );

        if (!pixels)
        {
            const char* reason = stbi_failure_reason();

            std::cerr   << "Failed to load texture: " << path
                        << "\nReason: "
                        << (reason ? reason : "Unknown error")
                        << '\n';

            return false;
        }

        int maxTextureSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

        if (width > maxTextureSize || height > maxTextureSize)
        {
            std::cerr   << "Texture exceeds OpenGL size limit: "
                        << path << '\n';
            
            stbi_image_free(pixels);
            return false;
        }

        // Release any prior textures
        Shutdown();

        glGenTextures(1, &m_TextureID);

        if (m_TextureID == 0)
        {
            std::cerr   << "Failed to create OpenGL texture: "
                        << path << '\n';

            stbi_image_free(pixels);
            return false;
        }

        // Preserve the current binding on the active texture unit.
        int previousTexture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,                  // Mipmap level
            GL_RGBA8,           // GPU storage format
            width,
            height,
            0,                  // Border: must be 0
            GL_RGBA,            // Input pixel format
            GL_UNSIGNED_BYTE,   // Input component type
            pixels
        );

        glBindTexture(
            GL_TEXTURE_2D,
            static_cast<unsigned int>(previousTexture)
        );

        // OpenGL has copied the pixels, release the CPU copy.
        stbi_image_free(pixels);

        m_Width = width;
        m_Height = height;

        return true;
    }

    void Texture2D::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture2D::Shutdown()
    {
        if (m_TextureID != 0)
        {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }

        m_Width = 0;
        m_Height = 0;
    }
}