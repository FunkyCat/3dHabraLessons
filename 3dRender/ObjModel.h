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
	std::vector< Vec2f >& getUVs();
	std::vector< Face >& getFaces();

private:
	std::vector< Vec3f> _vertexes;
	std::vector< Vec2f > _uvs;
	std::vector< Face > _faces;
};

#endif