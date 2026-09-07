/**
 * @file Renderer.hpp
 * @author Matthew2025-byte
 * @brief Prototype renderer
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "Registry.hpp"
#include "Components.hpp"
#include "TextureManager.hpp"
#include "Sprite.hpp"

namespace Garnet {


class Renderer {
    SDL_Renderer* renderer;
    Garnet::TextureManager& textureManager;

    std::vector<SDL_Texture*> TextureIDs;
    


    void RenderTexture(SDL_Renderer* renderer, Garnet::Components::Transform transform, SDL_Texture* texture);
    public:
    Renderer(SDL_Renderer* renderer, Garnet::TextureManager& manager) :
        renderer(renderer), textureManager(manager) {}
    
    void update(Garnet::Registry& registry);

    void renderSprites(SpriteBuffer sprites);
};

}