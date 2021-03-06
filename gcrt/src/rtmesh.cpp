#include "RTMesh.h"

/**
 * RTMesh::SphereIntersect - Intersect a ray with a sphere by solving quadratic equation.
 *
 * @param ray   Ray to intersect with this sphere.
 * @param intsc Closest intersection point of ray, if it hits.
 */

void RTMesh::SphereIntersect(Ray ray, Intersection &intsc)
{
    intsc.t     = -1.0;
    dvec3 d     = ray.dir;
    dvec3 o     = ray.org;
    dvec3 cnt   = orgn;

    double a    = dot(d, d);
    double b    = 2.0 * dot(d, o - cnt);
    double c    = dot(o - cnt, o - cnt) - r * r;

    double det = b * b - 4.0 * a * c;

    if (det < 0.0) return;

    double t1 = (-b - sqrt(det)) / (2.0 * a);
    double t2 = (-b + sqrt(det)) / (2.0 * a);

    if (t1 > 0.0)
    {
        dvec3 n         = normalize(o + t1 * d - orgn);
        intsc.t         = t1;
        intsc.normal    = n;
        intsc.tan       = dvec3(n.y, -n.x, 0.0);
        intsc.mat       = mat;
        
        intsc.uv.x      = acos(n.z / r) / glm::pi<double>();
        intsc.uv.y      = atan2(n.y, n.x);

        if (n.y < 0.0 && n.x < 0.0) intsc.uv.y += glm::pi<double>();
        else if (n.y < 0.0) intsc.uv.y += glm::pi<double>() / 2.0;
        else if (n.x < 0.0) intsc.uv.y -= glm::pi<double>() / 2.0;

        intsc.uv.y /= (2.0 * glm::pi<double>());

        return;
    }

    if (t2 > 0.0)
    {
        dvec3 n         = normalize(o + t1 * d - orgn);
        intsc.t         = t2;
        intsc.normal    = n;
        intsc.tan       = dvec3(n.y, -n.x, 0.0);
        intsc.mat       = mat;
        
        intsc.uv.x      = acos(n.z / r) / glm::pi<double>();
        intsc.uv.y      = atan2(n.y, n.x);

        if (n.y < 0.0 && n.x < 0.0) intsc.uv.y += glm::pi<double>();
        else if (n.y < 0.0) intsc.uv.y += glm::pi<double>() / 2.0;
        else if (n.x < 0.0) intsc.uv.y -= glm::pi<double>() / 2.0;

        intsc.uv.y /= (2.0 * glm::pi<double>());

        return;
    }
}

/**
 * [RTMesh::PlaneIntersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void RTMesh::PlaneIntersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    intsc.normal = dvec3(normal.x, normal.y, normal.z);

    dvec3 n = dvec3(normal.x, normal.y, normal.z);

    if (dot(n, ray.dir) == 0.0) return;

    double d = normal.w;
    intsc.t = (d - dot(n, ray.org)) / (dot(n, ray.dir));
    intsc.mat = mat;
}

/**
 * Submesh::Intersect Check if ray intersects this submesh.
 * @param ray  Ray to intersect.
 * @param intc Intersection point information.
 */

void Submesh::Intersect(Ray ray, Intersection &intsc)
{
    root.Intersect(ray, pos, norm, tan, faces, intsc);
    intsc.mat = mat;
}

/**
 * [RTMesh::SkeletalMeshIntersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void RTMesh::SkeletalMeshIntersect(Ray ray, Intersection &intsc)
{
    double minDist = DBL_MAX;
    intsc.t = -1.0;
    Intersection subMeshIntsc;

    bbox.Intersect(ray, subMeshIntsc);
    if (subMeshIntsc.t <= 0.0) return;

    for (uint32_t i = 0; i < submeshes.size(); i++)
    {
        submeshes[i].root.box.Intersect(ray, subMeshIntsc);

        if (subMeshIntsc.t > 0.0)
        {
            submeshes[i].Intersect(ray, subMeshIntsc);
            if (subMeshIntsc.t > 0.0 && subMeshIntsc.t < minDist)
            {
                minDist = subMeshIntsc.t;
                intsc = subMeshIntsc;
            }
        }
    }
}

/**
 * [RTMesh::CornellBoxIntersect description]
 *
 * @param ray   [description]
 * @param intsc [description]
 */

