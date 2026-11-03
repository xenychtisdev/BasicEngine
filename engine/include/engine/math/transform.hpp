#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    struct Transform
    {
        glm::vec3 position{0.0f};
        float rotationRadians = 0.0f;
        glm::vec3 scale{1.0f};

        glm::mat4 GetMatrix() const
        {
            glm::mat4 model(1.0f);  // Identity matrix

            model = glm::translate(model, position);
            model = glm::rotate(
                model,
                rotationRadians,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            model = glm::scale(model, scale);

            return model;
        }
    };
}