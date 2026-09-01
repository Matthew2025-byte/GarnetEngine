#include "GarnetEngine/Scene.hpp"
#include "GarnetEngine/Components.hpp"

void Garnet::Scene::update(float dt, Registry& registry) {
    for (auto& call : callbacks) {
        call(dt, registry);
    }
}
