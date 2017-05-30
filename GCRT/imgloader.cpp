#include "imgloader.h"

/**
 * LoadTexture
 */

GLuint ImgLoader::LoadTexture(string &fileName)
{
    ilInit();

    ILuint imgID;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    if (!ilLoadImage(fileName.c_str()))
    {
        return 0;
    }

    uint32_t w = ilGetInteger(IL_IMAGE_WIDTH);
    uint32_t h = ilGetInteger(IL_IMAGE_HEIGHT);

    BYTE* pData = ilGetData();
    uint32_t imgBytes = 3 * w * h;
    GLuint texID = 0;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w,
        h, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
    glGenerateMipmap(GL_TEXTURE_2D);

    ilBindImage(0);
    ilDeleteImage(imgID);

    return texID;
}