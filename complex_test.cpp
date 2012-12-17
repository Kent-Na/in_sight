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

	{
		auto f = fopen("/home/czel/3a.log","r");
		std::vector<double> fvx;
		std::vector<double> fvy;
		std::vector<double> fvz;
		std::vector<double> fvt;
		std::vector<double> fvm;
		while (not feof(f)){
			float val[4];
			fread(&val, sizeof (float), 4, f);
			fvx.push_back(val[0]);
			fvy.push_back(val[1]);
			fvz.push_back(val[2]);
			fvt.push_back(val[3]);
			fvm.push_back(sqrt(val[1]*val[1]+val[2]*val[2]+val[0]*val[0]));
		}

		(new is::Data_1d(c, &fvx.front(), fvx.size()))
			->scale_name("ta")
			->name("acc_x");
		(new is::Data_1d(c, &fvy.front(), fvy.size()))
			->scale_name("ta")
			->name("acc_y");
		(new is::Data_1d(c, &fvz.front(), fvz.size()))
			->scale_name("ta")
			->name("acc_z");
		(new is::Data_1d(c, &fvt.front(), fvt.size()))
			->scale_name("ta")
			->name("acc_t")
			->view_class(&is::View_1d_label::klass);
		(new is::Data_1d(c, &fvm.front(), fvm.size()))
			->scale_name("ta")
			->name("acc_mag");
	}

	{
		auto f = fopen("/home/czel/3j.log","r");
		std::vector<double> fvx;
		std::vector<double> fvy;
		std::vector<double> fvz;
		std::vector<double> fsx;
		std::vector<double> fvt;

		double sigmax=0;
		while (not feof(f)){
			float val[4];
			fread(&val, sizeof (float), 4, f);
			sigmax += val[0];
			//if (s++ % 10)
			//	continue;
			fvx.push_back(val[0]);
			fsx.push_back(sigmax);
			fvy.push_back(val[1]);
			fvz.push_back(val[2]);
			fvt.push_back(val[3]);
		}

		size_t stable_sample_count = 100;
		double offset = fsx[stable_sample_count];
		double time = fvt[stable_sample_count]-fvt[0];
		double k1 = offset/time;
		double k2 = (time/stable_sample_count);
		for (size_t i= 0; i<fsx.size(); i++){
			fsx[i] -= k1 * (fvt[i]-fvt[0]);
			fsx[i] *= k2;
			fsx[i] = sin(fsx[i]);
		}

		DcQuaternion q(1,0,0,0);
		std::vector<double> gx;
		std::vector<double> gy;
		std::vector<double> gz;
		for (size_t i= 0; i<fsx.size(); i++){
			DcVector rot_v(fvx[i], fvy[i], fvz[i]);
			double dt = k2;
			double theta = -0.5*rot_v.magnitude()*dt;
			DcQuaternion rotq = DcRotateionQuaternion(theta,rot_v);
			q = rotq*q;
			DcVector g(0,0,-1);
			g = q.rotate(g);
			gx.push_back(g.x);
			gy.push_back(g.y);
			gz.push_back(g.z);
		}

		(new is::Data_1d(c, &fvx.front(), fvx.size()))
			->scale_name("tr")
			->name("rot_x");
		(new is::Data_1d(c, &fsx.front(), fsx.size()))
			->scale_name("tr")
			->name("ang_x");
		(new is::Data_1d(c, &fvy.front(), fvy.size()))
			->scale_name("tr")
			->name("rot_y");
		(new is::Data_1d(c, &fvz.front(), fvz.size()))
			->scale_name("tr")
			->name("rot_z");
		(new is::Data_1d(c, &fvt.front(), fvt.size()))
			->scale_name("tr")
			->name("rot_t")
			->view_class(&is::View_1d_label::klass);

		(new is::Data_1d(c, &gx.front(), gx.size()))
			->scale_name("tr")
			->name("g_x");
		(new is::Data_1d(c, &gy.front(), gy.size()))
			->scale_name("tr")
			->name("g_y");
		(new is::Data_1d(c, &gz.front(), gz.size()))
			->scale_name("tr")
			->name("g_z");
	}

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
