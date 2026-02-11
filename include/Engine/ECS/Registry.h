#pragma once
#include "ComponentPool.h"

// Registry for handling all the future components avoidig overloading

namespace Engine {
	

	class Registry {
	public:
		uint32_t create() { m_NextEntity++; }
		
	
	private:
		uint32_t m_NextEntity = 0;
	};
}