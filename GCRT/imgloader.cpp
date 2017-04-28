#include "imgloader.h"

void ImgLoader::LoadImg(string &fileName, Img &img)
{
    ilInit();

    ILuint imgID;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    ilLoadImage(fileName.c_str());

    img.w = ilGetInteger(IL_IMAGE_WIDTH);
    img.h = ilGetInteger(IL_IMAGE_HEIGHT);

    BYTE* pData = ilGetData();
    uint32_t imgBytes = 3 * img.w * img.h;

    img.pixels.resize(imgBytes);

    for (uint32_t i = 0; i < imgBytes; i++)
    {
        img.pixels[i] = pData[i];
    }

    ilBindImage(0);
    ilDeleteImage(imgID);
}