#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

// Struct to represent the indices of a single polygon face in a 3D model
struct Face
{
  std::array<int, 3> vertexIndices;
  std::array<int, 3> normalIndices;
  std::array<int, 3> texIndices;
};

// Declares a function to load 3D model data from an OBJ file
bool loadOBJ(
  const char *path,                        // Path to the OBJ file
  std::vector<glm::vec3> &out_vertices,    // Container for model vertices
  std::vector<glm::vec3> &out_normals,     // Container for vertex normals
  std::vector<glm::vec3> &out_texcoords,   // Container for texture coordinates
  std::vector<Face>& out_faces             // Container for faces of the model
);
