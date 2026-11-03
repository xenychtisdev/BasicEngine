#pragma once

#include <engine/core/Export.hpp>

namespace Engine
{
    class Window;

    enum class Key
    {
        W, A, S, D,
        Q, E,
        X, Z,
        R, 
        Escape
    };

    class ENGINE_API Input
    {
    public:
        explicit Input(const Window& window);

        bool IsKeyDown(Key key) const;

    private:
        const Window& m_Window;
    };
}