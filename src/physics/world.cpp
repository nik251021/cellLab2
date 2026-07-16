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

void world::updateMetabolism(float dt) {
    m_registry.view<Methabolism>().each([this, dt](auto entity, auto& met) {
        if (met.isActive) {
            if (met.atf > 0) {
                met.atf -= met.atfConsumptionRate * dt;
            } else {
                met.isActive = false;
            }
        } else {
            if (met.atf <= 0) {
                auto& mass = m_registry.get<Mass>(entity);
                mass.value -= met.massConsumptionRate * dt;
                if (mass.value <= 0) {
                    m_registry.destroy(entity);
                }
            } else {
                met.isActive = true;
            }
        }
    });
}

void world::updateAdhesion(float dt) {
    auto view = m_registry.view<Adhesion>();

    view.each([this, dt](auto entity, auto& adj){
        if (!m_registry.valid(adj.cellA) || !m_registry.valid(adj.cellB)) {
            m_registry.destroy(entity);
            return;
        }

        auto& posA = m_registry.get<Position>(adj.cellA);
        auto& posB = m_registry.get<Position>(adj.cellB);
        auto& velA = m_registry.get<Velocity>(adj.cellA);
        auto& velB = m_registry.get<Velocity>(adj.cellB);

        glm::vec2 delta = posA.value - posB.value;
        float dist = glm::length(delta);

        if (dist > adj.maxLength) {
            m_registry.destroy(entity);
            return;
        }

        glm::vec2 direction = (dist > 0.0001f) ? (delta / dist) : glm::vec2(0.0f);

        float springForce = adj.strength * (dist - adj.restLength);

        glm::vec2 relativeVel = velA.value - velB.value;
        float dampingFactor = 10.0f;
        float dampingForce = glm::dot(relativeVel, direction) * dampingFactor;

        float totalForceMag = springForce + dampingForce;
        glm::vec2 force = -totalForceMag * direction;

        auto& fA = m_registry.get<Force>(adj.cellA);
        auto& fB = m_registry.get<Force>(adj.cellB);
        
        fA.value += force;
        fB.value -= force;

        auto& metA = m_registry.get<Methabolism>(adj.cellA);
        auto& metB = m_registry.get<Methabolism>(adj.cellB);
        
        float transferRate = 5.0f;
        float diff = (metA.atf - metB.atf) * transferRate * dt;
        
        metA.atf -= diff;
        metB.atf += diff;
    });
}

void world::applyPhysicsForces(float dt) {
    m_registry.view<Velocity, Mass, Force>().each([this, dt](auto& vel, auto& mass, auto& force) {
        glm::vec2 acceleration = (force.value + glm::vec2(0.0f, curSettings.gravity * mass.value) - (vel.value * curSettings.viscosity)) / mass.value;
        
        vel.value += acceleration * dt;
        vel.value *= glm::clamp(1.0f - curSettings.friction * dt, 0.0f, 1.0f);
        
        force.value = glm::vec2(0.0f); 
    });
}

void world::integratePosition(float dt) {
    auto view = m_registry.view<Position, Velocity, RenderData>();
    view.each([this, dt](auto& pos, auto& vel, auto& rd) {
        pos.value += vel.value * dt;

        const float bounce = 0.5f;
        if (pos.value.x < 0) { pos.value.x = 0; vel.value.x *= -bounce; }
        else if (pos.value.x > curSettings.sizeX) { pos.value.x = curSettings.sizeX; vel.value.x *= -bounce; }
        
        if (pos.value.y < 0) { pos.value.y = 0; vel.value.y *= -bounce; }
        else if (pos.value.y > curSettings.sizeY) { pos.value.y = curSettings.sizeY; vel.value.y *= -bounce; }
    });
}

void world::resolveCollisions(float dt) {
    m_grid.clear();
    auto view = m_registry.view<Position, Velocity, RenderData>();
    
    view.each([this](auto entity, auto& pos, auto&, auto&) {
        m_grid.add(entity, pos.value);
    });

    view.each([this](auto entityA, auto& posA, auto& velA, auto& rdA) {
        int neighbors[9];
        m_grid.getNeighboringCells(posA.value, neighbors);

        for (int i = 0; i < 9; ++i) {
            auto it = m_grid.cells.find(neighbors[i]);
            if (it == m_grid.cells.end()) continue;

            for (auto entityB : it->second) {
                if (entityA >= entityB) continue;

                auto& posB = m_registry.get<Position>(entityB);
                auto& rdB = m_registry.get<RenderData>(entityB);

                glm::vec2 delta = posA.value - posB.value;
                float distSq = glm::dot(delta, delta);
                float minDist = rdA.radius + rdB.radius;

                if (distSq < minDist * minDist && distSq > 0.000001f) {
                    float dist = std::sqrt(distSq);
                    glm::vec2 normal = delta / dist;
                    float overlap = minDist - dist;

                    auto& m1 = m_registry.get<Mass>(entityA);
                    auto& m2 = m_registry.get<Mass>(entityB);
                    float totalMass = m1.value + m2.value;

                    posA.value += normal * (overlap * (m2.value / totalMass));
                    posB.value -= normal * (overlap * (m1.value / totalMass));
                    
                    auto& velB = m_registry.get<Velocity>(entityB);
                    glm::vec2 relativeVel = velA.value - velB.value;
                    float velAlongNormal = glm::dot(relativeVel, normal);
                    
                    if (velAlongNormal < 0) {
                        float restitution = 0.8f; 
                        float j = -(1.0f + restitution) * velAlongNormal;
                        j /= (1.0f / m1.value + 1.0f / m2.value);
                        
                        glm::vec2 impulse = j * normal;
                        velA.value += (1.0f / m1.value) * impulse;
                        velB.value -= (1.0f / m2.value) * impulse;
                    }
                }
            }
        }
    });
}

void world::update(float dt) {
    updateMetabolism(dt);
    updateAdhesion(dt);
    applyPhysicsForces(dt);
    integratePosition(dt);
    resolveCollisions(dt);
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