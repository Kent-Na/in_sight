#include <math.h>
#include "in_sight.h"
//#include "sub_systems/BnFFT.h"
//#include "sub_systems/DcVector.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	size_t w = 1280;
	size_t h = 720;
	uint8_t img[w*h];


	for (size_t y = 0; y<h; y++){
		for (size_t x = 0; x<w; x++){
			img[y*w+x] = (x/(double)w)*(y/(double)h)*255;
		}
	}

	is::Image_info info;
	info.w = w;
	info.h = h;
	info.channel = 1;
	info.bytes_per_pixel = 1;
	info.bytes_per_row = w;
	(new is::View_2d(c, img, &info))
		->scale_name_x("x")
		->scale_name_y("y")
		->name("frame-1");

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
