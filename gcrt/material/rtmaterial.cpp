#include "rtmaterial.h"

/**
 * RTMaterial::EvalBSDF - Evaluate BDSF of material for given camera and output ray. Fall through
 * to appropriate material implementation.
 *
 * @param  sampleRay        Sample ray pointing from scene pointing at current intersection point.
 * @param  sampleRayColor   Color coming from sample ray.
 * @param  intsc            Intersection point to evaludate BDSF.
 * @param  camRay           Ray from camera.
 *
 * @return                  Color from evaluating BDSF.
 */

dvec3 RTMaterial::EvalBSDF(Ray sampleRay, dvec3 sampleRayColor, Intersection intsc, Ray camRay)
{
    dvec3 bsdf(0.0);

    switch (type)
    {
    case LAMBERT:

        bsdf = LambertEvalBSDF(sampleRay, sampleRayColor, intsc, camRay);
        break;

    case FRESNEL:

        bsdf = FresnelEvalBSDF(sampleRay, sampleRayColor, intsc, camRay);
        break;

    case MIRROR:

        bsdf = MirrorEvalBSDF(sampleRay, sampleRayColor, intsc, camRay);
        break;

    case BLINN:

        bsdf = BlinnEvalBSDF(sampleRay, sampleRayColor, intsc, camRay);
        break;

    case TEX:
    default:

        break;
    }

    return bsdf;
}

/**
 * RTMaterial::GetBSDFSamples - When a camera ray hits surface, generate bounce rays based on
 * this material. Route to appropriate material implementation.
 * 
 * @param  numSamples   Requested number of BSDF sample rays.
 * @param  camRay       Ray coming from camera.
 * @param  intsc        Current intersection point.
 * @param  sampleRays   List of output sample rays based on material.
 *
 * @return              Number of rays actually returned. For example, a perfect mirror surface will
 *                      only generate one bounce ray.
 */

uint32_t RTMaterial::GetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray>& sampleRays)
{
    uint32_t numSamplesOut = 0;

    switch (type)
    {
    case LAMBERT:

        numSamplesOut = LambertGetBSDFSamples(numSamples, camRay, intsc, sampleRays);
        break;

    case FRESNEL:

        numSamplesOut = FresnelGetBSDFSamples(numSamples, camRay, intsc, sampleRays);
        break;

    case MIRROR:

        numSamplesOut = MirrorGetBSDFSamples(numSamples, camRay, intsc, sampleRays);
        break;

    case BLINN:

        numSamplesOut = BlinnGetBSDFSamples(numSamples, camRay, intsc, sampleRays);
        break;

    case TEX:
    default:

        break;
    }

    return numSamplesOut;
}

/**
 * [RTMaterial::BSDFPDF description]
 * @param  sampleRay  [description]
 * @param  camRay [description]
 * @param  intsc  [description]
 * @return        [description]
 */

double RTMaterial::BSDFPDF(Ray sampleRay, Ray camRay, Intersection intsc)
{
    double pdf = 0.0;

    switch (type)
    {
    case LAMBERT:

        pdf = LambertBSDFPDF(sampleRay, camRay, intsc);
        break;

    case FRESNEL:

        break;

    case MIRROR:

        break;

    case BLINN:

        pdf = BlinnBSDFPDF(sampleRay, camRay, intsc);
        break;

    case TEX:
    default:

        break;
    }

    return pdf;
}

/**
 * [TexMaterial::GetDiffuseColor description]
 * @return       [description]
 */

dvec3 TexMaterial::GetDiffuseColor(Ray ray, Intersection intsc)
{
    return diffTex.Sample(intsc.uv);
}

/**
 * [TexMaterial::Load description]
 * @param diffTexFile [description]
 * @param normTexFile [description]
 */

void TexMaterial::Load(string diffTexFile, string normTexFile)
{
    diffTex.Load(diffTexFile);
    normTex.Load(normTexFile);
}

/**
 * [TexMaterial::PerturbNormal description]
 * @param intsc  [description]
 * @param normal [description]
 */

void TexMaterial::PerturbNormal(Intersection &intsc)
{
    dvec3 nPerturb  = normTex.Sample(intsc.uv);
    dvec3 bitan     = cross(intsc.normal, intsc.tan);
    dmat3 tbn       = dmat3(intsc.tan, bitan, intsc.normal);
    intsc.normal    = tbn * nPerturb;
}