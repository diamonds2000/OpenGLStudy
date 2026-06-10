#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "render.h"
#include <string>

RenderData create_triangle();
RenderData create_cube(float size);
RenderData create_sphere(float radius, int slices, int stacks);
RenderData create_bunny(const std::string& objPath);

#endif // GEOMETRY_H