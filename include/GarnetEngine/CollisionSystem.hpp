/**
 * @file CollisionSystem.hpp
 * @brief Provides collision detection and resolution for colliders.
 */

#pragma once

#include "Registry.hpp"

#include "TransformComponent.hpp"
#include "ColliderComponent.hpp"
#include "RigidbodyComponent.hpp"

#include "Collision.hpp"
#include "CollisionCheck.hpp"

namespace Garnet {

    /**
     * @brief Detects and resolves collider intersections.
     *
     * The CollisionSystem performs pairwise collision checks
     * on entities containing Transform and Collider
     * components. When a collision is detected, overlapping
     * entities are separated based on their Rigidbody
     * properties.
     *
     * Currently only axis-aligned bounding box collisions
     * are supported.
     */
    class CollisionSystem {

    public:

        /**
         * @brief Processes collisions for all colliders.
         *
         * All collider pairs are tested for intersection.
         * When a collision occurs, penetration is resolved
         * by moving entities apart along the collision normal.
         *
         * Entities without a Rigidbody are treated as static.
         *
         * @param registry ECS registry containing component data.
         *
         * @code
         * CollisionSystem::Update(registry);
         * @endcode
         */
        static void Update(Registry& registry)
        {
            registry.each<
                Components::Transform,
                Components::Collider
            >(
                Entity a,
                Components::Transform & transformA,
                Components::Collider & colliderA
                {
                    registry.each<
                        Components::Transform,
                        Components::Collider
                    >(
                        Entity b,
                            Components::Transform & transformB,
                            Components::Collider & colliderB
                        {
                            if (a >= b) {
                                return;
                            }

                            Collision collision =
                                CheckAABB(
                                    transformA,
                                    colliderA,
                                    transformB,
                                    colliderB
                                );

                            if (!collision.hit) {
                                return;
                            }

                            const bool aHasBody =
                                registry.hasComponent<
                                    Components::Rigidbody
                                >(a);

                            const bool bHasBody =
                                registry.hasComponent<
                                    Components::Rigidbody
                                >(b);

                            if (!aHasBody && !bHasBody) {
                                return;
                            }

                            if (aHasBody && !bHasBody) {

                                transformA.position =
                                    transformA.position -
                                    collision.normal *
                                    collision.penetration;

                                return;
                            }

                            if (!aHasBody && bHasBody) {

                                transformB.position =
