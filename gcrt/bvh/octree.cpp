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
        faceIdcs.push_back(face);
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
        faceIdcs.push_back(face);
    }
}

void IntersectFaces(Ray ray, const vector<uint32_t>& faceIdcs, const vector<dvec3>& pos, 
    const vector<dvec3>& norm, const vector<dvec3>& tan, const vector<uvec3>& faces,
    Intersection &intsc)
{
    intsc.t = DBL_MAX;

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

        if (b1 < 0.0 || b2 < 0.0 || (b1 + b2 > 1.0)) continue;

        if (t > 0.0 && t < intsc.t)
        {
            intsc.t = t;

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
 * Intersect Intersect a ray with this octree and get list of potentially hit triangles.
 *
 * @param ray      Ray to intersect with this octree.
 * @param pos      Vertex positions for this mesh.
 * @param norm     Vertex normals for this mesh.
 * @param tan      Vertex tangets for this mesh.
 * 
 */

void Octree::Intersect(const Ray ray, const vector<dvec3>& pos, const vector<dvec3>& norm,
    const vector<dvec3>& tan, const vector<uvec3>& faces, Intersection &intsc)
{
    double tMin = DBL_MAX;
    Intersection intscFaces;
    IntersectFaces(ray, faceIdcs, pos, norm, tan, faces, intscFaces);

    if (intscFaces.t > 0.0)
    {
        tMin = intscFaces.t;
        intsc = intscFaces;
    }

    priority_queue<pair<uint32_t, double>> childDist;
    
    for (uint32_t i = 0; i < maxOctreeChildren; i++)
    {
        auto child = children[i];

        if (child)
        {
            child->box.Intersect(ray, intsc);
            if (intsc.t > 0.0 && intsc.t < tMin) childDist.push({ i, intsc.t });
        }
    }

    while (!childDist.empty())
    {
        Intersection childIntsc;
        auto child = childDist.top();
        childDist.pop();
        children[child.first]->Intersect(ray, pos, norm, tan, faces, childIntsc);

        if (childIntsc.t < tMin)
        {
            tMin = childIntsc.t;
            intsc = childIntsc;
        }

        if (!childDist.empty())
        {
            if (childIntsc.t < childDist.top().second) break;
        }
    }
}