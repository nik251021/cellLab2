#pragma once
#include <glm/glm.hpp>
#include <string>

#include <physics/world.hpp>
#include <bridge/renderBridge.hpp>

class worldApi
{
private:
    world curWorld;
public:
    worldApi() : curWorld("world1.json") {}
    
    void spawnCell(
        std::string name,
        float posX, float posY, 
        float velocityX, float velocityY,
        glm::vec4 color
    ) {
        curWorld.spawnCell(
            name, 
            glm::vec2(posX, posY), 
            glm::vec2(velocityX, velocityY),
            color
        );
    }
    void update(float dt, RenderBridge& rb) {
        curWorld.update(dt);
        curWorld.prepareRenderer(rb);
    }
};