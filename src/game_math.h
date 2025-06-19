#if !defined(HEADER_MATH_H)
#define HEADER_MATH_H

#include <math.h>

union V2 {
    struct { F32 x, y;};
    struct { S32 ix, iy;};
    struct { U32 ux, uy;};
    F32 e[2];
};

typedef union V3 {
    struct { F32 x, y, z; };
    struct { F32 r, g, b; };
    F32 e[3];
} RGB;

typedef union V4 {
    struct { F32 x, y, z, w; };
    struct { F32 r, g, b, a; };
    F32 e[4];

    struct {
        V3 xyz;
        F32 _unusedW;
    };
    struct {
        RGB rgb;
        F32 _unusedA;
    };
} RGBA;

struct M4x4 {
    F32 e[4][4];
};

inline B32 operator==(V3 v1, V3 v2) {
    B32 xbool = (v1.x == v2.x);
    B32 ybool = (v1.y == v2.y);
    B32 zbool = (v1.z == v2.z);
    B32 result = (xbool && ybool && zbool);
    return result;
}

inline V3 operator-(V3 v) {
    V3 result = {-v.x, -v.y, -v.z};
    return result;
}

inline V3 operator-(V3 v1, V3 v2) {
    V3 result = {};
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

inline V3 operator*(V3 v, F32 value) {
    V3 result = {};
    result.x = v.x * value;
    result.y = v.y * value;
    result.z = v.z * value;
    return result;
}

inline V3 operator*(F32 value, V3 v) {
    V3 result = {};
    result.x = v.x * value;
    result.y = v.y * value;
    result.z = v.z * value;
    return result;
}

inline V3 operator*(V3 v1, V3 v2) {
    V3 result = {};
    result.x = v1.x * v2.x;
    result.y = v1.y * v2.y;
    result.z = v1.z * v2.z;
    return result;
}

inline V3 operator+(V3 v, F32 value) {
    V3 result = {};
    result.x = v.x + value;
    result.y = v.y + value;
    result.z = v.z + value;
    return result;
}

inline V3 operator+(V3 v1, V3 v2) {
    V3 result = {};
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

inline V3 operator/(V3 v, F32 value) {
    V3 result = {};
    result.x = v.x / value;
    result.y = v.y / value;
    result.z = v.z / value;
    return result;
}

inline V3 &operator+=(V3 &v1, V3 v2) {
    v1 = v1 + v2;
    return v1;
}

inline V3 &operator-=(V3 &v1, V3 v2) {
    v1 = v1 - v2;
    return v1;
}

inline V3 cross(V3 v1, V3 v2) {
    V3 result = {};
    result.x = (v1.y * v2.z) - (v1.z * v2.y);
    result.y = (v1.z * v2.x) - (v1.x * v2.z);
    result.z = (v1.x * v2.y) - (v1.y * v2.x);
    return result;
}

inline V3 mult(V3 v, F32 value) {
    V3 result = {};
    result.x = v.x * value;
    result.y = v.y * value;
    result.z = v.z * value;
    return result;
}

inline F32 dot(V3 v1, V3 v2) {
    F32 x = v1.x * v2.x;
    F32 y = v1.y * v2.y;
    F32 z = v1.z * v2.z;
    F32 result = x + y + z;
    return result;
}

inline V3 hadamard(V3 v1, V3 v2) {
    V3 result = {};
    result.x = v1.x * v2.x;
    result.y = v1.y * v2.y;
    result.z = v1.z * v2.z;
    return result;
}

inline V3 normalize(V3 v) {
    F32 d = dot(v, v);
    F32 inv = 1.0f / sqrtf(d);
    V3 result = mult(v, inv);
    return result;
}

inline M4x4 m4x4_identity(void) {
    M4x4 result = {{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return result;
}

inline M4x4 transpose(M4x4 m) {
    M4x4 result = {};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            result.e[i][j] = m.e[j][i];
    }

    return result;
}

inline M4x4 operator*(M4x4 A, M4x4 B) {
    M4x4 result = {};

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            result.e[r][c] = 0;
            for (int i = 0; i < 4; i++)
                result.e[r][c] += A.e[r][i] * B.e[i][c];
        }
    }

    return result;
}

inline M4x4 &operator*=(M4x4 &A, M4x4 B) {
    A = A * B;
    return A;
}

inline V3 operator*(V3 v, M4x4 A) {
    M4x4 m = {};
    V4 B = {v.x, v.y, v.z, 1};

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            m.e[r][c] = 0;
            for (int i = 0; i < 4; i++)
                m.e[r][c] += A.e[r][i] * B.e[i];
        }
    }

    V3 result = {};
    result.x = m.e[0][3];
    result.y = m.e[1][3];
    result.z = m.e[2][3];

    return result;
}

inline V3 &operator*=(V3 &A, M4x4 B) {
    A = A * B;
    return A;
}

#endif
