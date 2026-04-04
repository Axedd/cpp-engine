#pragma once
#include "Registry.h"


namespace ECS {

	class Entity {
	public:
		Entity() = default;
		Entity(uint32_t id, Registry* registry)
			: m_Id(id), m_Registry(registry) { }


		template<typename T, typename...Args>
		T& add(Args&&... args) {
			return m_Registry->emplace<T>(m_Id, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get() {
			return m_Registry->get<T>(m_Id);
		}

		template<typename T>
		void remove() {
			m_Registry->remove<T>(m_Id);
		}

		operator uint32_t() const { return m_Id; }

		bool isValid() const { return m_Id != 0xFFFFFFFF && m_Registry != nullptr; }

	private:
		uint32_t m_Id = 0xFFFFFFFF;
		Registry* m_Registry = nullptr;
	};
}