#pragma once

#include <vector>

class IndexBuffer {
public:
    IndexBuffer(const std::vector<unsigned int> &data);

    virtual ~IndexBuffer();

    void bind();
    void unbind();

private:
    unsigned int m_id;
};