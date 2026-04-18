#pragma once
#include <cstdint>

class VertexBuffer {
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;
private:
	uint32_t m_RendererID; // OpenGL internal ID
};