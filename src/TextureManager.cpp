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


Garnet::TextureID Garnet::TextureManager::Load(std::string file, std::unordered_map<std::string, std::string> properties) {
    std::filesystem::path filepath = root_folder / file;
    std::string ext = filepath.extension().string();
    std::string name = filepath.stem().string();
    if (!std::filesystem::exists(filepath)) {
        SDL_Log("Failed to find file: %s", std::filesystem::absolute(filepath).string().c_str());
        return InvalidTexture;
    }

    SDL_Texture* texture;
    std::string cache_key;

    if (ext == ".svg") {
        int width = std::stoi(properties["width"]);
        cache_key = name + "_" + std::to_string(width) + ext;

        auto it = this->textureCache.find(cache_key);
        if (it != this->textureCache.end()) {
            return it->second;
        }

        texture = LoadTextureFromSVG(filepath.string().c_str(), width);
    }
    else {
        auto it = this->textureCache.find(file);
        if (it != this->textureCache.end()) {
            SDL_Log("Loading from cache: %s", cache_key);
            return it->second;
        }
        cache_key = std::move(file);

        texture = IMG_LoadTexture(renderer, filepath.string().c_str());
    }
    
    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return InvalidTexture;
    }

    TextureID id(static_cast<uint32_t>(this->textures.size()));
    this->textureCache.emplace(cache_key, id);
    this->textures.push_back(texture);

    SDL_Log("Created Texture: %s - index %i", cache_key.c_str(), id.index);
    return id;
}

Garnet::TextureID Garnet::TextureManager::findTexture(std::string name) {
    auto it = this->textureCache.find(name);
    if (it != this->textureCache.end()) {
        return it->second;
    }
    SDL_Log("Unable to find texture: %s", name.c_str());
    SDL_Log("An error occured while finding texture: %s", SDL_GetError());
    throw std::runtime_error("Unable to find texture");
}

SDL_Texture* Garnet::TextureManager::getTexture(Garnet::TextureID id) {
#ifdef _DEBUG
	if (id.index >= this->textures.size())
		SDL_Log("Attempted to access Texture[%i] - Size is %i", id.index, this->textures.size());
	return this->textures.at(id.index);
#else
	return this->textures[id.index];
#endif
}

SDL_Texture* Garnet::TextureManager::getTexture(std::string name) {
    return getTexture(findTexture(name));
}
