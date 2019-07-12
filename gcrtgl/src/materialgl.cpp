#include "materialgl.h"

/**
 * [RMaterial::RMaterial description]
 */

RMaterial::RMaterial()
{
    useNormalMap = 0;
    useDiffuseMap = 0;
    useSSS = 0;
    useShadows = 0;
    spec = 1.0;

    diffTexName = string("NA");
    normalTexName = string("NA");
}

/**
 * [RMaterial::SetDiffuseTex description]
 * @param diffuseTex [description]
 * @param name       [description]
 */

void RMaterial::SetDiffuseTex(GLuint diffuseTex, string name)
{
    diffTexName = name;
    diffuseTexID = diffuseTex;
    useDiffuseMap = 1;
}

/**
 * [RMaterial::SetNormalTex description]
 * @param normalTex [description]
 * @param name      [description]
 */

void RMaterial::SetNormalTex(GLuint normalTex, string name)
{
    normalTexName = name;
    normalTexID = normalTex;
    useNormalMap = 1;
}

/**
 * [RMaterial::ApplyMaterial description]
 * @param program [description]
 */

void RMaterial::ApplyMaterial(GLuint program)
{
    GLuint useNormalID = glGetUniformLocation(program, "useNormalMap");
    glUniform1i(useNormalID, useNormalMap);

    GLuint useDiffuseID = glGetUniformLocation(program, "useDiffuseMap");
    glUniform1i(useDiffuseID, useDiffuseMap);

    GLuint useSSSID = glGetUniformLocation(program, "useSSS");
    glUniform1i(useSSSID, useSSS);

    GLuint useShadowsID = glGetUniformLocation(program, "useShadows");
    glUniform1i(useShadowsID, useShadows);

    GLuint specID = glGetUniformLocation(program, "shininess");
    glUniform1f(specID, spec);

    GLuint phongID = glGetUniformLocation(program, "usePhong");
    glUniform1i(phongID, usePhong);

    GLuint useEnvMapID = glGetUniformLocation(program, "useEnvMap");
    glUniform1i(useEnvMapID, useEnvMap);

    // Texture or diffuse color.

    if (useDiffuseMap == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexID);
        GLuint diffTexID = glGetUniformLocation(program, "diffuseTex");
        glUniform1i(diffTexID, 0);
    }
    else
    {
        GLuint kdID = glGetUniformLocation(program, "kd");
        glUniform3fv(kdID, 1, &kd[0]);
    }

    // Normal map or geometry normals

    if (useNormalMap == 1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexID);
        GLuint normTexID = glGetUniformLocation(program, "normalTex");
        glUniform1i(normTexID, 1);
    }
}

/**
 * [RMaterial::SetLights description]
 * @param dirLights [description]
 * @param ptLights  [description]
 * @param program   [description]
 */

void RMaterial::SetLights(vector<Light> &lights, GLuint program)
{
    if (lights[0].type == DIRECTIONAL)
    {
        vec3 dir = lights[0].dir;
        vec3 pos = -dir;

        mat4 depthView = lookAt(-dir, dir, vec3(0.0, 1.0, 0.0));
        mat4 depthProj = ortho(-10.0, 10.0, -10.0, 10.0, 1.0, 100.0);

        GLuint lightPosID = glGetUniformLocation(program, "lightPos");
        glUniform3fv(lightPosID, 1, &pos[0]);

        GLuint lightViewID = glGetUniformLocation(program, "lightView");
        glUniformMatrix4fv(lightViewID, 1, false, &depthView[0][0]);

        GLuint lightProjID = glGetUniformLocation(program, "lightProj");
        glUniformMatrix4fv(lightProjID, 1, false, &depthProj[0][0]);
    }
}