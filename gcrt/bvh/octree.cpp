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

bool BBox::Overlaps(
    dvec3 trivert0,
    dvec3 trivert1,
    dvec3 trivert2
)
{
    dvec3 boxcenter     = dvec3(0.5 * (max.x + min.x), 0.5 * (max.y + min.y), 0.5 * (max.z + min.z));
    dvec3 boxhalfsize   = dvec3(0.5 * (max.x - min.x), 0.5 * (max.y - min.y), 0.5 * (max.z - min.z));
    dvec3 normal        = normalize(cross(trivert1 - trivert0, trivert2 - trivert0));

    if ((trivert0.x >= min.x && trivert0.x <= max.x) && (trivert0.y >= min.y && trivert0.y <= max.y) && (trivert0.z >= min.z && trivert0.z <= max.z))
        if ((trivert1.x >= min.x && trivert1.x <= max.x) && (trivert1.y >= min.y && trivert1.y <= max.y) && (trivert1.z >= min.z && trivert1.z <= max.z))
            if ((trivert2.x >= min.x && trivert2.x <= max.x) && (trivert2.y >= min.y && trivert2.y <= max.y) && (trivert2.z >= min.z && trivert2.z <= max.z))
                return true;

    double d, min, max;
    double p0, p1, p2;
    dvec3 fe, vmin, vmax;
    dvec3 v0, v1, v2;
    dvec3 e0, e1, e2;

    v0 = trivert0 - boxcenter;
    v1 = trivert1 - boxcenter;
    v2 = trivert2 - boxcenter;

    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;

    fe = abs(e0);

    p0 = e0.z * v0.y - e0.y * v0.z;
    p2 = e0.z * v2.y - e0.y * v2.z;

    min = p2;
    max = p0;

    if (p0 < p2)
    {
        min = p0;
        max = p2;
    }

    d = fe.z * boxhalfsize.y + fe.y * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p0 = -e0.z * v0.x + e0.x * v0.z;
    p2 = -e0.z * v2.x + e0.x * v2.z;

    min = p2;
    max = p0;

    if (p0 < p2) { min = p0; max = p2; }

    d = fe.z * boxhalfsize.x + fe.x * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p1 = e0.y * v1.x - e0.x * v1.y;
    p2 = e0.y * v2.x - e0.x * v2.y;

    min = p1;
    max = p2;

    if (p2 < p1)
    {
        min = p2;
        max = p1;
    }

    d = fe.y * boxhalfsize.x + fe.x * boxhalfsize.y;

    if (min > d || max < -d) return false;

    fe = abs(e1);

    p0 = e1.z * v0.y - e1.y * v0.z;
    p2 = e1.z * v2.y - e1.y * v2.z;

    min = p2;
    max = p0;

    if (p0 < p2)
    {
        min = p0;
        max = p2;
    }

    d = fe.z * boxhalfsize.y + fe.y * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p0 = -e1.z * v0.x + e1.x * v0.z;
    p2 = -e1.z * v2.x + e1.x * v2.z;

    min = p2;
    max = p0;

    if (p0 < p2)
    {
        min = p0;
        max = p2;
    }

    d = fe.z * boxhalfsize.x + fe.x * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p0 = e1.y * v0.x - e1.x * v0.y;
    p1 = e1.y * v1.x - e1.x * v1.y;

    min = p1;
    max = p0;

    if (p0 < p1)
    {
        min = p0;
        max = p1;
    }

    d = fe.y * boxhalfsize.x + fe.x * boxhalfsize.y;

    if (min > d || max < -d) return false;

    fe = abs(e2);

    p0 = e2.z * v0.y - e2.y * v0.z;
    p1 = e2.z * v1.y - e2.y * v1.z;

    min = p1;
    max = p0;

    if (p0 < p1)
    {
        min = p0;
        max = p1;
    }

    d = fe.z * boxhalfsize.y + fe.y * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p0 = -e2.z * v0.x + e2.x * v0.z;
    p1 = -e2.z * v1.x + e2.x * v1.z;

    min = p1;
    max = p0;

    if (p0 < p1)
    {
        min = p0;
        max = p1;
    }

    d = fe.z * boxhalfsize.x + fe.x * boxhalfsize.z;

    if (min > d || max < -d) return false;

    p1 = e2.y * v1.x - e2.x * v1.y;
    p2 = e2.y * v2.x - e2.x * v2.y;

    min = p1;
    max = p2;

    if (p2 < p1)
    {
        min = p2;
        max = p1;
    }

    d = fe.y * boxhalfsize.x + fe.x * boxhalfsize.y;

    if (min > d || max < -d) return 0;

    min = v0.x;
    max = v0.x;

    if (v1.x < min) min = v1.x;
    if (v1.x > max) max = v1.x;
    if (v2.x < min) min = v2.x;
    if (v2.x > max) max = v2.x;

    if (min > boxhalfsize.x || max < -boxhalfsize.x) return false;

    min = v0.y;
    max = v0.y;

    if (v1.y < min) min = v1.y;
    if (v1.y > max) max = v1.y;
    if (v2.y < min) min = v2.y;
    if (v2.y > max) max = v2.y;

    if (min > boxhalfsize.y || max < -boxhalfsize.y) return false;

    min = v0.z;
    max = v0.z;

    if (v1.z < min) min = v1.z;
    if (v1.z > max) max = v1.z;
    if (v2.z < min) min = v2.z;
    if (v2.z > max) max = v2.z;

    if (min > boxhalfsize.z || max < -boxhalfsize.z) return false;

    if (normal.x > 0.0f)
    {
        vmin.x = -boxhalfsize.x;
        vmax.x = boxhalfsize.x;
    }
    else
    {
        vmin.x = boxhalfsize.x;
        vmax.x = -boxhalfsize.x;
    }

    if (normal.y > 0.0f)
    {
        vmin.y = -boxhalfsize.y;
        vmax.y = boxhalfsize.y;
    }
    else
    {
        vmin.y = boxhalfsize.y;
        vmax.y = -boxhalfsize.y;
    }

    if (normal.z > 0.0f)
    {
        vmin.z = -boxhalfsize.z;
        vmax.z = boxhalfsize.z;
    }
    else
    {
        vmin.z = boxhalfsize.z;
        vmax.z = -boxhalfsize.z;
    }

    d = -(normal.x * v0.x + normal.y * v0.y + normal.z * v0.z);
    if (normal.x * vmin.x + normal.y * vmin.y + normal.z * vmin.z + d > 0.0f) return false;
    if (normal.x * vmax.x + normal.y * vmax.y + normal.z * vmax.z + d >= 0.0f) return true;

    return false;
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
    bool childIntersectsFace[maxOctreeChildren] = { false };
    uint32_t depthAllowFaces = 8;

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

                    double hx       = (box.max.x - box.min.x) / 2.0;
                    double hy       = (box.max.y - box.min.y) / 2.0;
                    double hz       = (box.max.z - box.min.z) / 2.0;

                    childBox.min.x  = box.min.x + ((double)x) * hx;
                    childBox.min.y  = box.min.y + ((double)y) * hy;
                    childBox.min.z  = box.min.z + ((double)z) * hz;

                    childBox.max.x  = box.min.x + ((double)(x + 1)) * hx;
                    childBox.max.y  = box.min.y + ((double)(y + 1)) * hy;
                    childBox.max.z  = box.min.z + ((double)(z + 1)) * hz;

                    if (childBox.Contains(p0, p1, p2) == true)
                    {
                        childContainsFace           = true;

                        children[child]             = make_shared<Octree>();
                        children[child]->box        = childBox;
                        children[child]->depth      = depth + 1;
                        children[child]->maxDepth   = maxDepth;
                        children[child]->Insert(p0, p1, p2, face);

                        goto exit;
                    }
                    else if (depth < depthAllowFaces && childBox.Overlaps(p0, p1, p2))
                    {
                        children[child]             = make_shared<Octree>();
                        children[child]->box        = childBox;
                        children[child]->depth      = depth + 1;
                        children[child]->maxDepth   = maxDepth;

                        children[child]->Insert(p0, p1, p2, face);

                        childIntersectsFace[child] = true;
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
                    else if (children[child]->box.Overlaps(p0, p1, p2)) childIntersectsFace[child] = true;
                }
            }
        }
    }

exit:

    if (childContainsFace == false)
    {
        if (depth < depthAllowFaces)
        {
            for (uint32_t i = 0; i < maxOctreeChildren; i++)
            {
                if (childIntersectsFace[i]) children[i]->Insert(p0, p1, p2, face);
            }
        }
        else
        {
            faceIdcs.push_back(face);
        }
    }
}

/**
 * IntersectFaces
 *
 * @param ray
 * @param faceIdcs
 * @param pos
 * @param norm
 * @param tan
 * @param faces
 * @param intsc
 */

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
            Intersection childIntsc;
            child->box.Intersect(ray, childIntsc);
            if (childIntsc.t > 0.0 && childIntsc.t < tMin) childDist.push({ i, childIntsc.t });
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
            if (tMin < childDist.top().second) break;
        }
    }
}