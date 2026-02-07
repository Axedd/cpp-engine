#pragma once
#include <vector>
#include <stdint.h>
#include <memory>
#include <unordered_map>
#include <typeinfo>

// Registry for handling all the future components avoidig overloading

namespace Engine {
		

	class IComponentPool {
	public:
		virtual ~IComponentPool() = default;
		virtual void remove(uint32_t entity) = 0;
	};

	template<typename T>
	class ComponentPool : public IComponentPool {
	private:
		// Dense: Data is densly grouped for fast iteration
		std::vector<T> m_Components;
		std::vector<uint32_t> m_DenseEntities; // Which entity owns component X?

		// Sparse: Index is EntityID, value of index is Dense-vector
		// Use 0xFFFFFFFF (max uint32) as "empty" placeholder
		std::vector<uint32_t> m_Sparse;
	};

	class Registry {
	public:
		uint32_t create() { m_NextEntity++; }
		
		// Avoids function overloading instead we use Variadic Templates
		template<typename T, typename... Args> 
		T& emplace(uint32_t entity, Args&&... Args) {

		}

		template<typename T>
		T& get(uint32_t entity) {

		}
	
	private:
		uint32_t m_NextEntity = 0;
	};
}