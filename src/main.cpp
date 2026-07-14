#include "rendering/window.hpp"
#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"
#include "bridge/instance_data.hpp"
#include "bridge/renderBridge.hpp"
#include <vector>

#include <glm/glm.hpp>

int main() {
    Window window(1000, 1000, "Cell Lab");
    Camera camera(1000.0f, 1000.0f);
    Renderer renderer;

    RenderBridge renderBridge;


    while (!window.shouldClose()) {
        //future phys engine

        //

        //test
        renderBridge.drawCell(
            glm::vec2(0.0f, 0.0f),            // pos
            50.0f,                            // rad
            0.0f,                             // type
            glm::vec4(1.0f, 0.5f, 0.0f, 1.0f) // color
        );
        //

        renderer.beginScene(camera);
        renderer.drawCells(renderBridge.getQueue());
        renderer.endScene();

        renderBridge.clear();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}