#include "texture.h"

/**
 * [Texture::Load description]
 * @param imgPathIn [description]
 */

void Texture::Load(string imgPathIn)
{
    imgPath = imgPathIn;
    ilInit();

    ILuint imgID;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    if (!ilLoadImage(imgPath.c_str()))
    {
        return;
    }

    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    BYTE* pData = ilGetData();

    pixels.reserve(w * h);

    for (uint32_t r = 0; r < h; r++)
    {
        for (uint32_t c = 0; c < w; c++)
        {
            dvec3 pixel;
            uint32_t offset = 3 * (r * w + c);
            pixel.r = pData[offset];
            pixel.g = pData[offset + 1];
            pixel.b = pData[offset + 2];
            pixels[r * w + c] = pixel;
        }
    }

    ilBindImage(0);
    ilDeleteImage(imgID);
}

/**
 * [Texture::Sample description]
 * @param  uv [description]
 * @return    [description]
 */

dvec3 Texture::Sample(dvec2 uv)
{
    double x = uv.x * (double)w;
    double y = uv.y * (double)h;

    uint32_t xh = (uint32_t)ceil(x);
    uint32_t xl = (uint32_t)floor(x);

    uint32_t yh = (uint32_t)ceil(y);
    uint32_t yl = (uint32_t)floor(y);

    double tx = x - (double)xl;
    double ty = y - (double)yl;

    dvec3 ul = pixels[w * yl + xl];
    dvec3 ll = pixels[w * yh + xl];
    dvec3 ur = pixels[w * yl + xh];
    dvec3 lr = pixels[w * yh + xh];

    dvec3 out = ul * (1 - tx) * (1 - ty) +
        ur * tx * (1 - ty) +
        ll * (1 - tx) * ty +
        lr * tx * ty;

    return out;
}