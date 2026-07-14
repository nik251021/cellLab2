#pragma once

#include <physics/world.hpp>
#include <bridge/renderBridge.hpp>

class worldApi
{
private:
    world curWorld;
public:
    worldApi() : curWorld("world1.json") {}
    
    void spawnCell(
        float posX, float posY, 
        float velocityX, float velocityY
    ) {
        curWorld.spawnCell(glm::vec2(posX, posY), glm::vec2(velocityX, velocityY));
    }
    void update(float dt, RenderBridge& rb) {
        curWorld.update(dt);
        curWorld.prepareRenderer(rb);
    }
};