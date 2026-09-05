#include "GarnetEngine/CollisionSystem.hpp"

#include <cmath>

using namespace Garnet;
using vec2 = Garnet::vec2;

// Internal functionality not intended for external use
namespace {
static bool overlap(float aMin, float aMax, float bMin, float bMax) {
	return aMin <= bMax && bMin <= aMax;
}
}  // namespace

bool CollisionSystem::AABB(vec2 posA, Components::Collider& colliderA, vec2 posB,
						   Components::Collider& colliderB) {
	vec2 aMin = posA + colliderA.offset - (colliderA.size * 0.5f);
	vec2 aMax = posA + colliderA.offset + (colliderA.size * 0.5f);
	vec2 bMin = posB + colliderB.offset - (colliderB.size * 0.5f);
	vec2 bMax = posB + colliderB.offset + (colliderB.size * 0.5f);

	bool overlapX = overlap(aMin.x, aMax.x, bMin.x, bMax.x);
	bool overlapY = overlap(aMin.y, aMax.y, bMin.y, bMax.y);
	return overlapX && overlapY;
}

void CollisionSystem::collisionCircleCircle(vec2 posA, Components::Rigidbody& rigidBodyA,
											float radiusA, vec2 posB,
											Components::Rigidbody& rigidBodyB,
											float radiusB) {
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

void CollisionSystem::System(float dt, Registry& registry,
							 const std::vector<Entity>& entities) {
	const size_t entityCount = entities.size();

	for (size_t i = 0; i < entityCount; i++) {
		auto& transformA = registry.getComponent<Components::Transform>(entities[i]);
		auto& rigidBodyA = registry.getComponent<Components::Rigidbody>(entities[i]);
		auto& colliderA = registry.getComponent<Components::Collider>(entities[i]);

		for (size_t j = i + 1; j < entityCount; j++) {
			auto& transformB = registry.getComponent<Components::Transform>(entities[j]);
			auto& rigidBodyB = registry.getComponent<Components::Rigidbody>(entities[j]);
			auto& colliderB = registry.getComponent<Components::Collider>(entities[j]);

			if (AABB(transformA.position, colliderA, transformB.position, colliderB)) {
				if (colliderA.type == Components::colliderType::Circle && colliderB.type == Components::colliderType::Circle) {
					float radiusA = registry.getComponent<Components::circleCollider>(entities[i]).radius;
					float radiusB = registry.getComponent<Components::circleCollider>(entities[j]).radius;
					collisionCircleCircle(transformA.position, rigidBodyA, radiusA, transformB.position, rigidBodyB, radiusB);
				}
			}
		}
	}
}
