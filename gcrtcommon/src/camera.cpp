#include "camera.h"

const uint32_t KEY_W = 0x57;
const uint32_t KEY_A = 0x41;
const uint32_t KEY_S = 0x53;
const uint32_t KEY_D = 0x44;
const uint32_t KEY_P = 0x50;
const uint32_t KEY_L = 0x4C;
const uint32_t KEY_0 = 0x30;
const uint32_t KEY_1 = 0x31;
const uint32_t KEY_2 = 0x32;
const uint32_t KEY_3 = 0x33;
const uint32_t KEY_4 = 0x34;
const uint32_t KEY_5 = 0x35;
const uint32_t KEY_6 = 0x36;
const uint32_t KEY_7 = 0x37;
const uint32_t KEY_8 = 0x38;
const uint32_t KEY_9 = 0x39;

Camera::Camera(uint32_t w, uint32_t h) : pos(vec3(10.0)), lookDir(normalize(vec3(-1.0))), up(vec3(0.0, 0.0, 1.0)),
    p1(0.0f), p2(40.0f), txSpeed(0.2f), fovSpeed(0.2f), rotSpeed(0.01f), downKeys{false},
    bMouseDown(false), aspect((float)w / (float)h), fov(90.0f), nclip(1.0f), fclip(10000.0f)
{
}

/**
 * Camera::HandleMouseDown Handle mouse click down events. Camera is panned
 * by clicking and dragging. Collect initial mouse position on click.
 *
 * @param mouse Mouse x and y coordinates.
 */

void Camera::HandleMouseDown(LPARAM mouse)
{
    bMouseDown = true;
    p1.x = (float)GET_X_LPARAM(mouse);
    p1.y = (float)GET_Y_LPARAM(mouse) + 40.0f;
}

/**
 * Camera::HandleMouseUp Handle mouse click up events. Camera is panned
 * by clicking and dragging. Reset mouse positions when click is released.
 */

void Camera::HandleMouseUp()
{
    p1.x = 0.0f;
    p1.y = 0.0f;
    p2.x = 0.0f;
    p2.y = 0.0f;
    bMouseDown = false;
}

/**
 * Camera::HandleMouseMove When mouse button is down and mouse moving,
 * continuously update mouse positions.
 *
 * @param mouse New mouse x and y coordinates.
 */

void Camera::HandleMouseMove(LPARAM mouse)
{
    if (bMouseDown == true)
    {
        p2.x = p1.x;
        p2.y = p1.y;
        p1.x = (float)GET_X_LPARAM(mouse);
        p1.y = (float)GET_Y_LPARAM(mouse) + 40.0f;

        float d = 1;
        p1.x = p2.x + (p1.x - p2.x) * d;
        p1.y = p2.y + (p1.y - p2.y) * d;
    }
}

/**
 * Camera::HandleKeyDown Register any keyboard presses.
 * @param key Key that was pressed.
 */

void Camera::HandleKeyDown(WPARAM key)
{
    downKeys[key] = true;
}

/**
 * Camera::HandleKeyUp Register any keyboard releases.
 * @param key Register key that was released.
 */

void Camera::HandleKeyUp(WPARAM key)
{
    downKeys[key] = false;
}

/**
 * Camera::GetView Get view matrix for MVP matrix from current camera.
 * @return View matrix.
 */

mat4 Camera::GetView()
{
    return lookAt(
        pos,
        lookDir,
        up
    );
}

/**
 * Camera::GetProjection Get projection for MVP matrix from current camera.
 * @return Projection matrix.
 */

mat4 Camera::GetProjection()
{
    return perspective(
        glm::radians(fov),
        aspect,
        nclip,
        fclip
    );
}

/**
 * Camera::Update Update camera for current frame from mouse and keyboard
 * inputs.
 */

void Camera::Update()
{
    vec3 fwd = lookDir - pos;

    // Forward

    if (downKeys[KEY_W] == true)
    {
        pos += txSpeed * fwd;
        lookDir += txSpeed * fwd;
    }

    // Back

    if (downKeys[KEY_S] == true)
    {
        pos -= txSpeed * fwd;
        lookDir -= txSpeed * fwd;
    }

    // Right

    if (downKeys[KEY_D] == true)
    {
        vec3 right = normalize(cross(fwd, up));
        pos += txSpeed * right;
        lookDir += txSpeed * right;
    }

    // Left

    if (downKeys[KEY_A] == true)
    {
        vec3 right = normalize(cross(fwd, up));
        pos -= txSpeed * right;
        lookDir -= txSpeed * right;
    }

    // FOV Wider

    if (downKeys[KEY_P] == true)
    {
        fov += fovSpeed;
        if (fov > 160.0)
        {
            fov = 160.0;
        }
    }

    // FOV Narrow

    if (downKeys[KEY_L] == true)
    {
        fov -= fovSpeed;
        if (fov < 10.0)
        {
            fov = 10.0;
        }
    }

    // Mouse rotations.

    if (bMouseDown == true)
    {
        if (p2.x != 0 && p2.y != 0)
        {
            vec3 fwd = lookDir - pos;
            vec3 level = normalize(vec3(fwd.x, fwd.y, 0.0f));

            float theta = dot(fwd, level);
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            
            
            float dphi = rotSpeed * dx / cosf(theta);
            float dtheta = rotSpeed * dy;

            mat4 rot = rotate(dphi, vec3(0.0, 0.0, 1.0));
            vec4 tmp4 = rot * vec4(fwd, 1);
            tmp4.z += dtheta;
            vec3 tmp3 = normalize(vec3(tmp4.x, tmp4.y, tmp4.z));
            lookDir = pos + tmp3;

            p2.x = p1.x;
            p2.y = p1.y;
        }
    }
}