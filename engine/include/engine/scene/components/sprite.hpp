#pragma once

#include <glm/vec3.hpp>

namespace Engine
{
    struct Sprite
    {
        glm::vec3 tint{1.0f, 1.0f, 1.0f};
        float opacity = 1.0f;
    };
}