#include "renderer.h"

/**
 * UpdateImGui -
 */

void Renderer::UpdateImGui()
{
    ImGuiIO& io = ImGui::GetIO();

    // Send inputs to IMGUI and start a new frame.

    ImGuiGCRTSetMouse(
        mousePos[0],
        mousePos[1],
        mouseDown[0],
        mouseDown[1],
        mouseDown[2]
    );

    for (uint32_t i = 0; i < ImGuiKeys.size(); i++)
    {
        ImGuiGCRTSetKey(ImGuiKeys[i], 1);
    }

    ImGuiGCRTNewFrame();
    RenderSceneWindow();
    //RenderModelWindow();
    RenderRendererWindow();
    ImGui::ShowTestWindow();
    ImGui::Render();

    // Clear out any key inputs

    for (uint32_t i = 0; i < ImGuiKeys.size(); i++)
    {
        ImGuiGCRTSetKey(ImGuiKeys[i], 0);
    }
    ImGuiKeys.resize(0);
}

/**
 * RenderSceneWindow
 */

void Renderer::RenderSceneWindow()
{
    bool open = true;
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene");

    map<string, Model>::iterator modelIt;
    map<string, RMaterial>::iterator matIt;
    map<string, GLuint>::iterator texIt;

    static int curDiffTex;
    static int curNormTex;
    static int curMat;
    static int curMesh;

    // Models

    if (ImGui::CollapsingHeader("Models"))
    {
        static float newPos[3];

        for (modelIt = scn.models.begin(); modelIt != scn.models.end(); modelIt++)
        {
            if (ImGui::TreeNode((*modelIt).first.c_str()))
            {
                ImGui::Text("Material: %s", (*modelIt).second.mat.name.c_str());
                
                ImGui::Text(
                    "Pos - x:%3.2f y:%3.2f z:%3.2f",
                    (*modelIt).second.pGeom->pos.x,
                    (*modelIt).second.pGeom->pos.y,
                    (*modelIt).second.pGeom->pos.z
                );

                ImGui::TreePop();
            }
        }

        if (ImGui::Button("New Model..."))
        {
            ImGui::OpenPopup("New Model...");
        }

        if (ImGui::BeginPopupModal("New Model...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputFloat3("Position", newPos, -1);

            ImGui::Combo(
                "Material",
                &curMat,
                &scn.materialNames[0],
                (int)scn.materialNames.size()
            );

            ImGui::Combo(
                "Mesh",
                &curMesh,
                &scn.geometryNames[0],
                (int)scn.geometryNames.size()
            );

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    // Lights

    if (ImGui::CollapsingHeader("Lights"))
    {

    }

    // Camera

    if (ImGui::CollapsingHeader("Camera"))
    {

    }

    // Materials

    if (ImGui::CollapsingHeader("Materials"))
    {
        for (matIt = scn.materials.begin(); matIt != scn.materials.end(); matIt++)
        {
            if (ImGui::TreeNode((*matIt).first.c_str()))
            {
                ImGui::Text("Specular: %3.2f", (*matIt).second.spec);
                
                ImGui::Text(
                    "Kd: %3.2f, %3.2f, %3.2f", 
                    (*matIt).second.kd.x,
                    (*matIt).second.kd.y,
                    (*matIt).second.kd.z
                );
                
                ImGui::Text("Diffuse Texture: %s", (*matIt).second.diffTexName.c_str());
                ImGui::Text("Normal Texture: %s", (*matIt).second.normalTexName.c_str());

                ImGui::TreePop();
            }
        }

        if (ImGui::Button("New Material..."))
        {
            ImGui::OpenPopup("New Material...");
        }

        if (ImGui::BeginPopupModal("New Material...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static float newKd[3];
            static float newSpec;
            static char newName[256];
            static int useShadows;

            ImGui::InputText("Name", newName, 256);
            ImGui::InputFloat3("Diffuse", newKd, -1);
            ImGui::InputFloat("Specular", &newSpec);
            ImGui::InputInt("Use Shadows", &useShadows);

            ImGui::Combo(
                "Diffuse Texture", 
                &curDiffTex, 
                &scn.diffTexNames[0],
                (int)scn.diffTexNames.size()
            );
            
            ImGui::Combo(
                "Normal Texture", 
                &curNormTex, 
                &scn.normTexNames[0],
                (int)scn.normTexNames.size()
            );

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                RMaterial newMat;
                newMat.name = newName;
                newMat.kd = vec3(newKd[0], newKd[1], newKd[2]);
                newMat.spec = newSpec;
                newMat.UseShadows(useShadows == 0 ? false : true);
                
                newMat.SetNormalTex(
                    scn.normTextures[scn.normTexNames[curNormTex]], 
                    scn.normTexNames[curNormTex]
                );
                
                newMat.SetDiffuseTex(
                    scn.diffTextures[scn.diffTexNames[curDiffTex]], 
                    scn.diffTexNames[curDiffTex]
                );

                scn.materials[newName] = newMat;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    // Textures

    if (ImGui::CollapsingHeader("Textures"))
    {
        ImGui::Indent();
        static bool loadFailed = false;
        static char newTexName[256];
        static char newTexPath[256];

        for (texIt = scn.diffTextures.begin(); texIt != scn.diffTextures.end(); texIt++)
        {
            ImGui::Text((*texIt).first.c_str());
        }
        
        for (texIt = scn.normTextures.begin(); texIt != scn.normTextures.end(); texIt++)
        {
            ImGui::Text((*texIt).first.c_str());
        }

        if (ImGui::Button("New..."))
        {
            ImGui::OpenPopup("New...");
        }

        if (loadFailed)
        {
            ImGui::Text("Failed to load texture:");
            ImGui::Text(newTexPath);
        }

        if (ImGui::BeginPopupModal("New...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Name", newTexName, 256);
            ImGui::InputText("Path", newTexPath, 256);
            loadFailed = false;

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                GLuint newID = ImgLoader::LoadTexture(string(newTexPath));
                if (newID != 0)
                {
                    scn.AddDiffTexture(newTexName, newID);
                }
                else
                {
                    loadFailed = true;
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Unindent();
    }

    ImGui::End();
}

/**
* RenderModelWindow
*/

void Renderer::RenderModelWindow()
{
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Model Properties");

    // Material.

    ImGui::Text("Material");

    if (ImGui::InputFloat3("DiffuseColor", (float*)&selected.kd, -1) && selected.name != string(""))
    {
        scn.models[selected.name].mat.kd = selected.kd;
    }

    if (ImGui::InputFloat("Specular", (float*)&selected.specular))
    {
        scn.models[selected.name].mat.spec = selected.specular;
    }

    // Geometry.

    ImGui::Text("Geometry");

    if (ImGui::InputFloat3("Pos", (float*)&selected.pos))
    {
        scn.models[selected.name].pGeom->Translate(selected.pos);
    }

    if (ImGui::InputFloat3("Scale", (float*)&selected.scale))
    {
        scn.models[selected.name].pGeom->Scale(selected.scale);
    }

    ImGui::End();
}

/**
* RenderRendererWindow
*/

void Renderer::RenderRendererWindow()
{
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Renderer");

    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate
    );

    ImGui::End();
}