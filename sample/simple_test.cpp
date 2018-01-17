#include <math.h>
#include "in_sight.h"
#include "../tc_math/tc_math.h"

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

	for (int i = 0; i<256; i++){
		data[i] = cd::gaussian(i-127,0.02);
		//data[i] = cos(2*M_PI*i/256);
	}

	new is::Data_1d<double>(c, data, 256);

	//cd::gaussian_lpf(data, 256, 0.01);

	//BnRealFFT<8> fft;
	//fft.execute((BnComplex*)data);

	cd::real_fft_n2<8> fft;
	fft.execute(data);

	new is::Data_1d<double>(c, data, 256);

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
