#pragma once
#include <unordered_map>
#include <array>

namespace Garnet {

enum class KeyEvent : uint8_t {
    DOWN,
    UP,
};

struct Keyboard {
    std::array<KeyEvent, SDL_SCANCODE_COUNT> keyboard{ KeyEvent::UP };

    /**
     * @brief Set a scancode position
     * 
     * @param code The SDL_Scancode to assign
     * @param event Sets the position of the given key
     */
    void setKey(SDL_Scancode code, KeyEvent event) {
        keyboard[static_cast<size_t>(code)] = event;
    }
    /**
     * @brief Gets the current position of a key
     * 
     * @param code The SDL_Scancode to retrieve
     * @return The current position of the key
     */
    KeyEvent getKey(SDL_Scancode code) const {
        return keyboard[static_cast<size_t>(code)];
    }
};

struct Mouse {
    vec2 position;
    vec2 delta;
    bool buttonStates[5] = {false, false, false, false, false};
};

struct Input {
    Keyboard keyboard;
    Mouse mouse;
};

}