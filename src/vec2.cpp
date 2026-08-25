#include "GarnetEngine/vec2.hpp"
#include <cmath>
namespace Garnet {
    float vec2::magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    vec2 vec2::normalized() const {
        float length = magnitude();
        return *this / (length == 0 ? 1 : length);
    }

    float vec2::dot(const vec2& other) const {
        return x * other.x + y * other.y;
    }



    std::string vec2::str() const {
        return std::to_string(x) + ", " + std::to_string(y);
    }



    vec2 vec2::operator+(const vec2& other) const { return vec2(this->x + other.x, this->y + other.y); }

    vec2 vec2::operator-(const vec2& other) const { return vec2(this->x - other.x, this->y - other.y); }

    vec2 vec2::operator*(float scalar) const { return vec2(this->x * scalar, this->y * scalar); }


    vec2 vec2::operator/(float scalar) const { return vec2(this->x / scalar, this->y / scalar); }
}
