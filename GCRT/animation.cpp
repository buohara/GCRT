#include "animation.h"

/**
 * KeyFrame -
 */

KeyFrame::KeyFrame(float tIn, mat4 poseIn)
{
    t = tIn;
    pose = matrix4x4ToVersor((mv::Float*)&poseIn[0]);
}

/**
 * AddKF - - Code lifted from GA for Computer Scientists, Ch13 ex4.
 */

void Animation::AddKF(KeyFrame &kf)
{
    kfs.push_back(kf);

    if (kfs.size() > 1)
    {
        TRversor &src = kfs[kfs.size() - 1].pose;
        TRversor &dst = kfs[kfs.size()].pose;
        poseLogs.push_back(log(_TRversor(inverse(src) * dst)));
    }
}

/**
 * GetAnimationMatrix - Code lifted from GA for Computer Scientists, Ch13 ex4.
 */

mat4 Animation::GetAnimationMatrix(float t)
{
    mat4 out(1.0);

    for (uint32_t i = 0; i < kfs.size(); i++)
    {
        if (t < kfs[i].t)
        {
            float alpha = (t - kfs[i - 1].t) / (kfs[i].t - kfs[i - 1].t);
            TRversor src = kfs[i - 1].pose;
            dualLine log = poseLogs[i - 1];
            TRversor interp = _TRversor(src * exp(_dualLine(alpha * log)));
            TRversor interpInv = inverse(interp);

            out = GetMatrixFromVersor(interp, interpInv);
        }
    }

    return out;
}

/**
 * GetAnimationMatrix - Code lifted from GA for Computer Scientists, Ch13 ex3.
 */

mat4 Animation::GetMatrixFromVersor(TRversor &vers, TRversor &versInv)
{
    flatPoint imageOfE1NI = _flatPoint(vers * e1ni * versInv);
    flatPoint imageOfE2NI = _flatPoint(vers * e2ni * versInv);
    flatPoint imageOfE3NI = _flatPoint(vers * e3ni * versInv);
    flatPoint imageOfNONI = _flatPoint(vers * noni * versInv);

    omFlatPoint M(imageOfE1NI, imageOfE2NI, imageOfE3NI, imageOfNONI);

    mat4 out;

    for (uint32_t i = 0; i < 16; i++)
    {
        uint32_t r = i / 4;
        uint32_t c = i % 4;
        out[c][r] = M.m_c[i];
    }

    return out;
}