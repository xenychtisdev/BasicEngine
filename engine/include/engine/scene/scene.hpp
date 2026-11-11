#pragma once

#include <entt/entity/registry.hpp>

namespace Engine
{
    struct Scene
    {
    public:
        entt::registry& GetRegistry()
        {
            return m_Registry;
        }

        const entt::registry& GetRegistry() const
        {
            return m_Registry;
        }

    private:
        entt::registry m_Registry;
    };
}