#pragma once
#include "glm/glm.hpp"
#include "physics/world.hpp"

struct Position {
    glm::vec2 value;
};
struct Velocity {
    glm::vec2 value;
};
struct Mass {
    float value;
};
struct Force {
    glm::vec2 value;
};
struct Methabolism {
    float atf;
    float maxAtf;
    float atfConsumptionRate;
    float massConsumptionRate;
    float efficiency = 1;
    float massEfficiency = 0.5;
    bool isActive = true;
};
struct SplitComponent {
    float splitMass;
    bool makeAdhesin;
};
struct GenomeComponent {
    std::string genomeName;
    int currentModuleIndex;
};
struct RenderData {
    glm::vec4 color;
    float radius;
    float type;
};

struct Adhesion {
    entt::entity cellA;
    entt::entity cellB;

    float restLength;
    float maxLength;
    float strength;
};