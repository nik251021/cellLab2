#include <iostream>
#include <filesystem>

#include <window/window.hpp>
#include <renderer/cells_renderer.hpp>

//Test includes: 
#include <renderer/shader.hpp>
#include <renderer/buffers.hpp>

//Main game cycle and delta time

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    window curWindow(800, 600, "Cell lab");

    float vertices[] = {
    -0.5f, -0.5f, 
     0.5f, -0.5f, 
     0.0f,  0.5f
    };

    unsigned int indices[] = { 0, 1, 2 };

    shader TestShader(fs::path("data/shaders/basic.vert"), fs::path("data/shaders/basic.frag"));

    vertexArrayBuffer vao;
    vertexBuffer vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
    indexBuffer  ebo(indices, 3);

    vao.addBuffer(vbo, 0, 2, 2 * sizeof(float), (void*)0);

    while (!curWindow.shouldClose()) {
        curWindow.pollEvents();

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        TestShader.bind();
        vao.bind();
        ebo.bind();

        glDrawElements(GL_TRIANGLES, ebo.getCount(), GL_UNSIGNED_INT, nullptr);

        curWindow.swapBuffers();
    }
    
    curWindow.shutDown();
    return 0;
}