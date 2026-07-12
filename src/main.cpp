#include <iostream>

#include <window/window.hpp>
#include <renderer/cells_renderer.hpp>

//Main game cycle and delta time
int main(int argc, char* argv[]) {
    window curWindow(800, 600, "Cell lab");

    while (!curWindow.shouldClose()) {
        curWindow.pollEvents();

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        curWindow.swapBuffers();
    }
    
    curWindow.shutDown();
    return 0;
}