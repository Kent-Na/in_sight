#include "is_pch.h"

#include "is_header_all.h"
#include "is_texture.h"
#include "is_core.h"
#include "is_color.h"
#include "is_graphics.h"
#include "is_data_list.h"

namespace is{
namespace GLUT{
	Core* shared_core = NULL;

	void display(){
		if (!shared_core)
			return;

		shared_core->update((Size){640, 480});
	}

	void keyboard(unsigned char Key, int x, int y){
		exit(0);
	}
}}

namespace is{

	void Core::update(Size s){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_VIEWPORT);
		glLoadIdentity();
		glOrtho(0, 640, 0, 480, -100, 100);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		color::background();
		draw_rect(0,0,s.w,s.h);

		const size_t list_w = 84;

		Virtical_layouter layouter;
		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			View* v = (*itr)->default_view();
			v->data = *itr;
			layouter.add_view(v);
		}

		Size vs = {s.w-list_w, s.h};
		layouter.layout(vs);

		glPushMatrix();
		glTranslated(list_w, 0, 0);
		layouter.update(vs);
		glPopMatrix();

/*
		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			View* v = (*itr)->default_view();
			Size vs = {s.w-list_w, s.h};
			v->update(vs, *itr);
			glPopMatrix();
		}
		*/

		Data_list dl;
		Size dls = {list_w, s.h};
		dl.update(this, dls);

		glutSwapBuffers();
	}

	void Core::run_GLUT(int argc, char** argv){
		if (GLUT::shared_core){
			printf("err. run_GLUT can not handle multi core object.");
			return;
		}
		GLUT::shared_core = this;

		glutInit(&argc, argv);

		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutInitWindowSize(640, 480);
		glutCreateWindow("In side of my brain");

		glutDisplayFunc(GLUT::display);
		glutKeyboardFunc(GLUT::keyboard);
		glutSwapBuffers();
		glutMainLoop();
	}
}


