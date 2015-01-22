#include <iostream>
#include <algorithm>

#include "tgaimage.h"
#include "ObjModel.h"

const Vec2i imgSize = { 600, 800 };

void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
	bool transpose = false;
	if (abs(y1 - y0) > abs(x0 - x1)) {
		transpose = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int dErr = abs(dy) * 2;
	int err = 0;
	int y = y0;
	for (int x = x0; x != x1; x++) {
		if (transpose) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
		err += dErr;
		if (err > dx) {
			y += (y1 > y0 ? 1 : -1);
			err -= dx * 2;
		}
	}
}

void line(Vec2i t0, Vec2i t1, TGAImage& image, const TGAColor& color) {
	line(t0.x, t0.y, t1.x, t1.y, image, color);
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage& image, const TGAColor& color, int* zBuffer) {
	if (t0.y > t1.y) {
		std::swap(t0, t1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
	}

	int height = t2.y - t0.y;
	int segHeight = t1.y - t0.y;
	bool second = false;
	for (int i = 0; i < height; i++) {
		if (!second && (i >= t1.y - t0.y)) {
			second = true;
			segHeight = t2.y - t1.y;
		}
		float alpha = float(i) / height;
		float beta = float(second ? t0.y - t1.y + i : i) / segHeight;
		Vec3i a = t0 + (t2 - t0) * alpha;
		Vec3i b = (second ? t1 : t0) + (second ? t2 - t1 : t1 - t0) * beta;
		if (a.x > b.x) {
			std::swap(a, b);
		}
		for (int x = a.x; x <= b.x; x++) {
			float phi = (a.x == b.x ? 1. : float(x - a.x) / float(b.x - a.x));
			Vec3i p = Vec3f(a) + Vec3f(b - a) * phi;
			int idx = p.x + p.y * imgSize.x;
			if (zBuffer[idx] < p.z) {
				zBuffer[idx] = p.z;
				image.set(p.x, p.y, color);
			}
		}
	}
}



int main(int argc, char* argv[])
{
	using namespace std;

	TGAImage image(imgSize.x, imgSize.y, TGAImage::RGB);

	ObjModel model("obj/african_head.obj");

	Vec3f minVx, maxVx;
	for (int i = 0; i < 3; i++) {
		minVx.raw[i] = numeric_limits<float>::max();
		maxVx.raw[i] = numeric_limits<float>::min();
	}
	const vector<Vec3f> vx = model.getVertexes();
	for (auto iter = vx.begin(); iter != vx.end(); iter++) {
		for (int i = 0; i < 3; i++) {
			minVx.raw[i] = min(minVx.raw[i], iter->raw[i]);
			maxVx.raw[i] = max(maxVx.raw[i], iter->raw[i]);
		}
	}
	Vec3f dVx;
	for (int i = 0; i < 3; i++) {
		dVx.raw[i] = maxVx.raw[i] - minVx.raw[i];
	}

	float imgRatio = float(imgSize.x) / float(imgSize.y);
	float modelRatio = dVx.x / dVx.y;
	float scale = (imgRatio > modelRatio ? imgSize.y / dVx.y : imgSize.x / dVx.x);
	Vec2i imgMove = { imgSize.x - int(dVx.x * scale), imgSize.y - int(dVx.y * scale) };

	const vector<Vec3i> tx = model.getTriangles();

	Vec3f lightDir = {0, 0, -1};
	int* zBuffer = new int[imgSize.x * imgSize.y];
	for (auto iter = tx.begin(); iter != tx.end(); iter++) {
		Vec3i screenCoord[3];
		Vec3f worldCoord[3];
		for (int i = 0; i < 3; i++) {
			worldCoord[i] = vx[static_cast<size_t>(iter->raw[i])];
			screenCoord[i].x = imgMove.x / 2 + int((worldCoord[i].x - minVx.x) * scale);
			screenCoord[i].y = imgMove.y / 2 + int((worldCoord[i].y - minVx.y) * scale);
			screenCoord[i].z = int((worldCoord[i].z - minVx.z) * scale);
		}

		Vec3f norm = (worldCoord[2] - worldCoord[0]) ^ (worldCoord[1] - worldCoord[0]);
		norm.normalize();
		float intensity = norm * lightDir;
		unsigned char color = intensity * 255;
		if (intensity >= 0) {
			triangle(screenCoord[0], screenCoord[1], screenCoord[2], image,
				TGAColor(color, color, color, 0), zBuffer);
		}
	}
	


	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}