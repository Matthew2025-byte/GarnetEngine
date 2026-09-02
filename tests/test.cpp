#include <GarnetEngine/Registry.hpp>
#include <doctest/doctest.h>
#include <algorithm>

struct health { int h = 100; };
struct pos { int x=0; int y=0; };

TEST_CASE("Test Registry") {
    Garnet::Registry registry;
    Garnet::Entity e1 = registry.createEntity();
    Garnet::Entity e2 = registry.createEntity();
    Garnet::Entity e3 = registry.createEntity();

    registry.addComponent<health>(e1);
    registry.addComponent<health>(e2, {50});
    registry.addComponent<health>(e3);

    registry.addComponent<pos>(e1);
    registry.addComponent<pos>(e2);

    SUBCASE("Check createEntity") {
        CHECK_EQ(e1, 0);
        CHECK_EQ(e2, 1);
        CHECK_EQ(e3, 2);
    }
    SUBCASE("Check addComponent") {
        CHECK(registry.getComponent<health>(e1).h == 100);
        CHECK(registry.getComponent<health>(e2).h == 50);
    }
    

    SUBCASE("Check hasComponent") {
        CHECK(registry.hasComponent<pos>(e1));
        CHECK_FALSE(registry.hasComponent<pos>(e3));
    }
    SUBCASE("Find the smallest pool") {
        CHECK_EQ(registry.findSmallestPool<health, pos>(), std::type_index(typeid(pos)));
    }

    SUBCASE("Get entities with pos") {
        auto entities = registry.getEntities<pos>();
        CHECK(std::ranges::contains(entities, e1));
        CHECK(std::ranges::contains(entities, e2));
    }

    SUBCASE("Batch component modification") {
        registry.each<pos>([](Garnet::Entity _, pos& p){
            p.x += 1;
        });

        CHECK_EQ(registry.getComponent<pos>(e1).x, 1);
        CHECK_EQ(registry.getComponent<pos>(e2).x, 1);
    }
}