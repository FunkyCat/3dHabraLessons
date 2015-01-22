#ifndef __OBJ_MODEL_H__
#define __OBJ_MODEL_H__

#include "geometry.h"
#include <vector>
#include <string>


class Face {
public:
	Vec3i vertexes;
	Vec3i uvs;
	Vec3i normals;

	Face() { }
	Face(const Vec3i vertexes, const Vec3i uvs, const Vec3i normals)
		: vertexes(vertexes), uvs(uvs), normals(normals) { }
};

class ObjModel {
public:
	ObjModel ();
	ObjModel (const std::string& fileName);
	ObjModel (const ObjModel& other);
	std::vector< Vec3f >& getVertexes();
	std::vector< Vec2f >& getUVs();
	std::vector< Vec3f >& getNormals();
	std::vector< Face >& getFaces();

private:
	std::vector< Vec3f> _vertexes;
	std::vector< Vec2f > _uvs;
	std::vector< Vec3f > _normals;
	std::vector< Face > _faces;
};

#endif