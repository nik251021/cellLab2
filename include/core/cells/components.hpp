#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct Physics {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float maxRadius;
};

struct Bio {
    float energy;
    float age;
    float maxAge;
};

struct PhagocyteTag{};
struct PhotocyteTag{};
struct DevorocyteTag{};
struct LypociteTag{};