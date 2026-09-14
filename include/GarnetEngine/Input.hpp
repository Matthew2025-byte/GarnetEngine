#pragma once
#include <unordered_map>

namespace Garnet {

enum keyEvent {
    DOWN,
    UP,
};

struct Keyboard {
    std::unordered_map<SDL_Scancode, keyEvent> keyboard;

    void setKey(SDL_Scancode code, keyEvent event) {
        keyboard[code] = event;
    }
    /**
     * @brief Get the current key state
     * 
     * @param code scancode to get
     * @return The current position of the key
     */
    keyEvent getKey(SDL_Scancode code) {
        auto it = keyboard.find(code);
        if (it == keyboard.end()) {
            return UP;
        }
        return it->second;
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