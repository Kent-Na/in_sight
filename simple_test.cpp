#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "is_pch.h"
#include "is_color.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_graphics.h"
#include "is_core.h"
#include "is_data_1d.h"

int main(int argc, char** argv)
{
	auto c = new is::Core;

	double data[256];
	for (int i = 0; i<256; i++){
		data[i] = cos(i/10.0);
	}
	new is::Data_1d(c, data, 256);

	for (int i = 0; i<256; i++){
		data[i] = sin(i/20.0);
	}
	new is::Data_1d(c, data, 256);

	c->run_GLUT(argc, argv);

	return 0;
}
