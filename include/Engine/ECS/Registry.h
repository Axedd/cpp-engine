#pragma once
#include "ComponentPool.h"

// Registry for handling all the future components avoidig overloading
// Coordinates all the pools

namespace Engine {

	class Registry {
	public:
		// Create unique Entity ID
		uint32_t create() {
			return m_NextEntity++;
		}

		// Add component to entity
		template<typename T, typename... Args>
		T& emplace(uint32_t entity, Args&&... args) {
			return getPool<T>()->emplace(entity, std::forward<Args>(args)...);
		}

		// Get component from an Entity
		template<typename T>
		T& get(uint32_t entity) {
			return getPool<T>()->get(entity);
		}

		// Remove a specific component from the entity
		template<typename T>
		void remove(uint32_t entity) {
			getPool<T>()->remove(entity);
		}
		
	
	private:
		template<typename T>
		ComponentPool<T>* getPool() {
			size_t typeHash = typeid(T).hash_code();

			// If pool doesn't exists for entity we create one
			if (m_ComponentPools.find(typeHash) == m_ComponentPools.end()) {
				m_ComponentPools[typeHash] = std::make_unique <ComponentPool<T>>();
			}

			// Type cast general IComponentPool to the specific ComponentPool
			return static_cast<ComponentPool<T>*>(m_ComponentPools[typeHash].get());
		}


		uint32_t m_NextEntity = 0;

		// stores all pools - unique_ptr makes sure the automatic cleaning is in-place
		std::unordered_map<size_t, std::unique_ptr<IComponentPool>> m_ComponentPools;
	};
}