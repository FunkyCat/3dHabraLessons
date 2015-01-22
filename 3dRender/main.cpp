#include <iostream>
#include <algorithm>

#include "tgaimage.h"
#include "ObjModel.h"

const TGAColor red(255, 0, 0, 0);
const TGAColor white(255, 255, 255, 0);
const TGAColor green(0, 255, 0, 0);

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

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color) {
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
		Vec2i a = t0 + (t2 - t0) * alpha;
		Vec2i b = (second ? t1 : t0) + (second ? t2 - t1 : t1 - t0) * beta;
		a.y = b.y = i + t0.y;
		line(a, b, image, color);
	}
}

const Vec2i imgSize = { 600, 800 };


int main(int argc, char* argv[])
{
	using namespace std;

	TGAImage image(imgSize.x, imgSize.y, TGAImage::RGB);

	//triangle({ 50, 50 }, { 20, 250 }, { 100, 200 }, image, red);

	ObjModel model("obj/african_head.obj");

	
	Vec3f minVx, maxVx;
	for (int i = 0; i < 3; i++) {
		minVx.raw[i] = numeric_limits<float>::max();
		maxVx.raw[i] = numeric_limits<float>::min();
	}
	const vector<Vec3f> vx = model.getVertexes();
	for (auto iter = vx.begin(); iter != vx.end(); iter++) {
		for (int i = 0; i < 3; i++) {
			minVx.raw[i] = min (minVx.raw[i], iter->raw[i]);
			maxVx.raw[i] = max (maxVx.raw[i], iter->raw[i]);
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
	for (auto iter = tx.begin(); iter != tx.end(); iter++) {
		Vec2i screenCoord[3];
		for (int i = 0; i < 3; i++) {
			size_t idx = static_cast<size_t>(iter->raw[i]);
			screenCoord[i].x = imgMove.x / 2 + int((vx[idx].x - minVx.x) * scale);
			screenCoord[i].y = imgMove.y / 2 + int((vx[idx].y - minVx.y) * scale);
		}
		triangle(screenCoord[0], screenCoord[1], screenCoord[2], image,
			TGAColor(char(rand() % 256), char(rand() % 256), char(rand() % 256), 0));
	}
	


	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}