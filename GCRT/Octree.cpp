#include "Octree.h"

/**
 * BBox::Intersect Intersect a ray with a box using plane-slicing method.
 *
 * @param ray   Ray to intersect with this box.
 * @param intsc Information about intersection point.
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
 * BBox::Contains Does this box contain a given triangle.
 *
 * @param  p0 First triangle vertex.
 * @param  p1 Second triangle vertex.
 * @param  p2 Third triangle vertex.
 *
 * @return    True if all points contained in box.
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
 * BBox::Overlaps Check if a triangle overlaps this box.
 *
 * @param p0 First triangle vertex.
 * @param p1 First triangle vertex.
 * @param p2 First triangle vertex.
 *
 * @return True if any points are contained in this box.
 */

bool BBox::Overlaps(dvec3 p0, dvec3 p1, dvec3 p2)
{
    if (p0.x < max.x && p0.x > min.x &&
        p0.y < max.y && p0.y > min.y &&
        p0.z < max.z && p0.z > min.z
        )
    {
        return true;
    }

    if (p1.x < max.x && p1.x > min.x &&
        p1.y < max.y && p1.y > min.y &&
        p1.z < max.z && p1.z > min.z
        )
    {
        return true;
    }

    if (p2.x < max.x && p2.x > min.x &&
        p2.y < max.y && p2.y > min.y &&
        p2.z < max.z && p2.z > min.z
        )
    {
        return true;
    }

    return false;
}

/**
 * Insert Insert a triangle into this octree.
 *
 * @param p0   First triangle vertex.
 * @param p1   Second triangle vertex.
 * @param p2   Third triangle vertex.
 * @param face Face index/ID for this triangle.
 */

void Octree::Insert(dvec3 p0, dvec3 p1, dvec3 p2, uint32_t face)
{
    if (depth == maxDepth - 1)
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
 * Intersect Intersect a ray with this octree and get list of potentially hit triangles.
 *
 * @param ray      Ray to intersect with this octree.
 * @param faceIdcs Output list of potentially hit triangles.
 * @param faceCnt  Number of potentially hit triangles.
 */

void Octree::Intersect(Ray ray, uint32_t faceIdcs[], uint32_t &faceCnt)
{
    for (auto &face : faces)
    {
        faceIdcs[faceCnt++] = face;
    }

    double minT = 10000.0;

    for (auto &child : children)
    {
        if (child)
        {
            Intersection intsc;
            child->box.Intersect(ray, intsc);

            if (intsc.t > 0.0 && intsc.t < minT)
            {
                child->Intersect(ray, faceIdcs, faceCnt);
            }
        }
    }
}

void Octree::Intersect2(Ray ray, uint32_t faceIdcs[], uint32_t &faceCnt)
{
    uint32_t numChildrenHit = 0;
    uint32_t numChildrenVisited = 0;

    struct ChildIntsc
    {
        float t;
        uint32_t childIdx;
        bool visited;
    };

    ChildIntsc childIntscs[8];

    for (auto &face : faces)
    {
        faceIdcs[faceCnt++] = face;
    }

    for (uint32_t i = 0; i < 8; i++)
    {
        Intersection intsc;
        children[i]->box.Intersect(ray, intsc);

        if (intsc.t > 0.0)
        {
            childIntscs[numChildrenHit++] = { intsc.t, i, false };
        }
    }

    while (numChildrenVisited < numChildrenHit)
    {
        double minT = FLT_MAX;
        uint32_t closestChild = 8;

        for (uint32_t i = 0; i < numChildrenHit; i++)
        {
            if (childIntscs[i].visited == false && childIntscs[i].t < minT)
            {
                minT = childIntscs[i].t;
                closestChild = i;
            }
        }

        children[closestChild]->Intersect(ray, faceIdcs, faceCnt);
        childIntscs[closestChild].visited = true;
    }
}