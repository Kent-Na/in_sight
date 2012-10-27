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

	//S is root class.
	template <typename S, typename T>
	class Class_object : Class_object <S,S>{
		public:
		virtual S* get_instance(){ return new T;}
	};

	template <typename S>
	class Class_object<S,S>{
		public:
		virtual S* get_instance(){ return new S;}
	};

	template <typename T = Data>
	using Data_class = Class_object<Data, T>;
	template <typename T = View>
	using View_class = Class_object<View, T>;

	class Data{
		public:
		std::string _name;
		Data(){
			_name = "dummy";
		}
		//create and return new default view
		virtual View* default_view() { return NULL; }
		Data* name(std::string str){
			_name = str;
			return this;
		}

		std::string name(){
			return _name;
		}
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

	class Image_info:public Data{
		size_t width;
		size_t height;
		size_t row_bytes;
		size_t bytes_per_pixel;
	};

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

/////////////////////////////////////////////////

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
			bool cond2 = p.x+s.w >= p0.x;
			bool cond3 = p.y+s.h >= p0.y;
			return cond0 && cond1 && cond2 && cond3;
		}
	};

	class View{
		public:
		Data* data;
		virtual size_t min_h() { return 0; }
		virtual size_t max_h() { return 1000; }
		virtual size_t min_w() { return 0; }
		virtual size_t max_w() { return 0; }

		virtual void update(Core *c, Size s) const { return; }
		virtual void mouse_move(Core *c, Size s, Point p) const { return; }
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
	};

}
