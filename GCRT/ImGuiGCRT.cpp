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

    if (ImGui::CollapsingHeader("Models"))
    {
        map<string, Model>::iterator it;
        map<string, Model> &models = scn.models;

        for (it = models.begin(); it != models.end(); it++)
        {
            ImGui::TreeNode((*it).first.c_str());
        }
    }

    if (ImGui::CollapsingHeader("Lights"))
    {

    }

    if (ImGui::CollapsingHeader("Camera"))
    {

    }

    if (ImGui::CollapsingHeader("Materials"))
    {
        map<string, RMaterial>::iterator it;
        map<string, RMaterial> &materials = scn.materials;

        for (it = materials.begin(); it != materials.end(); it++)
        {
            ImGui::TreeNode((*it).first.c_str());
        }

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