#ifndef PLVR_MATH_H
#define PLVR_MATH_H

// NOTE: When talking about angles, its in radians unless specified otherwise

#include "plvr_basic.h"
// TODO: Look into https://en.wikipedia.org/wiki/Fast_inverse_square_root#Algorithm
// TODO: Replace math.h functions to remove dependency
#include <math.h>
#include <cglm/cglm.h>  // TODO: Replace with own funcs (was doing this origionally but takes ages)

#define PI 3.14159265358979323846f 

typedef struct Recti
{
    int x, y, w, h;
}
Recti;

typedef struct Vector2i
{
    int x, y;
}
Vector2i;

typedef struct Vector3i
{
    int x, y, z;
}
Vector3i;

typedef struct Vector4i
{
    int x, y, z, w;
}
Vector4i;


typedef struct Rect
{
    float x, y, w, h;
}
Rect;

typedef struct Vector2
{
    float x, y;
}
Vector2;

typedef struct Vector3
{
    float x, y, z;
}
Vector3;

typedef struct Vector4
{
    float x, y, z, w;
}
Vector4;

typedef Vector4 Versor;

typedef float Matrix2[2][2];
typedef float Matrix3[3][3];
typedef float Matrix4[4][4];

#define MAT4_IDENTITY_INIT { \
{ 1.0f, 0.0f, 0.0f, 0.0f }, \
{ 0.0f, 1.0f, 0.0f, 0.0f }, \
{ 0.0f, 0.0f, 1.0f, 0.0f }, \
{ 0.0f, 0.0f, 0.0f, 1.0f } \
}
#define MAT4_IDENTITY (Matrix4)MAT4_IDENTITY_INIT

//
/* NOTE: Scalers and util functions: */
//

#define PLVR_MIN(a, b) a < b ? a : b
#define PLVR_MAX(a, b) a > b ? a : b
#define PLVR_RAD(degrees) degrees * (PI / 180.0f);
#define PLVR_DEG(radians) radians * (180.0f / PI);

macro float
rad(float degrees)
{
    // NOTE: Degrees to radians
    return degrees * (PI / 180.0f);
}

macro float
deg(float radians)
{
    // NOTE: Radians to degrees
    return radians * (180.0f / PI);
}

macro float
lerp(float from, float to, float delta)
{
    return from + delta * (to - from);
}

macro float
rad_rotation_clamp(float radians)
{
    return fmod(radians, 2.0f * PI);
}

//
/* NOTE: Vector2 */
//

macro Vector2
v2_add(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x + v.x, u.y + v.y };
}

macro Vector2
v2_sub(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x - v.x, u.y - v.y };
}


macro Vector2
v2_mul(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x * v.x, u.y * v.y };
}

macro Vector2
v2_scale(Vector2 v, float s)
{
    return (Vector2){ v.x * s, v.y * s };
}

macro Vector2
v2_div(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x / v.x, u.y / v.y };
}

macro Vector2
v2_divs(Vector2 v, float s)
{
    return (Vector2){ v.x / s, v.y / s };
}

macro float
v2_inner(Vector2 u, Vector2 v)
{
    return (u.x * v.x) + (u.y * v.y);
}

macro void
v2_outer(Vector2 u, Vector2 v, Matrix2 dest)
{
    dest[0][0] = u.x * v.x;
    dest[0][1] = u.x * v.y;
    dest[1][0] = u.y * v.x;
    dest[1][1] = u.y * v.y;
}

