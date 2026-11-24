#pragma once

#include <cstddef>
#include <string>

namespace Engine
{
    struct Animation
    {
        std::string clipId;
        std::size_t currentFrame = 0;
        double elapsed = 0.0;
    };
}