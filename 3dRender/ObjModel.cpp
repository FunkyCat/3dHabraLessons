#include "ObjModel.h"
#include <fstream>
#include <sstream>

ObjModel::ObjModel()
{
}

ObjModel::ObjModel(const std::string& fileName)
{
	std::ifstream inFile(fileName.c_str());
	if (inFile.fail()) {
		return;
	}
	std::string line;
	while (getline(inFile, line)) {
		std::istringstream ss(line);
		std::string type;
		if (ss >> type) {
			return;
		}
		if (type == "v") {
			float x, y, z;
			if (!(ss >> x >> y >> z)) {
				return;
			}
			_vertexes.push_back(Vec3f(x, y, z));
		}
		else if (type == "f") {
			int va, vb, vc, trash;
			if (!(ss >> va >> trash >> trash >> vb >> trash >> trash >> vc >> trash >> trash)) {
				return;
			}
			va--, vb--, vc--;
			_triangles.push_back(Vec3i(va, vb, vc));
		}
	}
}

ObjModel::ObjModel(const ObjModel& other)
{
	_vertexes = other._vertexes;
	_triangles = other._triangles;
}

std::vector<Vec3f>& ObjModel::getVertexes()
{
	return _vertexes;
}

std::vector<Vec3i>& ObjModel::getTriangles()
{
	return _triangles;
}