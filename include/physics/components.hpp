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
//Currently not used
struct Energy    {
    float value;
}; 
//Currently not used
struct Genome {
    int id;
};
//Currently not used
struct RenderData {
    glm::vec4 color;
    float radius;
    float type;
};