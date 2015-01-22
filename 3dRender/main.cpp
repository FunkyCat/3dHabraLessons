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

void triangle(Vec3i t0, Vec3i t1, Vec3i t2,
	Vec2f uv0, Vec2f uv1, Vec2f uv2,
	Vec3f n0, Vec3f n1, Vec3f n2,
	TGAImage& image, TGAImage& diffuse, Vec3f lightDir, int* zBuffer) {
	if (t0.y > t1.y) {
		std::swap(t0, t1);
		std::swap(uv0, uv1);
		std::swap(n0, n1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
		std::swap(uv0, uv2);
		std::swap(n0, n2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
		std::swap(uv1, uv2);
		std::swap(n1, n2);
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
		Vec2f uvA = uv0 + (uv2 - uv0) * alpha;
		Vec2f uvB = (second ? uv1 : uv0) + (second ? uv2 - uv1 : uv1 - uv0) * beta;
		Vec3f nA = n0 + (n2 - n0) * alpha;
		Vec3f nB = (second ? n1 : n0) + (second ? n2 - n1 : n1 - n0) * beta;
		if (a.x > b.x) {
			std::swap(a, b);
			std::swap(uvA, uvB);
			std::swap(nA, nB);
		}
		for (int x = a.x; x <= b.x; x++) {
			float phi = (a.x == b.x ? 1.f : float(x - a.x) / float(b.x - a.x));
			Vec3i p = Vec3f(a) + Vec3f(b - a) * phi;
			p.x = x, p.y = i + t0.y;
			int idx = p.x + p.y * imgSize.x;
			if (zBuffer[idx] < p.z) {
				zBuffer[idx] = p.z;
				
				Vec3f nP = nA + (nB - nA) * phi;
				nP.normalize();
				nP = nP * -1.0f;
				float intensity = std::max(0.0f, nP * lightDir);
				Vec2f uvP = uvA + (uvB - uvA) * phi;
				TGAColor color = diffuse.get(int(uvP.x * diffuse.get_width()),
					int(uvP.y * diffuse.get_height()));
				color = { (unsigned char)(color.r * intensity + 0.5),
					(unsigned char)(color.g * intensity + 0.5),
					(unsigned char)(color.b * intensity + 0.5),
					color.a };
				//color = { (unsigned char)(255 * intensity), (unsigned char)(255 * intensity), (unsigned char)(255 * intensity), color.a };
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
	TGAImage diffuse;
	diffuse.read_tga_file("obj/african_head_diffuse.tga");
	diffuse.flip_vertically();

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

	const vector<Face> fx = model.getFaces();
	const vector<Vec2f> uvx = model.getUVs();
	const vector<Vec3f> nx = model.getNormals();

	Vec3f lightDir = {0, 0, -1};
	int* zBuffer = new int[imgSize.x * imgSize.y];
	memset(zBuffer, 0, sizeof(int) * imgSize.x * imgSize.y);
	for (const Face& face : fx) {
		Vec3i screenCoord[3];
		Vec2f uvCoord[3];
		Vec3f worldCoord[3];
		Vec3f normCoord[3];
		for (int i = 0; i < 3; i++) {
			worldCoord[i] = vx[static_cast<size_t>(face.vertexes.raw[i])];
			uvCoord[i] = uvx[static_cast<size_t>(face.uvs.raw[i])];
			normCoord[i] = nx[static_cast<size_t>(face.normals.raw[i])];
			screenCoord[i].x = imgMove.x / 2 + int((worldCoord[i].x - minVx.x) * scale);
			screenCoord[i].y = imgMove.y / 2 + int((worldCoord[i].y - minVx.y) * scale);
			screenCoord[i].z = int((worldCoord[i].z - minVx.z) * scale);
		}

		triangle(screenCoord[0], screenCoord[1], screenCoord[2],
			uvCoord[0], uvCoord[1], uvCoord[2],
			normCoord[0], normCoord[1], normCoord[2],
			image, diffuse, lightDir, zBuffer);
	}
	


	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}