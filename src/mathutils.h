// src/mathutils.h
#pragma once
#include <cmath>
#include <algorithm>

namespace Math
{
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

    struct Vec2
    {
        float x, y;
        Vec2() : x(0), y(0) {}
        Vec2(float _x, float _y) : x(_x), y(_y) {}
        Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
        Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
        Vec2 operator*(float s)       const { return { x * s, y * s }; }
        float Length() const { return sqrtf(x * x + y * y); }
        float Distance(const Vec2& o) const { return (*this - o).Length(); }
    };

    struct Vec3
    {
        float x, y, z;
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
        Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
        Vec3 operator*(float s)       const { return { x * s, y * s, z * s }; }
        float Length() const { return sqrtf(x * x + y * y + z * z); }
        float Distance(const Vec3& o) const { return (*this - o).Length(); }
        float Dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
        Vec3 Cross(const Vec3& o) const
        {
            return { y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x };
        }
        Vec3 Normalized() const
        {
            float len = Length();
            if (len < 0.0001f) return { 0, 0, 0 };
            return { x / len, y / len, z / len };
        }
    };


    inline bool WorldToScreen(const Vec3& world, Vec2& screen,
        const float* matrix, int screenW, int screenH)
    {
        if (!matrix) return false;


        float VecXx = matrix[4], VecXy = matrix[5], VecXz = matrix[6], VecXw = matrix[7];
        float VecYx = matrix[8], VecYy = matrix[9], VecYz = matrix[10], VecYw = matrix[11];
        float VecZx = matrix[12], VecZy = matrix[13], VecZz = matrix[14], VecZw = matrix[15];

        float screenX = VecXx * world.x + VecXy * world.y + VecXz * world.z + VecXw;
        float screenY = VecYx * world.x + VecYy * world.y + VecYz * world.z + VecYw;
        float screenZ = VecZx * world.x + VecZy * world.y + VecZz * world.z + VecZw;

        if (screenZ <= 0.1f) return false;

        screenZ = 1.0f / screenZ;
        screenX *= screenZ;
        screenY *= screenZ;

        screen.x = (screenW / 2.0f) + (0.5f * screenX * screenW + 0.5f);
        screen.y = (screenH / 2.0f) - (0.5f * screenY * screenH + 0.5f);

        return true;
    }

    template<typename T>
    inline T Clamp(T value, T min, T max)
    {
        return (value < min) ? min : (value > max) ? max : value;
    }

    inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
}