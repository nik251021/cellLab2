#pragma once
#include "glm/glm.hpp"

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
    float atfConsumptionRate; // If atf > 0
    float massConsumptionRate; // If atf < 0, the mass will be drained to keep cell alive, but cell will be disabled
    float efficiency = 1;
    float massEfficiency = 0.5;
    bool isActive = true;
};
//Currently not used
struct Genome {
    int id;
};
struct RenderData {
    glm::vec4 color;
    float radius;
    float type;
};