#include "GarnetEngine/TextureManager.hpp"

SDL_Texture* Garnet::TextureManager::LoadTextureFromSVG(const char* filepath, int size) {
        SDL_IOStream* stream = SDL_IOFromFile(filepath, "rb");
        if (!stream) { SDL_Log("Failed to load file to memory: %s", SDL_GetError()); return nullptr; }
        SDL_Surface* surface = IMG_LoadSizedSVG_IO(stream, size, 0);
        SDL_CloseIO(stream);
        if (!surface) { SDL_Log("Failed to create surface: %s", SDL_GetError()); return nullptr; }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) { SDL_Log("Failed to create texture: %s", SDL_GetError()); return nullptr; }       
        return texture;
    }


Garnet::TextureID Garnet::TextureManager::Load(const char* file, SDL_PropertiesID properties) {
    auto it = TextureCache.find(file);
    if (it != TextureCache.end()) return it->second;

    std::filesystem::path filepath = root_folder / file;
    std::string ext = filepath.extension().string();
    if (!std::filesystem::exists(filepath)) {
        SDL_Log("Failed to find file: %s", std::filesystem::absolute(filepath).string().c_str());
        return InvalidTexture;
    }

    SDL_Texture* texture;
    if (ext == ".svg") {
        texture = LoadTextureFromSVG(filepath.string().c_str(), SDL_GetFloatProperty(properties, GARNET_SVG_RASTER_WIDTH, 10));
    }
    else {
        texture = IMG_LoadTexture(renderer, filepath.string().c_str());
    }
    if (!texture) { SDL_Log("Failed to create texture: %s", SDL_GetError()); return InvalidTexture; }
    TextureID id { static_cast<uint32_t>(Textures.size()) };
    TextureCache.emplace(file, id);
    Textures.push_back(texture);

    SDL_Log("Created Texture at %i", id.index);
    return id;
}

SDL_Texture* Garnet::TextureManager::getTexture(Garnet::TextureID id) {
    #ifdef _DEBUG
        if (id.index >= Textures.size()) SDL_Log("Attempted to access Texture[%i] - Size is %i", id.index, Textures.size());
        return Textures.at(id.index);
    #else
        return Textures[id.index];
    #endif
}