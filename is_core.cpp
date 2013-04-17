#include "is_config.h"
#include "is_pch.h"

#include "is_header_all.h"
#include "is_texture.h"
#include "is_color.h"
#include "is_graphics.h"
#include "is_event.h"
#include "is_view.h"
#include "is_core.h"


namespace is{

	Core::Core(){
		default_view = new View_list;
		main_window = NULL;
	}

	size_t Core::get_scale(std::string name){
		if (scale.count(name))
			return scale[name];
		return 0;
	}
	void Core::set_scale(std::string name, size_t value){
		scale[name] = value;
	}
	void Core::add(View *v){
		if (default_view){
			default_view->add(v);
		}
		else if (main_window){
			main_window->add(v);
		}
		else{
			printf("WTF\n");
		};
	}

	Window::Window(Core* c):core(c){
		event = new Event;
		root_view = c->default_view;
		c->main_window = this;
		c->default_view = NULL;
	}

	Window::~Window(){
		delete event;
	}

	void Window::setup_matrix(Size s){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_VIEWPORT);
		glLoadIdentity();
		glOrtho(0, s.w, 0, s.h, -100, 100);
	}

	void Window::add(View *v){
		root_view->add(v);
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

		if (root_view){
			root_view->frame(Rect(0,0,s.w, s.h));
			root_view->update(core);
		}

	}

	View* Window::view_at(Point p){
		if (root_view)
			return root_view->view_at(p);
		return NULL;
	}

	void Window::mouse_down(Size s, Point p, uint8_t button_id){
		event->cursor(p);
		if (m_tracker.count(button_id))
			printf("bad thing happen\n");

		View* target_view = view_at(p);
		Mouse_event_tracker* tracker = 
			target_view->begin_mouse_event(core, event, button_id);
		if (tracker){
			tracker->button_id(button_id);
			tracker->mouse_down(core, event);
			m_tracker[button_id] = tracker;
		}
	}

	void Window::mouse_drag(Size s, Point p){
		event->cursor(p);
		for (auto itr = m_tracker.begin();
				itr != m_tracker.end(); itr++){
			Mouse_event_tracker* tracker = itr->second;
			tracker->mouse_drag(core, event);
		}
	}

	void Window::mouse_up(Size s, Point p, uint8_t button_id){
		event->cursor(p);
		if (m_tracker.count(button_id)){
			Mouse_event_tracker* tracker = m_tracker[button_id];
			tracker->mouse_up(core, event);
			m_tracker.erase(button_id);
			delete tracker;
		}
	}

	void Window::mouse_move(Size s, Point p){
		event->cursor(p);
		View* target_view = view_at(p);
		if (target_view)
			target_view->mouse_move(core, event);

	}
	void Window::wheel_move(Size s, Point p, uint32_t dx, uint32_t dy){
		event->cursor(p);
		View* target_view = view_at(p);
		if (target_view)
			target_view->scroll(core, event, dx, dy, 0);
	}
	void Window::key_down(Size s, uint8_t key){
		if (key == 27){
			exit(0);
		}
		root_view->key_down(core, event, key);

		View* target_view = view_at(event->cursor());
		if (!target_view) return;
	
		int32_t amount = 32;

		if (key == 'n'){
			target_view->scroll(core, event, amount, 0, 0);
		}
		if (key == 'h'){
			target_view->scroll(core, event, -amount, 0, 0);
		}
		if (key == 'c'){
			target_view->scroll(core, event, 0, amount, 0);
		}
		if (key == 't'){
			target_view->scroll(core, event, 0, -amount, 0);
		}
	}
	void Window::key_up(Size s, uint8_t key){

	}
}


