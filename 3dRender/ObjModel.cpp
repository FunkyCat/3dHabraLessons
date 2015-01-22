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
				continue;
			}
			_vertexes.push_back({ x, y, z });
		}
		else if (type == "vt") {
			float u, v;
			if (!(ss >> u >> v)) {
				continue;
			}
			_uvs.push_back({ u, v });
		}
		else if (type == "vn") {
			float x, y, z;
			if (!(ss >> x >> y >> z)) {
				continue;
			}
			_normals.push_back({ x, y, z });
		}
		else if (type == "f") {
			Vec3i vs;
			Vec3i uvs;
			Vec3i ns;
			char ctrash;
			int i = 0;
			while (ss >> vs.raw[i] >> ctrash >> uvs.raw[i] >> ctrash >> ns.raw[i]) {
				vs.raw[i]--;
				uvs.raw[i]--;
				ns.raw[i]--;
				i++;
			}
			_faces.push_back({ vs, uvs, ns });
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

std::vector<Vec2f>& ObjModel::getUVs()
{
	return _uvs;
}

std::vector<Vec3f>& ObjModel::getNormals()
{
	return _normals;
}

std::vector<Face>& ObjModel::getFaces()
{
	return _faces;
}
