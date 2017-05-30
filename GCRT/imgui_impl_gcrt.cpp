#include "imgui_impl_gcrt.h"

/** 
 * Globals
 */

static double g_Time = 0.0f;
static bool g_MousePressed[3] = { false, false, false };
static float g_MouseWheel = 0.0f;
static GLuint g_FontTexture = 0;
static int g_ShaderHandle = 0;
static int g_VertHandle = 0;
static int g_FragHandle = 0;
static int g_AttribLocationTex = 0; 
static int g_AttribLocationProjMtx = 0;
static int g_AttribLocationPosition = 0;
static int g_AttribLocationUV = 0;
static int g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0; 
static unsigned int g_VaoHandle = 0;
static unsigned int g_ElementsHandle = 0;

/**
 * ImGuiGCRTRenderDrawLists - Main rendering callback for IMGUI. Save current GCRT
 * GL state, set IMGUI render state, draw, then restore GCRT GL state.
 */

void ImGuiGCRTRenderDrawLists(ImDrawData *drawData)
{
    ImGuiIO& io = ImGui::GetIO();
    drawData->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state (lifted from ImGui_ImplGlfwGL3_RenderDrawLists).
    
    GLint last_active_texture; 
    glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    
    GLint last_program; 
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    
    GLint last_texture; 
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    
    GLint last_array_buffer; 
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    
    GLint last_element_array_buffer;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);

    GLint last_vertex_array;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    
    GLint last_blend_src_rgb; 
    glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
    
    GLint last_blend_dst_rgb; 
    glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
    
    GLint last_blend_src_alpha; 
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
    
    GLint last_blend_dst_alpha; 
    glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
    
    GLint last_blend_equation_rgb; 
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    
    GLint last_blend_equation_alpha; 
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    
    GLint last_viewport[4]; 
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    
    GLint last_scissor_box[4]; 
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    
    GLboolean last_enable_blend         = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face     = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test    = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test  = glIsEnabled(GL_SCISSOR_TEST);

    // Setup ImGUI render state.

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    
    const float ortho_projection[4][4] =
    {
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        { -1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(g_VaoHandle);

    // Do the draws.

    float fbHeight = io.DisplaySize.y;
    float fbWidth = io.DisplaySize.x;

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        
        glBufferData(
            GL_ARRAY_BUFFER, 
            (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), 
            (const GLvoid*)cmd_list->VtxBuffer.Data, 
            GL_STREAM_DRAW
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, 
            (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), 
            (const GLvoid*)cmd_list->IdxBuffer.Data, 
            GL_STREAM_DRAW
        );

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                
                glScissor(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);
                
                glDrawElements(
                    GL_TRIANGLES, 
                    (GLsizei)pcmd->ElemCount, 
                    sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 
                    idx_buffer_offset
                );
            }

            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore GCRT GL state.

    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    
    glBlendFuncSeparate(
        last_blend_src_rgb, 
        last_blend_dst_rgb, 
        last_blend_src_alpha, 
        last_blend_dst_alpha
    );
    
    if (last_enable_blend)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    
    if (last_enable_cull_face)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    
    if (last_enable_depth_test)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    if (last_enable_scissor_test)
    {
        glEnable(GL_SCISSOR_TEST);
    }
    else
    {
        glDisable(GL_SCISSOR_TEST);
    }
    
    glViewport(
        last_viewport[0], 
        last_viewport[1], 
        (GLsizei)last_viewport[2], 
        (GLsizei)last_viewport[3]
    );

    glScissor(
        last_scissor_box[0], 
        last_scissor_box[1], 
        (GLsizei)last_scissor_box[2], 
        (GLsizei)last_scissor_box[3]
    );
}

/**
 * ImGuiGCRTSetMouse -
 */

void ImGuiGCRTSetMouse(double x, double y, bool lDown, bool rDown, bool mDown)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    io.MouseDown[0] = lDown;
    io.MouseDown[1] = mDown;
    io.MouseDown[2] = mDown;
}

/**
 * ImGuiGCRTNewFrame - 
 */

void ImGuiGCRTNewFrame()
{
    if (!g_FontTexture)
    {
        ImGuiGCRTCreateDeviceObjects();
    }

    ImGuiIO& io = ImGui::GetIO();
    
    // Setup time

    auto now = system_clock::now();
    auto nowSec = time_point_cast<std::chrono::seconds>(now);
    double currentTime = (double)nowSec.time_since_epoch().count();

    io.DeltaTime = g_Time > 0.0 ? (float)(currentTime - g_Time) : (float)(1.0f / 60.0f);
    g_Time = currentTime;

    ImGui::NewFrame();
}

/**
 * ImGuiGCRTCreateFontsTexture -
 */

bool ImGuiGCRTCreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

/**
 * ImGuiGCRTCreateDeviceObjects -
 */

bool ImGuiGCRTCreateDeviceObjects()
{
    // Backup GL state

    GLint last_texture; 
    GLint last_array_buffer;
    GLint last_vertex_array;
    
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    
    glVertexAttribPointer(
        g_AttribLocationPosition, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(ImDrawVert), 
        (GLvoid*)OFFSETOF(ImDrawVert, pos)
    );
    
    glVertexAttribPointer(
        g_AttribLocationUV, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(ImDrawVert), 
        (GLvoid*)OFFSETOF(ImDrawVert, uv)
    );
    
    glVertexAttribPointer(
        g_AttribLocationColor, 
        4, 
        GL_UNSIGNED_BYTE, 
        GL_TRUE, 
        sizeof(ImDrawVert), 
        (GLvoid*)OFFSETOF(ImDrawVert, col)
    );

#undef OFFSETOF

    ImGuiGCRTCreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

/**
 * ImGuiGCRTInit
 */

void ImGuiGCRTInit(HWND hwnd, uint32_t w, uint32_t h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.ImeWindowHandle = hwnd;
    io.RenderDrawListsFn = ImGuiGCRTRenderDrawLists;

    io.KeyMap[ImGuiKey_Enter] = 0xd;    // VK_ENTER
    io.KeyMap[ImGuiKey_Tab] = 0x9;      // VK_TAB
    io.KeyMap[ImGuiKey_Backspace] = 0x8;// VK_BACK
}

/**
 * ImGuiGCRTResize
 */

void ImGuiGCRTResize(uint32_t w, uint32_t h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
}

/**
 * ImGuiGCRTSetKey
 */

void ImGuiGCRTSetKey(int key, int action)
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (action == 1)
    {
        io.KeysDown[key] = true;
    }

    if (action == 0)
    {
        io.KeysDown[key] = false;
    }

    io.KeyShift = io.KeysDown[0x10]; // VK_SHIFT;
}