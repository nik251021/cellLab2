#include "rendering/buffers/VAO.hpp"
#include <glad/glad.h>

VertexArray::VertexArray() { glGenVertexArrays(1, &m_rendererID); }
VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_rendererID); }

void VertexArray::bind() const { glBindVertexArray(m_rendererID); }
void VertexArray::unbind() const { glBindVertexArray(0); }

void VertexArray::addBuffer(const VertexBuffer& vb) {
    bind();
    vb.bind();
    // Настройка для простых 2D позиций (2 float'а)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

void VertexArray::setIndexBuffer(const IndexBuffer& ib) {
    bind();
    ib.bind();
}

void VertexArray::addInstanceBuffer(const InstanceBuffer& ib) {
    bind();
    ib.bind();

    size_t stride = sizeof(InstanceData);

    // 1. Позиция (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(InstanceData, position));
    glVertexAttribDivisor(1, 1);

    // 2. Радиус (float)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(InstanceData, radius));
    glVertexAttribDivisor(2, 1);

    // 3. Тип клетки (float)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(InstanceData, cellType));
    glVertexAttribDivisor(3, 1);

    // 4. Цвет (vec4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(InstanceData, color));
    glVertexAttribDivisor(4, 1);
}