#include "rtrenderer.h"

void RTRenderer::Init(uint32_t w, uint32_t h)
{
    imageW = w;
    imageH = h;
    image.resize(imageW * imageH);

    vec3 camPos = vec3(10.0, 10.0, 10.0);
    vec3 camLook = vec3(0.0, 0.0, 0.0);

    scn.plane.normal = vec4(0.0, 0.0, 1.0, 0.0);
    scn.plane.ka = vec3(0.0, 0.0, 0.0);
    scn.plane.kd = vec3(0.1, 0.3, 0.7);

    RTSphere sph;
    sph.orgn = vec3(2.0, -2.0, 2.0);
    sph.r = 1.5;
    sph.ka = vec3(0.2, 0.02, 0.01);
    sph.kd = vec3(0.7, 0.1, 0.1);

    RTLight light;
    light.pos = vec3(0.0, 5.0, 10.0);
    light.color = vec3(1.0, 1.0, 1.0);
    scn.lights.push_back(light);

    scn.spheres.push_back(sph);
    scn.cam.Init(imageW, imageH, camPos, camLook, 75.0);
}

void RTRenderer::Render()
{
    for (uint32_t y = 0; y < imageH; y++)
    {
        for (uint32_t x = 0; x < imageW; x++)
        {
            vector<vec2> samples;
            samples.resize(1);
            sampler.GenerateSamples(1, x, y, samples);
            Ray ray = scn.cam.GenerateRay(samples[0]);

            vec3 curColor = vec3(0.1, 0.1, 0.1);
            double minDist = DBL_MAX;
            
            Intersection intsc = scn.plane.Intersect(ray);

            if (intsc.t > 0.0)
            {
                minDist = intsc.t;
                curColor = scn.plane.ka;

                Ray newRay;
                newRay.org = ray.org + ray.dir * (float)intsc.t;
                newRay.dir = normalize(scn.lights[0].pos - newRay.org);
                double theta = dot(newRay.dir, intsc.normal);

                if (theta > 0.0)
                {
                    Intersection intsc2 = scn.spheres[0].Intersect(newRay);
                    if (intsc2.t < 0.0)
                    {
                        float d = length(scn.lights[0].pos - newRay.org);
                        curColor += intsc.kd * (float)theta;
                    }
                }
            }

            for (uint32_t i = 0; i < scn.spheres.size(); i++)
            {
                intsc = scn.spheres[i].Intersect(ray);

                if ((intsc.t > 0.0) && (intsc.t < minDist))
                {
                    minDist = intsc.t;
                    curColor = scn.spheres[i].ka;

                    Ray newRay;
                    newRay.org = ray.org + ray.dir * (float)intsc.t;
                    newRay.dir = normalize(scn.lights[0].pos - newRay.org);
                    double theta = dot(newRay.dir, intsc.normal);

                    if (theta > 0.0)
                    {
                        float d = length(scn.lights[0].pos - newRay.org);
                        curColor += intsc.kd * (float)theta;
                    }
                }
            }

            image[y * imageW + x] = curColor;
        }
    }
}

void RTRenderer::SaveImage(string fileName)
{
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);

    ILuint imgID = ilGenImage();
    ilTexImage(
        imageW,
        imageH,
        1,
        3,
        IL_RGB,
        IL_FLOAT,
        &image[0]
    );

    ilSave(IL_JPG, fileName.c_str());
}