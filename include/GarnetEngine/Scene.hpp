#pragma once
#include "Registry.hpp"
#include "Renderer.hpp"

namespace Garnet {
    class Scene {
    public:
        /**
         * @brief Runs all bound methods
         * 
         * @param dt deltaTime for physics updates
         */
        void update(float dt);
        void save(const std::string& path) const;
        
        /**
         * @brief Binds a method to Scene.update(dt)
         * @details Argument types are automatically determined at compile time
         * 
         * @param func Function to bind matching (float dt, Components&...)
         */
        template <typename... Components>
        void bind(void (*func)(float, Entity, Components&...)) {
            callbacks.emplace_back([this, func](float dt) {
                registry.each<Components...>([func, dt](Entity e, Components&... c) {
                    func(dt, e, c...);
                });
            });
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
            registry.addComponent<T>(entity, component);
        }

        /**
         * @brief Gets the internal registry object
         * @returns Reference to Scene->Registry
         */
        Registry& getRegistry() { return registry; }
        const Registry& getRegistry() const { return registry; }

    private:
        Registry registry;
        std::vector<std::function<void(float)>> callbacks;
    };
}