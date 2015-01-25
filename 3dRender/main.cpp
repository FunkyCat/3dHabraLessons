#include <iostream>
#include <algorithm>

#include "tgaimage.h"
#include "ObjModel.h"



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
	TGAImage& image, TGAImage& diffuse, TGAImage& normals,
	Vec3f lightDir, int* zBuffer, Vec2i imgSize) {
	if (t0.y > t1.y) {
		std::swap(t0, t1);
		std::swap(uv0, uv1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
		std::swap(uv0, uv2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
		std::swap(uv1, uv2);
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
		if (a.x > b.x) {
			std::swap(a, b);
			std::swap(uvA, uvB);
		}
		for (int x = a.x; x <= b.x; x++) {
			float phi = (a.x == b.x ? 1.f : float(x - a.x) / float(b.x - a.x));
			Vec3i p = Vec3f(a) + Vec3f(b - a) * phi;
			p.x = x, p.y = i + t0.y;
			int idx = p.x + p.y * imgSize.x;
			if (p.x >= 0 && p.x < imgSize.x && p.y >= 0 && p.y < imgSize.y && zBuffer[idx] < p.z) {
				zBuffer[idx] = p.z;
				Vec2f uvP = uvA + (uvB - uvA) * phi;
				int u = int(uvP.x * diffuse.get_width());
				int v = int(uvP.y * diffuse.get_height());
				TGAColor nCol = normals.get(u, v);
				Vec3f nP = { static_cast<float>(nCol.r - 128), static_cast<float>(nCol.g - 128), static_cast<float>(nCol.b - 128) };
				nP.normalize();
				nP = nP * -1.0f;
				float intensity = std::max(0.0f, nP * lightDir);
				TGAColor color = diffuse.get(u, v);
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


Matrix makeViewPort(float x, float y, float w, float h, float depth) {
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.0f;
	m[1][3] = y + h / 2.0f;
	m[2][3] = depth / 2.0f;
	
	m[0][0] = w / 2.0f;
	m[1][1] = h / 2.0f;
	m[2][2] = depth / 2.0f;

	return m;
}


Matrix makeLookAt(Vec3f eye, Vec3f center, Vec3f up) {
	Vec3f z = (eye - center).normalize();
	Vec3f x = (up ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	Matrix res = Matrix::identity(4);
	for (int i = 0; i < 3; i++) {
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
		res[i][3] = -center[i];
	}
	return res;
}

Matrix vector2matrix(const Vec3f v) {
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.0f;
	return m;
}

Vec3f matrix2vector(const Matrix& m) {
	return{ m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0] };
}

const Vec2i imgSize = { 4000, 4000 };
Vec3f lightDir = Vec3f(1, 1, -2).normalize();
Vec3f eye = { 1, 1, 3 };
Vec3f center = { 0, 0, 0 };


int main(int argc, char* argv[])
{
	using namespace std;

	TGAImage image(imgSize.x, imgSize.y, TGAImage::RGB);
	ObjModel model("obj/african_head.obj");
	TGAImage diffuseTex;
	diffuseTex.read_tga_file("obj/african_head_diffuse.tga");
	diffuseTex.flip_vertically();
	TGAImage normalsTex;
	normalsTex.read_tga_file("obj/african_head_nm.tga");
	normalsTex.flip_vertically();

	int* zBuffer = new int[imgSize.x * imgSize.y];
	memset(zBuffer, 0, sizeof(int) * imgSize.x * imgSize.y);

	Matrix viewMat = makeLookAt(eye, center, Vec3f(0, 1, 0));
	Matrix projMat = Matrix::identity(4);
	projMat[3][2] = -1.0f / (eye-center).len();
	Matrix viewPortMat = makeViewPort(imgSize.x / 8.0f, imgSize.y / 8.0f,
		static_cast<float>(imgSize.x) / 4.0f * 3.0f, static_cast<float>(imgSize.y) / 4.0f * 3.0f, 255.0f);

	const vector<Vec3f> vx = model.getVertexes();
	const vector<Face> fx = model.getFaces();
	const vector<Vec2f> uvx = model.getUVs();
	const vector<Vec3f> nx = model.getNormals();

	Matrix resultMat = viewPortMat * projMat * viewMat;
	int idx = 0;

	int cnt = 0;

	for (const Face& face : fx) {
		Vec3i screenCoord[3];
		Vec2f uvCoord[3];
		Vec3f worldCoord[3];
		for (int i = 0; i < 3; i++) {
			worldCoord[i] = vx[static_cast<size_t>(face.vertexes.raw[i])];
			uvCoord[i] = uvx[static_cast<size_t>(face.uvs.raw[i])];
			screenCoord[i] = Vec3f(viewPortMat * projMat * Matrix(worldCoord[i]));
		}

		float viewCheck = ((screenCoord[0] - screenCoord[1]) ^ (screenCoord[0] - screenCoord[2])) * Vec3f(0, 0, 1);
		//if (viewCheck >= 0) {
			cnt++;
			triangle(screenCoord[0], screenCoord[1], screenCoord[2],
				uvCoord[0], uvCoord[1], uvCoord[2],
				image, diffuseTex, normalsTex, lightDir, zBuffer, imgSize);
		//}
	}

	std::cerr << "Count: " << cnt << std::endl;
	

	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}