#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_core.h"
#include "is_glut.h"


namespace is{
namespace GLUT{

	std::map<int,Window*> window_map;

	void display(){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		window->update(s);
		glutSwapBuffers();
	}

	void mouse(int Button, int State, int x, int y){
		if (State != GLUT_UP)
			return;

		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		Point p = {(size_t)x, (size_t)s.h-y};

		const int amount_of_schroll = 20;
		if (Button == 0)
			window->mouse(s, p);
		if (Button == 3)//up
			window->wheel_move(s, p, 0, amount_of_schroll);
		if (Button == 4)//down
			window->wheel_move(s, p, 0, -amount_of_schroll);
		if (Button == 5)//left
			window->wheel_move(s, p, amount_of_schroll, 0);
		if (Button == 6)//right
			window->wheel_move(s, p, -amount_of_schroll, 0);
		window->update(s);
		glutSwapBuffers();
	}

	void motion(int x, int y){

	}

	void passive_motion(int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		Point p = {(size_t)x, (size_t)s.h-y};

		window->mouse_move(s, p);
		window->update(s);
		glutSwapBuffers();
	}

	void keyboard(unsigned char Key, int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		window->key_down(s, Key);
		window->update(s);
		glutSwapBuffers();
	}

	void init(int argc, char** argv){
		glutInit(&argc, argv);
	}
	void run(){
		glutMainLoop();
	}

	//It is GLUT window.
	Window::Window(Core *c){
		int old_window_id = glutGetWindow();

		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutInitWindowSize(640, 480);
		glutCreateWindow("In side of my brain");

		int window_id = glutGetWindow();
		window_map[window_id] = this;

		core = c;

		glutDisplayFunc(is::GLUT::display);
		glutKeyboardFunc(is::GLUT::keyboard);
		glutMouseFunc(is::GLUT::mouse);
		glutMotionFunc(is::GLUT::motion);
		glutPassiveMotionFunc(is::GLUT::passive_motion);
		glutSwapBuffers();
		if (old_window_id != 0)
			glutSetWindow(old_window_id);
	}
}}

