#include "rtmaterial.h"

/**
 * RTMaterial::RTMaterial - Blinn material constructor.
 */

RTMaterial::RTMaterial(MatType type, string name, double ks) : type(type), name(name), ks(ks) 
{
    assert(type == BLINN);
}

/**
 * RTMaterial::BlinnEvalBSDF Evaluate surface BSDF for given input and
 * output rays.
 *
 * @param  rayOut  Outgoing ray (going away from camera).
 * @param  colorIn Color returned along outgoing ray.
 * @param  intsc   Surface intersection data.
 * @param  rayIn   Incoming ray (coming from camera).
 * @return         BSDF value.
 */

dvec3 RTMaterial::BlinnEvalBSDF(
    Ray rayIn,
    dvec3 colorIn,
    Intersection intsc,
    Ray rayOut
)
{
    dvec3 r = normalize(reflect(-rayIn.dir, intsc.normal));
    double spec = dot(r, -rayOut.dir);
    spec = pow(spec, ks + 1.0);
    return 0.3 * spec * dvec3(2.0, 2.0, 2.0) * colorIn;
}

/**
 * RTMaterial::BlinnGetBSDFSamples Generate rays samples for MC estimator.
 *
 * @param numSamples Number of samples requested.
 * @param rayIn      Ray coming from camera.
 * @param intsc      Surface intersection data.
 * @param raysOut    Out sample rays (going away from camera).
 *
 * @return           Number of samples generated (some materials, like perfect mirrors,
 *                   only generate one output ray).
 */

uint32_t RTMaterial::BlinnGetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    dvec3 bitan = cross(intsc.normal, intsc.tan);
    dmat3 tbn = dmat3(intsc.tan, bitan, intsc.normal);

    for (uint32_t i = 0; i < numSamples; i++)
    {
        double u1 = (double)rand() / (double)RAND_MAX;
        double u2 = (double)rand() / (double)RAND_MAX;

        double cosTheta = pow(u1, 1.0 / (ks + 1.0));
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        double phi = 2.0 * pi<double>() * u2;

        dvec3 halfDir = dvec3(0.0);
        halfDir.x = sinTheta * cos(phi);
        halfDir.y = sinTheta * sin(phi);
        halfDir.z = cosTheta;
        halfDir = tbn * halfDir;

        dvec3 refl = normalize(reflect(rayIn.dir, intsc.normal));

        if (dot(halfDir, intsc.normal) < 0.0)
        {
            halfDir = -halfDir;
        }

        raysOut[i].dir = rayIn.dir + 2.0 * dot(-rayIn.dir, halfDir) * halfDir;;
        raysOut[i].org = org;
    }

    return numSamples;
}

/**
 * RTMaterial::BlinnBSDFPDF Return PDF for given sample ray. Used for weighting
 * samples in MC estimator.
 *
 * @param  rayIn  Ray coming from camera.
 * @param  rayOut Sample ray to get PDF for.
 * @param  intsc  Surface intersection data.
 * @return        PDF for sample ray.
 */

double RTMaterial::BlinnBSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 refl      = normalize(reflect(-rayOut.dir, intsc.normal));
    double cosAlpha = dot(refl, -rayIn.dir);
    cosAlpha        = pow(cosAlpha, ks + 1.0);
    double out      = (ks + 1.0) * cosAlpha / (8.0 * pi<double>());
    return out;
}