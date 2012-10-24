#include <math.h>
#include "is_pch.h"
#include "is_color.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_graphics.h"
#include "is_core.h"
#include "is_data_1d.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	double data[256];

	//sin wave
	for (int i = 0; i<256; i++){
		data[i] = cos(i/3.0) + sin(i/6.0);
	}
	(new is::Data_1d(c, data, 256))->name("original");

	is::filter_1d_MA_LPF(data, 256, 13);
	(new is::Data_1d(c, data, 256-13))->name("LPF");

	for (int i = 0; i<256; i++){
		data[i] = cos(i/3.0) + sin(i/6.0);
	}
	is::filter_1d_MA_HPF(data, 256, 13);
	(new is::Data_1d(c, data, 256-13))->name("HPF");

	for (int i = 0; i<256; i++){
		data[i] = i%33;
	}
	(new is::Data_1d(c, data, 256-13))->name("try wave");

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
