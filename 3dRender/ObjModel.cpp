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
			_vertexes.push_back({ x, y, z });
		}
		else if (type == "vt") {
			float u, v;
			if (!(ss >> u >> v)) {
				return;
			}
			_uvs.push_back({ u, v });
		}
		else if (type == "f") {
			Vec3i vs;
			Vec3i uvs;
			char ctrash;
			int i = 0, trash;
			while (ss >> vs.raw[i] >> ctrash >> uvs.raw[i] >> ctrash >> trash) {
				vs.raw[i]--;
				uvs.raw[i]--;
				i++;
			}
			_faces.push_back({ vs, uvs });
		}
	}
}

ObjModel::ObjModel(const ObjModel& other)
{
	_vertexes = other._vertexes;
	_uvs = other._uvs;
	_faces = other._faces;
}

std::vector<Vec3f>& ObjModel::getVertexes()
{
	return _vertexes;
}

std::vector<Face>& ObjModel::getFaces()
{
	return _faces;
}

std::vector<Vec2f>& ObjModel::getUVs()
{
	return _uvs;
}