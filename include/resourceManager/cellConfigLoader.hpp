#pragma once

#include <glm/glm.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

struct CellTemplate {
    std::string displayName;
    float maxRadius;
    float Default_mass;
    float foodStorage;
    float typeId;
    glm::vec4 color;
    float maxCollisionStrength;
    
    float maxAtf;
    float atfConsumptionRate;
    float massConsumptionRate;
};

inline CellTemplate loadCellTemplate(const std::string& path) {
    std::ifstream file(path);
    nlohmann::json j;
    file >> j;

    CellTemplate t;
    t.displayName = j["Display_name"];
    t.maxRadius = j["Max_radius"];
    t.Default_mass = j["Default_mass"];
    t.foodStorage = j["Food_storage"];
    t.typeId = j["typeid"];
    t.maxCollisionStrength = j["Max_collision strength"];
    
    t.maxAtf = j["Metabolism"]["maxAtf"];
    t.atfConsumptionRate = j["Metabolism"]["atfConsumptionRate"];
    t.massConsumptionRate = j["Metabolism"]["massConsumptionRate"];
    
    t.color = glm::vec4(
        static_cast<float>(j["Default_color"]["red"]) / 255.0f,
        static_cast<float>(j["Default_color"]["green"]) / 255.0f,
        static_cast<float>(j["Default_color"]["blue"]) / 255.0f,
        1.0f
    );
    return t;
}