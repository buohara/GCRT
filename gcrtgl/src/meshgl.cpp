#include "meshgl.h"

MeshGL::MeshGL(MeshType type, uint32_t rows, uint32_t cols) : 
    pos(vec3(0.0)), dims(vec3(1.0)), angles(vec3(0.0)),
    scl(mat4(1.0)), rot(mat4(1.0)), trans(mat4(1.0)),
    model(mat4(1.0)), modelInv(mat4(1.0)), animated(false)
{
    assert(type == PLANE);

    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositionsPlane(pos, rows, cols);
    GenNormalsPlane(norms, rows, cols);
    GenUVsPlane(uvs, rows, cols);
    GenTangentsPlane(tans, rows, cols);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    numVerts = (uint32_t)pos.size();
    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, boneIDs, boneWts);
}

MeshGL::MeshGL(MeshType type, uint32_t numSectors) :
    pos(vec3(0.0)), dims(vec3(1.0)), angles(vec3(0.0)),
    scl(mat4(1.0)), rot(mat4(1.0)), trans(mat4(1.0)),
    model(mat4(1.0)), modelInv(mat4(1.0)), animated(false)
{
    assert(type == CYLINDER);

    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositionsCylinder(pos, numSectors, numSideVerts, topOffset, bottomOffset, numCapVerts);
    GenNormalsCylinder(norms, numSectors);
    GenUVsCylinder(uvs, numSectors);
    GenTansCylinder(tans, numSectors);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

MeshGL::MeshGL(MeshType type, string file) :
    pos(vec3(0.0)), dims(vec3(1.0)), angles(vec3(0.0)),
    scl(mat4(1.0)), rot(mat4(1.0)), trans(mat4(1.0)),
    model(mat4(1.0)), modelInv(mat4(1.0)), animated(false)
{
    assert(type == SKELETAL);

    Assimp::Importer importer;

    const aiScene& scene = *(importer.ReadFile(
        file,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
    ));

    subMeshes.resize(scene.mNumMeshes);
    animated = (scene.mNumAnimations > 0);
    map<string, mat4> boneOffsets;

    LoadVertexAndBoneData(scene, boneOffsets);

    if (animated == true)
    {
        aiNode& scnRoot = *(scene.mRootNode);
        skeleton.root.name = scnRoot.mName.C_Str();

        skeleton.globalInverse = aiMatrix4x4ToGlm(scnRoot.mTransformation.Inverse());
        CreateBoneHierarchy(scnRoot, skeleton.root, boneOffsets);
        LoadAnimations(scene, skeleton.root);
    }
}

MeshGL::MeshGL(MeshType type) : scl(mat4(1.0)), rot(mat4(1.0)), trans(mat4(1.0)),
model(mat4(1.0)), modelInv(mat4(1.0)), animated(false)
{
    assert(type == BOX);

    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositionsBox(pos);
    GenNormalsBox(norms);
    GenUVsBox(uvs);
    GenTansBox(tans);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    numVerts = (uint32_t)pos.size();

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

MeshGL::MeshGL(MeshType type, uint32_t numSectors, uint32_t numRings, bool invertIn)
{
    assert(type == SPHERE);

    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;
    invert = invertIn;

    GenPositionsSphere(pos, numSectors, numRings, numSideVerts, topOffset, bottomOffset, numCapVerts);
    GenNormalsSphere(norms, numSectors, numRings);
    GenUVsSphere(uvs, numSectors, numRings);
    GenTansSphere(tans, numSectors, numRings);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

/**
 * MeshGL::Translate Translate a model.
 *
 * @param tx Model's new position.
 */

void MeshGL::Translate(vec3 tx)
{
    pos = tx;
    trans = translate(tx);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * MeshGL::Scale Scale a model up/down.
 *
 * @param dimsIn Model's new (x,y,z) dimensions.
 */

void MeshGL::Scale(vec3 dimsIn)
{
    dims = dimsIn;
    scl = scale(mat4(1.0f), dimsIn);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * MeshGL::Rotate rotate a model about (x, y, z) axes.
 *
 * @param thetas Amount of rotation (in radians) to rotate model around each
 * axis.
 */

void MeshGL::Rotate(vec3 thetas)
{
    angles = thetas;
    mat4 rotx = rotate(thetas[0], vec3(1.0, 0.0, 0.0));
    mat4 roty = rotate(thetas[1], vec3(0.0, 1.0, 0.0));
    mat4 rotz = rotate(thetas[2], vec3(0.0, 0.0, 1.0));

    rot = rotz * roty * rotx;
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * MeshGL::SetAnimMatrices After updating animation, load bone matrices to GPU.
 *
 * @param renderProgram Shader program to set bones for.
 */

void MeshGL::SetAnimMatrices(GLuint renderProgram)
{
    uint32_t numBones = (uint32_t)skeleton.bones.size();
    GLuint bonesID = glGetUniformLocation(renderProgram, "bones");
    glUniformMatrix4fv(bonesID, numBones, false, (GLfloat*)&skeleton.bones[0]);
}

void MeshGL::Draw()
{
    switch (type)
    {
    case BOX:

        glBindVertexArray(subMeshes[0].vaoID);
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
        glBindVertexArray(0);
        break;

    case PLANE:

        glBindVertexArray(subMeshes[0].vaoID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
        glBindVertexArray(0);
        break;

    case CYLINDER:

        glBindVertexArray(subMeshes[0].vaoID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
        glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
        glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
        glBindVertexArray(0);
        break;

    case SPHERE:

        glBindVertexArray(subMeshes[0].vaoID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
        glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
        glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
        glBindVertexArray(0);
        break;

    case SKELETAL:

        for (uint32_t i = 0; i < subMeshes.size(); i++)
        {
            glBindVertexArray(subMeshes[i].vaoID);
            glDrawElements(GL_TRIANGLES, subMeshes[i].numIdcs, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        break;

    default:

        break;
    }
}

/**
 * MeshGL::LoadVertexAndBoneData Load vertex and bone data from an Assimp mesh.
 *
 * @param scene       Input Assimp mesh to load (in)
 * @param boneOffsets List of local bone offsets from mesh space to local space (out).
 */

void MeshGL::LoadVertexAndBoneData(
    const aiScene& scene,
    map<string, mat4>& boneOffsets
)
{
    for (uint32 i = 0; i < scene.mNumMeshes; i++)
    {
        aiMesh& mesh = *(scene.mMeshes[i]);

        vector<vec3> pos;
        vector<vec3> norm;
        vector<vec2> uv;
        vector<uint32_t> idcs;
        vector<ivec4> boneIDs;
        vector<vec4> boneWeights;

        subMeshes[i].numVerts = mesh.mNumVertices;
        subMeshes[i].numIdcs = 3 * mesh.mNumFaces;

        LoadVertexData(mesh, pos, norm, uv, idcs);

        if (animated == false)
        {
            boneIDs.resize(pos.size(), ivec4(0));
            boneWeights.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

            InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
            continue;
        }

        // Bone IDs and weights.

        LoadBoneData(mesh, boneIDs, boneWeights, boneOffsets);
        InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
    }
}

/**
 * Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
 *
 * @param subMeshIdx  Mesh submesh index (for composite meshes).
 * @param pos         Vertex positions.
 * @param norms       Vertex normals.
 * @param uvs         Vertex UVs.
 * @param tans        Vertex tangents.
 * @param boneIDs     Vertex bones.
 * @param boneWeights Vertex bone weights.
 */

void MeshGL::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos, 
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize    = 3 * numVerts * sizeof(float);
    size_t normBufSize   = 3 * numVerts * sizeof(float);
    size_t uvBufSize     = 2 * numVerts * sizeof(float);
    size_t tanBufSize    = 3 * numVerts * sizeof(float);
    size_t boneIDBufSize = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].tanVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].tanVboID);
    glBufferData(GL_ARRAY_BUFFER, tanBufSize, &tans[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

/**
 * Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
 *
 * @param subMeshIdx  Mesh submesh index (for composite meshes).
 * @param pos         Vertex positions.
 * @param norms       Vertex normals.
 * @param uvs         Vertex UVs.
 * @param boneIDs     Vertex bones.
 * @param boneWeights Vertex bone weights.
 */

void MeshGL::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize    = 3 * numVerts * sizeof(float);
    size_t normBufSize   = 3 * numVerts * sizeof(float);
    size_t uvBufSize     = 2 * numVerts * sizeof(float);
    size_t boneIDBufSize = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

/**
* Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
*
* @param subMeshIdx  Mesh submesh index (for composite meshes).
* @param pos         Vertex positions.
* @param norms       Vertex normals.
* @param uvs         Vertex UVs.
* @param idcs        Triangle vertex indices.
* @param boneIDs     Vertex bones.
* @param boneWeights Vertex bone weights.
*/

void MeshGL::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<uint32_t> &idcs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts       = (uint32_t)pos.size();
    uint32_t numIdcs        = (uint32_t)idcs.size();

    size_t posBufSize       = 3 * numVerts * sizeof(float);
    size_t normBufSize      = 3 * numVerts * sizeof(float);
    size_t uvBufSize        = 2 * numVerts * sizeof(float);
    size_t idxBufSize       = numIdcs * sizeof(uint32_t);
    size_t boneIDBufSize    = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize    = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glGenBuffers(1, &subMeshes[subMeshIdx].idxVboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[subMeshIdx].idxVboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBufSize, &idcs[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}