#include <inttypes.h>
#include <stddef.h>


namespace is{
	class Data;
	class Node;

	class Core;
	class View;
	struct Frame;
	class Layouter;
	class Window;

	struct Size{
		size_t w;
		size_t h;
		Size(){ }
		Size(size_t w0, size_t h0){
			w = w0;
			h = h0;
		}
	};

	struct Point{
		size_t x;
		size_t y;
	};

	struct Rect{
		Point p;
		Size s;
		Rect(Point p, Size s){
			this->p = p;
			this->s = s;
		}
		Rect(Point p0, Point p1){
			p.x = std::min(p0.x, p1.x);
			p.y = std::min(p0.y, p1.y);
			s.w = std::max(p0.x, p1.x) - p.x;
			s.h = std::max(p0.y, p1.y) - p.y;
		}
		Rect(size_t x, size_t y, size_t w, size_t h){
			p.x = x;
			p.y = y;
			s.w = w;
			s.h = h;
		}

		bool in_side(Point p0){
			bool cond0 = p.x <= p0.x;
			bool cond1 = p.y <= p0.y;
			bool cond2 = p.x+s.w > p0.x;
			bool cond3 = p.y+s.h > p0.y;
			return cond0 && cond1 && cond2 && cond3;
		}
	};

	enum Event_code{
		scroll_x_plus,
		scroll_x_minus,
		scroll_y_plus,
		scroll_y_minus,

		scroll_x_plus_page,
		scroll_x_minus_page,
		scroll_y_plus_page,
		scroll_y_minus_page,
	};

	//S is root class.
	template <typename S, typename T = S>
	class Class_object : public Class_object <S,S>{
		public:
		virtual S* new_instance(){ return new T;}
	};

	template <typename S>
	class Class_object<S,S>{
		public:
		virtual S* new_instance(){ return new S;}
	};

	using View_class = Class_object<View,View>;
	using Data_class = Class_object<Data>;

	class View{
		public:
		Data* data;
		virtual ~View(){ return; }
		virtual size_t min_h() { return 0; }
		virtual size_t max_h() { return 200; }
		virtual size_t min_w() { return 0; }
		virtual size_t max_w() { return 200; }

		virtual void update(Core *c, Size s) const { return; }
		virtual void mouse_move(Core *c, Size s, Point p) { return; }
		virtual void wheel_move(Core *c, Size s, Point p,
								int32_t dx, int32_t dy) { return; }
		virtual void event(Core *c, Size s, Event_code code) { return; }
	};

	struct Frame{
		size_t x;
		size_t y;
		size_t w;
		size_t h;

		View* view;
		void update(Core *c) const {
			glPushMatrix();
			glTranslated(x,y,0);
			Size s = {w, h};
			view->update(c, s);
			glPopMatrix();
		}
		void mouse_move(Core *c, Point p) const { 
			Size s = {w, h};
			Point pv = {p.x-x, p.y-y};
			view->mouse_move(c,s,pv);
		}
		void wheel_move(Core *c, Point p, int32_t dx, int32_t dy) const { 
			Size s = {w, h};
			Point pv = {p.x-x, p.y-y};
			view->wheel_move(c,s,pv,dx,dy);
		}
		void event(Core *c, Event_code code){
			Size s = {w, h};
			view->event(c, s, code);
		}
	};

	class Data{
		std::string _name;
		View_class *_view_class;

		public:
		Data(){
			_name = "dummy";
			_view_class = NULL;
		}
		//create and return new default view
		virtual View* default_view() { 
			if (not _view_class)
				return NULL;
			return _view_class->new_instance();
		}

		///
		//name
		Data* name(std::string str){
			_name = str;
			return this;
		}
		std::string name() const{
			return _name;
		}

		///
		//view_class
		Data* view_class(View_class* klass){
			_view_class = klass;
			return this;
		}
		View_class* view_class() const{
			return _view_class;
		}

        Data* group(const char* name){
            return this;
        }
	};

	class Data_group{
		std::vector<Data*> data_list;

	};

	enum data_flag : uint32_t{
		visible = 1<<0
	};

	struct Data_with_flags{
		Data* data;
		uint32_t flag;
		Data_with_flags(Data* d, uint32_t f):data(d),flag(f) {};
		Data_with_flags(Data* d):data(d),flag(0) {};
	};

	using Data_list = std::vector<Data_with_flags>;

	struct Image_info{
		size_t w;//width
		size_t h;//height
		size_t channel;//1 for grayscale, 3 for RGB...
		size_t bytes_per_row;
		size_t bytes_per_pixel;
	};
/*
	class Port{
		Node *holder;
		Port *target;
		virtual Data_class<>* type();
	};

	class Node{
		uint32_t frag;
		enum node_frag{
			is_updated
		};

		virtual size_t input_count();
		virtual Data_class<>* input_type_at(size_t idx);
		virtual void set_input(size_t idx, Data data);

		virtual size_t output_count();
		virtual Data_class<>* output_type_at(size_t idx);
		virtual void set_output(size_t idx, Data data);
	};

	class Filter:public Node{
		virtual void apply();
	};

	class Function_filter:public Filter{
		void (*func)(Data *in, Data *out);
	};

*/
/////////////////////////////////////////////////

}