void RTMesh::CornellBoxIntersect(Ray ray, Intersection &intsc)
{
    double minDist = DBL_MAX;
    intsc.t = -1.0;
    Intersection subMeshIntsc;

    bbox.Intersect(ray, subMeshIntsc);
    if (subMeshIntsc.t <= 0.0) return;

    for (uint32_t i = 0; i < submeshes.size(); i++)
    {
        submeshes[i].root.box.Intersect(ray, subMeshIntsc);

        if (subMeshIntsc.t > 0.0)
        {
            submeshes[i].Intersect(ray, subMeshIntsc);
            if (subMeshIntsc.t > 0.0 && subMeshIntsc.t < minDist)
            {
                minDist = subMeshIntsc.t;
                intsc = subMeshIntsc;
            }
        }
    }
}

/**
 * RTMesh::CreateCornell - Create a Cornell box mesh.
 *
 * @param r Red matte material index.
 * @param g Green matte material index.
 * @param w White matte material index.
 */

void RTMesh::CreateCornell(uint32_t r, uint32_t g, uint32_t w)
{
    submeshes.resize(5);
    for (uint32_t i = 0; i < 5; i++) submeshes[i].root.depthAllowFaces = 0;
    
    dvec3 min = dvec3(-9.0, -5.0, 0.0);
    dvec3 max = dvec3(5.0, 5.0, 5.0);

    bbox.min = min - 0.1;
    bbox.max = max + 0.1;

    // ceiling

    submeshes[0].pos.push_back(dvec3(max.x, max.y, max.z));
    submeshes[0].pos.push_back(dvec3(min.x, max.y, max.z));
    submeshes[0].pos.push_back(dvec3(min.x, min.y, max.z));
    submeshes[0].pos.push_back(dvec3(max.x, min.y, max.z));

    submeshes[0].norm.push_back(dvec3(0.0, 0.0, -1.0));
    submeshes[0].norm.push_back(dvec3(0.0, 0.0, -1.0));
    submeshes[0].norm.push_back(dvec3(0.0, 0.0, -1.0));
    submeshes[0].norm.push_back(dvec3(0.0, 0.0, -1.0));

    submeshes[0].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[0].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[0].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[0].tan.push_back(dvec3(1.0, 0.0, 0.0));

    submeshes[0].root.box.min = dvec3(min.x, min.y, max.z - 0.001);
    submeshes[0].root.box.max = dvec3(max.x, max.y, max.z + 0.001);

    submeshes[0].faces.push_back(dvec3(0, 2, 1));
    submeshes[0].faces.push_back(dvec3(0, 3, 2));

    submeshes[0].root.maxDepth = 1;
    submeshes[0].root.depth = 0;

    submeshes[0].root.Insert(
        submeshes[0].pos[0], 
        submeshes[0].pos[2], 
        submeshes[0].pos[1],
        0
    );

    submeshes[0].root.Insert(
        submeshes[0].pos[0],
        submeshes[0].pos[3],
        submeshes[0].pos[2],
        1
    );

    submeshes[0].mat = w;

    // floor

    submeshes[1].root.maxDepth = 1;
    submeshes[1].root.depth = 0;

    submeshes[1].pos.push_back(dvec3(max.x, max.y, min.z));
    submeshes[1].pos.push_back(dvec3(min.x, max.y, min.z));
    submeshes[1].pos.push_back(dvec3(min.x, min.y, min.z));
    submeshes[1].pos.push_back(dvec3(max.x, min.y, min.z));

    submeshes[1].norm.push_back(dvec3(0.0, 0.0, 1.0));
    submeshes[1].norm.push_back(dvec3(0.0, 0.0, 1.0));
    submeshes[1].norm.push_back(dvec3(0.0, 0.0, 1.0));
    submeshes[1].norm.push_back(dvec3(0.0, 0.0, 1.0));

    submeshes[1].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[1].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[1].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[1].tan.push_back(dvec3(1.0, 0.0, 0.0));

    submeshes[1].root.box.min = dvec3(min.x, min.y, min.z - 0.001);
    submeshes[1].root.box.max = dvec3(max.x, max.y, min.z + 0.001);

    submeshes[1].faces.push_back(dvec3(0, 1, 2));
    submeshes[1].faces.push_back(dvec3(0, 2, 3));

    submeshes[1].root.Insert(
        submeshes[1].pos[0],
        submeshes[1].pos[1],
        submeshes[1].pos[2],
        0
    );

    submeshes[1].root.Insert(
        submeshes[1].pos[0],
        submeshes[1].pos[2],
        submeshes[1].pos[3],
        1
    );

    submeshes[1].mat = w;

    // left wall

    submeshes[2].root.maxDepth = 1;
    submeshes[2].root.depth = 0;

    submeshes[2].pos.push_back(dvec3(max.x, min.y, max.z));
    submeshes[2].pos.push_back(dvec3(min.x, min.y, max.z));
    submeshes[2].pos.push_back(dvec3(min.x, min.y, min.z));
    submeshes[2].pos.push_back(dvec3(max.x, min.y, min.z));

    submeshes[2].norm.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[2].norm.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[2].norm.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[2].norm.push_back(dvec3(0.0, 1.0, 0.0));

    submeshes[2].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[2].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[2].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[2].tan.push_back(dvec3(1.0, 0.0, 0.0));

    submeshes[2].root.box.min = dvec3(min.x, min.y - 0.001, min.z);
    submeshes[2].root.box.max = dvec3(max.x, min.y + 0.001, max.z);

    submeshes[2].faces.push_back(dvec3(0, 1, 2));
    submeshes[2].faces.push_back(dvec3(0, 2, 3));

    submeshes[2].root.Insert(
        submeshes[2].pos[0],
        submeshes[2].pos[1],
        submeshes[2].pos[2],
        0
    );

    submeshes[2].root.Insert(
        submeshes[2].pos[0],
        submeshes[2].pos[2],
        submeshes[2].pos[3],
        1
    );

    submeshes[2].mat = r;

    // right wall

    submeshes[3].root.maxDepth = 1;
    submeshes[3].root.depth = 0;

    submeshes[3].pos.push_back(dvec3(max.x, max.y, max.z));
    submeshes[3].pos.push_back(dvec3(min.x, max.y, max.z));
    submeshes[3].pos.push_back(dvec3(min.x, max.y, min.z));
    submeshes[3].pos.push_back(dvec3(max.x, max.y, min.z));

    submeshes[3].norm.push_back(dvec3(0.0, -1.0, 0.0));
    submeshes[3].norm.push_back(dvec3(0.0, -1.0, 0.0));
    submeshes[3].norm.push_back(dvec3(0.0, -1.0, 0.0));
    submeshes[3].norm.push_back(dvec3(0.0, -1.0, 0.0));

    submeshes[3].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[3].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[3].tan.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[3].tan.push_back(dvec3(1.0, 0.0, 0.0));

    submeshes[3].root.box.min = dvec3(min.x, max.y - 0.001, min.z);
    submeshes[3].root.box.max = dvec3(max.x, max.y + 0.001, max.z);

    submeshes[3].faces.push_back(dvec3(0, 1, 2));
    submeshes[3].faces.push_back(dvec3(0, 2, 3));

    submeshes[3].root.Insert(
        submeshes[3].pos[0],
        submeshes[3].pos[1],
        submeshes[3].pos[2],
        0
    );

    submeshes[3].root.Insert(
        submeshes[3].pos[0],
        submeshes[3].pos[2],
        submeshes[3].pos[3],
        1
    );

    submeshes[3].mat = g;

    // back wall

    submeshes[4].root.maxDepth = 1;
    submeshes[4].root.depth = 0;

    submeshes[4].pos.push_back(dvec3(min.x, max.y, max.z));
    submeshes[4].pos.push_back(dvec3(min.x, min.y, max.z));
    submeshes[4].pos.push_back(dvec3(min.x, min.y, min.z));
    submeshes[4].pos.push_back(dvec3(min.x, max.y, min.z));

    submeshes[4].norm.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[4].norm.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[4].norm.push_back(dvec3(1.0, 0.0, 0.0));
    submeshes[4].norm.push_back(dvec3(1.0, 0.0, 0.0));

    submeshes[4].tan.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[4].tan.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[4].tan.push_back(dvec3(0.0, 1.0, 0.0));
    submeshes[4].tan.push_back(dvec3(0.0, 1.0, 0.0));

    submeshes[4].root.box.min = dvec3(min.x - 0.1, min.y, min.z);
    submeshes[4].root.box.max = dvec3(min.x + 0.1, max.y, max.z);

    submeshes[4].faces.push_back(dvec3(0, 1, 2));
    submeshes[4].faces.push_back(dvec3(0, 2, 3));

    submeshes[4].root.Insert(
        submeshes[4].pos[0],
        submeshes[4].pos[1],
        submeshes[4].pos[2],
        0
    );

    submeshes[4].root.Insert(
        submeshes[4].pos[0],
        submeshes[4].pos[2],
        submeshes[4].pos[3],
        1
    );

    submeshes[4].mat = w;
}

