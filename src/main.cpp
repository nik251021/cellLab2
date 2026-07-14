#include "rendering/window.hpp"
#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"
#include "bridge/instance_data.hpp"
#include <vector>

#include <glm/glm.hpp>

int main() {
    Window window(1000, 1000, "Cell Lab");
    Camera camera(1000.0f, 1000.0f);
    Renderer renderer;

    std::vector<InstanceData> cells = {
        InstanceData(glm::vec2(250.0f, 250.0f), 50.0f, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
        InstanceData(glm::vec2(500.0f, 500.0f), 50.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
    };

    while (!window.shouldClose()) {
        renderer.beginScene(camera);
        renderer.drawCells(cells);
        renderer.endScene();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}