/**
 * @file Collision.hpp
 * @brief Provides collision detection and resolution systems.
 */

#pragma once

#include <cmath>

#include "Components.hpp"
#include "Scene.hpp"
#include "vec2.hpp"

/**
 * @brief Contains collision detection and resolution functionality.
 *
 */
namespace Garnet::CollisionSystem {
// Internal helper function to check for overlap between two 1D intervals
static bool overlap(float aMin, float aMax, float bMin, float bMax) {
	return aMin <= bMax && bMin <= aMax;
}

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
		  Garnet::Components::Collider& colliderB) {
	vec2 aMin = posA + colliderA.offset - (colliderA.size * 0.5f);
	vec2 aMax = posA + colliderA.offset + (colliderA.size * 0.5f);
	vec2 bMin = posB + colliderB.offset - (colliderB.size * 0.5f);
	vec2 bMax = posB + colliderB.offset + (colliderB.size * 0.5f);

	bool overlapX = overlap(aMin.x, aMax.x, bMin.x, bMax.x);
	bool overlapY = overlap(aMin.y, aMax.y, bMin.y, bMax.y);
	return overlapX && overlapY;
}


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
						   vec2 posB, Garnet::Components::Rigidbody& rigidBodyB, float radiusB) {
	using vec2 = Garnet::vec2;
	vec2 delta = posB - posA;
	float distSq = delta.x * delta.x + delta.y * delta.y;

	const float radiusSum = radiusA + radiusB;
	const float radiusSumSq = radiusSum * radiusSum;

	if (distSq >= radiusSumSq) {
		return;
	}

	float dist = std::sqrt(distSq);
	vec2 normal = delta / dist;

	vec2 relativeVelocity = rigidBodyB.velocity - rigidBodyA.velocity;
	float velAlongNormal = relativeVelocity.dot(normal);

	if (velAlongNormal > 0) {
		return;
	}

	float restitution = std::min(rigidBodyA.restitution, rigidBodyB.restitution);
	float impulseMagnitude =
		-(1 + restitution) * velAlongNormal / (rigidBodyA.inverseMass() + rigidBodyB.inverseMass());

	vec2 impulse = normal * impulseMagnitude;

	rigidBodyA.velocity -= impulse * rigidBodyA.inverseMass();
	rigidBodyB.velocity += impulse * rigidBodyB.inverseMass();

	float overlap = radiusSum - dist;
	vec2 correction = normal * (overlap * 0.5f);

	posA -= correction;
	posB += correction;
}

/**
 * @brief Handles rigidbody collision detection and resolution for all entities with collider and
 * rigidbody components
 *
 * @param dt deltaTime
 * @param registry Reference to the registry to check
 * @param entities Vector of entities that match the required components (Transform, Rigidbody,
 * Collider)
 */
void collisionSystem(float dt, Garnet::Registry& registry,
					 const std::vector<Garnet::Entity>& entities) {
	const size_t entityCount = entities.size();

	for (size_t i = 0; i < entityCount; i++) {
		auto& transformA = registry.getComponent<Garnet::Components::Transform>(entities[i]);
		auto& rigidBodyA = registry.getComponent<Garnet::Components::Rigidbody>(entities[i]);
		auto& colliderA = registry.getComponent<Garnet::Components::Collider>(entities[i]);

		for (size_t j = i + 1; j < entityCount; j++) {
			auto& transformB = registry.getComponent<Garnet::Components::Transform>(entities[j]);
			auto& rigidBodyB = registry.getComponent<Garnet::Components::Rigidbody>(entities[j]);
			auto& colliderB = registry.getComponent<Garnet::Components::Collider>(entities[j]);

			if (AABB(transformA.position, colliderA, transformB.position, colliderB)) {
				if (colliderA.type == Garnet::Components::colliderType::Circle &&
					colliderB.type == Garnet::Components::colliderType::Circle) {
					float radiusA =
						registry.getComponent<Garnet::Components::circleCollider>(entities[i])
							.radius;
					float radiusB =
						registry.getComponent<Garnet::Components::circleCollider>(entities[j])
							.radius;
					collisionCircleCircle(transformA.position, rigidBodyA, radiusA,
										  transformB.position, rigidBodyB, radiusB);
				}
			}
		}
	}
}
}  // namespace Garnet::CollisionSystem