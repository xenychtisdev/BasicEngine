#pragma once

#include <glm/vec3.hpp>

namespace Engine
{
    struct Sprite
    {
        glm::vec3 tint{1.0f, 1.0f, 1.0f};
        float opacity = 1.0f;

        std::string textureId;

        // Minimum U/V, maximum U/V
        glm::vec4 uvRect{0.0f, 0.0f, 1.0f, 1.0f};
    };
}