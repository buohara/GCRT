#include "spherelight.h"

/**
 * [SphereLight::Intersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void SphereLight::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t     = -1.0;
    dvec3 d     = ray.dir;
    dvec3 o     = ray.org;
    dvec3 cnt   = pos;

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
        dvec3 n         = normalize(o + t1 * d - pos);
        intsc.t         = t1;
        intsc.normal    = n;
        intsc.mat = "Light";
        return;
    }

    if (t2 > 0.0)
    {
        dvec3 n         = normalize(o + t1 * d - pos);
        intsc.t         = t2;
        intsc.normal    = n;
        intsc.mat = "Light";
        return;
    }
}

/**
 * [SphereLight::GetLightSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

void SphereLight::GetLightSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    uint32_t sampleSet = sampler.NextSet();

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray rayOut;
        rayOut.org = org;
        rayOut.dir = 
            normalize((0.4 * sampler.samples[sampleSet][i] + dvec4(pos, 1.0)) - dvec4(org, 1.0));
        raysOut.push_back(rayOut);
    }
}

/**
 * [SphereLight::Init description]
 * @param sampleSets [description]
 * @param setSize    [description]
 */

void SphereLight::Init(uint32_t sampleSets, uint32_t setSize)
{
    sampler.GenerateSamples(sampleSets, setSize);
}

/**
 * [SphereLight::GetEmission description]
 * @return [description]
 */

dvec3 SphereLight::EvalEmission(Ray rayOut, Intersection intsc)
{
    double cosTheta = dot(-rayOut.dir, intsc.normal);
    return cosTheta * pwr / (r * r);
}

/**
 * [SphereLight::GetLightPDF description]
 * @param  rayIn [description]
 * @param  intsc [description]
 * @return       [description]
 */

double SphereLight::GetLightPDF(Ray rayIn, Intersection intsc)
{
    double cosTheta = dot(-rayIn.dir, intsc.normal);
    double t = intsc.t;
    double fourPi = 4.0 * glm::pi<double>();

    return cosTheta / (fourPi * t * t);
}