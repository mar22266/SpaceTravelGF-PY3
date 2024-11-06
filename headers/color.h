#pragma once  

#include <SDL2/SDL.h> 
#include <algorithm>  
#include <iostream>  

// Define a Color struct to hold RGBA color values
struct Color {
    Uint8 r;  
    Uint8 g;  
    Uint8 b;  
    Uint8 a; 

    // Default constructor that initializes color to black with full opacity
    Color() : r(0), g(0), b(0), a(255) {}

    // Constructor with integer RGB and optional alpha values
    Color(int red, int green, int blue, int alpha = 255) {
        r = static_cast<Uint8>(std::min(std::max(red, 0), 255));
        g = static_cast<Uint8>(std::min(std::max(green, 0), 255));
        b = static_cast<Uint8>(std::min(std::max(blue, 0), 255));
        a = static_cast<Uint8>(std::min(std::max(alpha, 0), 255));
    }

    // Constructor with floating-point RGB and optional alpha values
    Color(float red, float green, float blue, float alpha = 1.0f) {
        r = static_cast<Uint8>(std::min(std::max(red * 255, 0.0f), 255.0f));
        g = static_cast<Uint8>(std::min(std::max(green * 255, 0.0f), 255.0f));
        b = static_cast<Uint8>(std::min(std::max(blue * 255, 0.0f), 255.0f));
        a = static_cast<Uint8>(std::min(std::max(alpha * 255, 0.0f), 255.0f));
    }

    // Overload the + operator to add colors with clamping to avoid overflow
    Color operator+(const Color& other) const {
        return Color(
            std::min(255, int(r) + int(other.r)),
            std::min(255, int(g) + int(other.g)),
            std::min(255, int(b) + int(other.b)),
            std::min(255, int(a) + int(other.a))
        );
    }

    // Overload the * operator to scale color components by a floating-point factor
    Color operator*(float factor) const {
        return Color(
            std::clamp(static_cast<int>(r * factor), 0, 255),
            std::clamp(static_cast<int>(g * factor), 0, 255),
            std::clamp(static_cast<int>(b * factor), 0, 255),
            std::clamp(static_cast<int>(a * factor), 0, 255)
        );
    }

    // Friend function to allow scaling of colors by a factor using multiplication from the left
    friend Color operator*(float factor, const Color& color) {
        return color * factor; 
    }
};
