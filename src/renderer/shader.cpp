#include <renderer/shader.hpp>

bool shader::readShaderFromPath(const fs::path& vertexShader, const fs::path& fragShader) {
    std::string vertexShaderSrc, fragShaderSrc;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexShader);
        fShaderFile.open(fragShader);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexShaderSrc = vShaderStream.str();
        fragShaderSrc = fShaderStream.str();
    }
    catch(std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::cerr << "Paths were:\nVert: " << vertexShader << "\nFrag: " << fragShader << std::endl;
        return false;
    }

    if (!createShader(vertexShaderSrc, fragShaderSrc)) {
        std::cerr << "Error in creating shader" << std::endl;
        return false;
    }
    return true;
}
bool shader::createShader(const std::string& vertexShaderSrc, const std::string& fragShaderSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* CVShaderSrc = vertexShaderSrc.c_str();
    const GLchar* CFshaderSrc = fragShaderSrc.c_str();

    glShaderSource(vertexShader, 1, &CVShaderSrc, nullptr);
    glShaderSource(fragShader, 1, &CFshaderSrc, nullptr);

    glCompileShader(vertexShader);
    glCompileShader(fragShader);

    GLint vertex_compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertex_compiled);
    if (vertex_compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(vertexShader, 1024, &log_length, message);
        return false;
    }

    GLint frag_compiled;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &frag_compiled);
    if (frag_compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(fragShader, 1024, &log_length, message);
        return false;
    }

    this->id = glCreateProgram();
    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragShader);
    glLinkProgram(this->id);

    GLint program_linked;
    glGetProgramiv(this->id, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(this->id, 1024, &log_length, message);
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    return true;
}

void shader::bind() const{
    glUseProgram(this->id);
}
void shader::unbind() const{
    glUseProgram(0);
}

shader::~shader() {
    glDeleteProgram(id);
}