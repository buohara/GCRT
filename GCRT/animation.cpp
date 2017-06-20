#include "animation.h"

/**
 * [KeyFrame::KeyFrame description]
 * @param tIn    [description]
 * @param poseIn [description]
 */

KeyFrame::KeyFrame(float tIn, mat4 poseIn)
{
    t = tIn;
    poseMat = poseIn;
    pose = c3ga::matrix4x4ToVersor((c3ga::mv::Float*)&poseIn[0], true);
}

/**
 * [Animation::AddKF description]
 * @param kf [description]
 */

void Animation::AddKF(KeyFrame &kf)
{
    kfs.push_back(kf);

    if (kfs.size() > 1)
    {
        c3ga::TRversor &src = kfs[kfs.size() - 2].pose;
        c3ga::TRversor &dst = kfs[kfs.size() - 1].pose;
        poseLogs.push_back(log(c3ga::_TRversor(inverse(src) * dst)));
    }
}

/**
 * [Animation::GetAnimationMatrix description]
 * @param  t [description]
 * @return   [description]
 */

mat4 Animation::GetAnimationMatrix(float t)
{
    mat4 out = kfs[0].poseMat;

    for (uint32_t i = 0; i < kfs.size(); i++)
    {
        if (t < kfs[i].t)
        {
            float alpha = (t - kfs[i - 1].t) / (kfs[i].t - kfs[i - 1].t);
            c3ga::TRversor src = kfs[i - 1].pose;
            c3ga::dualLine log = poseLogs[i - 1];
            c3ga::TRversor interp = _TRversor(src * exp(_dualLine(alpha * log)));
            c3ga::TRversor interpInv = inverse(interp);

            out = GetMatrixFromVersor(interp, interpInv);
            break;
        }
    }

    return out;
}

/**
 * [Animation::GetMatrixFromVersor description]
 * @param  vers    [description]
 * @param  versInv [description]
 * @return         [description]
 */

mat4 Animation::GetMatrixFromVersor(c3ga::TRversor &vers, c3ga::TRversor &versInv)
{
    c3ga::flatPoint imageOfE1NI = c3ga::_flatPoint(vers * c3ga::e1ni * versInv);
    c3ga::flatPoint imageOfE2NI = c3ga::_flatPoint(vers * c3ga::e2ni * versInv);
    c3ga::flatPoint imageOfE3NI = c3ga::_flatPoint(vers * c3ga::e3ni * versInv);
    c3ga::flatPoint imageOfNONI = c3ga::_flatPoint(vers * c3ga::noni * versInv);

    c3ga::omFlatPoint M(imageOfE1NI, imageOfE2NI, imageOfE3NI, imageOfNONI);

    mat4 out;

    for (uint32_t i = 0; i < 16; i++)
    {
        uint32_t r = i / 4;
        uint32_t c = i % 4;
        out[r][c] = M.m_c[i];
    }

    return out;
}