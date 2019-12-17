#ifndef PLVR_MATH_H
#define PLVR_MATH_H

#include "plvr_basic.h"
// TODO: Look into https://en.wikipedia.org/wiki/Fast_inverse_square_root#Algorithm
// TODO: Replace math.h functions to remove dependency
#include <math.h>


#define PI 3.14159265358979323846

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

#define MAT4_FILL_INIT(value) { \
{ value, value, value, value }, \
{ value, value, value, value }, \
{ value, value, value, value }, \
{ value, value, value, value } \
}
#define MAT4_FILL(value) (Matrix4)MAT4_FILL_INIT

/* Scalers and util functions: */

macro float  // NOTE: Degrees to radians
deg2rad(float degrees)
{
    return degrees * (PI / 180);
}

macro float  // NOTE: Radians to degrees
rad2deg(float radians)
{
    return radians / (180 / PI);
}

/* Vector2 */
macro Vector2  // NOTE: Component-wise multiplication
Vector2_mul(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x * v.x, u.y * v.y };
}

macro Vector2  // NOTE: Vector * scaler
Vector2_scale(Vector2 v, float s)
{
    return (Vector2){ v.x * s, v.y * s };
}

macro Vector2  // NOTE: Component-wise division
Vector2_div(Vector2 u, Vector2 v)
{
    return (Vector2){ u.x / v.x, u.y / v.y };
}

macro Vector2  // NOTE: Vector / scaler
Vector2_divs(Vector2 v, float s)
{
    return (Vector2){ v.x / s, v.y / s };
}

macro float  // NOTE: Inner product
Vector2_inner(Vector2 u, Vector2 v)
{
    return (u.x * v.x) + (u.y * v.y);
}

macro void // NOTE: Outer product
Vector2_outer(Vector2 u, Vector2 v, Matrix2 dest)
{
    dest[0][0] = u.x * v.x;
    dest[0][1] = u.x * v.y;
    dest[1][0] = u.y * v.x;
    dest[1][1] = u.y * v.y;
}

macro float  // NOTE: Vector length/norm squared
Vector2_lengthsq(Vector2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

macro float  // NOTE: Vector length/norm
Vector2_length(Vector2 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

macro Vector2  // NOTE: Vector normalize
Vector2_normalized(Vector2 v)
{
    return Vector2_divs(v, Vector2_length(v));
}


/* Matrix2: */
macro void
Matrix2_copy(Matrix2 source, Matrix2 dest)
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


/* Matrix4: */
macro void
Matrix4_copy(Matrix4 source, Matrix4 dest)
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
Matrix4_scale(Matrix4 m, float s)
{
    m[0][0] = m[0][0] * s;
    m[0][1] = m[0][1] * s;
    m[0][2] = m[0][2] * s;
    m[0][3] = m[0][3] * s;

    m[1][0] = m[1][0] * s;
    m[1][1] = m[1][1] * s;
    m[1][2] = m[1][2] * s;
    m[1][3] = m[1][3] * s;

    m[2][0] = m[2][0] * s;
    m[2][1] = m[2][1] * s;
    m[2][2] = m[2][2] * s;
    m[2][3] = m[2][3] * s;

    m[3][0] = m[3][0] * s;
    m[3][1] = m[3][1] * s;
    m[3][2] = m[3][2] * s;
    m[3][3] = m[3][3] * s;
}

macro void
Matrix4_create_projection(float near_plane, float far_plane, float fovy, float aspect, Matrix4 dest)
{
    // NOTE: Fovy is in radians
    dest = MAT4_IDENTITY;
    dest[0][0] = aspect * fovy;
    dest[1][1] = fovy;
    dest[2][2] = far_plane / (far_plane - near_plane);
    dest[2][3] = 1.0f;
    dest[3][3] = 0.0f;
}

#endif  // PLVR_MATH_H