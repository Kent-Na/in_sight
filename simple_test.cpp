#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "is_pch.h"
#include "is_color.h"
#include "is_header_all.h"
#include "is_data_1d.h"

int main(int argc, char** argv)
{
	double data[256];
	for (int i = 0; i<256; i++){
		data[i] = sin(i/10.0);
	}

	auto c = new is::core;
	new is::data_1d(c, data, 256);

	c->run_GLUT(argc, argv);

	return 0;
}
