#pragma once

#include <glm/vec3.hpp>

namespace Engine
{
    struct Velocity
    {
        glm::vec3 linear{0.0f, 0.0f, 0.0f}; // World units / sec
    };
}