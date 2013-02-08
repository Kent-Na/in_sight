#include <math.h>
#include "in_sight.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	double data[256];

	//sin wave
	for (int i = 0; i<256; i++){
		data[i] = cos(i/10.0);
	}
	new is::Data_1d<double>(c, data, 256);

	//cos wave
	for (int i = 0; i<256; i++){
		data[i] = sin(i/20.0);
	}
	new is::Data_1d<double>(c, data, 256);

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
