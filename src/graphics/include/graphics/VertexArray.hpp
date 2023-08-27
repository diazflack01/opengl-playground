#pragma once

#include <vector>
#include <optional>
#include <glad/glad.h>

class VertexBuffer;

struct VertexAttributeElement {
    unsigned int index;
    int size;
    unsigned int type;
    unsigned int normalized;
    long offset;

    static int sizeOfType(unsigned int type);
};

class VertexAttributesLayout {
public:
    void add(int size, int type, bool normalized);

    [[nodiscard]] int stride() const;

    [[nodiscard]] const std::vector<VertexAttributeElement>& getVertexAttributeElements() const;

private:
    [[nodiscard]] std::optional<int> offset(unsigned int index) const;

    std::vector<VertexAttributeElement> m_vertexArrayElements{};
};

class VertexArray {
public:
    VertexArray(VertexBuffer& vertexBuffer, const VertexAttributesLayout& vertexAttributes);
    ~VertexArray();

    void bind();

    void unbind();

private:
    VertexBuffer& m_vertexBuffer;
    unsigned int m_id{0};
};