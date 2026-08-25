#pragma once
#include <SDL3/SDL.h>
#include "Registry.hpp"
#include "Components.hpp"
#include "TextureManager.hpp"

namespace Garnet {




class RenderSystem {
    SDL_Renderer* renderer;
    Garnet::TextureManager& textureManager;

    std::vector<SDL_Texture*> TextureIDs;
    uint32_t textureIndex = 0;
    


    void RenderTexture(SDL_Renderer* renderer, Garnet::Components::Transform transform, Garnet::TextureID textureID) {
        SDL_Texture* texture = textureManager.getTexture(textureID);

        float width = static_cast<float>(texture->w);
        float height = static_cast<float>(texture->h);
        SDL_FRect rect = { transform.position.x - width/2, transform.position.y - height/2, width, height };

        SDL_RenderTextureRotated(renderer, texture, NULL, &rect, static_cast<double>(transform.rotation), NULL, SDL_FLIP_NONE);
    }
    public:
    RenderSystem(SDL_Renderer* renderer, Garnet::TextureManager& manager) :
        renderer(renderer), textureManager(manager) {}
    
    void update(Garnet::Registry& registry) {
        SDL_RenderClear(renderer);
        
        registry.each<Garnet::TextureID, Garnet::Components::Transform>([&](const Garnet::Entity entity, Garnet::TextureID texture, const Garnet::Components::Transform& transform) {
            RenderTexture(this->renderer, transform, texture);
        });
        
        SDL_RenderPresent(renderer);
    }
};


}
