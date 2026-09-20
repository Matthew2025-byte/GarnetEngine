#include "GarnetEngine/Renderer.hpp"

void Garnet::Renderer::RenderTexture(SDL_Renderer* renderer, Garnet::Components::Transform transform, SDL_Texture* texture) {
    #ifdef _DEBUG
    if (!texture) {
        SDL_Log("Texture is nullptr: %s", SDL_GetError());
        throw std::runtime_error("Texture is nullptr");
    }
    #endif
    float width = static_cast<float>(texture->w);
    float height = static_cast<float>(texture->h);
    SDL_FRect rect = { transform.position.x - width/2, transform.position.y - height/2, width, height };

    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, static_cast<double>(transform.rotation), NULL, SDL_FLIP_NONE);
}

void Garnet::Renderer::renderSprites(SpriteBuffer sprites) {
    for (auto sprite : sprites.getSprites()) {
        RenderTexture(this->renderer, sprite.transform, sprite.texture);
    }
}