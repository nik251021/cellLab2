#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class window
{
private:
    GLFWwindow* Window;
public:
    window(int width, int height, std::string name);

    bool shouldClose();

    bool isKeyPressed(int keyCode);
    bool isMouseButtonPressed(int buttonCode);
    glm::vec2 getMousePos();
    
    void swapBuffers() {
        if (this->Window) {
            glfwSwapBuffers(this->Window);
        }
    }

    void pollEvents();

    void shutDown() {
        glfwTerminate();
    }
};