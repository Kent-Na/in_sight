#include "is_pch.h"

#include "is_header_all.h"
#include "is_texture.h"

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
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			view* v = (*itr)->default_view();
			v->update(s, *itr);
		}

		text_texture tex_gen;
		size tex_size = {256, 12};
		GLuint tex = tex_gen.generate(tex_size, "test the ppyy.");
		glBindTexture(GL_TEXTURE_2D, tex);
		glColor4f(1,0,0,1);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,1);	glVertex2f(0,0);
		glTexCoord2f(0,0);	glVertex2f(0,12);
		glTexCoord2f(1,0);	glVertex2f(256,12);
		glTexCoord2f(1,1);	glVertex2f(256,0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

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


