#pragma once

#include <vector>

class VertexBuffer {
public:
    VertexBuffer(const std::vector<float>& data);

    virtual ~VertexBuffer();

    void bind();
    void unbind();

private:
    unsigned int m_id;
};