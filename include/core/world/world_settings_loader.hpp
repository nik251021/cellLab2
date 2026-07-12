#include <fstream>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <core/world/world_settings.hpp>
using json = nlohmann::json;

WorldSettings loadSettings(const std::string& path) {
    std::ifstream file(path);
    json j;
    file >> j;

    WorldSettings settings;
    settings.friction = j["friction"].get<float>();
    settings.viscosity = j["viscosity"].get<float>();
    settings.gravity = glm::vec2(j["gravity"][0].get<float>(), j["gravity"][1].get<float>());
    
    return settings;
}