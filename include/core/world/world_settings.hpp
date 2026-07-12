#pragma once
#include <glm/glm.hpp>

struct WorldSettings {
    float friction;
    //Friction of myocytes when they are active
    float MyoFriction;

    float viscosity;
    glm::vec2 gravity;
};