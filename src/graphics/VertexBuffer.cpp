#include <VertexBuffer.hpp>

#include <glad/glad.h>

VertexBuffer::VertexBuffer(const std::vector<float> &data) {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
    unbind();
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
