/**
 * @file Registry.hpp
 * @author Matthew2025-byte
 * @brief Contains Registry, Garnets ECS controller
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <vector>
#include <array>
#include <memory>
#include <limits>
#include <cstdint>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <span>

namespace Garnet {
using Entity = uint32_t;

template <typename T>
struct TypeTag {
	using type = T;
};

struct IComponentPool {
	virtual ~IComponentPool() = default;
	virtual std::unique_ptr<IComponentPool> clone() const = 0;
};

/**
 * @brief relationship between entities and components
 * @tparam T The type of component to store
 */
template <typename T, size_t PageSize = 1024>
class ComponentPool : public IComponentPool {
	public:
	/**
	 * @brief Checks if an entity is in the component pool
	 * @param entity Entity to check
	 * @returns True if present
	 */
	bool contains(Entity e) const {
		return e < sparse.size() && sparse[e] != INVALID;
	}

	/**
	 * @brief Provides a reference to the component associated with an entity
	 *
	 * @param entity Entity to get a component from
	 * @returns A reference to a component
	 */
	T& get(Entity e) {
		assert(contains(e));
		size_t pos = sparse[e];
		return (*data[pos / PageSize])[pos % PageSize];
	}

	T* tryGet(Entity e) {
		if (e <= sparse.size()) return nullptr;
		size_t pos = sparse[e];
		if (pos == INVALID) return nullptr;
		return &(*data[pos / PageSize])[pos % PageSize];
	}
	/**
	 * @brief Adds a new component to an entity
	 *
	 * @param entity Entity to add a component to
	 * @param component Component data to add
	 * @returns A reference to the added component
	 */
	T& add(Entity e, T value) {
		if (e >= sparse.size())
			sparse.resize(e + 1, INVALID);

		if (sparse[e] != INVALID) {
			size_t pos = sparse[e];
			(*data[pos / PageSize])[pos % PageSize] = std::move(value);
			return (*data[pos / PageSize])[pos % PageSize];
		}
		size_t pos = count;
		size_t page = pos / PageSize;
		size_t slot = pos % PageSize;

		if (page >= dense.size()) {
			dense.resize(page + 1);
			data.resize(page + 1);
		}
		if (!dense[page]) {
			dense[page] = std::make_unique<std::array<Entity, PageSize>>();
			data[page] = std::make_unique<std::array<T, PageSize>>();
		}

		(*dense[page])[slot] = e;
		(*data[page])[slot] = std::move(value);
		sparse[e] = static_cast<uint32_t>(pos);
		++count;

		return (*data[page])[slot];
	}
	
	/**
	 * @brief Removes an entity from the component pool
	 *
	 * @param entity Entity to remove from the component pool
	 */
	void remove(Entity e) {
		assert(contains(e));
		size_t pos = sparse[e];
		size_t lastPos = count - 1;

		size_t page = pos / PageSize;
		size_t slot = pos % PageSize;
		size_t lastPage = lastPos / PageSize;
		size_t lastSlot = lastPos % PageSize;

		Entity lastEntity = (*dense[lastPage])[lastSlot];

		(*dense[page])[slot] = lastEntity;
		(*data[page])[slot] = std::move((*data[lastPage])[lastSlot]);

		sparse[lastEntity] = static_cast<uint32_t>(pos);
		sparse[e] = INVALID;

		--count;
	}

	/**
	 * @brief Gets all registered entities
	 *
	 * @return std::vector<Entity> of all assigned entities
	 */
	const std::vector<Entity> getEntities() const {
		std::vector<Entity> result;
		result.reserve(count);
		for (size_t pos = 0; pos < count; pos += PageSize) {
			size_t page = pos / PageSize;
			size_t chunk = std::min(PageSize, count - pos);
			auto& arr = *dense[page];
			result.append_range(std::span(arr.data(), chunk));
		}
		return result;
	}

	/**
	 * @brief Get the size of the pool
	 *
	 * @return Integer value representing the number of valid entities
	 */
	size_t size() const { return count; }
	
	/**
	 * @brief Executes a provided method on all entries in the component pool
	 *
	 * @tparam Func Callable type
	 * @param func Function called on the component pool.  Must accept an Entity and Component
	 * reference
	 */
	template <typename Func>
	void each(Func func) {
		for (size_t pos = 0; pos < count; ++pos) {
			size_t page = pos / PageSize;
			size_t slot = pos % PageSize;
			const Entity& e = (*dense[page])[slot];
			func(e, (*data[page])[slot]);
		}
	}

	ComponentPool() = default;
	ComponentPool(const ComponentPool& other)
		: sparse(other.sparse), count(other.count) {
			dense.reserve(other.dense.size());
			for (auto& page : other.dense)
				dense.push_back(page ? std::make_unique<std::array<Entity, PageSize>>(*page) : nullptr);
			data.reserve(other.data.size());
			for (auto& page : other.data)
				data.push_back(page ? std::make_unique<std::array<T, PageSize>>(*page) : nullptr);
		}
	ComponentPool(ComponentPool&&) = default;
	ComponentPool& operator=(ComponentPool&&) = default;
	ComponentPool& operator=(const ComponentPool&) = delete;

