#include <physics/world.hpp>
#include <physics/components.hpp>

world::world(std::string name) {
    this->curSettings = getWorldSettings(name);
}

void world::spawnCell(glm::vec2 pos, glm::vec2 vel) {
    auto entity = m_registry.create();
    
    m_registry.emplace<Position>(entity, pos);
    m_registry.emplace<Velocity>(entity, vel);
    m_registry.emplace<Mass>(entity, 1.0f);
    m_registry.emplace<RenderData>(entity, glm::vec4(1.0f), 0.5f, 0.0f);
}

void world::update(float dt) {
    auto view = m_registry.view<Position, Velocity>();
    
    view.each([dt](auto& pos, auto& vel) {
        pos.value += vel.value * dt;
    });
}

void world::prepareRenderer(RenderBridge& rb) {
    auto view = m_registry.view<Position, RenderData>();
    
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& rd  = view.get<RenderData>(entity);
        
        rb.drawCell(pos.value, rd.radius, rd.type, rd.color);
    }
}