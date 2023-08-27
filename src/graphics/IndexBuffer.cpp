#include <IndexBuffer.hpp>

#include <glad/glad.h>

IndexBuffer::IndexBuffer(const std::vector<unsigned int> &data) {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * data.size(), data.data(), GL_STATIC_DRAW);
    unbind();
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
