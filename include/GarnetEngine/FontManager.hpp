#pragma once
#include <SDL3_ttf/SDL_textengine.h>
#include <unordered_map>
#include <filesystem>

namespace Garnet {
    class FontManager;
}

class Garnet::FontManager {
    TTF_TextEngine* textEngine;
    std::unordered_map<std::string, TTF_Font*> fonts;

    std::filesystem::path fontDir;

    public:
    FontManager(SDL_Renderer* renderer, std::filesystem::path fontDir) {
        this->textEngine = TTF_CreateRendererTextEngine(renderer);
        if (!this->textEngine) { SDL_Log("Failed to initilize Text Engine: %s", SDL_GetError()); }
        this->fontDir = fontDir;
    }

    /**
     * @brief Loads a ttc font into memory
     * 
     * This method currently only supports ttc types.  More will be added later
     * 
     * @param file name of the file to load (ex. font.ttc)
     * @returns The requested font
     */
    TTF_Font* Load(const char* file) {
        std::filesystem::path filepath = fontDir / file;
        std::string fileName = filepath.stem().string();
        
        if (!std::filesystem::exists(filepath)) {
            SDL_Log("File does not exist: %s", filepath.string().c_str());
            throw std::runtime_error("Asset does not exist: " + filepath.string());
        }
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetStringProperty(props, TTF_PROP_FONT_CREATE_FILENAME_STRING, filepath.string().c_str());
        SDL_SetFloatProperty(props, TTF_PROP_FONT_CREATE_SIZE_FLOAT, 24.0f);

        TTF_Font* font = TTF_OpenFontWithProperties(props);
        SDL_DestroyProperties(props);

        if (!font) {
            SDL_Log("Failed to open font: %s", filepath.string().c_str());
            return nullptr;
        }

        this->fonts[fileName] = font;
        return this->fonts.find(fileName)->second;
    }

    TTF_Font* getFont(const std::string& font) {
        auto it = fonts.find(font);
        if (it == fonts.end()) {
            throw std::runtime_error("Font "+font+" does not exist");
            return nullptr;
        }
        return it->second;
    }
};