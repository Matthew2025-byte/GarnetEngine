/**
 * @file Sprite.hpp
 * @author Matthew 5-byte
 * @brief Sprite tracking and rendering
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <SDL3/SDL.h>

#include <cassert>
#include <unordered_map>
#include <vector>

#include "Components.hpp"

namespace Garnet {

struct Sprite {
	TextureID id;
	Components::Transform transform;
	std::string textureName;
	

	bool operator==(const Sprite& other) {
		return this->id.index == other.id.index && this->transform == other.transform;
	}
	Sprite(std::string textureName, Components::Transform transform)
		: textureName(textureName), transform(transform) {}
	Sprite() = default;
};

/**
 * @brief Buffer for tracking changes to sprites
 *
 */
class SpriteBuffer {
	public:
	/**
	 * @brief Applies a provided delta buffer to the sprite buffer
	 *
	 * @param delta Buffer to apply
	 */
	void push(const std::unordered_map<int, Sprite>& delta);
	/**
	 * @brief Applies the internal delta buffer and clears it
	 *
	 */
	void push();

	const std::unordered_map<int, Sprite> getDelta() const { return spriteDelta; }
	std::vector<Sprite>& getSprites() { return sprites; }
	void setDelta(std::unordered_map<int, Sprite> delta);
	void setSprites(std::vector<Sprite> sprites);
	void addSprite(Entity entity, Sprite sprite);

	Sprite& operator[](Entity entity);

	private:
	std::unordered_map<Entity, int> entity_index;
	std::vector<Sprite> sprites;
	std::unordered_map<int, Sprite> spriteDelta;
};

}  // namespace Garnet