#include "model_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include <glm/glm.hpp>

// ---------------------------------------------------------------------------
// Simple Wavefront OBJ loader
// Supports: v, vn, f (triangles only, with or without texcoord/normal refs)
// Computes smooth (per-vertex) normals if the file has no vn entries.
// ---------------------------------------------------------------------------
RenderData create_from_obj(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "[OBJ] Failed to open: " << filepath << std::endl;
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> fileNormals;
    bool hasFileNormals = false;

    // Each face vertex: (position_index, normal_index)
    // normal_index < 0 means "not specified"
    struct FaceVert { int p; int n; };
    std::vector<FaceVert> faceVerts;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tok;
        iss >> tok;

        if (tok == "v")
        {
            glm::vec3 p;
            iss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tok == "vn")
        {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            fileNormals.push_back(glm::normalize(n));
            hasFileNormals = true;
        }
        else if (tok == "f")
        {
            // Parse each vertex reference in the face
            std::string part;
            while (iss >> part)
            {
                FaceVert fv = {0, -1};
                // Split on '/' — handles: v, v/t, v//n, v/t/n
                std::vector<std::string> parts;
                size_t start = 0, end;
                while ((end = part.find('/', start)) != std::string::npos)
                {
                    parts.push_back(part.substr(start, end - start));
                    start = end + 1;
                }
                parts.push_back(part.substr(start));

                // parts[0] = vertex index
                fv.p = std::stoi(parts[0]) - 1; // OBJ is 1-based
                if (parts.size() >= 3 && !parts[2].empty())
                {
                    fv.n = std::stoi(parts[2]) - 1;
                }
                faceVerts.push_back(fv);
            }
        }
    }
    file.close();

    if (positions.empty())
    {
        std::cerr << "[OBJ] No vertices in: " << filepath << std::endl;
        return {};
    }

    // ---- Compute smooth normals if needed ----
    std::vector<glm::vec3> computedNormals;
    if (!hasFileNormals)
    {
        computedNormals.resize(positions.size(), glm::vec3(0.0f));
        // Iterate over triangles (groups of 3 face vertices)
        for (size_t i = 0; i + 2 < faceVerts.size(); i += 3)
        {
            const glm::vec3& v0 = positions[faceVerts[i+0].p];
            const glm::vec3& v1 = positions[faceVerts[i+1].p];
            const glm::vec3& v2 = positions[faceVerts[i+2].p];
            glm::vec3 faceN = glm::cross(v1 - v0, v2 - v0);
            float len = glm::length(faceN);
            if (len > 1e-8f) faceN /= len;
            computedNormals[faceVerts[i+0].p] += faceN;
            computedNormals[faceVerts[i+1].p] += faceN;
            computedNormals[faceVerts[i+2].p] += faceN;
        }
        for (auto& n : computedNormals)
        {
            float len = glm::length(n);
            if (len > 1e-8f) n /= len; else n = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }

    // ---- Expand face indices into flat vertex/normal arrays ----
    std::vector<float> verts;
    std::vector<float> norms;
    verts.reserve(faceVerts.size() * 3);
    norms.reserve(faceVerts.size() * 3);

    for (size_t i = 0; i < faceVerts.size(); i += 3)
    {
        for (int k = 0; k < 3; ++k)
        {
            const auto& fv = faceVerts[i + k];
            const glm::vec3& pos = positions[fv.p];
            verts.push_back(pos.x);
            verts.push_back(pos.y);
            verts.push_back(pos.z);

            glm::vec3 n;
            if (hasFileNormals)
                n = (fv.n >= 0) ? fileNormals[fv.n] : glm::vec3(0.0f, 1.0f, 0.0f);
            else
                n = computedNormals[fv.p];
            norms.push_back(n.x);
            norms.push_back(n.y);
            norms.push_back(n.z);
        }
    }

    int vertexCount = static_cast<int>(verts.size() / 3);
    RenderData rd = create_render_data(verts.data(), norms.data(), vertexCount);

    std::cout << "[OBJ] Loaded " << filepath << ": "
              << positions.size() << " unique vertices, "
              << vertexCount / 3 << " triangles" << std::endl;

    return rd;
}


