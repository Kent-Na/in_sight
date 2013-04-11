#include <math.h>
#include <functional>
#include "../in_sight/in_sight.h"
#include "../tc_math/tc_math.h"


int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	new is::GLUT::Window(c);

	double dat[100];
	for (int i=0; i<100; i++){
		dat[i] = sin(i/10.0);
	}

	(new is::View_1d<>(c, dat, 100))
		->name("dat");


	is::GLUT::run();

	return 0;
}
