#include "RTMesh.h"

/**
 * [RTSphere::Intersect description]
 * @param  ray [description]
 */

void RTSphere::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;

    dvec3 d = ray.dir;
    dvec3 o = ray.org;
    dvec3 cnt = orgn;

    double a = dot(d, d);
    double b = 2.0 * dot(d, o - cnt);
    double c = dot(o - cnt, o - cnt) - r * r;

    double det = b * b - 4.0 * a * c;

    if (det < 0.0)
    {
        return;
    }

    double t1 = (-b - sqrt(det)) / (2.0 * a);
    double t2 = (-b + sqrt(det)) / (2.0 * a);

    if (t1 > 0.0)
    {
        intsc.t = t1;
        intsc.normal = normalize(o + t1 * d - orgn);
        intsc.mat = mat;
        return;
    }

    if (t2 > 0.0)
    {
        intsc.t = t2;
        intsc.normal = normalize(o + t2 * d - orgn);
        intsc.mat = mat;
        return;
    }
}

/**
 * [RTPlane::Intersect description]
 * @param  ray [description]
 */

void RTPlane::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    intsc.normal = dvec3(normal.x, normal.y, normal.z);

    dvec3 n = dvec3(normal.x, normal.y, normal.z);

    if (dot(n, ray.dir) == 0.0)
    {
        return;
    }

    double d = normal.w;
    intsc.t = (d - dot(n, ray.org)) / (dot(n, ray.dir));
    intsc.mat = mat;
}

/**
 * [Submesh::Intersect description]
 * @param ray  [description]
 * @param intc [description]
 */

void Submesh::Intersect(Ray ray, Intersection &intsc)
{
    // Triangle intersection method using barycentric coordinates.
    // Adapted from the PBRT book.

    intsc.t = DBL_MAX;
    vector<uint32_t> faceIdcs;
    faceIdcs.reserve(1024);
    root.Intersect(ray, faceIdcs);

    for (uint32_t i = 0; i < faceIdcs.size(); i++)
    {
        uvec3 curFace = faces[faceIdcs[i]];

        dvec3 p0 = pos[curFace.x];
        dvec3 p1 = pos[curFace.y];
        dvec3 p2 = pos[curFace.z];

        dvec3 e1 = p1 - p0;
        dvec3 e2 = p2 - p0;
        dvec3 s = ray.org - p0;

        dvec3 s1 = cross(ray.dir, e2);
        dvec3 s2 = cross(s, e1);

        double a = 1.0 / dot(s1, e1);

        double t = a * dot(s2, e2);
        double b1 = a * dot(s1, s);
        double b2 = a * dot(s2, ray.dir);

        if (b1 < 0.0 || b2 < 0.0 || (b1 + b2 > 1.0))
        {
            continue;
        }

        if (t > 0.0 && t < intsc.t)
        {
            intsc.t = t;
            intsc.mat = mat;
            intsc.normal =
                (1.0 - b1 - b2) * norm[curFace.x] +
                b1 * norm[curFace.y] +
                b2 * norm[curFace.z];
        }
    }
}

/**
 * [RTMesh::Intersect description]
 * @param ray  [description]
 * @param intc [description]
 */

void RTMesh::Intersect(Ray ray, Intersection &intsc)
{
    double minDist = DBL_MAX;
    intsc.t = -1.0;

    for (uint32_t i = 0; i < submeshes.size(); i++)
    {
        Intersection subMeshIntsc;
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
 * [RTMesh::LoadModel description]
 * @param file [description]
 */

void RTMesh::LoadModel(string file)
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
        submeshes[i].mat = "Glass";

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
 * [RTBox::Intersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void RTBox::Intersect(Ray ray, Intersection &intsc)
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