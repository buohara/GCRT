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

void Material::LoadTexture(string &fileName)
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
        texture.h, 0, GL_RGB, GL_UNSIGNED_BYTE, &texture.pixels[0]);
}

/**
 * SetShaderParams -
 */

void Material::SetShaderParams(GLuint progID)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint kdID = glGetUniformLocation(progID, "kd");
    glUniform3fv(kdID, 1, &kd[0]);

    GLuint kaID = glGetUniformLocation(progID, "ka");
    glUniform3fv(kaID, 1, &ka[0]);
}