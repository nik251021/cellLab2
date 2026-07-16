#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <bridge/renderBridge.hpp>
#include <resourceManager/worldLoader.hpp>
#include <resourceManager/genome_registry.hpp>

#include <resourceManager/cellConfigLoader.hpp>

struct SpatialGrid {
    static constexpr int CELL_SIZE = 50;
    std::unordered_map<int, std::vector<entt::entity>> cells;

    int getCellId(glm::vec2 pos) const {
        int x = static_cast<int>(pos.x / CELL_SIZE);
        int y = static_cast<int>(pos.y / CELL_SIZE);
        return x * 10000 + y;
    }
    
    void clear() { cells.clear(); }
    void add(entt::entity e, glm::vec2 pos) { cells[getCellId(pos)].push_back(e); }

    void getNeighboringCells(glm::vec2 pos, int* out) const {
        int cx = static_cast<int>(pos.x / CELL_SIZE);
        int cy = static_cast<int>(pos.y / CELL_SIZE);
        
        int i = 0;
        for(int x = cx - 1; x <= cx + 1; ++x) {
            for(int y = cy - 1; y <= cy + 1; ++y) {
                out[i++] = x * 10000 + y;
            }
        }
    }
};

class world {
    friend class worldApi;
private:
    std::unordered_map<std::string, CellTemplate> m_cellTemplates;
    worldSettings curSettings;
    entt::registry m_registry;
    GenomeRegistry& m_genomeRegistry;
    SpatialGrid m_grid;
    void updateMetabolism(float dt);
    void updateAdhesion(float dt);
    void applyPhysicsForces(float dt);
    void integratePosition(float dt);
    void resolveCollisions(float dt);
    void updateDivision(float dt);
    void transferAdhesions(entt::entity oldParent, const std::vector<entt::entity>& children);
public:
    world(std::string worldName, GenomeRegistry& registry);
    void loadCellConfigs(const std::vector<std::string>& filePaths);
    
    entt::entity spawnCell(const std::string& type, glm::vec2 pos, glm::vec2 vel, glm::vec4 color);
    entt::entity spawnCellFromModule(const std::string& genomeName, int moduleIndex, glm::vec2 position);
    void forceSplit(entt::entity cell);

    entt::entity makeAdhesin(entt::entity cell1, entt::entity cell2, float restLength, float maxLength, float strength);

    void update(float dt);
    void prepareRenderer(RenderBridge& rb);
};