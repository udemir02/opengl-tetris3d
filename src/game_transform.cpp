#if !defined(SOURCE_TRANSFORM_CPP)
#define SOURCE_TRANSFORM_CPP

#include "game_math.h"

static F32 radians(F32 deg) {
    F32 result = deg * (M_PI / 180);
    return result;
}

static int min3(V3 v) {
    F32 min = v.x;
    int index = 0;

    if (min > v.y) {
        min = v.y;
        index = 1;
    }

    if (min > v.z) {
        min = v.z;
        index = 2;
    }

    return index;
}

static M4x4 transform_onb(V3 u) {
    V3 v = u;

    int min = min3(u);

    v.e[min] = 0.0f;

    if (min == 0) {
        F32 tmp = -v.y;
        v.y = v.z;
        v.z = tmp;
    } else if (min == 1) {
        F32 tmp = -v.x;
        v.x = v.z;
        v.z = tmp;
    } else if (min == 2) {
        F32 tmp = -v.x;
        v.x = v.y;
        v.y = tmp;
    }

    V3 w = cross(u, v);

    M4x4 result = {{
        {u.x,  u.y,  u.z,  0.0f},
        {v.x,  v.y,  v.z,  0.0f},
        {w.x,  w.y,  w.z,  0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return result;
}

static M4x4 translate(V3 t) {
    M4x4 result = {{
        {1.0f, 0.0f, 0.0f,  t.x},
        {0.0f, 1.0f, 0.0f,  t.y},
        {0.0f, 0.0f, 1.0f,  t.z},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    return result;
}

static M4x4 rotate(V3 pos, V3 axis, F32 deg) {
    V3 normal = normalize(axis);

    M4x4 T    = translate(pos);
    M4x4 Tinv = translate(-pos);
    M4x4 M    = transform_onb(normal);
    M4x4 Minv = transpose(M);

    F32 rad = radians(deg);

    M4x4 Rx = {{
        {1.0f,      0.0f,       0.0f, 0.0f},
        {0.0f, cosf(rad), -sinf(rad), 0.0f},
        {0.0f, sinf(rad),  cosf(rad), 0.0f},
        {0.0f,      0.0f,       0.0f, 1.0f}
    }};

    M4x4 result = Tinv * Minv * Rx * M * T;

    return result;
}

static M4x4 rotate(V3 axis, F32 deg) {
    V3 pos    = {};
    V3 normal = normalize(axis);

    M4x4 T    = translate(pos);
    M4x4 Tinv = translate(-pos);
    M4x4 M    = transform_onb(normal);
    M4x4 Minv = transpose(M);

    F32 rad = radians(deg);

    M4x4 Rx = {{
        {1.0f,      0.0f,       0.0f, 0.0f},
        {0.0f, cosf(rad), -sinf(rad), 0.0f},
        {0.0f, sinf(rad),  cosf(rad), 0.0f},
        {0.0f,      0.0f,       0.0f, 1.0f}
    }};

    M4x4 result = Tinv * Minv * Rx * M * T;

    return result;
}

static M4x4 scale(V3 pos, V3 s) {
    M4x4 T    = translate(-pos);
    M4x4 Tinv = translate(pos);

    M4x4 S = {{
        { s.x, 0.0f, 0.0f, 0.0f},
        {0.0f,  s.y, 0.0f, 0.0f},
        {0.0f, 0.0f,  s.z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    M4x4 result = Tinv * S * T;
    return result;
}

static M4x4 scale(V3 s) {
    V3 pos = {};

    M4x4 T    = translate(-pos);
    M4x4 Tinv = translate(pos);

    M4x4 S = {{
        { s.x, 0.0f, 0.0f, 0.0f},
        {0.0f,  s.y, 0.0f, 0.0f},
        {0.0f, 0.0f,  s.z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    M4x4 result = Tinv * S * T;
    return result;
}

static M4x4 scale(F32 s) {
    V3 pos = {};

    M4x4 T    = translate(-pos);
    M4x4 Tinv = translate(pos);

    M4x4 S = {{
        {   s, 0.0f, 0.0f, 0.0f},
        {0.0f,    s, 0.0f, 0.0f},
        {0.0f, 0.0f,    s, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    M4x4 result = Tinv * S * T;
    return result;
}
static M4x4 reflection(V4 plane) {
    V3 normal = normalize(plane.xyz);
    V3 point  = { 0.0f, 0.0f, -plane.w / plane.z };

    M4x4 T      = translate(-point);
    M4x4 Tinv   = translate(point);
    M4x4 M      = transform_onb(normal);
    M4x4 Minv   = transpose(M);
    M4x4 S      = scale({0.0f, 0.0f, 0.0f}, {-1.0f, 1.0f, 1.0f});
    M4x4 result = Tinv * Minv * S * M * T;

    return result;
}

static M4x4 shear(char axis, F32 shear) {
    M4x4 result = {};

    if (axis == 'x')
        result = {{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            {shear, 1.0f, 0.0f, 0.0f},
            {shear, 0.0f, 1.0f, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f}
        }};
    else if (axis == 'y')
        result = {{
            {1.0f, shear, 0.0f, 0.0f},
            {0.0f,  1.0f, 0.0f, 0.0f},
            {0.0f, shear, 1.0f, 0.0f},
            {0.0f,  0.0f, 0.0f, 1.0f}
        }};
    else if (axis == 'z')
        result = {{
            {1.0f, 0.0f, shear, 0.0f},
            {0.0f, 1.0f, shear, 0.0f},
            {0.0f, 0.0f,  1.0f, 0.0f},
            {0.0f, 0.0f,  0.0f, 1.0f}
        }};

    return result;
}

static M4x4 perspective(F32 fov, S32 width, S32 height, F32 near, F32 far) {
    F32 tangent      = tanf(radians(fov/2));
    F32 aspect_ratio = (F32)width / height;

    F32 top   = near * tangent;
    F32 right = top * aspect_ratio;

    M4x4 result = {};

    result.e[0][0] = near / right;
    result.e[1][1] = near / top;
    result.e[2][2] = -(far + near) / (far - near);
    result.e[2][3] = (-2 * far * near) / (far - near);
    result.e[3][2] = -1;

    return result;
}

static M4x4 ortho(F32 width, F32 height, F32 near, F32 far) {
    M4x4 result = {};

    F32 half_width  = width / 2;
    F32 half_height = height / 2;

    result.e[0][0] = 1.0f / half_width;
    result.e[1][1] = 1.0f / half_height;
    result.e[2][2] = -2 / (far - near);
    result.e[2][3] = -(far + near) / (far - near);
    result.e[3][3] = 1;

    result.e[0][3] = -1;
    result.e[1][3] = -1;

    return result;
}

static M4x4 look_at(V3 eye, V3 target, V3 up_dir) {
    V3 forward = normalize(eye - target);
    V3 left    = normalize(cross(up_dir, forward));
    V3 up      = cross(forward, left);

    M4x4 result = {};

    result.e[0][0] = left.x;
    result.e[0][1] = left.y;
    result.e[0][2] = left.z;
    result.e[0][3] = dot(-left, eye);

    result.e[1][0] = up.x;
    result.e[1][1] = up.y;
    result.e[1][2] = up.z;
    result.e[1][3] = dot(-up, eye);

    result.e[2][0] = forward.x;
    result.e[2][1] = forward.y;
    result.e[2][2] = forward.z;
    result.e[2][3] = dot(-forward, eye);

    result.e[3][3] = 1;

    return result;
}

#endif
