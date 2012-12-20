#include <math.h>
#include "in_sight.h"
#include "sub_systems/BnFFT.h"
#include "sub_systems/DcVector.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	size_t w = 1280;
	size_t h = 720;
	uint8_t img[w*h];

	auto f = fopen("/home/czel/sample_raw_b.raw","r");
	for (int i=0; i<20; i++)
		fread(img, w*h, 1, f);
	fclose(f);


	is::Image_info info;
	info.w = w;
	info.h = h;
	info.bytes_per_pixel = 1;
	info.bytes_per_row = w;
	(new is::Data_2d(c, img, &info))
		->scale_name_x("x")
		->scale_name_y("y")
		->name("frame-1");

	info.w = 512;
	info.h = 512;
	info.bytes_per_pixel = 1;
	info.bytes_per_row = w;
	(new is::Data_2d(c, img, &info))
		->scale_name_x("x")
		->scale_name_y("y")
		->name("cell");

	is::Size cell_size(512, 512);
	uint8_t cell[cell_size.w*cell_size.h];
	capture_image(img, &info, cell, cell_size);
	BnComplex c_cell[cell_size.w*cell_size.h];
	for (size_t i= 0; i<cell_size.w*cell_size.h; i++){
		c_cell[i] = cell[i];
	}
	BnComplex2DFFT<BnComplexFFT2n<9>> fft;
	fft.execute(c_cell, 512);
	for (size_t i= 0; i<cell_size.w*cell_size.h; i++){
		double v = sqrt((c_cell[i]*c_cell[i].conjugate()).re)/20.0;
		if (v>255)
			cell[i] = 255;
		else
			cell[i] = v;
	}

	info.w = 512;
	info.h = 512;
	info.bytes_per_pixel = 1;
	info.bytes_per_row = 512;
	(new is::Data_2d(c, cell, &info))
		->scale_name_x("x")
		->scale_name_y("y")
		->name("fft");

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
