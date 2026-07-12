#pragma once

#include <glad/glad.h>

class vertexBuffer
{
private:

public:
    GLuint id;
    vertexBuffer(const void* data, unsigned int size, GLenum usage);

    void bind() const;
    void unbind() const;

    void updateData(const void* data, unsigned int size);

    ~vertexBuffer();
};

class indexBuffer {
private:
    GLuint id = 0;
    unsigned int m_Count = 0;
public:
    indexBuffer(const unsigned int* data, unsigned int count);
    ~indexBuffer();

    void bind() const;
    void unbind() const;
    
    inline unsigned int getCount() const { return m_Count; }
};

class vertexArrayBuffer
{
private:
    GLuint id = 0;
public:
    vertexArrayBuffer();
    ~vertexArrayBuffer();

    void bind() const;
    void unbind() const;

    void addBuffer(const vertexBuffer& vb, GLuint index, GLint size, GLsizei stride, const void* offset);
};