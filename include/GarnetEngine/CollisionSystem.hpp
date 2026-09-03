/**
 * @file Collision.hpp
 * @brief Provides collision detection and resolution systems.
 */

#pragma once

#include "Components.hpp"
#include "Scene.hpp"
#include "vec2.hpp"

/**
 * @brief Contains collision detection and resolution functionality.
 *
 */
namespace Garnet::CollisionSystem {

/**
 * @brief Performs an axis-aligned bounding box (AABB) collision test.
 *
 * @param posA Center of object #1
 * @param colliderA Collider of object #1
 * @param posB Center of object #2
 * @param colliderB Collider of object #2
 * @return true if the objects are colliding, false otherwise
 */
bool AABB(vec2 posA, Garnet::Components::Collider& colliderA, vec2 posB,
		  Garnet::Components::Collider& colliderB);

/**
 * @brief Performs Circle on Circle collision detection and resolution between two entities.
 *
 * @param posA Position of entity A
 * @param rigidBodyA Rigidbody component of entity A
 * @param radiusA Radius of entity A
 * @param posB Position of entity B
 * @param rigidBodyB Rigidbody component of entity B
 * @param radiusB Radius of entity B
 */
void collisionCircleCircle(vec2 posA, Garnet::Components::Rigidbody& rigidBodyA, float radiusA,
						   vec2 posB, Garnet::Components::Rigidbody& rigidBodyB, float radiusB);

/**
 * @brief Handles rigidbody collision detection and resolution for all entities with collider and
 * rigidbody components
 *
 * @param dt deltaTime
 * @param registry Reference to the registry to check
 * @param entities Vector of entities that match the required components (Transform, Rigidbody,
 * Collider)
 */
void System(float dt, Garnet::Registry& registry, const std::vector<Garnet::Entity>& entities);
}  // namespace Garnet::CollisionSystem