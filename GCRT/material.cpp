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

void BasicMaterial::SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights)
{
    vec3 lightPos = ptLights[0].pos;
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

void BasicShadowMaterial::SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights)
{
    vec3 dirLightPos = dirLights[0].pos;
    vec3 dirLightLook = dirLights[0].look;

    mat4 depthView = lookAt(dirLightPos, dirLightLook, vec3(0.0, 1.0, 0.0));
    mat4 depthProj = ortho(-30.0, 30.0, -30.0, 30.0, 0.1, 100.0);

    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &dirLightPos[0]);

    GLuint lightViewID = glGetUniformLocation(program, "lightView");
    glUniformMatrix4fv(lightViewID, 1, false, &depthView[0][0]);

    GLuint lightProjID = glGetUniformLocation(program, "lightProj");
    glUniformMatrix4fv(lightProjID, 1, false, &depthProj[0][0]);
}

/**
 * SetShaderParams -
 */

void BumpMaterial::ApplyMaterial()
{
    // Diffuse texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    GLuint diffTexID = glGetUniformLocation(program, "diffuseTex");
    glUniform1i(diffTexID, 0);

    // Normal texture.

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexID);

    GLuint normTexID = glGetUniformLocation(program, "normalTex");
    glUniform1i(normTexID, 1);
}

/**
 * SetLights -
 */

void BumpMaterial::SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights)
{
    vec3 lightPos = ptLights[0].pos;
    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);
}

/**
 * SetShaderParams -
 */

void BumpShadowMaterial::ApplyMaterial()
{
    // Diffuse texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    GLuint diffTexID = glGetUniformLocation(program, "diffuseTex");
    glUniform1i(diffTexID, 0);

    // Normal texture.

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexID);

    GLuint normTexID = glGetUniformLocation(program, "normalTex");
    glUniform1i(normTexID, 1);

    // Shadow map texture.

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexID);

    GLuint depthID = glGetUniformLocation(program, "depthTex");
    glUniform1i(depthID, 2);
}

/**
 * SetLights -
 */

void BumpShadowMaterial::SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights)
{
    vec3 dirLightPos = dirLights[0].pos;
    vec3 dirLightLook = dirLights[0].look;

    mat4 depthView = lookAt(dirLightPos, dirLightLook, vec3(0.0, 1.0, 0.0));
    mat4 depthProj = ortho(-30.0, 30.0, -30.0, 30.0, 0.1, 100.0);

    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &dirLightPos[0]);

    GLuint lightViewID = glGetUniformLocation(program, "lightView");
    glUniformMatrix4fv(lightViewID, 1, false, &depthView[0][0]);

    GLuint lightProjID = glGetUniformLocation(program, "lightProj");
    glUniformMatrix4fv(lightProjID, 1, false, &depthProj[0][0]);
}