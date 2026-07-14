#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "rendering/window.hpp"
#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"

#include "physics/worldApi.hpp"

//test includes:
#include "bridge/renderBridge.hpp"

void handleCameraInput(Camera& camera, GLFWwindow* window, float dt) {
    float speed = 500.0f / camera.getZoom(); 
    glm::vec2 pos = camera.getPosition();

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  pos.x -= speed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  pos.x += speed * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  pos.y += speed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  pos.y -= speed * dt;

    camera.setPosition(pos);
}

static Camera* g_camera = nullptr;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!g_camera) return;

    float zoom = g_camera->getZoom();
    
    if (yoffset > 0) {
        zoom *= 1.1f;
    } else if (yoffset < 0) {
        zoom /= 1.1f;
    }

    zoom = glm::clamp(zoom, 0.1f, 10.0f);
    g_camera->setZoom(zoom);
}

int main() {
    float lastTime = glfwGetTime();

    Window window(1000, 1000, "Cell Lab");
    Camera camera(1000.0f, 1000.0f);

    g_camera = &camera;
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);

    Renderer renderer;
    RenderBridge renderBridge;
    worldApi world;

    world.spawnCell(250, 0, 1, 1);

    while (!window.shouldClose()) {
        //delta time
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        //
        handleCameraInput(camera, window.getNativeWindow(), deltaTime);

        world.update(deltaTime, renderBridge);

        renderer.beginScene(camera);
        renderer.drawCells(renderBridge.getQueue());
        renderer.endScene();

        renderBridge.clear();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}