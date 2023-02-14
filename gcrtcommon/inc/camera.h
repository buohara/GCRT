#pragma once

#include "gcrtcommon.h"

using namespace std;
using namespace glm;

struct Camera
{
    Camera(uint32_t w, uint32_t h);

    vec3 pos;
    vec3 lookDir;
    vec3 up;

    vec2 p1;
    vec2 p2;

    float aspect;
    float fov;
    float nclip;
    float fclip;

    const float txSpeed;
    const float fovSpeed;
    const float rotSpeed;

    bool downKeys[256];
    bool bMouseDown;

    mat4 projection;
    mat4 view;

    double mouseTime;

    void HandleMouseDown(LPARAM mouse);
    void HandleMouseUp();

    void HandleMouseMove(LPARAM mouse);
    void HandleKeyDown(WPARAM key);
    void HandleKeyUp(WPARAM key);
    void Update();
    mat4 GetProjection();
    mat4 GetView();

private:

    Camera();
};