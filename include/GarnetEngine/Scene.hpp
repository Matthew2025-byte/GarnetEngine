#pragma once
#include "Registry.hpp"
#include "Renderer.hpp"

namespace Garnet {
    class Scene {
    public:
        void save(const std::string& path) const;
        
        template <typename... Components>
        void bind(void (*func)(float, Entity, Components&...)) {
            callbacks.emplace_back([this, func](float dt) {
                registry.each<Components...>([func, dt](Entity e, Components&... c) {
                    func(dt, e, c...);
                });
            });
        }
        void update(float dt); // forwards to all bound methods
        Registry& getRegistry() { return this->registry; }

        template <typename T>
        void addComponent(Entity entity, const T& component) {
            registry.addComponent<T>(entity, component);
        }

    private:
        Registry registry;
        std::vector<std::function<void(float)>> callbacks;
    };
}