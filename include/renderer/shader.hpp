#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace fs = std::filesystem;

class shader
{
private:
    bool readShaderFromPath(const fs::path& vertexShader, const fs::path& fragShader);
    bool createShader(const std::string& vertexShader, const std::string& fragShader);
public:
    GLuint id;
    
    shader(const fs::path& vertexShader, const fs::path& fragShader) {
        readShaderFromPath(vertexShader, fragShader);
    }
    shader(const std::string& vertexShader, const std::string& fragShader) {
        createShader(vertexShader, fragShader);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        
        glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void bind() const;
    void unbind() const;
    
    ~shader();
};