#include <inttypes.h>
#include <stddef.h>


namespace is{
	class Data;
	class Node;

	class View;
	class Frame;
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
		std::string name;
		Data(){
			name = "dummy";
		}
		//create and return new default view
		virtual View* default_view() { return NULL; }
	};

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
	};

	class View{
		public:
		Data* data;
		virtual size_t minWidth() { return 0; }
		virtual size_t maxWidth() { return 0; }
		virtual size_t minHeight() { return 0; }
		virtual size_t maxHeight() { return 0; }

		virtual void update(Size s) const { return; }

		//view never have output
		virtual size_t output_count() { return 0; }
		virtual Data_class<>* output_type_at(size_t idx) { return NULL; }
		virtual void set_output(size_t idx, Data data) { return; }
	};


	struct Frame{
		size_t x;
		size_t y;
		size_t w;
		size_t h;

		View* view;
		void update(){
			glPushMatrix();
			glTranslated(x,y,0);
			Size s = {w, h};
			view->update(s);
			glPopMatrix();
		}
	};

	class Layouter{
		protected:
		std::vector<View*> views;
		std::vector<Frame> frames;

		public:
		void add_view(View* view){
			views.push_back(view);
		}
		virtual void layout(Size s) { return; }
		void update(Size s){
			for (auto itr = frames.begin(); itr != frames.end(); itr++){
				itr->update();
			}
		}
	};

	class Virtical_layouter: public Layouter{
		public:
		void layout(Size s){
			double cell_h = s.h/(double)views.size();
			for (size_t i=0; i<views.size(); i++){
				Frame f;
				f.x = 0;
				f.w = s.w;
				f.y = ceil(cell_h*i);
				f.h = ceil(cell_h);
				f.view = views[i];
				frames.push_back(f);
			}
		}
	};

	class Window{
		void update();
	};

}
