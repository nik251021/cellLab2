#pragma once
#include <entt/entt.hpp>
#include "core/cells/components.hpp"

namespace Systems {
    void MovementSystem(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Physics>();
        
        for (auto entity : view) {
            auto& phys = view.get<Physics>(entity);
            
            phys.pos += phys.vel * deltaTime;
        }
    }
}