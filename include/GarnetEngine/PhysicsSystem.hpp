/**
 * @file PhysicsSystem.hpp
 * @brief Provides rigidbody integration and collision handling.
 */

#pragma once

namespace Garnet {

    /**
     * @brief Updates the physics simulation.
     *
     * Responsible for integrating rigidbody velocity,
     * applying forces and performing collision response.
     */
    class PhysicsSystem {

    public:

        /**
         * @brief Advances the physics simulation.
         *
         * Applies accumulated forces, updates rigidbody
         * velocity and moves entities based on their
         * current velocity.
         *
         * All forces accumulated during the frame are
         * cleared after integration.
         *
         * @param registry Registry containing component data.
         * @param dt Time elapsed since the previous frame.
         *
         * @code
         * physicsSystem.Update(
         *     registry,
         *     deltaTime
         * );
         * @endcode
         */
        static void Update(
            Registry& registry,
            float dt);
    };

}