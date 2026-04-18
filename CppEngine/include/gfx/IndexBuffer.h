#pragma once
#include <cstdint>

class IndexBuffer {
public:
	IndexBuffer(uint32_t* data, int32_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return m_Count; }
private:
	uint32_t m_RendererID;
	int32_t m_Count;
};