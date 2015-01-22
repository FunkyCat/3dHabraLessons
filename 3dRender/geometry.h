#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <ostream>

template <typename T>
class Vec3 {
public:
	union {
		struct { T x, y, z; };
		T raw[3];
	};
	Vec3 () : x(0), y(0), z(0) { }
	Vec3 (T x, T y, T z) : x (x), y (y), z (z) { }
	Vec3 (const Vec3<T>& other) : x (other.x), y (other.y), z (other.z) { }

	friend std::ostream& operator << (std::ostream& out, Vec3<T>& vec) { out << '(' << x << ", " << y << ", " << z << ')'; return out; }
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

template <typename T>
class Vec2 {
public:
	union {
		struct { T x, y; };
		T raw[2];
	};
	Vec2() : x(0), y(0) { }
	Vec2(T x, T y) : x(x), y(y) { }
	Vec2(const Vec2<T>& other) : x(other.x), y(other.y) { }

	friend std::ostream& operator << (std::ostream& out, Vec2<T>& vec) { out << '(' << x << ", " << y << ')'; return out; }
	Vec2 operator + (const Vec2<T>& other) { return { x + other.x, y + other.y }; }
	Vec2 operator - (const Vec2<T>& other) { return { x - other.x, y - other.y }; }
	Vec2 operator* (float a) { return { x * a, y * a }; }
};

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

#endif