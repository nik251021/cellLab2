#include "physics/systems/biologicalSystem.hpp"
#include "physics/systems/physicSystem.hpp"
#include <iostream>
#include <physics/world.hpp>
#include <physics/components.hpp>

world::world(std::string name, GenomeRegistry& registry) : m_genomeRegistry(registry){
    this->curSettings = getWorldSettings(name);

    loadCellConfigs({ "data/configs/phagocyte.json", "data/configs/photocyte.json"});
}

entt::entity world::spawnCell(const std::string& type, glm::vec2 pos, glm::vec2 vel, glm::vec4 color) {
    if (m_cellTemplates.find(type) == m_cellTemplates.end()) return entt::null;

    const auto& t = m_cellTemplates[type];
    
    auto entity = m_registry.create();
    m_registry.emplace<Position>(entity, pos);
    m_registry.emplace<Velocity>(entity, vel);
    m_registry.emplace<Mass>(entity, t.Default_mass);
    m_registry.emplace<Force>(entity, glm::vec2(0.0f));

    Methabolism met;
    met.atf = t.maxAtf;
    met.maxAtf = t.maxAtf;
    met.atfConsumptionRate = t.atfConsumptionRate;
    met.massConsumptionRate = t.massConsumptionRate;
    
    m_registry.emplace<Methabolism>(entity, met);
    m_registry.emplace<RenderData>(entity, color, t.maxRadius, t.typeId);

    return entity;
}

entt::entity world::spawnCellFromModule(const std::string& genomeName, int moduleIndex, glm::vec2 position) {
    const auto& mod = m_genomeRegistry.getModule(genomeName, moduleIndex);
    
    std::string type = mod.cell_type; 
    
    if (m_cellTemplates.find(type) == m_cellTemplates.end()) {
        std::cerr << "Error: Cell type '" << type << "' not found!" << std::endl;
        return entt::null;
    }
    bool shouldMakeAdhesin = false;
    if (mod.flags.count("makeAdhesin")) {
        shouldMakeAdhesin = mod.flags.at("makeAdhesin");
    }
    
    const auto& t = m_cellTemplates.at(type); 

    auto entity = m_registry.create();
    m_registry.emplace<GenomeComponent>(entity, genomeName, moduleIndex);
    m_registry.emplace<SplitComponent>(entity, mod.split_mass, shouldMakeAdhesin);
    std::cout << shouldMakeAdhesin << std::endl;
    m_registry.emplace<Position>(entity, position);
    m_registry.emplace<Velocity>(entity, glm::vec2(0.0f));
    m_registry.emplace<Mass>(entity, t.Default_mass);
    m_registry.emplace<Force>(entity, glm::vec2(0.0f));

    Methabolism met;
    met.atf = t.maxAtf; 
    met.maxAtf = t.maxAtf;
    met.atfConsumptionRate = t.atfConsumptionRate;
    met.massConsumptionRate = t.massConsumptionRate;
    m_registry.emplace<Methabolism>(entity, met);
    
    auto getColor = [&](const std::string& key, float defaultVal) {
        auto it = mod.params.find(key);
        return (it != mod.params.end()) ? it->second : defaultVal;
    };

    glm::vec4 color(
        getColor("color_r", 1.0f),
        getColor("color_g", 0.5f),
        getColor("color_b", 0.0f),
        getColor("color_a", 1.0f)
    );

    m_registry.emplace<RenderData>(entity, color, t.maxRadius, (float)t.typeId);

    return entity;
}

void world::forceSplit(entt::entity parent) {
    if (!m_registry.all_of<GenomeComponent, Position, Mass, SplitComponent>(parent)) return;

    auto& gen = m_registry.get<GenomeComponent>(parent);
    auto& pos = m_registry.get<Position>(parent);
    auto& mass = m_registry.get<Mass>(parent);
    auto& split = m_registry.get<SplitComponent>(parent);

    const auto& mod = m_genomeRegistry.getModule(gen.genomeName, gen.currentModuleIndex);
    if (mod.childs.empty()) return;

    float newMass = mass.value / 2.0f;
    std::vector<entt::entity> children;

    float angleRad = glm::radians(mod.params.count("split_angle") ? mod.params.at("split_angle") : 0.0f);
    
    glm::vec2 baseDir = glm::vec2(cos(angleRad), sin(angleRad));

    for (auto const& [key, nextModuleIndex] : mod.childs) {
        glm::vec2 direction = (key == "child1") ? baseDir : -baseDir;
        glm::vec2 offset = direction * 7.5f; 

        auto child = spawnCellFromModule(gen.genomeName, nextModuleIndex, pos.value + offset);

        if (child != entt::null) {
            m_registry.get<Mass>(child).value = newMass;
            children.push_back(child);
        }
    }

    if (!children.empty()) {
        transferAdhesions(parent, children);
    }

    if (split.makeAdhesin && children.size() >= 2) {
        makeAdhesin(children[0], children[1], 10.0f, 30.0f, 50.0f);
    }

    m_registry.destroy(parent);
}

void world::updateDivision(float dt) {
    std::vector<entt::entity> toSplit;

    m_registry.view<Mass, SplitComponent>().each([&](auto entity, auto& mass, auto& split) {
        if (mass.value >= split.splitMass) {
            toSplit.push_back(entity);
        }
    });

    for (auto e : toSplit) {
        if (m_registry.valid(e)) {
            forceSplit(e);
        }
    }
}

void world::transferAdhesions(entt::entity oldParent, const std::vector<entt::entity>& children) {
    auto view = m_registry.view<Adhesion>();
    
    std::vector<Adhesion> newAdhesions;

    for (auto entity : view) {
        auto& adj = view.get<Adhesion>(entity);
        
        if (adj.cellA == oldParent || adj.cellB == oldParent) {
            entt::entity partner = (adj.cellA == oldParent) ? adj.cellB : adj.cellA;

            for (auto child : children) {
                newAdhesions.push_back({child, partner, adj.restLength, adj.maxLength, adj.strength});
            }
            
            m_registry.destroy(entity);
        }
    }

    for (auto& adj : newAdhesions) {
        makeAdhesin(adj.cellA, adj.cellB, adj.restLength, adj.maxLength, adj.strength);
    }
}

entt::entity world::makeAdhesin(entt::entity cell1, entt::entity cell2, float restLength, float maxLength, float strength) {
    if (!m_registry.valid(cell1) || !m_registry.valid(cell2)) return entt::null;

    if (cell1 == cell2) return entt::null;
    
    auto e = m_registry.create();
    m_registry.emplace<Adhesion>(e, cell1, cell2, restLength, maxLength, strength);

    return e;
}
void onCollision(entt::entity e1, entt::entity e2) {

}



void world::update(float dt) {
    PhysicsSystem::update(m_registry, curSettings, dt);
    BiologicalSystem::update(m_registry, dt);
    updateDivision(dt);
}

void world::prepareRenderer(RenderBridge& rb) {
    auto view = m_registry.view<Position, RenderData>();
    
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& rd  = view.get<RenderData>(entity);
        
        rb.drawCell(pos.value, rd.radius, rd.type, rd.color);
    }
}

void world::loadCellConfigs(const std::vector<std::string>& filePaths) {
    for (const auto& path : filePaths) {
        CellTemplate t = loadCellTemplate(path);
        m_cellTemplates[t.displayName] = t;
    }
}