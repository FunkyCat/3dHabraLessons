#include "tgaimage.h"

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor& color)
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

const TGAColor red(255, 0, 0, 0);

int main(int argc, char* argv[])
{
	TGAImage image(100, 100, TGAImage::RGB);
	TGAColor red = TGAColor(255, 0, 0, 0);
	line(50, 50, 90, 65, image, red);
	line(90, 75, 50, 60, image, red);
	line(50, 50, 65, 90, image, red);
	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}