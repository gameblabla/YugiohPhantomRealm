#pragma once

// Small self-contained subset of GLM used by YugiohPhantomRealm.
// It intentionally implements only the vec/mat/quaternion operations that the
// project actually calls. Matrices are column-major and expose mat[col][row],
// matching GLM/OpenGL memory layout for glUniformMatrix4fv(..., &m[0][0]).

#include <cmath>
#include <algorithm>

namespace glm {

struct vec2 {
    float x, y;
    vec2() : x(0.0f), y(0.0f) {}
    explicit vec2(float s) : x(s), y(s) {}
    vec2(float x_, float y_) : x(x_), y(y_) {}
    float& operator[](int i) { return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }
};

struct vec3 {
    float x, y, z;
    vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    explicit vec3(float s) : x(s), y(s), z(s) {}
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float& operator[](int i) { return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }
};

struct vec4 {
    float x, y, z, w;
    vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    explicit vec4(float s) : x(s), y(s), z(s), w(s) {}
    vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    float& operator[](int i) { return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }
    vec4& operator*=(const vec4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
};

inline vec2 operator+(const vec2& a, const vec2& b) { return vec2(a.x + b.x, a.y + b.y); }
inline vec2 operator-(const vec2& a, const vec2& b) { return vec2(a.x - b.x, a.y - b.y); }
inline vec2 operator*(const vec2& a, float s) { return vec2(a.x * s, a.y * s); }
inline vec2 operator*(float s, const vec2& a) { return a * s; }

inline vec3 operator+(const vec3& a, const vec3& b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline vec3 operator-(const vec3& a, const vec3& b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline vec3 operator-(const vec3& v) { return vec3(-v.x, -v.y, -v.z); }
inline vec3 operator*(const vec3& a, const vec3& b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline vec3 operator*(const vec3& a, float s) { return vec3(a.x * s, a.y * s, a.z * s); }
inline vec3 operator*(float s, const vec3& a) { return a * s; }
inline vec3 operator/(const vec3& a, float s) { return vec3(a.x / s, a.y / s, a.z / s); }
inline vec3& operator+=(vec3& a, const vec3& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
inline vec3& operator-=(vec3& a, const vec3& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
inline vec3& operator*=(vec3& a, float s) { a.x *= s; a.y *= s; a.z *= s; return a; }
inline vec3& operator/=(vec3& a, float s) { a.x /= s; a.y /= s; a.z /= s; return a; }

inline vec4 operator+(const vec4& a, const vec4& b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
inline vec4 operator-(const vec4& a, const vec4& b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
inline vec4 operator*(const vec4& a, const vec4& b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
inline vec4 operator*(const vec4& a, float s) { return vec4(a.x * s, a.y * s, a.z * s, a.w * s); }
inline vec4 operator*(float s, const vec4& a) { return a * s; }
inline bool operator==(const vec4& a, const vec4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
inline bool operator!=(const vec4& a, const vec4& b) { return !(a == b); }

struct mat4 {
    vec4 c[4];
    mat4()
        : c{vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1)} {}
    explicit mat4(float diagonal)
        : c{vec4(diagonal,0,0,0), vec4(0,diagonal,0,0), vec4(0,0,diagonal,0), vec4(0,0,0,diagonal)} {}
    mat4(float x0, float y0, float z0, float w0,
         float x1, float y1, float z1, float w1,
         float x2, float y2, float z2, float w2,
         float x3, float y3, float z3, float w3)
        : c{vec4(x0,y0,z0,w0), vec4(x1,y1,z1,w1), vec4(x2,y2,z2,w2), vec4(x3,y3,z3,w3)} {}
    vec4& operator[](int i) { return c[i]; }
    const vec4& operator[](int i) const { return c[i]; }
    mat4& operator*=(const mat4& rhs);
};

struct mat3 {
    vec3 c[3];
    mat3()
        : c{vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)} {}
    explicit mat3(float diagonal)
        : c{vec3(diagonal,0,0), vec3(0,diagonal,0), vec3(0,0,diagonal)} {}
    mat3(float x0, float y0, float z0,
         float x1, float y1, float z1,
         float x2, float y2, float z2)
        : c{vec3(x0,y0,z0), vec3(x1,y1,z1), vec3(x2,y2,z2)} {}
    explicit mat3(const mat4& m)
        : c{vec3(m[0].x, m[0].y, m[0].z), vec3(m[1].x, m[1].y, m[1].z), vec3(m[2].x, m[2].y, m[2].z)} {}
    vec3& operator[](int i) { return c[i]; }
    const vec3& operator[](int i) const { return c[i]; }
};

inline float dot(const vec3& a, const vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
inline float length(const vec3& v) { return std::sqrt(dot(v, v)); }
inline vec3 normalize(const vec3& v) {
    float len = length(v);
    return (len > 0.0f) ? (v / len) : vec3(0.0f);
}

inline vec2 mix(const vec2& a, const vec2& b, float t) { return a * (1.0f - t) + b * t; }
inline vec3 mix(const vec3& a, const vec3& b, float t) { return a * (1.0f - t) + b * t; }
inline vec4 mix(const vec4& a, const vec4& b, float t) { return a * (1.0f - t) + b * t; }

inline vec4 operator*(const mat4& m, const vec4& v) {
    return vec4(
        m[0].x*v.x + m[1].x*v.y + m[2].x*v.z + m[3].x*v.w,
        m[0].y*v.x + m[1].y*v.y + m[2].y*v.z + m[3].y*v.w,
        m[0].z*v.x + m[1].z*v.y + m[2].z*v.z + m[3].z*v.w,
        m[0].w*v.x + m[1].w*v.y + m[2].w*v.z + m[3].w*v.w);
}

inline mat4 operator*(const mat4& a, const mat4& b) {
    mat4 r(0.0f);
    for (int col = 0; col < 4; ++col) {
        r[col] = a * b[col];
    }
    return r;
}
inline mat4& mat4::operator*=(const mat4& rhs) { *this = *this * rhs; return *this; }

inline vec3 operator*(const mat3& m, const vec3& v) {
    return vec3(
        m[0].x*v.x + m[1].x*v.y + m[2].x*v.z,
        m[0].y*v.x + m[1].y*v.y + m[2].y*v.z,
        m[0].z*v.x + m[1].z*v.y + m[2].z*v.z);
}
inline vec3 operator*(const vec3& v, const mat3& m) {
    return vec3(
        v.x*m[0].x + v.y*m[0].y + v.z*m[0].z,
        v.x*m[1].x + v.y*m[1].y + v.z*m[1].z,
        v.x*m[2].x + v.y*m[2].y + v.z*m[2].z);
}

inline mat4 transpose(const mat4& m) {
    return mat4(
        m[0].x, m[1].x, m[2].x, m[3].x,
        m[0].y, m[1].y, m[2].y, m[3].y,
        m[0].z, m[1].z, m[2].z, m[3].z,
        m[0].w, m[1].w, m[2].w, m[3].w);
}

inline mat4 translate(const mat4& m, const vec3& v) {
    mat4 t;
    t[3] = vec4(v.x, v.y, v.z, 1.0f);
    return m * t;
}
inline mat4 scale(const mat4& m, const vec3& v) {
    mat4 s;
    s[0].x = v.x;
    s[1].y = v.y;
    s[2].z = v.z;
    return m * s;
}

inline mat4 rotate(float angle, const vec3& axis) {
    vec3 a = normalize(axis);
    float c = std::cos(angle);
    float s = std::sin(angle);
    float omc = 1.0f - c;

    return mat4(
        c + a.x*a.x*omc,          a.y*a.x*omc + a.z*s,   a.z*a.x*omc - a.y*s,   0.0f,
        a.x*a.y*omc - a.z*s,      c + a.y*a.y*omc,       a.z*a.y*omc + a.x*s,   0.0f,
        a.x*a.z*omc + a.y*s,      a.y*a.z*omc - a.x*s,   c + a.z*a.z*omc,       0.0f,
        0.0f,                     0.0f,                  0.0f,                  1.0f);
}
inline mat4 rotate(const mat4& m, float angle, const vec3& axis) { return m * rotate(angle, axis); }
inline vec3 rotate(const vec3& v, float angle, const vec3& axis) {
    vec4 r = rotate(angle, axis) * vec4(v.x, v.y, v.z, 0.0f);
    return vec3(r.x, r.y, r.z);
}

inline mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
    float tanHalf = std::tan(fovy * 0.5f);
    mat4 r(0.0f);
    r[0].x = 1.0f / (aspect * tanHalf);
    r[1].y = 1.0f / tanHalf;
    r[2].z = -(zFar + zNear) / (zFar - zNear);
    r[2].w = -1.0f;
    r[3].z = -(2.0f * zFar * zNear) / (zFar - zNear);
    return r;
}

inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);

    mat4 r;
    r[0].x = s.x;  r[0].y = u.x;  r[0].z = -f.x;
    r[1].x = s.y;  r[1].y = u.y;  r[1].z = -f.y;
    r[2].x = s.z;  r[2].y = u.z;  r[2].z = -f.z;
    r[3].x = -dot(s, eye);
    r[3].y = -dot(u, eye);
    r[3].z =  dot(f, eye);
    return r;
}

struct quat {
    float w, x, y, z;
    quat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    quat(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}
    explicit quat(const mat4& m) {
        float trace = m[0].x + m[1].y + m[2].z;
        if (trace > 0.0f) {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            w = 0.25f * s;
            x = (m[1].z - m[2].y) / s;
            y = (m[2].x - m[0].z) / s;
            z = (m[0].y - m[1].x) / s;
        } else if (m[0].x > m[1].y && m[0].x > m[2].z) {
            float s = std::sqrt(1.0f + m[0].x - m[1].y - m[2].z) * 2.0f;
            w = (m[1].z - m[2].y) / s;
            x = 0.25f * s;
            y = (m[1].x + m[0].y) / s;
            z = (m[2].x + m[0].z) / s;
        } else if (m[1].y > m[2].z) {
            float s = std::sqrt(1.0f + m[1].y - m[0].x - m[2].z) * 2.0f;
            w = (m[2].x - m[0].z) / s;
            x = (m[1].x + m[0].y) / s;
            y = 0.25f * s;
            z = (m[2].y + m[1].z) / s;
        } else {
            float s = std::sqrt(1.0f + m[2].z - m[0].x - m[1].y) * 2.0f;
            w = (m[0].y - m[1].x) / s;
            x = (m[2].x + m[0].z) / s;
            y = (m[2].y + m[1].z) / s;
            z = 0.25f * s;
        }
    }
};

inline quat normalize(const quat& q) {
    float len = std::sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
    return len > 0.0f ? quat(q.w/len, q.x/len, q.y/len, q.z/len) : quat();
}
inline quat operator*(const quat& a, const quat& b) {
    return quat(
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w);
}
inline quat mix(const quat& a, const quat& b, float t) {
    quat end = b;
    float d = a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
    if (d < 0.0f) end = quat(-b.w, -b.x, -b.y, -b.z);
    return normalize(quat(
        a.w * (1.0f - t) + end.w * t,
        a.x * (1.0f - t) + end.x * t,
        a.y * (1.0f - t) + end.y * t,
        a.z * (1.0f - t) + end.z * t));
}
inline mat4 mat4_cast(const quat& qin) {
    quat q = normalize(qin);
    float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
    float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
    float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

    return mat4(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz),       2.0f * (xz - wy),       0.0f,
        2.0f * (xy - wz),       1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx),       0.0f,
        2.0f * (xz + wy),       2.0f * (yz - wx),       1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f,                   0.0f,                   0.0f,                   1.0f);
}

inline float radians(float degrees) { return degrees * 0.017453292519943295769f; }

} // namespace glm
