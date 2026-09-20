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
	SDL_Texture* texture;
	Components::Transform transform;

	bool operator==(const Sprite& other) {
		return this->texture == other.texture && this->transform == other.transform;
	}
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
	void push(const std::unordered_map<int, Sprite>& delta) {
		for (auto& [index, sprite] : delta) {
			if (index >= sprites.size()) {
				sprites.resize(index + 1);
			}
			sprites[index] = sprite;
		}
	}
	/**
	 * @brief Applies the internal delta buffer and clears it
	 *
	 */
	void push() {
		push(this->spriteDelta);
		spriteDelta.clear();
	}

	const std::unordered_map<int, Sprite> getDelta() const { return spriteDelta; }
	std::vector<Sprite>& getSprites() { return sprites; }
	void setDelta(std::unordered_map<int, Sprite> delta) {
		spriteDelta.clear();
		spriteDelta = delta;
	}
	void setSprites(std::vector<Sprite> sprites) { this->sprites = sprites; }
	void addSprite(Entity entity, Sprite sprite) {
		entity_index[entity] = sprites.size();
		sprites.push_back(sprite);
	}

	Sprite& operator[](Entity entity) {
		auto it = entity_index.find(entity);
		assert(it != entity_index.end() && "No sprite found with requested entity");
		Entity index = entity_index[entity];
		spriteDelta[index] = sprites[index];
		return spriteDelta[index];
	}

	private:
	std::unordered_map<Entity, int> entity_index;
	std::vector<Sprite> sprites;
	std::unordered_map<int, Sprite> spriteDelta;
};

}  // namespace Garnet