/**
 * @file CollisionCheck.hpp
 * @brief Provides collision detection functions for colliders.
 */

#pragma once

#include <cmath>

#include "Collision.hpp"
#include "ColliderComponent.hpp"
#include "TransformComponent.hpp"

namespace Garnet {

    /**
     * @brief Performs an axis-aligned bounding box collision test.
     *
     * Compares two AABB colliders and determines whether they
     * overlap. If a collision occurs, information describing
     * the overlap is returned for use by collision resolution
     * systems.
     *
     * The collision normal points from the first collider
     * toward the second collider.
     *
     * @param aTransform Transform associated with the first collider.
     * @param a First collider.
     * @param bTransform Transform associated with the second collider.
     * @param b Second collider.
     *
     * @returns A Collision structure describing the collision.
     *
     * @code
     * Collision collision =
     *     CheckAABB(
     *         playerTransform,
     *         playerCollider,
     *         wallTransform,
     *         wallCollider
     *     );
     *
     * if (collision.hit) {
     *     // Resolve collision
     * }
     * @endcode
     */
    [[nodiscard]]
    inline Collision CheckAABB(
        const Components::Transform& aTransform,
        const Components::Collider& a,
        const Components::Transform& bTransform,
        const Components::Collider& b)
    {
        Collision result;

        const vec2 aHalf =
            a.size * 0.5f;

        const vec2 bHalf =
            b.size * 0.5f;

        const vec2 aCenter =
            aTransform.position + a.offset;

        const vec2 bCenter =
            bTransform.position + b.offset;

        const float dx =
            bCenter.x - aCenter.x;

        const float dy =
            bCenter.y - aCenter.y;

        const float overlapX =
            (aHalf.x + bHalf.x) -
            std::abs(dx);

        const float overlapY =
            (aHalf.y + bHalf.y) -
            std::abs(dy);

        if (overlapX <= 0.0f ||
            overlapY <= 0.0f)
        {
            return result;
        }

        result.hit = true;

        if (overlapX < overlapY) {

            result.penetration =
                overlapX;

            result.normal =
                (dx > 0.0f)
                ? vec2{ 1.0f, 0.0f }
            : vec2{ -1.0f, 0.0f };
        }
        else {

            result.penetration =
                overlapY;

            result.normal =
                (dy > 0.0f)
                ? vec2{ 0.0f, 1.0f }
            : vec2{ 0.0f, -1.0f };
        }

        return result;
    }

}