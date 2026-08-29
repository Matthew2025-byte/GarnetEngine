#pragma once
#include <unordered_map>
#include <typeindex>
#include <any>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <limits>
#include <tuple>
#include <algorithm>


namespace Garnet {
    using Entity = uint32_t;

    template <typename T>
    struct TypeTag { using type = T; };
    // Component Pool
    /**
     * @brief relationship between entities and components
     * @tparam T The type of component to store
     */
    template <typename T>
    class ComponentPool {
        std::vector<Entity> entities;
        std::vector<T> components;

        public:
        /**
         * @brief Adds a new component to an entity
         * 
         * @param entity Entity to add a component to
         * @param component Component data to add
         * @throws std::runtime error if 
         */
        void add(Entity entity, const T& component) {
            if (contains(entity)) {
                throw std::runtime_error("Entity already has this component");
            }
            entities.push_back(entity);
            components.push_back(component);
        }

        /**
         * @brief Removes an entity from the component pool
         * 
         * @param entity Entity to remove from the component pool
         * @returns True if removed, false if the entity wasn't present
         */
        bool remove(const Entity entity) {
            for (size_t i = 0; i < entities.size(); i++) {
                if (entity == entities[i]) {
                    entities[i] = entities.back();
                    components[i] = components.back();
                    entities.pop_back();
                    components.pop_back();
                    return true;
                }
            }
            return false;
        }
        
        /**
         * @brief Checks if an entity is in the component pool
         * @param entity Entity to check
         * @returns True if present
         */
        bool contains(Entity entity) const {
            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i] == entity) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Provides a reference to the component associated with an entity
         * 
         * @param entity Entity to get a component from
         * @returns A reference to a component
         * @throws std::runtime_error if the entity does not have the component
         */
        T& get(Entity entity) {
            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i] == entity) {
                    return components[i];
                }
            }
            throw std::runtime_error("Entity does not have this component");
        }

        /**
         * @brief Get the size of the pool
         * 
         * @return Integer value representing the number of valid entities
         */
        size_t size() {
            return entities.size();
        }
    
        /**
         * @brief Executes a provided method on all entries in the component pool
         * 
         * @tparam Func Callable type
         * @param func Function called on the component pool.  Must accept an Entity and Component reference
         */
        template <typename Func>
        void each(Func&& func) {
            for (size_t i = 0; i < entities.size(); i++) {
                func(entities[i], components[i]);
            }
        }
    };

    class Registry {
        std::unordered_map<std::type_index, std::any> componentArray;
        std::vector<std::function<void(Registry&, Entity)>> componentRemovers; 
        Entity entityIndex = 0;

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
         */
        template <typename T>
        void addComponent(Entity entity, const T& component) {
            auto it = componentArray.find(typeid(T));

            if (it == componentArray.end()) {
                componentArray[typeid(T)] = ComponentPool<T>();
                componentRemovers.push_back([](Registry& registry, Entity entity) {
                    registry.getComponents<T>().remove(entity);
                });
            }
            getComponents<T>().add(entity, component);
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
         *
         * @throws std::runtime_error If the entity already has a component
         *         of type T.
         *
         * @note T must be default-constructible.
         */
        template<typename T>
        void addComponent(Entity entity)
        {
            addComponent(entity, T{});
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
            auto it = componentArray.find(typeid(T));
                
            if (it == componentArray.end()) {
                throw std::runtime_error("Pool does not exist");
            }
            return std::any_cast<ComponentPool<T>&>(it->second);
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
            auto it = componentArray.find(typeid(T));
            if (it == componentArray.end()) {
                return false;
            }
            return std::any_cast<ComponentPool<T>&>(it->second).contains(entity);
        }

        /**
         * @brief Retrieves a component reference for the associated entity
         * 
         * @tparam T Type of component to retrieve
         * @param entity Entity to retrieve from
         * @returns The requested component
         * @throws std::runtime_error if the pool doesn't exist or if the entity does not possess the entity
         */
        template <typename T>
        T& getComponent(Entity entity) {
            return getComponents<T>().get(entity);
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
            int smallest_size = std::numeric_limits<int>::max(); // Matches your int return type

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
        template<typename Primary, typename... Components, typename Func>
        void each(Func&& func) {
            auto& pool = getComponents<Primary>();
            pool.each([&](Entity entity, Primary& primary) {
                if ((hasComponent<Components>(entity) && ...)) {
                    func(entity, primary, getComponent<Components>(entity)...);
                }
            });
        }
    };
}