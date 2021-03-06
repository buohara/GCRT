#include "imgloader.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

/**
 * [ImgLoader::LoadTexture description]
 * @param  fileName [description]
 * @return          [description]
 */

GLuint ImgLoader::LoadTexture(string &fileName)
{
    ilInit();

    ILuint imgID;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    ILboolean res;

    if (!(res = ilLoadImage(fileName.c_str())))
    {
        return 0;
    }

    uint32_t w = ilGetInteger(IL_IMAGE_WIDTH);
    uint32_t h = ilGetInteger(IL_IMAGE_HEIGHT);

    BYTE* pData = ilGetData();
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