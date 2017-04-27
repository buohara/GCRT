#include "material.h"

/**
* SetDiffuse -
*/

void Material::SetDiffuse(vec3 kdIn)
{
    kd = kdIn;
}

/**
* SetAmbient -
*/

void Material::SetAmbient(vec3 kaIn)
{
    ka = kaIn;
}

/**
 * LoadTexture -
 */

void Material::LoadTexture(wstring &fileName)
{
    Img texture;
    ImgLoader ldr;
    ldr.LoadImg(fileName, texture);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.w,
        texture.h, 0, GL_RGB, GL_FLOAT, &texture.pixels[0]);
}