/**
 * [RTMesh::LoadAssimpModel description]
 * @param file [description]
 */

void RTMesh::LoadAssimpModel(string file)
{
    Assimp::Importer importer;

    const aiScene &scene = *(importer.ReadFile(
        file,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
    ));

    dvec3 meshmin = dvec3(DBL_MAX, DBL_MAX, DBL_MAX);
    dvec3 meshmax = dvec3(DBL_MIN, DBL_MIN, DBL_MIN);

    double scale = 1.0 / 20.0;
    submeshes.resize(scene.mNumMeshes);

    for (uint32 i = 0; i < scene.mNumMeshes; i++)
    {
        aiMesh &mesh = *(scene.mMeshes[i]);
        submeshes[i].pos.resize(mesh.mNumVertices);
        submeshes[i].norm.resize(mesh.mNumVertices);
        submeshes[i].faces.resize(mesh.mNumFaces);
        submeshes[i].tan.resize(mesh.mNumVertices);

        dvec3 submin = dvec3(DBL_MAX, DBL_MAX, DBL_MAX);
        dvec3 submax = dvec3(DBL_MIN, DBL_MIN, DBL_MIN);

        for (uint32_t j = 0; j < mesh.mNumVertices; j++)
        {
            submeshes[i].pos[j].x = mesh.mVertices[j].x * scale;
            submeshes[i].pos[j].y = mesh.mVertices[j].y * scale;
            submeshes[i].pos[j].z = mesh.mVertices[j].z * scale;

            submeshes[i].norm[j].x = mesh.mNormals[j].x;
            submeshes[i].norm[j].y = mesh.mNormals[j].y;
            submeshes[i].norm[j].z = mesh.mNormals[j].z;

            meshmax.x = glm::max<double>(submeshes[i].pos[j].x, meshmax.x);
            meshmax.y = glm::max<double>(submeshes[i].pos[j].y, meshmax.y);
            meshmax.z = glm::max<double>(submeshes[i].pos[j].z, meshmax.z);

            meshmin.x = glm::min<double>(submeshes[i].pos[j].x, meshmin.x);
            meshmin.y = glm::min<double>(submeshes[i].pos[j].y, meshmin.y);
            meshmin.z = glm::min<double>(submeshes[i].pos[j].z, meshmin.z);

            submax.x = glm::max<double>(submeshes[i].pos[j].x, submax.x);
            submax.y = glm::max<double>(submeshes[i].pos[j].y, submax.y);
            submax.z = glm::max<double>(submeshes[i].pos[j].z, submax.z);

            submin.x = glm::min<double>(submeshes[i].pos[j].x, submin.x);
            submin.y = glm::min<double>(submeshes[i].pos[j].y, submin.y);
            submin.z = glm::min<double>(submeshes[i].pos[j].z, submin.z);
        }

        submeshes[i].root.box.min = submin;
        submeshes[i].root.box.max = submax;
        submeshes[i].root.depth    = 1;
        submeshes[i].root.maxDepth = 10;
        submeshes[i].mat = 4;

        for (uint32_t j = 0; j < mesh.mNumFaces; j++)
        {
            submeshes[i].faces[j].x = mesh.mFaces[j].mIndices[0];
            submeshes[i].faces[j].y = mesh.mFaces[j].mIndices[1];
            submeshes[i].faces[j].z = mesh.mFaces[j].mIndices[2];

            dvec3 p0 = submeshes[i].pos[submeshes[i].faces[j].x];
            dvec3 p1 = submeshes[i].pos[submeshes[i].faces[j].y];
            dvec3 p2 = submeshes[i].pos[submeshes[i].faces[j].z];

            submeshes[i].root.Insert(p0, p1, p2, j);
        }
    }

    bbox.min = meshmin;
    bbox.max = meshmax;
}

