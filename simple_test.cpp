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
		data[i] = cos(i/10.0);
	}
	new is::Data_1d(c, data, 256);

	//cos wave
	for (int i = 0; i<256; i++){
		data[i] = sin(i/20.0);
	}
	new is::Data_1d(c, data, 256);

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
