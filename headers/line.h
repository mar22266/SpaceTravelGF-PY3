#pragma once  

#include <vector>  
#include <glm/glm.hpp>  
#include "fragment.h"  

// Function to generate line fragments between two 3D points projected onto a 2D plane
std::vector<Fragment> line(const glm::vec3& v1, const glm::vec3& v2) {
    // Convert 3D points to 2D points for line drawing
    glm::ivec2 p1(static_cast<int>(v1.x), static_cast<int>(v1.y));
    glm::ivec2 p2(static_cast<int>(v2.x), static_cast<int>(v2.y));

    std::vector<Fragment> fragments;  // Container to hold the line fragments

    // Calculate the differences and steps
    int dx = std::abs(p2.x - p1.x);
    int dy = std::abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;  // Determine the direction of the step in x
    int sy = (p1.y < p2.y) ? 1 : -1;  // Determine the direction of the step in y

    int err = dx - dy;  // Initial error term

    glm::ivec2 current = p1;  // Start from the first point

    while (true) {
        // Create a fragment at the current position
        Fragment fragment;
        fragment.x = current.x;
        fragment.y = current.y;
        fragments.push_back(fragment);  // Add the fragment to the list

        if (current == p2) {  // Stop if the end point is reached
            break;
        }

        // Bresenham's error handling and point stepping
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            current.x += sx;  // Step in x
        }
        if (e2 < dx) {
            err += dx;
            current.y += sy;  // Step in y
        }
    }

    return fragments;  // Return the list of fragments forming the line
}
