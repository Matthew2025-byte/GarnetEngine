/**
 * @file ColliderComponent.hpp
 * @brief Defines axis-aligned collision bounds for entities.
 */

#pragma once

#include "vec2.hpp"

namespace Garnet::Components {

    /**
     * @brief Axis-aligned bounding box collider.
     *
     * The collider component defines an entity's collision
     * bounds for broad and narrow phase collision testing.
     *
     * The size represents the dimensions of the collision
     * volume while the offset allows the volume to be shifted
     * relative to the entity's transform.
     */
    struct Collider {
        vec2 size{ 1.0f, 1.0f };
        vec2 offset{ 0.0f, 0.0f };

        bool isTrigger = false;
    };

}