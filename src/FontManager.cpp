#include "GarnetEngine/FontManager.hpp"

Garnet::FontManager::FontManager(SDL_Renderer* renderer, std::filesystem::path fontDir) {
    this->textEngine = TTF_CreateRendererTextEngine(renderer);
    if (!this->textEngine) { SDL_Log("Failed to initilize Text Engine: %s", SDL_GetError()); }
    this->root_folder = std::filesystem::path(fontDir);
}

Garnet::FontManager::FontManager(SDL_Renderer* renderer) {
    this->textEngine = TTF_CreateRendererTextEngine(renderer);
    if (!this->textEngine) { SDL_Log("Failed to initilize Text Engine: %s", SDL_GetError()); }
    this->root_folder = std::filesystem::path("assets/fonts");
}

TTF_Font* Garnet::FontManager::Load(const char* file) {
    std::filesystem::path filepath = this->root_folder / file;
    std::string name = filepath.stem().string();
    if (!std::filesystem::exists(filepath)) {
        SDL_Log("File does not exist: %s", std::filesystem::absolute(filepath).string().c_str());
        throw std::runtime_error("Asset does not exist: " + filepath.string());
    }

    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, TTF_PROP_FONT_CREATE_FILENAME_STRING, filepath.string().c_str());
    SDL_SetFloatProperty(props, TTF_PROP_FONT_CREATE_SIZE_FLOAT, 24.0f);

    TTF_Font* font = TTF_OpenFontWithProperties(props);
    SDL_DestroyProperties(props);

    if (!font) {
        SDL_Log("Failed to open font: %s", filepath.string().c_str());
        SDL_Log("Error: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Log("Loaded font: %s", name.c_str());
    this->fonts[name] = font;
    return this->fonts.find(name)->second;
}

TTF_Font* Garnet::FontManager::getFont(const std::string& font) const {
    auto it = this->fonts.find(font);
    if (it == fonts.end()) {
        throw std::runtime_error("Font " + font + " does not exist");
    }
    return it->second;
}