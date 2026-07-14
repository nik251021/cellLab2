#pragma once
#include <glm/glm.hpp>

struct InstanceData {
    glm::vec2 position;
    float radius;
    float cellType; // Например: 0 - круг, 1 - квадрат, 2 - треугольник
    glm::vec4 color; // RGBA

    InstanceData(glm::vec2 pos, float rad, float type, glm::vec4 col)
        : position(pos), radius(rad), cellType(type), color(col) {}
};