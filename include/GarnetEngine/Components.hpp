/**
 * @file Components.hpp
 * @author Matthew2025-byte
 * @brief Contains main components for Garnet
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include <SDL3/SDL.h>
#include "vec2.hpp"

namespace Garnet::Components {
    struct Transform {
        Garnet::vec2 position;
        float rotation;
    };
}