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

    Sprite& operator[] (int index) {
        spriteDelta[index] = sprites[index];
        return spriteDelta[index];
    }

    private:
    std::vector<Sprite> sprites;
    std::unordered_map<int, Sprite> spriteDelta;
};

}