#include <GarnetEngine/Registry.hpp>
#include <doctest/doctest.h>

struct health { int h = 100; };
struct pos { int x=0; int y=0; };

TEST_CASE("Test Registry") {
    Garnet::Registry registry;
    Garnet::Entity e1 = registry.createEntity();
    Garnet::Entity e2 = registry.createEntity();
    Garnet::Entity e3 = registry.createEntity();

    registry.addComponent<health>(e1);
    registry.addComponent<health>(e2);
    registry.addComponent<health>(e3);

    registry.addComponent<pos>(e1);
    registry.addComponent<pos>(e2);

    CHECK(registry.findSmallestPool<health, pos>() == std::type_index(typeid(pos)));
}