#include "Octree.h"

/**
 * [BBox::Intersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void BBox::Intersect(Ray ray, Intersection &intsc)
{
    double t0 = 0.0;
    double t1 = 0.0;
    double tmin = 0.0;
    double tmax = 0.0;
    double tmp;
    intsc.t = -1.0;

    // x planes

    double inv = 1.0 / ray.dir.x;
    tmin = (min.x - ray.org.x) * inv;
    tmax = (max.x - ray.org.x) * inv;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    t1 = tmax;
    t0 = tmin;

    // y planes

    inv = 1.0 / ray.dir.y;
    tmin = (min.y - ray.org.y) * inv;
    tmax = (max.y - ray.org.y) * inv;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    if (tmax < t0 || tmin > t1)
    {
        return;
    }

    t0 = tmin > t0 ? tmin : t0;
    t1 = tmax < t1 ? tmax : t1;

    // z planes

    inv = 1.0 / ray.dir.z;
    tmin = (min.z - ray.org.z) * inv;
    tmax = (max.z - ray.org.z) * inv;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    if (tmax < t0 || tmin > t1)
    {
        return;
    }

    t0 = tmin > t0 ? tmin : t0;
    t1 = tmax < t1 ? tmax : t1;

    intsc.t = (t0 < 0.0) ? t1 : t0;
}

/**
 * [BBox::Contains description]
 * @param  p0 [description]
 * @param  p1 [description]
 * @param  p2 [description]
 * @return    [description]
 */

bool BBox::Contains(dvec3 p0, dvec3 p1, dvec3 p2)
{
    if (p0.x > max.x || p0.x < min.x ||
        p0.y > max.y || p0.y < min.y ||
        p0.z > max.z || p0.z < min.z
        )
    {
        return false;
    }

    if (p1.x > max.x || p1.x < min.x ||
        p1.y > max.y || p1.y < min.y ||
        p1.z > max.z || p1.z < min.z
        )
    {
        return false;
    }

    if (p2.x > max.x || p2.x < min.x ||
        p2.y > max.y || p2.y < min.y ||
        p2.z > max.z || p2.z < min.z
        )
    {
        return false;
    }

    return true;
}

/**
 * [Insert description]
 * @param p0   [description]
 * @param p1   [description]
 * @param p2   [description]
 * @param face [description]
 */

void Octree::Insert(dvec3 p0, dvec3 p1, dvec3 p2, uint32_t face)
{
    if (depth == maxDepth)
    {
        faces.push_back(face);
        return;
    }

    bool childContainsFace = false;

    for (uint32_t x = 0; x < 2; x++)
    {
        for (uint32_t y = 0; y < 2; y++)
        {
            for (uint32_t z = 0; z < 2; z++)
            {
                uint32_t child = 4 * x + 2 * y + z;

                if (children[child] == nullptr)
                {
                    BBox childBox;

                    double hx = (box.max.x - box.min.x) / 2.0;
                    double hy = (box.max.y - box.min.y) / 2.0;
                    double hz = (box.max.z - box.min.z) / 2.0;

                    childBox.min.x = box.min.x + ((double)x) * hx;
                    childBox.min.y = box.min.y + ((double)y) * hy;
                    childBox.min.z = box.min.z + ((double)z) * hz;

                    childBox.max.x = box.min.x + ((double)(x + 1)) * hx;
                    childBox.max.y = box.min.y + ((double)(y + 1)) * hy;
                    childBox.max.z = box.min.z + ((double)(z + 1)) * hz;

                    if (childBox.Contains(p0, p1, p2) == true)
                    {
                        children[child] = make_shared<Octree>();
                        children[child]->box = childBox;
                        children[child]->depth = depth + 1;
                        children[child]->maxDepth = maxDepth;
                        children[child]->Insert(p0, p1, p2, face);
                        childContainsFace = true;
                        goto exit;
                    }

                }
                else
                {
                    if (children[child]->box.Contains(p0, p1, p2) == true)
                    {
                        children[child]->Insert(p0, p1, p2, face);
                        childContainsFace = true;
                        goto exit;
                    }
                }
            }
        }
    }

exit:

    if (childContainsFace == false)
    {
        faces.push_back(face);
    }
}

/**
 * [Intersect description]
 * @param ray      [description]
 * @param faceIdcs [description]
 */

void Octree::Intersect(Ray ray, vector<uint32_t> &faceIdcs)
{
    for (uint32_t i = 0; i < faces.size(); i++)
    {
        faceIdcs.push_back(faces[i]);
    }

    for (uint32_t x = 0; x < 2; x++)
    {
        for (uint32_t y = 0; y < 2; y++)
        {
            for (uint32_t z = 0; z < 2; z++)
            {
                uint32_t child = 4 * x + 2 * y + z;

                if (children[child] != nullptr)
                {
                    Intersection intsc;
                    children[child]->box.Intersect(ray, intsc);

                    if (intsc.t > 0.0)
                    {
                        children[child]->Intersect(ray, faceIdcs);
                    }
                }
            }
        }
    }
}