/**
 * [RTMesh::BoxIntersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void RTMesh::BoxIntersect(Ray ray, Intersection &intsc)
{
    double t0 = 0.0;
    double t1 = 0.0;
    double tmin = 0.0;
    double tmax = 0.0;
    double tmp;
    double flip = 1.0;

    // x planes

    double inv = 1.0 / ray.dir.x;
    tmin = (min.x - ray.org.x) * inv;
    tmax = (max.x - ray.org.x) * inv;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    t1 = tmax;
    t0 = tmin;
    intsc.normal = flip * dvec3(1.0, 0.0, 0.0);

    // y planes

    inv = 1.0 / ray.dir.y;
    tmin = (min.y - ray.org.y) * inv;
    tmax = (max.y - ray.org.y) * inv;
    flip = 1.0;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    if (tmax < t0 || tmin > t1)
    {
        intsc.t = -1.0;
        return;
    }

    if (tmin > t0)
    {
        intsc.normal = flip * dvec3(0.0, 1.0, 0.0);
        t0 = tmin;
    }

    if (tmax < t1)
    {
        t1 = tmax;
    }

    // z planes

    inv = 1.0 / ray.dir.z;
    tmin = (min.z - ray.org.z) * inv;
    tmax = (max.z - ray.org.z) * inv;
    flip = 1.0;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    if (tmax < t0 || tmin > t1)
    {
        intsc.t = -1.0;
        return;
    }

    if (tmin > t0)
    {
        t0 = tmin;
        intsc.normal = flip * dvec3(0.0, 0.0, 1.0);
    }

    if (tmax < t1)
    {
        t1 = tmax;
    }

    intsc.t = t0;
    intsc.mat = mat;
}

/**
 * [RTMesh::LoadPLYModel description]
 * @param file [description]
 */

