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
	float d = float(y1 - y0) / float(x1 - x0);
	for (int x = x0; x != x1; x++) {
		int y = y0 + int(d * (x - x0));
		if (transpose) {
			image.set(y, x, color);
		} else {
			image.set(x, y, color);
		}
	}
}

const TGAColor red(255, 0, 0, 0);

int main(int argc, char* argv[])
{
	TGAImage image(100, 100, TGAImage::RGB);
	TGAColor red = TGAColor(255, 0, 0, 0);
	for (int i = 0; i < 10 * 1000 * 1000; i++) {
		line(50, 50, 90, 65, image, red);
		line(90, 75, 50, 60, image, red);
		line(50, 50, 65, 90, image, red);
	}
	image.flip_vertically();
	image.write_tga_file("test.tga");
	return 0;
}