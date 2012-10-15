#include "is_pch.h"

#include "is_header_all.h"

namespace is{
namespace GLUT{
	core* shared_core = NULL;

	void display(){
		if (!shared_core)
			return;

		shared_core->update((size){640, 480});
	}

	void keyboard(unsigned char Key, int x, int y){
		exit(0);
	}
}}

namespace is{

	void core::update(size s){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_VIEWPORT);
		glLoadIdentity();
		glOrtho(0, 640, 0, 480, -100, 100);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			view* v = (*itr)->default_view();
			v->update(s, *itr);
		}
		glutSwapBuffers();
	}

	void core::run_GLUT(int argc, char** argv){
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


