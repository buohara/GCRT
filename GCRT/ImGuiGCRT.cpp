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
    RenderSelectionWindow();
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
    map<string, Tex>::iterator texIt;

    static int curDiffTex;
    static int curNormTex;
    static int curMat;
    static int curMesh;

    // Models

    if (ImGui::CollapsingHeader("Models"))
    {
        ImGui::Indent();

        static float newPos[3];
        static float newScale[3];
        static char newName[256];

        for (modelIt = scn.models.begin(); modelIt != scn.models.end(); modelIt++)
        {
            if (ImGui::TreeNode((*modelIt).first.c_str()))
            {
                ImGui::Text("Material: %s", (*modelIt).second.matName.c_str());
                
                ImGui::Text(
                    "Pos - x:%3.2f y:%3.2f z:%3.2f",
                    (*modelIt).second.pos.x,
                    (*modelIt).second.pos.y,
                    (*modelIt).second.pos.z
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
            ImGui::InputText("Name", newName, 256);
            ImGui::InputFloat3("Position", newPos, -1);
            ImGui::InputFloat3("Scale", newScale, -1);

            ImGui::Combo(
                "Material",
                &curMat,
                &scn.materialNames[0],
                (int)scn.materialNames.size()
            );

            ImGui::Combo(
                "Mesh",
                &curMesh,
                &scn.meshNames[0],
                (int)scn.meshNames.size()
            );

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                Model newModel;
                newModel.meshName = scn.meshNames[curMesh];
                newModel.matName = scn.materialNames[curMat];
                newModel.Translate(vec3(newPos[0], newPos[1], newPos[2]));
                newModel.Scale(vec3(newScale[0], newScale[1], newScale[2]));
                newModel.pickerColor = nextPickerColor();
                scn.AddModel(string(newName), newModel);

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Unindent();
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
        ImGui::Indent();

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
            static bool useShadows;
            static bool useDiffuseTex;

            ImGui::InputText("Name", newName, 256);
            ImGui::InputFloat3("Diffuse Color", newKd, -1);
            ImGui::InputFloat("Specular", &newSpec);
            ImGui::Checkbox("Use Shadows", &useShadows);

            ImGui::Checkbox("Use Diffuse Texture", &useDiffuseTex);

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
                    scn.normTextures[scn.normTexNames[curNormTex]].texID, 
                    scn.normTexNames[curNormTex]
                );
                
                if (useDiffuseTex)
                {
                    newMat.SetDiffuseTex(
                        scn.diffTextures[scn.diffTexNames[curDiffTex]].texID,
                        scn.diffTexNames[curDiffTex]
                    );
                }

                scn.AddMaterial(newName, newMat);

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Unindent();
    }

    // Textures

    if (ImGui::CollapsingHeader("Textures"))
    {
        ImGui::Indent();
        static bool loadFailed = false;
        static char newTexName[256];
        static char newTexPath[256];
        static bool normalTexture;

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
            ImGui::Checkbox("Normal Texture", &normalTexture);
            loadFailed = false;

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                GLuint newID = ImgLoader::LoadTexture(string(newTexPath));
                if (newID != 0)
                {
                    if (normalTexture)
                    {
                        scn.AddNormTexture(newTexName, string(newTexPath), newID);
                    }
                    else
                    {
                        scn.AddDiffTexture(newTexName, string(newTexPath), newID);
                    }
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

    // Save Dialog

    if (ImGui::Button("Save scene..."))
    {
        ImGui::OpenPopup("Save scene...");
    }

    if (ImGui::BeginPopupModal("Save scene...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char saveFile[256];
        ImGui::InputText("Save as...", saveFile, 256);

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            scn.Save(string(saveFile));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::End();
}

/**
* RenderSelectionWindow
*/

void Renderer::RenderSelectionWindow()
{
    if (selected == "")
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Model Properties");

    Model &model = scn.models[selected];
    RMaterial &mat = scn.materials[model.matName];

    vec3 kd = mat.kd;
    float spec = mat.spec;
    vec3 pos = model.pos;
    vec3 scale = model.dims;
    vec3 rotate = model.angles;

    // Material.

    ImGui::Text("Material");

    // Diffuse color.

    ImGui::Spacing();
    ImGui::Text("Diffuse");
    ImGui::Spacing();

    if (ImGui::InputFloat("Diffuse r", &kd.x, 0.01f, 1.0f))
    {
        mat.kd.x = kd.x;
    }

    if (ImGui::InputFloat("Diffuse g", &kd.y, 0.01f, 1.0f))
    {
        mat.kd.y = kd.y;
    }

    if (ImGui::InputFloat("Diffuse b", &kd.z, 0.01f, 1.0f))
    {
        mat.kd.z = kd.z;
    }

    ImGui::Spacing();

    if (ImGui::SliderFloat3("Diffuse", &kd[0], 0.0, 5.0))
    {
        mat.kd = kd;
    }

    // Shininess

    ImGui::Spacing();
    ImGui::Text("Specular");
    ImGui::Spacing();

    if (ImGui::InputFloat("Specular", &spec, 0.01f, 1.0f))
    {
        mat.spec = spec;
    }

    ImGui::Spacing();

    if (ImGui::SliderFloat("Specular ", &spec, 0.0f, 32.0f))
    {
        mat.spec = spec;
    }

    // Transofrmations.

    ImGui::Spacing();
    ImGui::Text("Transformation");
    ImGui::Spacing();

    // Translate

    ImGui::Spacing();
    ImGui::Text("Position");
    ImGui::Spacing();

    if (ImGui::InputFloat("Pos x", &pos.x, 0.01f, 1.0f))
    {
        model.Translate(pos);
    }

    if (ImGui::InputFloat("Pos y", &pos.y, 0.01f, 1.0f))
    {
        model.Translate(pos);
    }

    if (ImGui::InputFloat("Pos z", &pos.z, 0.01f, 1.0f))
    {
        model.Translate(pos);
    }

    ImGui::Spacing();

    if (ImGui::SliderFloat3("Pos", &pos[0], -20.0, 20.0))
    {
        model.Translate(pos);
    }

    // Scale

    ImGui::Spacing();
    ImGui::Text("Scale");
    ImGui::Spacing();

    if (ImGui::InputFloat("Scale x", &scale.x, 0.01f, 1.0f))
    {
        model.Scale(scale);
    }

    if (ImGui::InputFloat("Scale y", &scale.y, 0.01f, 1.0f))
    {
        model.Scale(scale);
    }

    if (ImGui::InputFloat("Scale z", &scale.z, 0.01f, 1.0f))
    {
        model.Scale(scale);
    }

    ImGui::Spacing();

    if (ImGui::SliderFloat3("Scale", &scale[0], -20.0, 20.0))
    {
        model.Scale(scale);
    }

    // Rotate

    ImGui::Spacing();
    ImGui::Text("Rotation");
    ImGui::Spacing();

    if (ImGui::InputFloat("Rot x", &rotate.x, 0.01f, 1.0f))
    {
        model.Rotate(rotate);
    }

    if (ImGui::InputFloat("Rot y", &rotate.y, 0.01f, 1.0f))
    {
        model.Rotate(rotate);
    }

    if (ImGui::InputFloat("Rot z", &rotate.z, 0.01f, 1.0f))
    {
        model.Rotate(rotate);
    }

    ImGui::Spacing();

    if (ImGui::SliderFloat3("Rotate", &rotate[0], -pi<float>(), pi<float>()))
    {
        model.Rotate(rotate);
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