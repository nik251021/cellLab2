#include <window/window.hpp>

window::window(int width, int height, std::string name) {
    if (!glfwInit()) {
        std::cerr << "Error in glfwInit" << std::endl;
        glfwTerminate();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    this->Window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (this->Window == nullptr) {
        std::cerr << "Can't initialize window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(this->Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD Init Failed" << std::endl;
        glfwTerminate();
    }
    glViewport(0, 0, width, height);

    std::cout << "Window was created with openGL context: " << glGetString(GL_VERSION) << std::endl;
}

bool window::shouldClose() {
    return(glfwWindowShouldClose(this->Window));
}

bool window::isKeyPressed(int keyCode) {
    int state = glfwGetKey(this->Window, keyCode);
    return state == GLFW_PRESS | state == GLFW_REPEAT;
};
bool window::isMouseButtonPressed(int buttonCode) {
    int state = glfwGetMouseButton(this->Window, buttonCode);
    return state == GLFW_PRESS | state == GLFW_REPEAT;
}
glm::vec2 window::getMousePos() {
    double xpos, ypos;
    glfwGetCursorPos(this->Window, &xpos, &ypos);

    return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void window::pollEvents() {
    glfwPollEvents();
}