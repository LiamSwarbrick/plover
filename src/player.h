#ifndef PLVR_PLAYER_H
#define PLVR_PLAYER_H

#include "plvr_basic.h"
#include "plvr_math.h"
#include "plvr_win32_platform.h"

// NOTE: Camera settings
static float mouse_sensitivity = 0.1f;
static float fovy = PLVR_RAD(90.0f);  // NOTE: In radians
static float near_plane = 0.05f;
static float far_plane = 500.0f;

// NOTE: Player movement settings
// ..

typedef struct Player
{
    Vector3 position;
    Vector3 rotation;
    float aspect;
    Matrix4 camera_matrix;  // NOTE: ViewProjection matrix
}
Player;

void
player_update(Player* player, float delta)
{
    // NOTE: Camera movement
    static Vector2i mouse_relative = { 0, 0 };
    mouse_get_relative(1920 / 2, 1080 / 2, &mouse_relative.x, &mouse_relative.y);
    mouse_set_pos(1920 / 2, 1080 / 2);
    
    player->rotation.x = player->rotation.x + (float)mouse_relative.y * mouse_sensitivity;
    // NOTE: X rotation lock so no going upside-down
    if (player->rotation.x < -90.0f)
    {
        player->rotation.x = -90.0f;
    }
    else if (player->rotation.x > 90.0f)
    {
        player->rotation.x = 90.0f;
    }
    
    player->rotation.y += (float)mouse_relative.x * mouse_sensitivity;

    float speed = 5.0f * delta;
    if (GetKeyState(VK_LCONTROL) < 0)
        speed *= 3.0f;
    float dx = speed * sinf(rad(player->rotation.y));
    float dz = speed * cosf(rad(player->rotation.y));
    if (key_is_down('W'))
    {
        player->position.x += dx;
        player->position.z -= dz;
    }
    else if (key_is_down('S'))
    {
        player->position.x -= dx;
        player->position.z += dz;
    }
    if (key_is_down('A'))
    {
        player->position.x -= dz;
        player->position.z -= dx;
    }
    else if (key_is_down('D'))
    {
        player->position.x += dz;
        player->position.z += dx;
    }
    
    if (key_is_down(VK_SPACE))
        player->position.y += speed;
    if (key_is_down(VK_SHIFT))
        player->position.y -= speed;
    
    // NOTE: View projection matrix
    Matrix4 view = MAT4_IDENTITY_INIT;
    m4_rotate(view, rad(player->rotation.x), (Vector3){ 1.0f, 0.0f, 0.0f }, view);
    m4_rotate(view, rad(player->rotation.y), (Vector3){ 0.0f, 1.0f, 0.0f }, view);
    m4_translate(view, v3_negate(player->position), view);
    
    Matrix4 projection;
    m4_perspective(near_plane, far_plane, fovy, player->aspect, projection);
    
    m4_mul(projection, view, player->camera_matrix);
}

#endif  // PLVR_PLAYER_H