void RTMesh::LoadPLYModel(string &file, uint32_t mat)
{
    int nElems;
    char **eList;
    int fileType;
    float version;
    PlyFile *ply;
    int num_elems;
    int nprops;
    char** comments;
    int num_obj_info;
    char **obj_info;
    int num_comments;

    PlyProperty vert_props[] = 
    {
        { "x", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,x), 0, 0, 0, 0 },
        { "y", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,y), 0, 0, 0, 0 },
        { "z", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,z), 0, 0, 0, 0 },
    };

    PlyProperty face_props[] = 
    {
        { "vertex_indices", PLY_UCHAR, PLY_INT, offsetof(Face,verts),
        1, PLY_UCHAR, PLY_UCHAR, offsetof(Face,nverts) },
    };

    char fileName[128];
    strcpy_s(fileName, file.c_str());
    cout << "Processing PLY file: " << file << endl;
    ply = ply_open_for_reading(fileName, &nElems, &eList, &fileType, &version);

    submeshes.resize(1);
    submeshes[0].root.depth = 0;
    submeshes[0].root.maxDepth = 10;
    submeshes[0].mat = mat;
    submeshes[0].root.depthAllowFaces = 10;

    mat4 scl    = scale(dvec3(15.0));
    mat4 roty   = rotate(pi<double>() / 2.0, dvec3(0.0, 1.0, 0.0));
    mat4 rotx   = rotate(pi<double>() / 2.0, dvec3(1.0, 0.0, 0.0));
    mat4 tx     = translate(dvec3(1.5, -1.0, 0.0));
    mat4 mvp    = tx * rotx * roty * scl;

    for (int i = 0; i < nElems; i++) 
    {
        char *elem_name = eList[i];
        PlyProperty **plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        uint32_t tenPct         = num_elems / 10;
        uint32_t vertPctCnt     = tenPct;
        uint32_t facePctCnt     = tenPct;
        uint32_t vertPct        = 10;
        uint32_t facePct        = 10;

        dvec3 max = { -1000.0, -1000.0, -1000.0 };
        dvec3 min = { 1000.0, 1000.0, 1000.0 };

        if (equal_strings("vertex", elem_name)) 
        {
            vector<Vertex> vlist(num_elems);
            ply_get_property(ply, elem_name, &vert_props[0]);
            ply_get_property(ply, elem_name, &vert_props[1]);
            ply_get_property(ply, elem_name, &vert_props[2]);

            cout << "\nLoading vertices...\n" << endl;

            for (int j = 0; j < num_elems; j++) 
            {
                if (j >= (int)vertPctCnt)
                {
                    cout << "\r" << vertPct << "%";
                    vertPct += 10;
                    vertPctCnt += tenPct;
                }

                ply_get_element(ply, (void*)&vlist[j]);

                dvec4 vert = dvec4(vlist[j].x, vlist[j].y, vlist[j].z, 1.0);
                vert = mvp * vert;

                submeshes[0].pos.push_back(dvec3(vert.x, vert.y, vert.z));

                if (vert.x > max.x) max.x = vert.x;
                if (vert.x < min.x) min.x = vert.x;
                if (vert.y > max.y) max.y = vert.y;
                if (vert.y < min.y) min.y = vert.y;
                if (vert.z > max.z) max.z = vert.z;
                if (vert.z < min.z) min.z = vert.z;
            }

            submeshes[0].root.box.min = min;
            submeshes[0].root.box.max = max;

            submeshes[0].norm.resize(num_elems);
            submeshes[0].tan.resize(num_elems);

            bbox.min = min;
            bbox.max = max;
        }

        if (equal_strings("face", elem_name)) 
        {
            vector<Face> flist(num_elems);
            ply_get_property(ply, elem_name, &face_props[0]);

            cout << "\n\nLoading faces...\n" << endl;

            uint32_t faceIdx = 0;

            for (int j = 0; j < num_elems; j++) 
            {
                if (j >= (int)facePctCnt)
                {
                    cout << "\r" << facePct << "%";
                    facePct += 10;
                    facePctCnt += tenPct;
                }

                ply_get_element(ply, (void*)&flist[j]);

                dvec3 t1 = submeshes[0].pos[flist[j].verts[1]] - submeshes[0].pos[flist[j].verts[0]];
                dvec3 t2 = submeshes[0].pos[flist[j].verts[2]] - submeshes[0].pos[flist[j].verts[0]];
                dvec3 n = -normalize(cross(t2, t1));

                // Sometimes there are degenerate faces where all three vertices are the same.
                // Exclude here.

                if (isnan(n.x) || isnan(n.y) || isnan(n.z))
                {
                    continue;
                }

                submeshes[0].faces.push_back(
                    uvec3((uint32_t)flist[j].verts[0],
                    (uint32_t)flist[j].verts[1],
                    (uint32_t)flist[j].verts[2])
                );

                submeshes[0].norm[flist[j].verts[0]] = n;
                submeshes[0].norm[flist[j].verts[1]] = n;
                submeshes[0].norm[flist[j].verts[2]] = n;

                submeshes[0].tan[flist[j].verts[0]] = normalize(t1);
                submeshes[0].tan[flist[j].verts[1]] = normalize(t1);
                submeshes[0].tan[flist[j].verts[2]] = normalize(t1);

                submeshes[0].root.Insert(
                    submeshes[0].pos[flist[j].verts[0]],
                    submeshes[0].pos[flist[j].verts[1]],
                    submeshes[0].pos[flist[j].verts[2]],
                    faceIdx
                );

                faceIdx++;
            }
        }
    }

    comments = ply_get_comments(ply, &num_comments);
    obj_info = ply_get_obj_info(ply, &num_obj_info);
    ply_close(ply);
}

