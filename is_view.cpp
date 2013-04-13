#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_event.h"
#include "is_color.h"
#include "is_graphics.h"
#include "is_texture.h"
#include "is_view.h"
#include "is_layouter.h"

namespace is{

	View::View(){
		_next = NULL;
		_back = NULL;

		_is_visible=false;
	}
	View* View::next_list_element() const{
		return _next;
	}

	View* View::back_list_element() const{
		return _back;
	}

	View* View::frame(Rect r){
		_frame = r;
		return this;
	}
	Rect View::frame() const{
		return _frame;
	}

	View* View::name(std::string str){
		_name = str;
		return this;
	}
	std::string View::name() const{
		return _name;
	}

	Point View::cursor_in_view_coord(Event *e) const{
		Point c = e->cursor();
		return Point(c.x-_frame.p.x, c.y-_frame.p.y);
	}

	void Red_view::update(Core *c){ 
		Rect f = frame();
		glColor4d(1, 0, 0, 1);
		draw_rect(f.p.x, f.p.y, f.s.w, f.s.h);
	}
	void Blue_view::update(Core *c){ 
		glColor4d(0, 0, 1, 1);
		Rect f = frame();
		draw_rect(f.p.x, f.p.y, f.s.w, f.s.h);
	}

	View_list::View_list(){
		first_view = NULL;
		last_view = NULL;
		list_begin = NULL;
		list_width = 84;
		slot_height = 14;
		active_view_count = 0;
		layouter = new Virtical_layouter_v1;
	}

	void View_list::add(View* v){
		if (first_view){
			View* old_last = last_view;
			last_view = v;
			old_last->_next = v;
			v->_back = old_last;
		}
		else{
			first_view = v;
			last_view = v;
			v->_next = NULL;
			v->_back = NULL;
			list_begin = v;
		}
	}

	void View_list::select_all(){
		View* v= list_begin;
		while(v){
			v->_is_visible = true;
			v= v->next_list_element();
		}
	}
	void View_list::deselect_all(){
		View* v= list_begin;
		while(v){
			v->_is_visible = false;
			v= v->next_list_element();
		}
	}

	size_t View_list::list_slot_count(){
		return 20;

	}
	void View_list::recalculate_list_begin(){
		size_t count = list_slot_count();
		if (! list_begin)
			return;
		if (! list_begin->back_list_element())
			return;

		size_t negative_offset = count;
		View* v = list_begin;
		for (int i= 0; i<count; i++){
			negative_offset --;
			if (! v->next_list_element())
				break;
			v= v->next_list_element();
		}

		for (int i= 0; i<negative_offset; i++){
			if (! list_begin->back_list_element())
				break;
			list_begin= list_begin->next_list_element();
		}
	}
	size_t View_list::slot_at(Event *e){
		Point p = cursor_in_view_coord(e);
		return p.y/slot_height;
	}
	View* View_list::view_at_slot(size_t slot_idx){
		View* v= list_begin;
		if (!v) return NULL;
		for (int i=0; i<slot_idx; i++){
			v= v->next_list_element();
			if (!v) break;
		}
		return v;
	}
	
	void View_list::layout(){
		Rect f = frame();
		f.p.x += list_width;
		f.s.w -= list_width;

		std::vector<View*> vs;

		View* v= list_begin;
		while(v){
			if (v->_is_visible){
				vs.push_back(v);
			}
			v= v->next_list_element();
		}

		active_view_count = layouter->layout(f, vs);
	}

	void View_list::update(Core *c){
		layout();
		draw_list();
		draw_view(c);
	}

	void View_list::draw_view(Core *c){
		size_t ct = 0;
		View* v= list_begin;
		while(v && ct != active_view_count){
			if (v->_is_visible){
				v->update(c);
				ct ++;
			}
			v= v->next_list_element();
		}
	}

	void View_list::draw_list(){
		Rect f = frame();
		f.s.w = list_width;

		size_t count = list_slot_count();
		recalculate_list_begin();

		//clear background
		color::light_background();
		draw_rect(f.p.x, f.p.y, list_width, f.s.h);

		//draw elements
		View* v = list_begin;
		for (int i=0; i<count; i++){
			if (v == NULL)
				break;
			draw(v, i);
			v = v->next_list_element();
		}
	}

	void View_list::draw(View* v, size_t slot_idx){
		Rect f = frame();

		const size_t text_size = slot_height;

		Text_texture *tex_gen = shared_text_texture();
		Size ts(list_width, text_size);

		GLuint tex = tex_gen->generate(ts, v->name().c_str());

		if (v->is_visible()){
			color::light_hilight();
			draw_texture(0, 0, slot_idx*text_size, ts.w, ts.h);
			color::light_text();
			draw_texture(tex, 0, slot_idx*text_size, ts.w, ts.h);
		}
		else{
			color::light_text();
			draw_texture(tex, 0, slot_idx*text_size, ts.w, ts.h);
		}

		glDeleteTextures(1,&tex);
	}
	View* View_list::view_at(Point p){
		Rect f = frame();
		Rect vf(f.p.x+list_width, f.p.y, f.s.w-list_width, f.s.h);
		Point cp(p.x-f.p.x, p.y-f.p.y);
		if (vf.in_side(cp)){
			size_t ct = 0;
			View* v= list_begin;
			while(v && ct != active_view_count){
				if (v->_is_visible){
					if (v->frame().in_side(cp))
						return v->view_at(p);
					ct ++;
				}
				v= v->next_list_element();
			}
		}
		return this;
	}
	Mouse_event_tracker* 
		View_list::begin_mouse_event(Core *c, Event *e, uint8_t button_id){
		View* v = view_at_slot(slot_at(e));
		if (v)
			v->_is_visible = !v->_is_visible;
		return NULL;
	}
	void View_list::key_down(Core *c, Event *e, uint8_t key_name){
		if (key_name == 'a'){
			select_all();
		}
		if (key_name == 'A'){
			deselect_all();
		}
	}
}
