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
 * LoadDiffuseTexture -
 */

void Material::LoadDiffuseTexture(string &fileName)
{
    Img texture;
    ImgLoader ldr;
    ldr.LoadImg(fileName, texture);

    glGenTextures(1, &diffuseTexID);
    glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.w,
        texture.h, 0, GL_RGB, GL_UNSIGNED_BYTE, &texture.pixels[0]);

    glGenerateMipmap(GL_TEXTURE_2D);
}

/**
 * LoadNormalTexture -
 */

void Material::LoadNormalTexture(string &fileName)
{
    Img texture;
    ImgLoader ldr;
    ldr.LoadImg(fileName, texture);

    glGenTextures(1, &normalTexID);
    glBindTexture(GL_TEXTURE_2D, normalTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.w,
        texture.h, 0, GL_RGB, GL_UNSIGNED_BYTE, &texture.pixels[0]);

    glGenerateMipmap(GL_TEXTURE_2D);
}

/**
 * SetShaderParams -
 */

void Material::SetShaderParams(GLuint progID)
{
    // Diffuse texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint diffTexID = glGetUniformLocation(progID, "diffuseTex");
    glUniform1i(diffTexID, 0);

    // Normal texture.

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint normTexID = glGetUniformLocation(progID, "normalTex");
    glUniform1i(normTexID, 1);

    // Flat colors.

    GLuint kdID = glGetUniformLocation(progID, "kd");
    glUniform3fv(kdID, 1, &kd[0]);

    GLuint kaID = glGetUniformLocation(progID, "ka");
    glUniform3fv(kaID, 1, &ka[0]);
}