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
    RenderModelWindow();
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
    map<string, Model> &models = scn.models;

    map<string, RMaterial>::iterator matIt;
    map<string, RMaterial> &materials = scn.materials;

    map<string, GLuint>::iterator texIt;
    map<string, GLuint> &textures = scn.textures;

    static int curItem;

    // Models

    if (ImGui::CollapsingHeader("Models"))
    {
        const char* matNames[256];
        uint32_t matCnt = 0;

        for (matIt = materials.begin(); matIt != materials.end(); matIt++)
        {
            matNames[matCnt++] = (*matIt).first.c_str();
        }

        for (modelIt = models.begin(); modelIt != models.end(); modelIt++)
        {
            if (ImGui::TreeNode((*modelIt).first.c_str()))
            {
                ImGui::Combo("Material", &curItem, matNames, matCnt);
                ImGui::TreePop();
            }
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
        if (ImGui::Button("New..."))
        {
            ImGui::OpenPopup("New...");
        }

        if (ImGui::BeginPopupModal("New...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static float newKd[3];
            static float newSpec;
            static char newName[256];

            const char* texNames[] = { "Dirt", "Grass" };
            const char* normalTexNames[] = { "DirtNormal", "GrassNormal" };

            ImGui::InputText("Name", newName, 256);
            ImGui::InputFloat3("Diffuse", newKd, -1);
            ImGui::InputFloat("Specular", &newSpec);

            ImGui::Combo("Diffuse Texture", &curItem, texNames, 2);
            ImGui::Combo("Normal Texture", &curItem, normalTexNames, 2);

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        for (matIt = materials.begin(); matIt != materials.end(); matIt++)
        {
            if (ImGui::TreeNode((*matIt).first.c_str()))
            {
                ImGui::TreePop();
            }
        }

    }

    // Textures

    if (ImGui::CollapsingHeader("Textures"))
    {
        ImGui::Indent();
        for (texIt = textures.begin(); texIt != textures.end(); texIt++)
        {
            ImGui::Text((*texIt).first.c_str());
        }
        
        if (ImGui::Button("New..."))
        {
            ImGui::OpenPopup("New...");
        }

        if (ImGui::BeginPopupModal("New...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char newTexName[256];
            static char newTexPath[256];

            ImGui::InputText("Name", newTexName, 256);
            ImGui::InputText("Path", newTexPath, 256);

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                GLuint newID = ImgLoader::LoadTexture(string(newTexPath));
                if (newID != 0)
                {
                    scn.textures[newTexName] = newID;
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