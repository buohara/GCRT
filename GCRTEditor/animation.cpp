#include "animation.h"

/**
 * Animation::AddKF Add a keyframe to this animation.
 * @param kf New keyframe.
 */

void Animation::AddKF(KeyFrame &kf)
{
    kfs.push_back(kf);
}

/**
 * Animation::GetAnimationMatrix Get interpolated animation at time t.
 * @param  t Time to get animation pose.
 * @return   Matrix for pose at time t.
 */

mat4 Animation::GetAnimationMatrix(float t)
{
    mat4 out = mat4(1.0);

    for (uint32_t i = 0; i < kfs.size(); i++)
    {
        if (t < kfs[i].t)
        {
            float alpha = 1.0f - (t - kfs[i - 1].t) / (kfs[i].t - kfs[i - 1].t);

            vec3 pos    = mix(kfs[i].trans, kfs[i - 1].trans, alpha);
            quat rot    = mix(kfs[i].rot, kfs[i - 1].rot, alpha);
            vec3 scale  = mix(kfs[i].scale, kfs[i - 1].scale, alpha);

            out = translate(pos) * mat4(rot) * glm::scale(scale);
            break;
        }
    }

    return out;
}