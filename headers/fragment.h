#pragma once  

#include <glm/glm.hpp>  
#include <cstdint>      
#include "color.h"      

// Struct to represent a vertex with essential attributes for 3D rendering
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tex;
  glm::vec3 worldPos;
  glm::vec3 originalPos;
};

// Struct to encapsulate data for fragments processed during the rasterization phase
struct Fragment {
  uint16_t x;
  uint16_t y;
  double z;
  Color color;
  float intensity;
  glm::vec3 worldPos;
  glm::vec3 originalPos;
};

// Struct for holding the final color and depth information of a fragment
struct FragColor {
  Color color;
  double z;
};