	std::unique_ptr<IComponentPool> clone() const override {
		return std::make_unique<ComponentPool<T, PageSize>>(*this);
	}

	private:
	static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();
	/**
	 * @brief Contains entity locations in the dense array
	 * @details Vector that contains every single entity in the pool, index equal to entity id and value reflecting position in the dense array
	 */
	std::vector<uint32_t> sparse;
	/**
	 * @brief Array of entity ids
	 * @details Allows for O(1) lookup for entity ownership of a component
	 */
	std::vector<std::unique_ptr<std::array<Entity, PageSize>>> dense;
	/**
	 * @brief Array of components
	 */
	std::vector<std::unique_ptr<std::array<T, PageSize>>> data;
	size_t count = 0;
};

class Registry {
	public:
	/**
	 * @brief Creates a unique entity id
	 * @returns Entity (uint32_t) ID
	 * @throws std::overflow_error if the Entity ID limit is exceeded
	 */
	Entity createEntity() {
		if (entityIndex == std::numeric_limits<Entity>::max()) {
			throw std::overflow_error("Entity ID limit exceeded");
		}
		return entityIndex++;
	}

	/**
	 * @brief Adds an Entity/Component pair to a Component Pool
	 *
	 * If the component pool does not exist is is automatically
	 * created and an entity remover is registered
	 *
	 * @tparam T Type of component
	 * @param entity Entity to assign to
	 * @param component Reference to the component to store
	 * @returns A reference to the added component
	 */
	template <typename T>
	T& addComponent(Entity entity, const T& component) {
		return getComponents<T>().add(entity, component);
	}

	/**
	 * @brief Adds a default-constructed component to an entity.
	 *
	 * Convenience overload for component types that do not require
	 * initialization data, such as tag or marker components.
	 *
	 * Internally forwards to the primary addComponent(Entity, const T&)
	 * overload using a default-constructed instance of T.
	 *
	 * @tparam T The component type to add.
	 * @param entity The entity that will receive the component.
	 * @returns A reference to the added component
	 *
	 * @throws std::runtime_error If the entity already has a component
	 *         of type T.
	 * @warning Returned reference is only valid until another component of the same type is added
	 * to the registry.
	 * @note T must be default-constructible.
	 */
	template <typename T>
	T& addComponent(Entity entity) {
		return addComponent(entity, T{});
	}

	/**
	 * @brief Adds several default initialized components to an entity
	 *
	 * @tparam Components List of components to add
	 * @param entity Entity to add them to
	 * @returns An std::tuple containing references to the created components
	 */
	template <typename... Components>
	std::tuple<Components&...> addComponents(Entity entity) {
		return std::tie(this->addComponent<Components>(entity)...);
	}

	/**
	 * @brief Provides a reference to a component pool
	 *
	 * @tparam T Component type to retrieve
	 * @returns A Component Pool reference
	 * @throws std::runtime_error if the pool does not exist
	 */
	template <typename T>
	ComponentPool<T>& getComponents() {
		size_t id = componentTypeId<T>();
		if (id >= componentArray.size())
			componentArray.resize(id + 1);
		if (!componentArray[id]) {
			componentArray[id] = std::make_unique<ComponentPool<T>>();
			componentRemovers.push_back([](Registry& registry, Entity entity) {
				if (registry.hasComponent<T>(entity))
					registry.getComponents<T>().remove(entity);
			});
		}
		return *static_cast<ComponentPool<T>*>(componentArray[id].get());
	}

	/**
	 * @brief Retrieves a component reference for the associated entity
	 *
	 * @tparam T Type of component to retrieve
	 * @param entity Entity to retrieve from
	 * @returns The requested component
	 * @throws std::runtime_error if the pool doesn't exist or if the entity does not possess the
	 * entity
	 */
	template <typename T>
	T& getComponent(Entity entity) {
		return getComponents<T>().get(entity);
	}

	/**
	 * @brief Get all entities that match the given component filter
	 *
	 * @tparam Components Components to filter from
	 * @return Vector of entities that match the filter
	 */
	template <typename... Components>
	std::vector<Entity> getEntities() {
		static_assert(sizeof...(Components) > 0, "Need at least one component type");

		using Primary = std::tuple_element_t<0, std::tuple<Components...>>;

		std::vector<Entity> entities;
		ComponentPool<Primary>& pool = getComponents<Primary>();
		for (auto& entity : pool.getEntities()) {
			if ((hasComponent<Components>(entity) && ...)) {
				entities.push_back(entity);
			}
		}
		return entities;
	}

