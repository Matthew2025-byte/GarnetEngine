/**
 * @file Components.hpp
 * @author Matthew2025-byte
 * @brief Contains main components for Garnet
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <SDL3/SDL.h>

#include "vec2.hpp"

namespace Garnet::Components {
struct Transform {
	Garnet::vec2 position;
	float rotation;
};

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
	vec2 size{1.0f, 1.0f};
	vec2 offset{0.0f, 0.0f};

	bool isTrigger = false;
};

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
	Garnet::vec2 velocity{};

	/// Accumulated forces applied during the current frame.
	Garnet::vec2 force{};

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
	float inverseMass() const { return (isStatic || mass <= 0.0f) ? 0.0f : 1.0f / mass; }
};
}  // namespace Garnet::Components