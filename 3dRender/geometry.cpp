#include "geometry.h"

template<> template<> Vec3f::Vec3(const Vec3i& other)
	: x(static_cast<float>(other.x)), y(static_cast<float>(other.y)), z(static_cast<float>(other.z)) { }

template<> template<> Vec3i::Vec3(const Vec3f& other)
	: x(static_cast<int>(other.x + 0.5f)), y(static_cast<int>(other.y + 0.5f)), z(static_cast<int>(other.z + 0.5f)) { }