	/**
	 * @brief Checks if an Entity has an associated component
	 *
	 * @tparam T Component type to check
	 * @param entity Entity to check
	 * @returns True if component exists
	 */
	template <typename T>
	bool hasComponent(Entity entity) {
		size_t id = componentTypeId<T>();
		if (id >= componentArray.size() || !componentArray[id])
			return false;
		return static_cast<ComponentPool<T>*>(componentArray[id].get())->contains(entity);
	}

	/**
	 * @brief Removes an entity from a specific component pool
	 *
	 * @tparam T Component Pool to remove from
	 * @param entity Entity to remove
	 */
	template <typename T>
	void removeComponent(Entity entity) {
		getComponents<T>().remove(entity);
	}

	/**
	 * @brief Removes an entity from the registry
	 *
	 * @param entity Entity to remove
	 */
	void removeEntity(Entity entity) {
		for (auto& remove : this->componentRemovers) {
			remove(*this, entity);
		}
	}

	/**
	 * @brief Finds the smallest available component pool
	 *
	 * @tparam Components Component pools to check
	 * @return Smallest pool available
	 */
	template <typename... Components>
	std::type_index findSmallestPool() {
		static_assert(sizeof...(Components) > 0, "Must have at least one type");

		std::type_index smallest_type = typeid(void);
		int smallest_size = std::numeric_limits<int>::max();  // Matches your int return type

		auto check_smallest = [&](auto tag) {
			using Component = typename decltype(tag)::type;

			int currentSize = this->template getComponents<Component>().size();

			if (currentSize < smallest_size) {
				smallest_size = currentSize;
				smallest_type = typeid(Component);
			}
		};

		(check_smallest(TypeTag<Components>{}), ...);

		return smallest_type;
	}

	/**
	 * @brief Sorts pools from smallest to biggest
	 *
	 * Creates a vector of the provided components ordered smallest to largest
	 * based on the number of entities each contains
	 *
	 * @tparam Components Components to check
	 * @return A vector of the given components ordered smallest to largest
	 */
	template <typename... Components>
	std::vector<std::type_index> sortPools() {
		static_assert(sizeof...(Components) > 0, "Must have at least once type");

		std::vector<std::pair<std::type_index, size_t>> pools;
		pools.reserve(sizeof...(Components));

		auto collect = [&](auto tag) {
			using Component = typename decltype(tag)::type;
			size_t size = this->template getComponents<Component>().size();
			pools.emplace_back(typeid(Component), size);
		};

		(collect(TypeTag<Components>{}), ...);

		std::sort(pools.begin(), pools.end(),
				  [](const auto& a, const auto& b) { return a.second < b.second; });

		std::vector<std::type_index> result;
		result.reserve(pools.size());
		for (auto& [type, size] : pools) {
			result.push_back(type);
		}

		return result;
	}

	/**
	 * @brief Calls a method on all entities in the registry that match a filter
	 *
	 * @tparam Primary Component type whose pool is used for iteration
	 * @tparam Components Additional components for filtering
	 * @param func callback run on the entities in the filter
	 *
	 * @code
	registry.each<Transform, Velocity>([](Entity entity, Transform& transform, Velocity& velocity) {
		transform += velocity;
	});
	 * @endcode
	 */
	template <typename Primary, typename... Components, typename Func>
	void each(Func&& func) {
		auto& pool = getComponents<Primary>();
		pool.each([&](Entity entity, Primary& primary) {
			if ((hasComponent<Components>(entity) && ...)) {
				func(entity, primary, getComponent<Components>(entity)...);
			}
		});
	}


	Registry() = default;
	Registry(const Registry& other) : entityIndex(other.entityIndex), typeToId(other.typeToId) {
		componentArray.reserve(other.componentArray.size());
		for (auto& pool : other.componentArray)
			componentArray.push_back(pool ? pool->clone() : nullptr);
		componentRemovers = other.componentRemovers;
	}
	Registry& operator=(const Registry& other) {
		if (this == &other) return *this;
		Registry tmp(other);
		std::swap(componentArray, tmp.componentArray);
		std::swap(componentRemovers, tmp.componentRemovers);
		std::swap(typeToId, tmp.typeToId);
		entityIndex = tmp.entityIndex;
		return *this;
	}
	Registry(Registry&&) = default;
	Registry& operator=(Registry&&) = default;
	private:
	std::vector<std::unique_ptr<IComponentPool>> componentArray;
	std::vector<std::function<void(Registry&, Entity)>> componentRemovers;
	Entity entityIndex = 0;
	std::unordered_map<std::type_index, std::size_t> typeToId;

	template <typename T>
	std::size_t componentTypeId() {
		auto it = typeToId.find(std::type_index(typeid(T)));
		if (it != typeToId.end())
			return it->second;
		std::size_t id = componentArray.size();
		typeToId[std::type_index(typeid(T))] = id;
		componentArray.emplace_back(nullptr);
		return id;
	}
};
}  // namespace Garnet