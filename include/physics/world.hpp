#pragma once
#include <string>
#include <entt/entt.hpp>

#include <bridge/renderBridge.hpp>

#include <resourceManager/worldLoader.hpp>

class world
{
private:
    worldSettings curSettings;
    entt::registry m_registry;
public:
    world(std::string worldName);
    void spawnCell(glm::vec2 pos, glm::vec2 vel);
    void update(float dt);
    void prepareRenderer(RenderBridge& rb);
};