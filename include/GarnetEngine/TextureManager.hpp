#pragma once
#include <unordered_map>
#include <string>
#include "Systems.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <filesystem>

namespace Garnet {
    class TextureManager;
    struct TextureID;
}

template <typename T>
bool isNull(T* ptr, const char* operation) {
    if (ptr) return false;
    SDL_Log("Failed to %s: %s", operation, SDL_GetError());
    return true;
}


struct Garnet::TextureID {
    uint32_t index = 0;

    bool isValid() {
        return (index != NULL);
    }
};
constexpr Garnet::TextureID InvalidTexture = {0};




#define GARNET_SVG_RASTER_WIDTH "Garnet.LoadFile.SVG.Raster_Width"


class Garnet::TextureManager {
    std::unordered_map<std::string, TextureID> TextureCache;
    std::vector<SDL_Texture*> Textures;

    SDL_Renderer* renderer;
    std::filesystem::path root_folder;

    SDL_Texture* LoadTextureFromSVG(const char* filepath, int size) {
        SDL_IOStream* stream = SDL_IOFromFile(filepath, "rb");
        if (isNull(stream, "Load file")) return nullptr;
        SDL_Surface* surface = IMG_LoadSizedSVG_IO(stream, size, 0);
        SDL_CloseIO(stream);
        if (isNull(surface, "Create surface from svg")) return nullptr;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_DestroySurface(surface);
        if (isNull(texture, "Create texture")) return nullptr;
        
        
        return texture;
    }

    public:
    explicit TextureManager(SDL_Renderer* renderer) 
        : TextureManager(renderer, "assets/textures/") {}
    explicit TextureManager(SDL_Renderer* renderer, const char* texture_root_folder) :
        renderer(renderer), root_folder(std::filesystem::path(texture_root_folder)) { Textures.push_back(nullptr); }

    /**
     * @brief Loads a texture into memory
     * The actual path of the file is calculated at runtime relative to the
     * texture manager's configured root folder. The caller only needs to
     * provide the texture's filename.  If a texture has already been loaded
     * its existing TextureID is returned instead of loading it again
     * 
     * @param file Name of the file (ex. player.png)
     * @param properties Allows for configuration of filetype specific properties
     * @returns Returns a TextureID
     */
    TextureID Load(const char* file, SDL_PropertiesID properties=0) {
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
            texture = LoadTextureFromSVG(filepath.string().c_str(), SDL_GetFloatProperty(properties, GARNET_SVG_RASTER_WIDTH, 10)); // Remove hardcoded 100 with SDL_PropertiesID
        }
        else {
            texture = IMG_LoadTexture(renderer, filepath.string().c_str());
            if (isNull(texture, "Load IMG")) return InvalidTexture;
        }
        TextureID id { static_cast<uint32_t>(Textures.size()) };
        TextureCache.emplace(file, id);
        Textures.push_back(texture);

        SDL_Log("Created Texture at %i", id.index);
        return id;
    }

    SDL_Texture* getTexture(TextureID id) {
        #ifdef _DEBUG
            if (id.index >= Textures.size()) SDL_Log("Attempted to access Texture[%i] - Size is %i", id.index, Textures.size());
            return Textures.at(id.index);
        #else
            return Textures[id.index];
        #endif
    }
};