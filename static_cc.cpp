#include <math.h>
#include "in_sight.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	//auto tracker = new cc::Tracker;

	size_t w = 1280;
	size_t h = 720;
	uint8_t img[w*h];

	auto f = fopen("home/czel/sample_raw_b.raw","r");
	while (not feof){
		fread(img, w*h, 1, f);
		
		//tracker->process(img, w, h);
	}

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
