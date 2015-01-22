#include "geometry.h"

template<> template<> Vec3f::Vec3(const Vec3i& other) : x(other.x), y(other.y), z(other.z) { }

template<> template<> Vec3i::Vec3(const Vec3f& other) : x(int(other.x + 0.5f)), y(int(other.y + 0.5f)), z(int(other.z + 0.5f)) { }