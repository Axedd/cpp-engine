#include "gfx/VertexArray.h"
#include <glad/glad.h>


VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::bind() const {
    glBindVertexArray(m_RendererID);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& vb, unsigned int index, int count, int stride, int offset) {
    bind(); // open VAO-folder
    vb.bind(); // choose the buffer with the numbers

    glEnableVertexAttribArray(index); // "Enable row number x in the manual" - analogy

    // Translation
    // index = amount of number per vertix
    // GL_FLOAT = what type of vertices?
    // stride = how far does it need to jump in the row for the next vertix
    // offset = where in the row does the data start we are looking for?
    glVertexAttribPointer(index, count, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
}
