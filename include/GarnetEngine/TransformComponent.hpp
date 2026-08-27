/**
 * @file TransformComponent.hpp
 * @brief Defines the Transform component used to store entity position and rotation.
 */

#pragma once

#include "vec2.hpp"

namespace Garnet::Components {

    /**
     * @brief Stores an entity's spatial transformation.
     *
     * The Transform component represents the position and
     * orientation of an entity within the game world.
     *
     * Most systems interact with this component directly.
     * Rendering systems use it to determine where objects
     * should be drawn, while physics systems update the
     * transform based on velocity and collision results.
     *
     * Rotation is stored in radians.
     */
    struct Transform {

        /**
         * @brief World-space position of the entity.
         */
        vec2 position{};

        /**
         * @brief Rotation around the Z-axis in radians.
         */
        float rotation = 0.0f;
    };

}