#pragma once

#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BlenderMesh : Mesh
{
    void Create(string file);
    void Draw();
};