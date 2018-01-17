#include <math.h>
#include "in_sight.h"
#include "sub_systems/BnFFT.h"
#include "sub_systems/DcVector.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	std::vector<double> acc_t;
	std::vector<double> acc_x;
	std::vector<double> acc_y;
	std::vector<double> acc_z;

	{
		auto f = fopen("/home/czel/1a.log","r");
		std::vector<double> fvm;
		while (not feof(f)){
			float val[4];
			fread(&val, sizeof (float), 4, f);
			acc_x.push_back(val[0]);
			acc_y.push_back(val[1]);
			acc_z.push_back(val[2]+0.027);
			acc_t.push_back(val[3]);
		}
		fclose(f);
	}

	std::vector<double> rot_x;
	std::vector<double> rot_y;
	std::vector<double> rot_z;
	std::vector<double> rot_t;
	{
		auto f = fopen("/home/czel/1j.log","r");
		double sigmax=0;
		while (not feof(f)){
			float val[4];
			fread(&val, sizeof (float), 4, f);
			rot_x.push_back(val[0]);
			rot_y.push_back(val[1]);
			rot_z.push_back(val[2]);
			rot_t.push_back(val[3]);
		}
		fclose(f);
	}

	
	//DcQuaternion rot_rot;

	///
	//process acc
	double acc_dt = 0;
	std::vector<double> acc_x_lpf;
	std::vector<double> acc_y_lpf;
	std::vector<double> acc_z_lpf;
	{
		std::vector<double> acc_dt_a;

		for (int i = 0; i< acc_t.size()-1; i++){
			acc_dt_a.push_back(acc_t[i+1]-acc_t[i]);
		}

		acc_dt = (acc_t[500]-acc_t[0])/500.0;

		
		acc_x_lpf.insert(acc_x_lpf.begin(), acc_x.begin(), acc_x.end());
		acc_y_lpf.insert(acc_y_lpf.begin(), acc_y.begin(), acc_y.end());
		acc_z_lpf.insert(acc_z_lpf.begin(), acc_z.begin(), acc_z.end());

		is::filter_1d_MA_HPF(&acc_x_lpf.front(), acc_x_lpf.size(), 21);
		is::filter_1d_MA_HPF(&acc_y_lpf.front(), acc_y_lpf.size(), 21);
		is::filter_1d_MA_HPF(&acc_z_lpf.front(), acc_z_lpf.size(), 21);

		(new is::Data_1d<double>(c, acc_x))
			->scale_name("ta")
			->name("acc_x");
		(new is::Data_1d<double>(c, acc_y))
			->scale_name("ta")
			->name("acc_y");
		(new is::Data_1d<double>(c, acc_z))
			->scale_name("ta")
			->name("acc_z");
		(new is::Data_1d<double>(c, acc_t))
			->scale_name("ta")
			->name("acc_t")
			->view_class(&is::View_1d_label<double>::klass);

		(new is::Data_1d<double>(c, acc_x_lpf))
			->scale_name("ta")
			->name("acc_x_lpf");
		(new is::Data_1d<double>(c, acc_y_lpf))
			->scale_name("ta")
			->name("acc_y_lpf");
		(new is::Data_1d<double>(c, acc_z_lpf))
			->scale_name("ta")
			->name("acc_z_lpf");

		(new is::Data_1d<double>(c, acc_dt_a))
			->scale_name("ta")
			->name("acc_dt");
	}

	///
	//process rot
	{

		size_t stable_sample_count = 50;
		double time = rot_t[stable_sample_count]-rot_t[0];
		double k2 = (time/stable_sample_count);

		double rot_x_0 = 0.0;
		double rot_y_0 = 0.0;
		double rot_z_0 = 0.0;

		for (size_t i= 0; i<stable_sample_count; i++){
			rot_x_0 += rot_x[i];
			rot_y_0 += rot_y[i];
			rot_z_0 += rot_z[i];
		}
		rot_x_0 /= stable_sample_count;
		rot_y_0 /= stable_sample_count;
		rot_z_0 /= stable_sample_count;

		for (size_t i = 0; i<rot_t.size(); i++){
			rot_x[i] -= rot_x_0;
			rot_y[i] -= rot_y_0;
			rot_z[i] -= rot_z_0;
		}

		/*
		   DcQuaternion r_fix = 
			DcRotateionQuaternion(0.5*atan(0.023),
								  DcVector(0,1,0));

		for (size_t i = 0; i<rot_t.size(); i++){
			DcVector v = r_fix.rotate(DcVector(rot_x[i],rot_y[i],rot_z[i]));
			rot_x[i] = v.x;
			rot_y[i] = v.y;
			rot_z[i] = v.z;
		}
		*/


		DcQuaternion q(1,0,0,0);
		std::vector<double> gx;
		std::vector<double> gy;
		std::vector<double> gz;
		std::vector<double> g_mag;

		std::vector<double> calc_rot_x;
		std::vector<double> calc_rot_y;
		std::vector<double> calc_rot_z;
/*
		for (size_t i= 0; i<rot_t.size(); i++){
			DcVector g(0,0,-1);
			if (i == 0){
				gx.push_back(g.x);
				gy.push_back(g.y);
				gz.push_back(g.z);
				continue;
			}
			double dt = (rot_t[i-1]-rot_t[i])*0.5;

			DcVector rot_v0(rot_x[i-1], rot_y[i-1], rot_z[i-1]);
			DcVector rot_v1(rot_x[i], rot_y[i], rot_z[i]);

			double theta0 = -0.5*0.5*(rot_v0+rot_v1).magnitude()*dt;
			double theta1 = -0.5*rot_v1.magnitude()*dt;
			DcQuaternion rotq0 = DcRotateionQuaternion(theta0,rot_v0);
			DcQuaternion rotq1 = DcRotateionQuaternion(theta1,rot_v1);
			q = rotq0*rotq1*q;
			g = q.rotate(g);
			gx.push_back(g.x);
			gy.push_back(g.y);
			gz.push_back(g.z);
			g_mag.push_back(DcVector(g.x,g.y,g.z).magnitude());
		}
		*/

		for (size_t i= 0; i<rot_t.size(); i++){
			DcVector rot_v(rot_x[i], rot_y[i], rot_z[i]);
			double dt;
			if (i == 0)
				dt = 0;
			else
				dt = rot_t[i-1]-rot_t[i];

			double theta = 0.5*rot_v.magnitude()*dt;
			DcQuaternion rotq = DcRotateionQuaternion(theta,rot_v);
			q = q*rotq;
			DcVector g(0,0,-1);
			g = q.rotate(g);
			gx.push_back(g.x);
			gy.push_back(g.y);
			gz.push_back(g.z);
			g_mag.push_back(DcVector(g.x,g.y,g.z).magnitude());
		}
	

		for (size_t i= 0; i<rot_t.size(); i++){
			DcVector rot_v(rot_x[i], rot_y[i], rot_z[i]);
			double theta = 0.5*rot_v.magnitude()*acc_dt;
			DcQuaternion rotq = DcRotateionQuaternion(theta,rot_v);

			DcVector e0(1,0,0);
			DcVector e1(0,1,0);
			DcVector e2(0,0,1);

			DcVector ed0 = rotq.rotate(e0);
			DcVector ed1 = rotq.rotate(e1);
			DcVector ed2 = rotq.rotate(e2);

			calc_rot_x.push_back(acos(
				DcTvector(e1.y,e1.z)*DcTvector(ed1.y,ed1.z))-
				abs(rot_x[i])*acc_dt);
			calc_rot_y.push_back(acos(
				DcTvector(e2.z,e2.x)*DcTvector(ed2.z,ed2.x))-
				abs(rot_y[i])*acc_dt);
			calc_rot_z.push_back(acos(
				DcTvector(e0.x,e0.y)*DcTvector(ed0.x,ed0.y))-
				abs(rot_z[i])*acc_dt);
		}

		std::vector<double> gxd;
		std::vector<double> gyd;
		std::vector<double> gzd;

		std::vector<double> r_acc_x;
		std::vector<double> r_acc_y;
		std::vector<double> r_acc_z;

		size_t j = 0;
		for (size_t i = 0; i<acc_t.size(); i++){
			double t = acc_t[i];
			for (NULL; j <rot_t.size(); j++){
				if (rot_t[j]>t){
					gxd.push_back(gx[j]);
					gyd.push_back(gy[j]);
					gzd.push_back(gz[j]);
					r_acc_x.push_back(acc_x[i]-gx[j]);
					r_acc_y.push_back(acc_y[i]-gy[j]);
					r_acc_z.push_back(acc_z[i]-gz[j]);
					break;
				}
			}
		}

		{
			double cost = DcVector(
				r_acc_x.back(),
				r_acc_y.back(),
				r_acc_z.back()).magnitude();
			printf("cost = %f\n", cost);

		}



		(new is::Data_1d<double>(c, rot_x))
			->scale_name("tr")
			->name("rot_x");
		(new is::Data_1d<double>(c, rot_y))
			->scale_name("tr")
			->name("rot_y");
		(new is::Data_1d<double>(c, rot_z))
			->scale_name("tr")
			->name("rot_z");
		(new is::Data_1d<double>(c, rot_t))
			->scale_name("tr")
			->name("rot_t")
			->view_class(&is::View_1d_label<double>::klass);

		(new is::Data_1d<double>(c, calc_rot_x))
			->scale_name("tr")
			->name("calc_rot_x");
		(new is::Data_1d<double>(c, calc_rot_y))
			->scale_name("tr")
			->name("calc_rot_y");
		(new is::Data_1d<double>(c, calc_rot_z))
			->scale_name("tr")
			->name("calc_rot_z");

		(new is::Data_1d<double>(c, gx))
			->scale_name("tr")
			->name("g_x");
		(new is::Data_1d<double>(c, gy))
			->scale_name("tr")
			->name("g_y");
		(new is::Data_1d<double>(c, gz))
			->scale_name("tr")
			->name("g_z");

		(new is::Data_1d<double>(c, g_mag))
			->scale_name("tr")
			->name("g_mag");

		(new is::Data_1d<double>(c, gxd))
			->scale_name("ta")
			->name("g_x_fix");
		(new is::Data_1d<double>(c, gyd))
			->scale_name("ta")
			->name("g_y_fix");
		(new is::Data_1d<double>(c, gzd))
			->scale_name("ta")
			->name("g_z_fix");

		(new is::Data_1d<double>(c, r_acc_x))
			->scale_name("ta")
			->name("acc_x_fix");
		(new is::Data_1d<double>(c, r_acc_y))
			->scale_name("ta")
			->name("acc_y_fix");
		(new is::Data_1d<double>(c, r_acc_z))
			->scale_name("ta")
			->name("acc_z_fix");


		auto gen_vp = [&](
			const std::vector<double> &acc_x,
			const std::vector<double> &acc_y,
			const std::vector<double> &acc_z){
			//velocity
			double vx = 0;
			double vy = 0;
			double vz = 0;

			std::vector<double> vel_x;
			std::vector<double> vel_y;
			std::vector<double> vel_z;

			//position
			double px = 0;
			double py = 0;
			double pz = 0;

			std::vector<double> pos_x;
			std::vector<double> pos_y;
			std::vector<double> pos_z;
			for (size_t i = 0; i<acc_t.size(); i++){
				px += 0.5*acc_x[i]*acc_dt*acc_dt+vx*acc_dt;
				py += 0.5*acc_y[i]*acc_dt*acc_dt+vy*acc_dt;
				pz += 0.5*acc_z[i]*acc_dt*acc_dt+vz*acc_dt;
				vx += acc_x[i]*acc_dt;
				vy += acc_y[i]*acc_dt;
				vz += acc_z[i]*acc_dt;
				pos_x.push_back(px);
				pos_y.push_back(py);
				pos_z.push_back(pz);
				vel_x.push_back(vx);
				vel_y.push_back(vy);
				vel_z.push_back(vz);
			}

			(new is::Data_1d<double>(c, vel_x))
				->scale_name("ta")
				->name("_vel_x");
			(new is::Data_1d<double>(c, vel_y))
				->scale_name("ta")
				->name("_vel_y");
			(new is::Data_1d<double>(c, vel_z))
				->scale_name("ta")
				->name("_vel_z");

			(new is::Data_1d<double>(c, pos_x))
				->scale_name("ta")
				->name("pos_x");
			(new is::Data_1d<double>(c, pos_y))
				->scale_name("ta")
				->name("pos_y");
			(new is::Data_1d<double>(c, pos_z))
				->scale_name("ta")
				->name("pos_z");
		};

		gen_vp(acc_x_lpf, acc_y_lpf, acc_z_lpf);
		gen_vp(r_acc_x, r_acc_y, r_acc_z);
	}

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
