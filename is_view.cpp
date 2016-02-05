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

		_is_visible = false;
		_is_temporaly_visible = false;
		_is_marked = false;

		_transpose = false;

		_name = "unnamed";
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

	View* View::header_frame(Rect r){
		_header_frame = r;
		return this;
	}
	Rect View::header_frame() const{
		return _header_frame;
	}

	View* View::contents_frame(Rect r){
		_contents_frame = r;
		return this;
	}
	Rect View::contents_frame() const{
		return _contents_frame;
	}

	View* View::name(std::string str){
		_name = str;
		return this;
	}
	View* View::name(const char* format, ...){
		va_list args;
		va_start(args, format);

		va_list args_copy;
		va_copy(args_copy, args);
		int len = vsnprintf(NULL, 0, format, args_copy)+1;
		va_end(args_copy);

		char* buffer = new char[len];
		vsnprintf(buffer, len, format, args);
		_name = buffer;

		delete[] buffer;
		va_end(args);
	}
	std::string View::name() const{
		return _name;
	}

	View* View::transpose(bool transpose){
		_transpose = transpose;
		return this;
	}

	void View::update(Core *c){
		if (not _transpose){
			_contents_frame.p.x = _frame.p.x;
			_contents_frame.p.y = _frame.p.y;
			_contents_frame.s.w = _frame.s.w;
			_contents_frame.s.h = _frame.s.h - header_size;
		}
		else{
			_contents_frame.p.y = _frame.p.x;
			_contents_frame.p.x = _frame.p.y;
			_contents_frame.s.h = _frame.s.w;
			_contents_frame.s.w = _frame.s.h - header_size;
		}

		_header_frame.p.x = _frame.p.x;
		_header_frame.p.y = _frame.p.y + _frame.s.h - header_size;
		_header_frame.s.w = _frame.s.w;
		_header_frame.s.h = header_size;

		if (not _transpose){
			update_contents(c);
		}
		else{
			glPushMatrix();
			double m[16] = {
				0.0, 1.0, 0.0, 0.0,
				1.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0,
			};
			glMultMatrixd(m);
			update_contents(c);
			glPopMatrix();
		}
		update_header(c);
	}
	View* View::view_at(Point p){ 
		return NULL; 
	}
	Mouse_event_tracker* 
		View::begin_mouse_event(Core *c, Event *e, uint8_t button_id){
		View* v = view_at(e->cursor());
		if (v) return v->begin_mouse_event(c, e, button_id);
		return NULL;
	}
	void View::mouse_move(Core *c, Event* e){
		View* v = view_at(e->cursor());
		if (v) v->mouse_move(c, e);
	}
	void View::scroll(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz){
		View* v = view_at(e->cursor());
		if (v) v->scroll(c, e, dx, dy, dz);
	}
	void View::scroll_to(Core *c, Event *e, 
				      std::string name, int32_t location){
		View* v = view_at(e->cursor());
		if (v) v->scroll_to(c, e, name, location);
	}
	void View::key_down(Core *c, Event *e, uint8_t key_name){
		View* v = view_at(e->cursor());
		if (v) v->key_down(c, e, key_name);
	}

	Point View::cursor_in_view_coord(Event *e) const{
		Point c = e->cursor();
		if (not _transpose)
			return Point(c.x-_frame.p.x, c.y-_frame.p.y);
		else
			return Point(c.y-_frame.p.y, c.x-_frame.p.x);
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
		mark_type = 0;
		layouter = new Virtical_layouter_v1;
		//layouter = new Horizontial_layouter_v1;
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
	void View_list::scroll_all
		(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz){
		size_t ct = 0;
		View* v= list_begin;
		while(v && ct != active_view_count){
			if (v->is_visible()){
				v->scroll(c, e, dx, dy, dz);
				ct ++;
			}
			v= v->next_list_element();
		}
	}
	void View_list::scroll_to_all
		(Core *c, Event* e, std::string name, int32_t location){
		size_t ct = 0;
		View* v= list_begin;
		while(v && ct != active_view_count){
			if (v->is_visible()){
				v->scroll_to(c, e, name, location);
				ct ++;
			}
			v= v->next_list_element();
		}
	}

	size_t View_list::list_slot_count(){
		Rect f = frame();
		return f.s.h/slot_height;
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
			auto back_elem = list_begin->back_list_element();
			if (! back_elem)
				break;
			list_begin = back_elem;
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
	
	void View_list::set_tmp_visible(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->set_temporaly_visible();
			v = v->next_list_element();
		}
	}
	void View_list::reset_tmp_visible(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->reset_temporaly_visible();
			v = v->next_list_element();
		}
	}
	void View_list::set_visible(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->set_visible();
			v = v->next_list_element();
		}
	}
	void View_list::reset_visible(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->reset_visible();
			v = v->next_list_element();
		}
	}
	void View_list::mark(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->mark();
			v = v->next_list_element();
		}
	}
	void View_list::unmark(size_t begin, size_t end){
		View* v = view_at_slot(std::min(begin, end));
		size_t count = abs(begin-end);
		for (int i = 0; i<=count; i++){
			if (v == NULL)
				return;
			v->unmark();
			v = v->next_list_element();
		}
	}

	bool View_list::is_visible(View* v){
		bool cond0 = mark_type == 0 && 
			(v->is_visible());
		bool cond1 = mark_type == 1 && 
			(v->is_visible() || v->is_marked());
		bool cond2 = mark_type == 2 && 
			(v->is_visible() && ! v->is_marked());
		return cond0 || cond1 || cond2;
	}

	void View_list::layout(){
		Rect f = frame();
		f.p.x += list_width;
		f.s.w -= list_width;

		std::vector<View*> vs;

		View* v= list_begin;
		while(v){
			if (is_visible(v)){
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
			if (is_visible(v)){
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
		color::background();
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
		const size_t mark_width = 8;

		Text_texture *tex_gen = shared_text_texture();
		Size ts(list_width-mark_width, text_size);

		GLuint tex = tex_gen->generate(ts, v->name().c_str());

		if (is_visible(v)){
			color::hilight();
			draw_texture(0, 0, slot_idx*text_size+1, mark_width, ts.h-2);
		}

		color::text();
		draw_texture(tex, mark_width, slot_idx*text_size, ts.w, ts.h);

		glDeleteTextures(1,&tex);
	}
	View* View_list::view_at(Point p){
		Rect f = frame();
		Rect vf(f.p.x+list_width, f.p.y, f.s.w-list_width, f.s.h);
		if (vf.in_side(p)){
			size_t ct = 0;
			View* v= list_begin;
			while(v && ct != active_view_count){
				if (v->is_visible()){
					if (v->frame().in_side(p))
						return v;
					ct ++;
				}
				v= v->next_list_element();
			}
		}
		return NULL;
	}


	class View_select_tracker:public Mouse_event_tracker{
		View_list* target;
		size_t begin;
		size_t end;

		public:
		View_select_tracker(View_list* t):target(t){ };
		~View_select_tracker(){ };

		void mouse_down(Core *c, Event *e){
			target->mark_as_visible();
			begin = target->slot_at(e);
			end = begin;
			target->mark(begin, end);
		}
		void mouse_drag(Core *c, Event *e){
			target->unmark(begin, end);
			end = target->slot_at(e);
			target->mark(begin, end);
		}
		void mouse_up(Core *c, Event *e){
			target->unmark(begin, end);
			target->set_visible(begin, end);
		}
	};

	class View_deselect_tracker:public Mouse_event_tracker{
		View_list* target;
		size_t begin;
		size_t end;

		public:
		View_deselect_tracker(View_list* t):target(t){ };
		~View_deselect_tracker(){ };

		void mouse_down(Core *c, Event *e){
			target->mark_as_invisible();
			begin = target->slot_at(e);
			end = begin;
			target->mark(begin, end);
		}
		void mouse_drag(Core *c, Event *e){
			target->unmark(begin, end);
			end = target->slot_at(e);
			target->mark(begin, end);
		}
		void mouse_up(Core *c, Event *e){
			target->unmark(begin, end);
			target->reset_visible(begin, end);
		}
	};
	Mouse_event_tracker* 
		View_list::begin_mouse_event(Core *c, Event *e, uint8_t button_id){

		View* cv = view_at(e->cursor());
		if (cv) return cv->begin_mouse_event(c, e, button_id);

		Rect f = frame();
		Rect lf(f.p.x, f.p.y, list_width, f.s.h);
		if (not lf.in_side(e->cursor())) return NULL;

		View* v = view_at_slot(slot_at(e));
		if (button_id == 0 && v && not v->is_visible()){
			return new View_select_tracker(this);
		}
		if (button_id == 0 && v && v->is_visible()){
			return new View_deselect_tracker(this);
		}
		if (v)
			v->_is_visible = !v->_is_visible;
		return NULL;
	}

	void View_list::key_down(Core *c, Event *e, uint8_t key_name){
		if (key_name == 'a'){
			select_all();
			return;
		}
		if (key_name == 'A'){
			deselect_all();
			return;
		}

		int32_t amount = 64;
		if (key_name == 'N'){
			scroll_all(c, e, amount, 0, 0);
			return;
		}
		if (key_name == 'H'){
			scroll_all(c, e, -amount, 0, 0);
			return;
		}
		if (key_name == 'C'){
			scroll_all(c, e, 0, amount, 0);
			return;
		}
		if (key_name == 'T'){
			scroll_all(c, e, 0, -amount, 0);
			return;
		}
		if (key_name == 'l'){
			layouter = new Virtical_layouter_v1;
			return;
		}
		if (key_name == 's'){
			layouter = new Horizontial_layouter_v1;
			return;
		}
		if (key_name == '/'){
			layouter = new Matrix_layouter;
			return;
		}

		View* v = view_at(e->cursor());
		if (not v) return;

		if (key_name == 'x'){
			v->reset_visible();
			return;
		}
		v->key_down(c, e, key_name);
	}
	void View_list::scroll_to(Core *c, Event *e, 
				      std::string name, int32_t location){
		scroll_to_all(c, e, name, location);
	}
	void View_list::scroll(
			Core *c, Event *e, int32_t dx, int32_t dy, int32_t dz){
		View* v = view_at(e->cursor());
		if (v){
			v->scroll(c, e, dx, dy, dz);
			return;
		}
		if (dy == 0){
			return;
		}
		else if (dy > 0){
			auto next = list_begin->next_list_element();	
			if (next){
				list_begin = next;
			}
		}
		else{
			auto back = list_begin->back_list_element();	
			if (back){
				list_begin = back;
			}
	}
}
}
