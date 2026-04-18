#include "gfx/IndexBuffer.h"
#include <glad/glad.h>


IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {
	glGenBuffers(1, &m_RendererID); // generate GL ID for the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); // everytime we reference GL_ELEMENT_ARRAY_BUFFER it leads back to this ID
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, m_RendererID); // Here we copy the coordinates for the vertices over to the GPU memory
} 

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::bind() const { 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); // Bind GL_ELEMENT_ARRAY_BUFFER to m_RendererID
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind is simply set to 0
}