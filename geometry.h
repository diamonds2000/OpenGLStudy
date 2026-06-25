#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "render.h"
#include <string>

RenderData create_triangle();
RenderData create_cube(float size, bool hasTangent = false);
RenderData create_sphere(float radius, int slices, int stacks);
RenderData create_bunny(const std::string& objPath);
RenderData create_skybox();

#endif // GEOMETRY_H