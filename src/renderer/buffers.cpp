#include <renderer/buffers.hpp>

//vertexBuffer:
vertexBuffer::vertexBuffer(const void* data, unsigned int size, GLenum usage) {
    glGenBuffers(1, &this->id);

    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void vertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void vertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vertexBuffer::updateData(const void* data, unsigned int size) {
    bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

vertexBuffer::~vertexBuffer() {
    glDeleteBuffers(1, &this->id);
}
//indexBuffer:
indexBuffer::indexBuffer(const unsigned int* data, unsigned int count) 
    : m_Count(count)
{
    glGenBuffers(1, &this->id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void indexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void indexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

indexBuffer::~indexBuffer() {
    glDeleteBuffers(1, &this->id);
}
//vertexArrayBuffer:
vertexArrayBuffer::vertexArrayBuffer() {
    glGenVertexArrays(1, &this->id);
}

void vertexArrayBuffer::bind() const {
    glBindVertexArray(this->id);
}

void vertexArrayBuffer::unbind() const {
    glBindVertexArray(0);
}

void vertexArrayBuffer::addBuffer(const vertexBuffer& vb, GLuint index, GLint size, GLsizei stride, const void* offset) {
    bind();
    vb.bind();
    
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, offset);
}

vertexArrayBuffer::~vertexArrayBuffer() {
    glDeleteVertexArrays(1, &this->id);
}