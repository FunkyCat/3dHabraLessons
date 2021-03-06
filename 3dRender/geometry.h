#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <ostream>
#include <vector>

class Matrix;

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
	template <typename V>
	Vec3(const Vec3<V>& other);
	Vec3(const Matrix& matrix);

	float len() {
		return sqrt(x * x + y * y + z * z);
	}

	Vec3<T>& normalize(const T l = 1) {
		(*this) = (*this) * (l / len());
		return *this;
	}

	friend std::ostream& operator << (std::ostream& out, Vec3<T>& vec) { out << '(' << x << ", " << y << ", " << z << ')'; return out; }
	Vec3 operator + (const Vec3<T>& other) {
		return { x + other.x, y + other.y, z + other.z };
	}
	Vec3 operator - (const Vec3<T>& other) {
		return { x - other.x, y - other.y, z - other.z };
	}
	Vec3 operator * (const float a) {
		return { static_cast<T>(x * a),
			static_cast<T>(y * a),
			static_cast<T>(z * a) };
	}
	T operator * (const Vec3<T>& other) {
		return x * other.x + y * other.y + z * other.z;
	}
	Vec3 operator ^ (const Vec3<T>& other) {
		return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
	}
	T& operator[] (const size_t idx) {
		return raw[idx];
	}
	const T& operator[] (const size_t idx) const {
		return raw[idx];
	}
};

typedef Vec3<float> Vec3f;
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
	Vec2 operator + (const Vec2<T>& other) {
		return { x + other.x, y + other.y };
	}
	Vec2 operator - (const Vec2<T>& other) {
		return { x - other.x, y - other.y };
	}
	Vec2 operator* (float a) {
		return { x * a, y * a };
	}
	T& operator[] (const size_t idx) {
		return raw[idx];
	}
	const T& operator[] (const size_t idx) const {
		return raw[idx];
	}
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

class Matrix {
	std::vector<std::vector<float> > _m;
	size_t _rows, _cols;
public:
	Matrix(size_t rows = 4, size_t cols = 4);
	Matrix(const Vec3f vector);
	inline int rowCount();
	inline int colCount();

	static Matrix identity(int dimensions);
	std::vector<float>& operator[](const size_t idx);
	const std::vector<float>& operator[](const size_t idx) const;
	Matrix operator*(const Matrix& other);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

#endif