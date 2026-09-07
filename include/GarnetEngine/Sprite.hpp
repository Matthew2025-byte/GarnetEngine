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
#include <vector>
#include <unordered_map>
#include "Components.hpp"

namespace Garnet {

struct Sprite {
    SDL_Texture* texture;
    TextureID id;
    Components::Transform transform;

    bool operator == (const Sprite& other) { return this->texture == other.texture && this->transform == other.transform; }
};

/**
 * @brief Buffer for tracking changes to sprites
 * 
 */
class SpriteBuffer {
    public:
    void push() {
        for (auto& [index, sprite] : spriteDelta) {
            sprites[index] = sprite;
        }
        spriteDelta.clear();
    }

    std::unordered_map<int, Sprite> getDelta() { return spriteDelta; }
    const std::vector<Sprite>& getSprites() const { return sprites; }
    void setDelta(std::unordered_map<int, Sprite> delta) { spriteDelta.clear(); spriteDelta = delta; }
    void setSprites(std::vector<Sprite> sprites) { this->sprites = sprites; }
    void addSprite(Entity entity, Sprite sprite) {
        entity_index[entity] = sprites.size();
        sprites.push_back(sprite);
    }

    Sprite& operator[] (Entity entity) {
        Entity index = entity_index[entity];
        spriteDelta[index] = sprites[index];
        return spriteDelta[index];
    }

    private:
    std::unordered_map<Entity, int> entity_index;
    std::vector<Sprite> sprites;
    std::unordered_map<int, Sprite> spriteDelta;
};

}