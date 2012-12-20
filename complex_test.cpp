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
		auto f = fopen("/home/czel/4a.log","r");
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
		auto f = fopen("/home/czel/4j.log","r");
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

	///
	//process acc
	double acc_dt = 0;
	{

		acc_dt = (acc_t[500]-acc_t[0])/500.0;

		(new is::Data_1d(c, acc_x))
			->scale_name("ta")
			->name("acc_x");
		(new is::Data_1d(c, acc_y))
			->scale_name("ta")
			->name("acc_y");
		(new is::Data_1d(c, acc_z))
			->scale_name("ta")
			->name("acc_z");
		(new is::Data_1d(c, acc_t))
			->scale_name("ta")
			->name("acc_t")
			->view_class(&is::View_1d_label::klass);
	}

	///
	//process rot
	{

		size_t stable_sample_count = 600;
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

		DcQuaternion r_fix = 
			DcRotateionQuaternion(0.5*atan(0.025),
								  DcVector(0,1,0));

		for (size_t i = 0; i<rot_t.size(); i++){
			DcVector v = r_fix.rotate(DcVector(rot_x[i],rot_y[i],rot_z[i]));
			rot_x[i] = v.x;
			rot_y[i] = v.y;
			rot_z[i] = v.z;
		}

		DcQuaternion q(1,0,0,0);
		std::vector<double> gx;
		std::vector<double> gy;
		std::vector<double> gz;
		std::vector<double> g_mag;
		for (size_t i= 0; i<rot_t.size(); i++){
			DcVector rot_v(rot_x[i], rot_y[i], rot_z[i]);
			double dt = k2;
			double theta = -0.5*rot_v.magnitude()*dt;
			DcQuaternion rotq = DcRotateionQuaternion(theta,rot_v);
			q = rotq*q;
			DcVector g(0,0,-1);
			g = q.rotate(g);
			gx.push_back(g.x);
			gy.push_back(g.y);
			gz.push_back(g.z);
			g_mag.push_back(DcVector(g.x,g.y,g.z).magnitude());
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
			px += 0.5*r_acc_x[i]*acc_dt*acc_dt+vx*acc_dt;
			py += 0.5*r_acc_y[i]*acc_dt*acc_dt+vy*acc_dt;
			pz += 0.5*r_acc_z[i]*acc_dt*acc_dt+vz*acc_dt;
			vx += r_acc_x[i]*acc_dt;
			vy += r_acc_y[i]*acc_dt;
			vz += r_acc_z[i]*acc_dt;
			pos_x.push_back(px);
			pos_y.push_back(py);
			pos_z.push_back(pz);
			vel_x.push_back(vx);
			vel_y.push_back(vy);
			vel_z.push_back(vz);
		}

		(new is::Data_1d(c, rot_x))
			->scale_name("tr")
			->name("rot_x");
		(new is::Data_1d(c, rot_y))
			->scale_name("tr")
			->name("rot_y");
		(new is::Data_1d(c, rot_z))
			->scale_name("tr")
			->name("rot_z");
		(new is::Data_1d(c, rot_t))
			->scale_name("tr")
			->name("rot_t")
			->view_class(&is::View_1d_label::klass);

		(new is::Data_1d(c, gx))
			->scale_name("tr")
			->name("g_x");
		(new is::Data_1d(c, gy))
			->scale_name("tr")
			->name("g_y");
		(new is::Data_1d(c, gz))
			->scale_name("tr")
			->name("g_z");

		(new is::Data_1d(c, g_mag))
			->scale_name("tr")
			->name("g_mag");

		(new is::Data_1d(c, gxd))
			->scale_name("ta")
			->name("g_x_fix");
		(new is::Data_1d(c, gyd))
			->scale_name("ta")
			->name("g_y_fix");
		(new is::Data_1d(c, gzd))
			->scale_name("ta")
			->name("g_z_fix");

		(new is::Data_1d(c, r_acc_x))
			->scale_name("ta")
			->name("acc_x_fix");
		(new is::Data_1d(c, r_acc_y))
			->scale_name("ta")
			->name("acc_y_fix");
		(new is::Data_1d(c, r_acc_z))
			->scale_name("ta")
			->name("acc_z_fix");

		(new is::Data_1d(c, vel_x))
			->scale_name("ta")
			->name("_vel_x");
		(new is::Data_1d(c, vel_y))
			->scale_name("ta")
			->name("_vel_y");
		(new is::Data_1d(c, vel_z))
			->scale_name("ta")
			->name("_vel_z");

		(new is::Data_1d(c, pos_x))
			->scale_name("ta")
			->name("pos_x");
		(new is::Data_1d(c, pos_y))
			->scale_name("ta")
			->name("pos_y");
		(new is::Data_1d(c, pos_z))
			->scale_name("ta")
			->name("pos_z");
	}

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
