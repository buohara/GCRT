#include "geometry.h"

/**
 * InitVertexObjects -
 *
 * @param verts
 * @param norms
 * @param norms
 * @param tans
 */

void Geometry::InitVertexObjects(
    vector<vec3> &pos, 
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans
)
{
    uint32_t numVerts = pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);
    size_t tanBufSize = 3 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &tanVboID);
    glBindBuffer(GL_ARRAY_BUFFER, tanVboID);
    glBufferData(GL_ARRAY_BUFFER, tanBufSize, &tans[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

/**
* InitVertexObjects -
*
* @param verts
* @param norms
* @param norms
*/

void Geometry::InitVertexObjects(
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs
)
{
    uint32_t numVerts = pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

/**
 * InitVertexObjects -
 */

void Geometry::InitModelMatrices()
{
    scl = mat4(1.0f);
    rot = mat4(1.0f);
    trans = mat4(1.0f);

    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Translate -
 */

void Geometry::Translate(vec3 tx)
{
    trans = translate(tx);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Scale -
 */

void Geometry::Scale(vec3 dims)
{
    scl = scale(mat4(1.0f), dims);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Check if a ray intersects a plane.
 */

float IntersectPlane(
    vec3 org,
    vec3 ray,
    vec3 p,
    vec3 norm,
    mat4 modelInv,
    vec3 checkBounds
)
{
    float d = dot(p - org, norm) / dot(ray, norm);

    if (d < 0.0)
    {
        return -1.0;
    }

    vec4 intsct = modelInv * vec4(org + d * ray, 1);
    bool intersects = true;

    if (checkBounds.x != 0.0)
    {
        intersects &= (intsct.x > -1.0) && (intsct.x < 1.0);
    }

    if (checkBounds.y != 0.0)
    {
        intersects &= (intsct.y > -1.0) && (intsct.y < 1.0);
    }

    if (checkBounds.z != 0.0)
    {
        intersects &= (intsct.z > -1.0) && (intsct.z < 1.0);
    }

    if (intersects == true)
    {
        return d;
    }
    else
    {
        return -1.0;
    }
}