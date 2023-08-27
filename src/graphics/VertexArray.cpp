#include <VertexArray.hpp>
#include <numeric>
#include <cassert>

#include <VertexBuffer.hpp>

void VertexAttributesLayout::add(int size, int type, bool normalized) {
    VertexAttributeElement vae{};

    vae.index = m_vertexArrayElements.size();
    vae.size = size;
    vae.type = [type]{
        switch (type) {
            case GL_FLOAT:
                return GL_FLOAT;
            case GL_UNSIGNED_BYTE:
                return GL_UNSIGNED_BYTE;
            default:
                assert(false && "Unsupported type");
        }
    }();

    vae.normalized = normalized ? GL_TRUE : GL_FALSE;
    vae.offset = offset(vae.index).value();
    m_vertexArrayElements.emplace_back(vae);
}

std::optional<int> VertexAttributesLayout::offset(unsigned int index) const {
    if (index <= m_vertexArrayElements.size()) {
        int retOffset = 0;
        for (auto i = 0u; i < index; i++) {
            retOffset += (m_vertexArrayElements[i].size *  VertexAttributeElement::sizeOfType(m_vertexArrayElements[i].type));
        }
        return {retOffset};
    }
    return std::nullopt;
}

int VertexAttributesLayout::stride() const {
    return std::accumulate(m_vertexArrayElements.begin(), m_vertexArrayElements.end(), 0, [](const size_t sum, const auto& attrib){
        return sum + (attrib.size * VertexAttributeElement::sizeOfType(attrib.type));
    });
}

const std::vector<VertexAttributeElement> &VertexAttributesLayout::getVertexAttributeElements() const {
    return m_vertexArrayElements;
}

int VertexAttributeElement::sizeOfType(unsigned int type) {
    switch (type) {
        case GL_FLOAT:
            return sizeof(float);
        case GL_UNSIGNED_BYTE:
            return sizeof(char);
        default:
            assert(false && "Unsupported type");
    }
}

VertexArray::VertexArray(VertexBuffer &vertexBuffer, const VertexAttributesLayout &vertexAttributes) : m_vertexBuffer{vertexBuffer} {
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
    m_vertexBuffer.bind();

    const auto& vertexAttribElements = vertexAttributes.getVertexAttributeElements();
    for (auto idx = 0u; idx < vertexAttribElements.size(); idx++) {
        glEnableVertexAttribArray(idx);
        const auto& e = vertexAttribElements[idx];
        glVertexAttribPointer(idx, e.size, e.type, e.normalized, vertexAttributes.stride(), (void*)e.offset);
    }
    unbind();
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() {
    glBindVertexArray(m_id);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
    m_vertexBuffer.unbind();
}
