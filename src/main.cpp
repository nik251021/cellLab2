#include <iostream>
#include <filesystem>
#include <entt/entt.hpp>

#include <window/window.hpp>
#include <camera/camera.hpp>
#include <renderer/shader.hpp>
#include <core/world/world_settings.hpp>
#include <core/world/world_settings_loader.hpp>

#include <renderer/buffers.hpp> 

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    window curWindow(800, 600, "Cell lab");
    Camera camera(0.0f, 800.0f, 0.0f, 600.0f);

    WorldSettings settings = loadSettings("data/world.json");

    std::cout << "Settings loaded! Gravity: " << settings.gravity.y << std::endl;

    entt::registry registry;

    float lastFrameTime = glfwGetTime();

    while (!curWindow.shouldClose()) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        curWindow.pollEvents();

        // 1. Обновление логики (ECS Systems)
        // Systems::MovementSystem(registry, deltaTime);
        // Systems::PhysicsSystem(registry, deltaTime);

        // 2. Отрисовка
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Здесь позже будет твой RenderSystem, который берет данные из registry
        // и рисует клетки через vao/vbo

        curWindow.swapBuffers();
    }
    
    curWindow.shutDown();
    return 0;
}