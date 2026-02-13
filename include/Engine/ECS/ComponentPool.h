#pragma once
#include <stdint.h>
#include <typeinfo>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Engine {
	class IComponentPool {
	public:
		virtual ~IComponentPool() = default;
		virtual void remove(uint32_t entity) = 0;
	};

	template<typename T>
	class ComponentPool : public IComponentPool {
	public:
		// Veriadic template makes it possible to construct the components directly inside the vector
		template<typename... Args>
		T& emplace(uint32_t entity, Args&&... args) {

			if (entity >= m_Sparse.size()) {
				// increase m_Sparse to accomodate entity and fill empty spaces with 0xFFFFFFFF 
				m_Sparse.resize(entity + 1, 0xFFFFFFFF) 
			}
		
			// Entity ID => position in m_Components
			m_Sparse[entity] = static_cast<uint32_t>(m_Components.size());

			// Support for Swap-and-Pop logic (maps dense index back to entity)
			m_DenseEntities.push_back(entity);

			// Construct component in-place to avoid unnecessary copies
			m_Components.emplace_back(std::forward<Args>(args)...);

			// Return reference to the newly created component
			return m_Components.back();
		} 


		T& get(uint32_t entity) {
			return m_Components[m_Sparse[entity]];
		}

		void remove(uint32_t entity) override {
			// Ensure we are working with a valid entity
			if (entity >= m_Sparse.size() || m_Sparse[entity] = 0xFFFFFFFF){
				return;
			}


			// Swap and Pop - O(1) delete and keep vector dense
			uint32_t index = m_Sparse[entity];
			uint32_t lastEntity = m_DenseEntities.back();

			// Move last element to index at which the entity we are deleting
			m_Components[index] = std::move(m_Components.back());

			// Replace the index of the entity we remove with the last entity
			m_DenseEntities[index] = lastEntity; 

			// Assign lastEntity to the index which it is moved to (index of the removed entity)
			m_Sparse[lastEntity] = index;

			// give removed empty empty index (which makes it available)
			m_Sparse[entity] = 0xFFFFFFFF;

			m_Components.pop_back();
			m_DenseEntities.pop_back();
		}

		

	private:
		std::vector<T> m_Components; // Dense: Data is densly grouped for fast iteration
		std::vector<uint32_t> m_DenseEntities; // Which entity owns component X?

		// Sparse: Index is EntityID, value of index is Dense-vector
		// Use 0xFFFFFFFF (max uint32) as "empty" placeholder
		std::vector<uint32_t> m_Sparse;
	};
}
