#ifndef __OBJ_MODEL_H__
#define __OBJ_MODEL_H__

#include "geometry.h"
#include <vector>
#include <string>

class ObjModel {
public:
	ObjModel ();
	ObjModel (const std::string& fileName);
	ObjModel (const ObjModel& other);
	std::vector< Vec3f >& getVertexes();
	std::vector< Vec3i >& getTriangles();

private:
	std::vector< Vec3f> _vertexes;
	std::vector< Vec3i > _triangles;
};

#endif