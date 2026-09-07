#include "GarnetEngine/Renderer.hpp"

void Garnet::Renderer::RenderTexture(SDL_Renderer* renderer, Garnet::Components::Transform transform, SDL_Texture* texture) {
    float width = static_cast<float>(texture->w);
    float height = static_cast<float>(texture->h);
    SDL_FRect rect = { transform.position.x - width/2, transform.position.y - height/2, width, height };

    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, static_cast<double>(transform.rotation), NULL, SDL_FLIP_NONE);
}


void Garnet::Renderer::update(Garnet::Registry& registry) {
    registry.each<Garnet::TextureID, Garnet::Components::Transform>([&](const Garnet::Entity entity, Garnet::TextureID textureID, const Garnet::Components::Transform& transform) {
        SDL_Texture* texture = this->textureManager.getTexture(textureID);
        this->RenderTexture(this->renderer, transform, texture);
    });
}

void Garnet::Renderer::renderSprites(std::vector<Sprite> sprites) {
    for (auto sprite : sprites) {
        SDL_Texture* spriteTexture = this->textureManager.getTexture(sprite.id);
        RenderTexture(this->renderer, sprite.transform, spriteTexture);
    }
}