#include "perlinterrain.h"

#if 0

/**
 * PerlinTerrain::Create Create a heightmap grid with spacing 1/ndiv, and height
 * generated with perlin noise.
 *
 * @param ndiv Number of divisions for grid spacing.
 */

void PerlinTerrain::Create(uint32_t ndiv)
{
    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;

    spacing = 1.0f / (float)ndiv;
}

/**
 * [PerlinTerrain::GenPositions description]
 * @param pos [description]
 */

void PerlinTerrain::GenPositions(vector<vec3> &pos)
{
    float dx = 0.0;
    while (dx <= 1.0)
    {
        float dy = 0.0;
        
        while (dy <= 1.0)
        {
            dy += spacing;
            //pos.push_back(vec3(dx, dy, Perlin2D(dx, dy)));
			pos.push_back(vec3(dx, dy, 1.0));
        }

        dx += spacing;
    }
}

/**
 * [PerlinTerrain::GenNormals description]
 * @param norms [description]
 */

void PerlinTerrain::GenNormals(vector<vec3> &norms)
{
 
}

/**
 * [PerlinTerrain::GenUVs description]
 * @param uvs [description]
 */

void PerlinTerrain::GenUVs(vector<vec2> &uvs)
{
    
}

/**
 * [PerlinTerrain::GenTans description]
 * @param tans [description]
 */

void PerlinTerrain::GenTans(vector<vec3> &tans)
{
    
}

/**
 * [PerlinTerrain::Draw description]
 */

void PerlinTerrain::Draw()
{

}

#endif