#include "material.h"

/**
 * SetShaderParams -
 */

void BasicMaterial::ApplyMaterial()
{
    GLuint kdID = glGetUniformLocation(program, "kd");
    glUniform3fv(kdID, 1, &kd[0]);
}

/**
 * SetLights -
 */

void BasicMaterial::SetLights(vec3 lightPos)
{
    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);
}

/**
 * SetShaderParams -
 */

void BasicShadowMaterial::ApplyMaterial()
{
    GLuint kdID = glGetUniformLocation(program, "kd");
    glUniform3fv(kdID, 1, &kd[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexID);

    GLuint depthID = glGetUniformLocation(program, "depthTex");
    glUniform1i(depthID, 0);
}

/**
 * SetLights -
 */

void BasicShadowMaterial::SetLights(vec3 lightPos)
{
    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);
}

/**
 * SetShaderParams -
 */

void BumpMaterial::ApplyMaterial()
{
    // Diffuse texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint diffTexID = glGetUniformLocation(program, "diffuseTex");
    glUniform1i(diffTexID, 0);

    // Normal texture.

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint normTexID = glGetUniformLocation(program, "normalTex");
    glUniform1i(normTexID, 1);
}

/**
 * SetLights -
 */

void BumpMaterial::SetLights(vec3 lightPos)
{
    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);
}