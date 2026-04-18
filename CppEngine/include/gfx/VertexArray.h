#pragma once
#include "VertexBuffer.h"
#include <cstdint>

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    // index: location in shader, count: numbers amount 
    // stride: jump to the next vertex, offset: starting poibt in the buffer
    void addBuffer(const VertexBuffer& vb, unsigned int index, int count, int stride, int offset);

private:
    uint32_t m_RendererID;
};