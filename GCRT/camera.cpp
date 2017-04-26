#include "camera.h"

const uint32_t KEY_W = 0x57;
const uint32_t KEY_A = 0x41;
const uint32_t KEY_S = 0x53;
const uint32_t KEY_D = 0x44;
const uint32_t KEY_P = 0x50;
const uint32_t KEY_L = 0x4C;

/**
 * Init - Initialize camera parameters.
 *
 * @param posIn Camera position.
 * @param lookAtIn Point camera is looking at.
 * @param upIn Which direction is up for the camera.
 * @param aspectIn Camera aspect ratio.
 * @param fovIn Field of view angle (degrees).
 * @param nClipIn Near clipping plane.
 * @param fClipIn Far clipping plane.
 */

void Camera::Init(
    vec3 posIn,
    vec3 lookAtIn,
    vec3 upIn,
    float aspectIn,
    float fovIn,
    float nclipIn,
    float fclipIn
)
{
    pos     = posIn;
    up      = upIn;
    aspect  = aspectIn;
    fov     = fovIn;
    nclip   = nclipIn;
    fclip   = fclipIn;

    vec3 fwd = lookAtIn - pos;
    vec3 fwdNormal = normalize(fwd);
    lookDir = pos + fwdNormal;
}


/**
 * HandleMouseDown - Store mouse coordinates on click button down.
 *
 * @param mouse LPARAM with x and y mouse coordinates of click.
 */

void Camera::HandleMouseDown(LPARAM mouse)
{
    bMouseDown = true;
    p1.x = (float)GET_X_LPARAM(mouse);
    p1.y = (float)GET_Y_LPARAM(mouse);
}

/**
 * HandleMouseUp - Clear all mouse coordinates when mouse released.
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
 * HandleMouseMove - Update new mouse coordinates on drag
 * (if mouse button is being held down).
 * 
 * @param mouse LPARAM with mouse coordinates.
 */

void Camera::HandleMouseMove(LPARAM mouse)
{
    if (bMouseDown == true)
    {
        p2.x = p1.x;
        p2.y = p1.y;
        p1.x = (float)GET_X_LPARAM(mouse);
        p1.y = (float)GET_Y_LPARAM(mouse);
    }
}

/**
 * HandleKeyDown - Mark a key as down when it's pressed.
 *
 * @param key VK code for pressed key.
 */

void Camera::HandleKeyDown(WPARAM key)
{
    switch (key)
    {
    case KEY_W:
        downKeys[KEY_W] = true;
        break;
    case KEY_A:
        downKeys[KEY_A] = true;
        break;
    case KEY_S:
        downKeys[KEY_S] = true;
        break;
    case KEY_D:
        downKeys[KEY_D] = true;
        break;
    case KEY_P:
        downKeys[KEY_P] = true;
        break;
    case KEY_L:
        downKeys[KEY_L] = true;
        break;
    default:
        break;
    }
}

/**
 * HandleKeyUp - Mark when a key is released.
 *
 * @param key VK code of released key.
 */

void Camera::HandleKeyUp(WPARAM key)
{
    switch (key)
    {
    case KEY_W:
        downKeys[KEY_W] = false;
        break;
    case KEY_A:
        downKeys[KEY_A] = false;
        break;
    case KEY_S:
        downKeys[KEY_S] = false;
        break;
    case KEY_D:
        downKeys[KEY_D] = false;
        break;
    case KEY_P:
        downKeys[KEY_P] = false;
        break;
    case KEY_L:
        downKeys[KEY_L] = false;
        break;
    default:
        break;
    }
}

/**
 * GetView - Get view matrix from current camera orientation.
 *
 * @return 4x4 view matrix.
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
 * GetProjection - Get projection matrix from current camera settings.
 *
 * @return 4x4 projection matrix.
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
 * Update - Update camera from mouse/keyboard inputs for this frame.
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