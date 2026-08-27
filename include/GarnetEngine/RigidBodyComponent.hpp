/**
 * @file RigidBodyComponent.hpp
 * @brief Defines the Rigidbody component used by the physics system.
 */

#pragma once

#include "vec2.hpp"

namespace Garnet::Components {

    /**
     * @brief Stores physical motion data for an entity.
     *
     * Rigidbody is responsible for tracking velocity and force
     * data used by the physics system. The component itself does
     * not move entities directly. Instead, systems use the stored
     * information to calculate acceleration and update a
     * Transform component.
     *
     * Static rigidbodies are unaffected by forces and are
     * commonly used for walls, floors, and other immovable
     * objects.
     */
    struct Rigidbody {

        /// Current velocity in units per second.
        vec2 velocity{};

        /// Accumulated forces applied during the current frame.
        vec2 force{};

        /// Mass of the object in kilograms.
        float mass = 1.0f;

        /// Bounciness factor used during collision resolution.
        float restitution = 0.2f;

        /// Prevents physical simulation when true.
        bool isStatic = false;

        /**
         * @brief Calculates the inverse mass.
         *
         * Physics solvers frequently use inverse mass rather
         * than mass directly as it avoids repeated division.
         * Static bodies always return zero.
         *
         * @returns Inverse mass value or zero for static bodies.
         */
        float inverseMass() const {
            return (isStatic || mass <= 0.0f)
                ? 0.0f
                : 1.0f / mass;
        }
    };

}