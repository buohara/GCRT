#include "animation.h"

dmat4 Animation::GetPose(double t)
{
    dmat4 pose;

    for (uint32_t i = 1; i < kfs.size(); i++)
    {
        auto& kf1 = kfs[i - 1];
        auto& kf2 = kfs[i];
        double ti = kf1.t;
        double tf = kf2.t;

        if (t >= ti && t <= tf)
        {
            double t0 = (t - ti) / (tf - ti);

            dvec3 pos = (1.0 - t0) * kf1.pos + t0 * kf2.pos;
            pose = translate(pos);
        }
    }

    return pose;
}