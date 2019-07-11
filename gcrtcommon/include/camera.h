#pragma once

#include "gcrtcommon.h"

using namespace std;
using namespace glm;

struct Camera
{
    vec3 pos;
    vec3 lookDir;
    vec3 up;

    vec2 p1;
    vec2 p2;

    float aspect;
    float fov;
    float nclip;
    float fclip;

    const float txSpeed = 0.2f;
    const float fovSpeed = 0.2f;
    const float rotSpeed = 0.01f;

    bool downKeys[256] = { false };
    bool bMouseDown = false;

    mat4 projection;
    mat4 view;

    double mouseTime;

    void Init(
        vec3 posIn, 
        vec3 lookAtIn,
        vec3 upIn,
        float aspectIn,
        float fovIn,
        float nclipIn,
        float fclipIn
    );

    void HandleMouseDown(LPARAM mouse);
    void HandleMouseUp();

    void HandleMouseMove(LPARAM mouse);
    void HandleKeyDown(WPARAM key);
    void HandleKeyUp(WPARAM key);
    void Update();
    mat4 GetProjection();
    mat4 GetView();
};