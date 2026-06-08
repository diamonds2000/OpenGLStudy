#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "render.h"
#include <string>

// Load a Wavefront OBJ file and create render data from it.
// Returns a RenderData with vertex positions and normals computed from the mesh.
RenderData create_from_obj(const std::string& filepath);

// Convenience: create the Stanford Bunny render data.
// If the .obj file doesn't exist at the given path, it returns an empty RenderData (vertexCount=0).
RenderData create_bunny(const std::string& objPath = "bunny.obj");

#endif // MODEL_LOADER_H
