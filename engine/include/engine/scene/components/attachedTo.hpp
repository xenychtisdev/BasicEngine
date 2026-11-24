#pragma once

#include <entt/entity/entity.hpp>

namespace Engine
{
    struct AttachedTo
    {
        entt::entity parent = entt::null;
    };
}