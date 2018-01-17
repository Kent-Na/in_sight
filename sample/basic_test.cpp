#include <math.h>
//#include <functional>
#include "../in_sight/in_sight.h"
//#include "../tc_math/tc_math.h"


int main(int argc, char** argv)
{
	is::native::init(argc,argv);

	auto c = new is::Core;

	new is::native::Window(c);

	size_t data_length = 500;
	double data[data_length];
	for (int i=0; i<data_length; i++){
		data[i] = sin(i*0.01);
	}

	new is::View_1d<>(c, data, data_length);

	is::native::run();

	return 0;
}
