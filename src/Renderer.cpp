#include "GarnetEngine/Renderer.hpp"

void Garnet::Renderer::RenderTexture(SDL_Renderer* renderer, Garnet::Components::Transform transform, SDL_Texture* texture) {
    #ifdef _DEBUG
    if (!texture) {
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
        if (!sprite.id.isValid()) {
            sprite.id = this->textureManager.findTexture(sprite.textureName);
        }
        SDL_Texture* texture = this->textureManager.getTexture(sprite.id);
        RenderTexture(this->renderer, sprite.transform, texture);
    }
}