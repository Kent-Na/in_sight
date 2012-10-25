#include "is_pch.h"

#include "is_header_all.h"
#include "is_layouter.h"
#include "is_texture.h"
#include "is_core.h"
#include "is_color.h"
#include "is_graphics.h"
#include "is_data_list.h"
#include "is_data_1d.h"

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

		Point p = {x, s.h-y};

		window->mouse(s, p);
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

		Point p = {x, s.h-y};

		window->mouse_move(s, p);
		window->update(s);
		glutSwapBuffers();
	}

	void keyboard(unsigned char Key, int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		exit(0);
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


namespace is{

	void Core::update(Size s){
		
	}

	void Window::setup_matrix(Size s){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_VIEWPORT);
		glLoadIdentity();
		glOrtho(0, s.w, 0, s.h, -100, 100);
	}

	void Window::setup_GL_option(){
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Window::update(Size s){

		setup_matrix(s);
		setup_GL_option();

		glBindTexture(GL_TEXTURE_2D,0);
		color::background();
		draw_rect(0,0,s.w,s.h);

		const size_t list_w = 84;
		
		Data_list &data_list = core->get_data_list();

		Virtical_layouter_v1 layouter;
		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			if (!(itr->flag&visible))
				continue;
			View* v = itr->data->default_view();
			v->data = itr->data;
			layouter.add_view(v);
		}

		{
			View* v = new View_param_bar();
			layouter.add_view(v);
		}

		Size vs = {s.w-list_w, s.h};
		layouter.layout(vs);

		glPushMatrix();
		glTranslated(list_w, 0, 0);
		layouter.update(core, vs);
		glPopMatrix();

		Data_list_view dl;
		Size dls = {list_w, s.h};
		dl.update(data_list, dls);
	}

	void Window::mouse(Size s, Point p){
		const size_t list_w = 84;
		Data_list_view dl;
		Rect dlr(0,0,list_w, s.h);

		if (dlr.in_side(p)){
			Data_list &data_list = core->get_data_list();
			dl.select(data_list, s, p);
		}
	}

	void Window::mouse_move(Size s, Point p){
		const size_t list_w = 84;
		Rect dlr(0,0,list_w, s.h);

		Data_list &data_list = core->get_data_list();
		Virtical_layouter_v1 layouter;
		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			if (!(itr->flag&visible))
				continue;
			View* v = itr->data->default_view();
			v->data = itr->data;
			layouter.add_view(v);
		}

		{
			View* v = new View_param_bar();
			layouter.add_view(v);
		}

		Size vs = {s.w-list_w, s.h};
		Point vp = {p.x-list_w, p.y};
		layouter.layout(vs);
		if (not dlr.in_side(p)){
			layouter.mouse_move(core, vs, vp);
		}
	}

}


