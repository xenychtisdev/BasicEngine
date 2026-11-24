#pragma once

#include <engine/renderer/texture2d.hpp>

#include <filesystem>
#include <memory>
#include <String>
#include <unordered_map>

namespace Engine
{
    class TextureStore
    {
    public:
        bool Load(const std::string& id, const std::filesystem::path& path)
        {
            if (m_Textures.contains(id))
                return false;

            auto texture = std::make_unique<Texture2D>();

            if (!texture->LoadFromFile(path.string()))
                return false;

            m_Textures.emplace(id, std::move(texture));
            return true;
        }

        const Texture2D* Find(const std::string& id) const
        {
            const auto found = m_Textures.find(id);
            return found != m_Textures.end() ? found->second.get() : nullptr;
        }

        void Clear()
        {
            m_Textures.clear();
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_Textures;
    };
}