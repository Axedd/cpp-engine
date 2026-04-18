#include "gfx/IndexBuffer.h"
#include <glad/glad.h>


IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, m_RendererID);
} 

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}