#pragma once
#include "ComponentPool.h"

// Registry for handling all the future components avoidig overloading
// Coordinates all the pools

namespace ECS {

	class Registry;

	class Entity {
	public:
		Entity(uint32_t id, Registry* registry) : m_Id(id), m_Registry(registry) {}
		Entity() : m_Id(0xFFFFFFFF), m_Registry(nullptr) {}

		template<typename T, typename... Args>
		T& add(Args&&... args);

		template<typename T>
		T& get();

		uint32_t getId() const { return m_Id; }
		operator uint32_t() const { return m_Id; }

	private:
		uint32_t m_Id;
		Registry* m_Registry;
	};

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

		Entity createEntity() {
			uint32_t id = create();
			return Entity(id, this);
		}

		void clear() {
			m_ComponentPools.clear();
			m_NextEntity = 0;
		}

		void destroy(uint32_t entity) {
			// remove entity from all pools
			for (auto const& [type, pool] : m_ComponentPools) {
				pool->remove(entity);
			}
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

		template<typename T>
		bool has(uint32_t entity) {
			size_t typeHash = typeid(T).hash_code();
			if (m_ComponentPools.find(typeHash) == m_ComponentPools.end()) return false;
			return static_cast<ComponentPool<T>*>(m_ComponentPools[typeHash].get())->has(entity);
		}

		uint32_t m_NextEntity = 0;

		// stores all pools - unique_ptr makes sure the automatic cleaning is in-place
		std::unordered_map<size_t, std::unique_ptr<IComponentPool>> m_ComponentPools;
	};

	// Entity Template Implementations 
	// These must be defined after the Registry class is fully defined

	template<typename T, typename... Args>
	T& Entity::add(Args&&... args) {
		return m_Registry->emplace<T>(m_Id, std::forward<Args>(args)...);
	}

	template<typename T>
	T& Entity::get() {
		return m_Registry->get<T>(m_Id);
	}

}