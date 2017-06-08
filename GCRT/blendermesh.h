#pragma once

#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BlenderMesh : Mesh
{
    uint32_t numVerts;

    void Create(string file);
    void Draw();
};