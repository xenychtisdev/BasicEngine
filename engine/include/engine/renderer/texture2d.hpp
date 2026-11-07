#pragma once

#include <engine/core/Export.hpp>
#include <string>

namespace Engine
{
    class ENGINE_API Texture2D
    {
    public:
        Texture2D() = default;
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        bool LoadFromFile(const std::string& path);
        void Shutdown();

        void Bind(unsigned int slot = 0) const;

        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        bool IsLoaded() const { return m_TextureID != 0; }

    private:
        unsigned int m_TextureID = 0;

        int m_Width = 0;
        int m_Height = 0;
    };
}