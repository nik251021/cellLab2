#include <physics/world.hpp>
#include <physics/components.hpp>

world::world(std::string name) {
    this->curSettings = getWorldSettings(name);

    loadCellConfigs({ "data/configs/phagocyte.json", "data/configs/photocyte.json"});
}

void world::spawnCell(const std::string& type, glm::vec2 pos, glm::vec2 vel, glm::vec4 color) {
    if (m_cellTemplates.find(type) == m_cellTemplates.end()) return;

    const auto& t = m_cellTemplates[type];
    
    auto entity = m_registry.create();
    m_registry.emplace<Position>(entity, pos);
    m_registry.emplace<Velocity>(entity, vel);
    m_registry.emplace<Mass>(entity, t.Default_mass);
    m_registry.emplace<Force>(entity, glm::vec2(0.0f));

    //methabolism
    Methabolism met;
    met.atf = t.maxAtf;
    met.maxAtf = t.maxAtf;
    met.atfConsumptionRate = t.atfConsumptionRate;
    met.massConsumptionRate = t.massConsumptionRate;
    
    m_registry.emplace<Methabolism>(entity, met);
    m_registry.emplace<RenderData>(entity, color, t.maxRadius, t.typeId);
}

bool tryToSplit() {
    return false;
}
// Cells logic on collision
void onCollision(entt::entity e1, entt::entity e2) {

}
// Cells logic on methabolism
void world::updateMetabolism(float dt) {
    m_registry.view<Methabolism>().each([this, dt](auto entity, auto& met) {
        // Локальная логика метаболизма
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
        //Checking up cells:
        if (!m_registry.valid(adj.cellA) || !m_registry.valid(adj.cellB)) {
            m_registry.destroy(entity);
            return;
        }
        //Physics:
        auto& posA = m_registry.get<Position>(adj.cellA);
        auto& posB = m_registry.get<Position>(adj.cellB);

        glm::vec2 delta = posA.value - posB.value;
        float dist = glm::length(delta);
        //Checking length:
        if (dist > adj.maxLength) {
            m_registry.destroy(entity);
            return;
        }

        glm::vec2 direction = (dist > 0.0001f) ? (delta / dist) : glm::vec2(0.0f);
        float forceMag = adj.strength * (dist - adj.restLength);
        glm::vec2 force = -forceMag * direction;

        auto& fA = m_registry.get<Force>(adj.cellA);
        auto& fB = m_registry.get<Force>(adj.cellB);
        
        fA.value += force;
        fB.value -= force;
        //Biology:
        auto& metA = m_registry.get<Methabolism>(adj.cellA);
        auto& metB = m_registry.get<Methabolism>(adj.cellB);
        
        float transferRate = 5.0f;
        float diff = (metA.atf - metB.atf) * transferRate * dt;
        
        metA.atf -= diff;
        metB.atf += diff;
        //Future signals:
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
    
    // Заполняем сетку
    view.each([this](auto entity, auto& pos, auto&, auto&) {
        m_grid.add(entity, pos.value);
    });

    // Обрабатываем коллизии
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