/**
 * RTMesh::RTMesh - Constructor to initialize a Cornell box. If only a type parameters is 
 * specified, assumed to be a Cornell box.
 */

RTMesh::RTMesh(MeshType type, uint32_t r, uint32_t g, uint32_t w) : type(type)
{
    assert(type == CORNELL);
    CreateCornell(r, g, w);
}

/**
 * RTMesh::RTMesh - Constructor to initilize a mesh from Assimp file.
 */

RTMesh::RTMesh(MeshType type, uint32_t mat, string fileName) : type(type), mat(mat)
{
    assert((type == ASSIMP) || (type == PLY));
    if (type == ASSIMP) LoadAssimpModel(fileName);
    if (type == PLY) LoadPLYModel(fileName, mat);
}

/**
 * RTMesh::RTMesh - Constructor to initialize a sphere.
 */

RTMesh::RTMesh(MeshType type, uint32_t mat, double r, dvec3 orgn) : r(r), orgn(orgn), mat(mat), type(type)
{
    assert(type == SPHERE);
}

/**
 * RTMesh::RTMesh - Constructor to initizlie a box.
 */

RTMesh::RTMesh(MeshType type, uint32_t mat, dvec3 min, dvec3 max) : min(min), max(max), type(type), mat(mat)
{
    assert(type == BOX);
}

/**
 * RTMesh::RTMesh - Constructor to initizlie a plane.
 */

RTMesh::RTMesh(MeshType type, uint32_t mat, dvec4 normal) : normal(normal), type(type), mat(mat)
{
    assert(type == PLANE);
}

/**
 * RTMesh::Intersect - Intersect a ray with this mesh. Intersection method depends on type
 * of mesh.
 *
 * @param ray   Input ray.
 * @param intsc Intersection information of ray with this mesh.
 */

void RTMesh::Intersect(Ray ray, Intersection &intsc)
{
    switch (type)
    {
    case SPHERE:

        SphereIntersect(ray, intsc);
        break;

    case PLANE:

        PlaneIntersect(ray, intsc);
        break;

    case BOX:

        BoxIntersect(ray, intsc);
        break;
    
    case PLY:
    case ASSIMP:

        SkeletalMeshIntersect(ray, intsc);
        break;

    case CORNELL:

        CornellBoxIntersect(ray, intsc);
        break;

    default:
        break;
    }
}