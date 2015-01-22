#include "ObjModel.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
	while (!inFile.eof()) {
		if (!getline(inFile, line)) {
			continue;
		}
		std::istringstream ss(line);
		std::string type;
		if (!(ss >> type)) {
			continue;
		}
		if (type == "v") {
			float x, y, z;
			if (!(ss >> x >> y >> z)) {
				return;
			}
			_vertexes.push_back(Vec3f(x, y, z));
		}
		else if (type == "f") {
			Vec3i vs;
			char ctrash;
			int i = 0, trash;
			while (ss >> vs.raw[i] >> ctrash >> trash >> ctrash >> trash) {
				vs.raw[i]--;
				i++;
			}
			_triangles.push_back(vs);
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

Vec3f ObjModel::vertex(size_t idx)
{
	return _vertexes[idx];
}

Vec3i ObjModel::triangle(size_t idx)
{
	return _triangles[idx];
}