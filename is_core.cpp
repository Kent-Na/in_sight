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


namespace is{

	void Core::update(Size s){
		
	}

	Window::Window(){
		core = NULL;
		layouter = NULL;
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

	void Window::reload(){
		std::map<Data*, View*> old_views;
		if (layouter){
			for (size_t i= 0; i<layouter->size(); i++){
				View* v = layouter->view(i);
				old_views[v->data] = v;
			}
			layouter->detach_all_view();
			delete layouter;
		}

		layouter = new Virtical_layouter_v1;

		Data_list &data_list = core->get_data_list();

		for (auto itr = data_list.begin(); 
				itr != data_list.end(); itr++){
			if (!(itr->flag&visible))
				continue;
			auto v_itr = old_views.find(itr->data);
			if (v_itr == old_views.end()){
				View* v = itr->data->default_view();
				v->data = itr->data;
				layouter->add_view(v);
			}
			else{
				layouter->add_view(v_itr->second);
				old_views.erase(v_itr);
			}
		}
	}

	void Window::update(Size s){

		setup_matrix(s);
		setup_GL_option();

		glBindTexture(GL_TEXTURE_2D,0);
		color::background();
		draw_rect(0,0,s.w,s.h);

		const size_t list_w = 84;
		
		//if (not layouter)
		reload();

		Size vs = {s.w-list_w, s.h};
		layouter->layout(vs);

		glPushMatrix();
		glTranslated(list_w, 0, 0);
		layouter->update(core, vs);
		glPopMatrix();

		Data_list &data_list = core->get_data_list();

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

		Size vs = {s.w-list_w, s.h};
		Point vp = {p.x-list_w, p.y};
		layouter->layout(vs);
		if (not dlr.in_side(p)){
			layouter->mouse_move(core, vs, vp);
		}
	}
	void Window::wheel_move(Size s, Point p, uint32_t dx, uint32_t dy){
		const size_t list_w = 84;
		Rect dlr(0,0,list_w, s.h);

		Size vs = {s.w-list_w, s.h};
		Point vp = {p.x-list_w, p.y};
		layouter->layout(vs);
		if (not dlr.in_side(p)){
			layouter->wheel_move(core, vs, vp, dx, dy);
		}
	}
	void Window::key_down(Size s, uint8_t key){
		if (key == 'a'){
			Data_list &data_list = core->get_data_list();
			for (size_t i=0; i<data_list.size(); i++)
				data_list[i].flag = true;
		}
		if (key == 'A'){
			Data_list &data_list = core->get_data_list();
			for (size_t i=0; i<data_list.size(); i++)
				data_list[i].flag = false;
		}
		if (key == 27){
			exit(0);
		}
	}

}


