#include "RTMesh.h"

/**
 * [RTSphere::Intersect description]
 * @param  ray [description]
 */

void RTSphere::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t     = -1.0;
    dvec3 d     = ray.dir;
    dvec3 o     = ray.org;
    dvec3 cnt   = orgn;

    double a    = dot(d, d);
    double b    = 2.0 * dot(d, o - cnt);
    double c    = dot(o - cnt, o - cnt) - r * r;

    double det = b * b - 4.0 * a * c;

    if (det < 0.0)
    {
        return;
    }

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

        if (n.y < 0.0 && n.x < 0.0)
        {
            intsc.uv.y += glm::pi<double>();
        }
        else if (n.y < 0.0)
        {
            intsc.uv.y += glm::pi<double>() / 2.0;
        }
        else if (n.x < 0.0)
        {
            intsc.uv.y -= glm::pi<double>() / 2.0;
        }

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

        if (n.y < 0.0 && n.x < 0.0)
        {
            intsc.uv.y += glm::pi<double>();
        }
        else if (n.y < 0.0)
        {
            intsc.uv.y += glm::pi<double>() / 2.0;
        }
        else if (n.x < 0.0)
        {
            intsc.uv.y -= glm::pi<double>() / 2.0;
        }

        intsc.uv.y /= (2.0 * glm::pi<double>());

        return;
    }
}

/**
 * [RTSphere::UpdateAnimation description]
 * @param  t [description]
 */

void RTSphere::UpdateAnimation(double t)
{
    dmat4 pose = animation.GetPose(t);
    dvec4 orgn4 = dvec4(0.0, 0.0, 0.0, 1.0);
    orgn4 = pose * orgn4;

    orgn.x = orgn4.x;
    orgn.y = orgn4.y;
    orgn.z = orgn4.z;
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
    intsc.t = DBL_MAX;
    uint32_t faceCnt = 0;

    uint32_t faceIdcs[1024];

    root.Intersect(ray, faceIdcs, faceCnt);

    for (uint32_t i = 0; i < faceCnt; i++)
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

            intsc.tan =
                (1.0 - b1 - b2) * tan[curFace.x] +
                b1 * tan[curFace.y] +
                b2 * tan[curFace.z];
        }
    }
}

/**
 * [AssimpMesh::Intersect description]
 * @param ray  [description]
 * @param intc [description]
 */

void SkeletalMesh::Intersect(Ray ray, Intersection &intsc)
{
    double minDist = DBL_MAX;
    intsc.t = -1.0;
    Intersection subMeshIntsc;

    bbox.Intersect(ray, subMeshIntsc);
    if (subMeshIntsc.t <= 0.0)
    {
        return;
    }

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
 * [CornellBox::Intersect description]
 * @param ray  [description]
 * @param intc [description]
 */

void CornellBox::Intersect(Ray ray, Intersection &intsc)
{
    double minDist = DBL_MAX;
    intsc.t = -1.0;
    Intersection subMeshIntsc;

    bbox.Intersect(ray, subMeshIntsc);
    if (subMeshIntsc.t <= 0.0)
    {
        return;
    }

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
 * [RTMesh::CreateCornellBox description]
 */

void CornellBox::Create()
{
    submeshes.resize(5);
    
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

    submeshes[0].mat = 4;

    // floor

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

    submeshes[1].mat = 4;

    // left wall

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

    submeshes[2].mat = 3;

    // right wall

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

    submeshes[3].mat = 2;

    // back wall

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

    submeshes[4].mat = 4;
}

/**
 * [SkeletalMesh::LoadModel description]
 * @param file [description]
 */

void SkeletalMesh::LoadModel(string file)
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

/**
 * [JSONMesh::LoadModel description]
 * @param file [description]
 */

void JSONMesh::LoadModel(string file)
{
    return;
}

/**
 * [JSONMesh::Intersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void JSONMesh::Intersect(Ray ray, Intersection &intsc)
{
    return;
}