macro float
v2_lengthsq(Vector2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

macro float
v2_length(Vector2 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

macro Vector2
v2_normalized(Vector2 v)
{
    return v2_divs(v, v2_length(v));
}

//
/* NOTE: Vector3 */
//

macro Vector3
v3_add(Vector3 u, Vector3 v)
{
    return (Vector3){ u.x + v.x, u.y + v.y, u.z + v.z };
}

macro Vector3
v3_sub(Vector3 u, Vector3 v)
{
    return (Vector3){ u.x - v.x, u.y - v.y, u.z - v.z };
}

macro Vector3
v3_mul(Vector3 u, Vector3 v)
{
    return (Vector3){ u.x * v.x, u.y * v.y, u.z * v.z };
}

macro Vector3
v3_scale(Vector3 v, float s)
{
    return (Vector3){ v.x * s, v.y * s, v.z * s };
}

macro Vector3
v3_div(Vector3 u, Vector3 v)
{
    return (Vector3){ u.x / v.x, u.y / v.y, u.y / v.y };
}

macro Vector3
v3_divs(Vector3 v, float s)
{
    return (Vector3){ v.x / s, v.y / s, v.z / s };
}

macro Vector3
v3_negate(Vector3 v)
{
    return (Vector3){ -v.x, -v.y, -v.z };
}

macro float
v3_inner(Vector3 u, Vector3 v)
{
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.y);
}

macro void
v3_outer(Vector3 u, Vector3 v, Matrix3 dest)
{
    dest[0][0] = u.x * v.x;
    dest[0][1] = u.x * v.y;
    dest[0][2] = u.x * v.z;

    dest[1][0] = u.y * v.x;
    dest[1][1] = u.y * v.y;
    dest[1][2] = u.y * v.z;

    dest[2][0] = u.z * v.x;
    dest[2][1] = u.z * v.y;
    dest[2][2] = u.z * v.z;
}

macro float
v3_lengthsq(Vector3 v)
{
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

macro float
v3_length(Vector3 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

macro Vector3
v3_normalized(Vector3 v)
{
    return v3_divs(v, v3_length(v));
}

//
/* NOTE: Matrix2: */
//

macro void
m2_copy(Matrix2 source, Matrix2 dest)
{
    dest[0][0] = source[0][0];
    dest[0][1] = source[0][1];
    dest[0][2] = source[0][2];
    dest[0][3] = source[0][3];

    dest[1][0] = source[1][0];
    dest[1][1] = source[1][1];
    dest[1][2] = source[1][2];
    dest[1][3] = source[1][3];
}

//
/* Matrix4: */
//

macro void
m4_copy(Matrix4 source, Matrix4 dest)
{
    dest[0][0] = source[0][0];
    dest[0][1] = source[0][1];
    dest[0][2] = source[0][2];
    dest[0][3] = source[0][3];

    dest[1][0] = source[1][0];
    dest[1][1] = source[1][1];
    dest[1][2] = source[1][2];
    dest[1][3] = source[1][3];

    dest[2][0] = source[2][0];
    dest[2][1] = source[2][1];
    dest[2][2] = source[2][2];
    dest[2][3] = source[2][3];

    dest[3][0] = source[3][0];
    dest[3][1] = source[3][1];
    dest[3][2] = source[3][2];
    dest[3][3] = source[3][3];
}

macro void
m4_scales(Matrix4 m, float s, Matrix4 dest)
{
    dest[0][0] = m[0][0] *= s;
    dest[1][1] = m[1][1] *= s;
    dest[3][2] = m[2][2] *= s;
}

macro void
m4_scale(Matrix4 m, Vector3 scale, Matrix4 dest)
{
    dest[0][0] = m[0][0] * scale.x;
    dest[1][1] = m[1][1] * scale.y;
    dest[3][2] = m[3][2] * scale.z;
}

macro Vector4
m4_mulv(Matrix4 m, Vector4 v)
{
    Vector4 dest;
    dest.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
    dest.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
    dest.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
    dest.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
    return dest;
}

macro void
m4_mul(Matrix4 m1, Matrix4 m2, Matrix4 dest)
{
    // Pulled from cglm
    float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
          a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
          a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
          a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

          b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
          b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
          b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
          b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];
    
    dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

void
m4_inv(Matrix4 m, Matrix4 dest)
{
    glm_mat4_inv(m, dest);
}

void
m4_perspective(float near_plane, float far_plane, float fovy, float aspect, Matrix4 dest)
{
    glm_perspective(fovy, aspect, near_plane, far_plane, dest);
}

void
m4_rotate(Matrix4 m, float angle, Vector3 axis, Matrix4 dest)
{
    vec3 glm_axis = { axis.x, axis.y, axis.z };
    glm_rotate(m, angle, glm_axis);
}

void
m4_translate(Matrix4 m, Vector3 t, Matrix4 dest)
{
    glm_translate_to(m, (vec3){ t.x, t.y, t.z }, dest);
}

#endif  // PLVR_MATH_H
