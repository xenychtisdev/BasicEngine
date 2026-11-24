#pragma once

#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace Engine
{
    struct AnimationClip
    {
        std::string textureId;
        std::vector<glm::vec4> frames;
        double frameDuration = 0.1; // Seconds per frame
    };
}