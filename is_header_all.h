#include <inttypes.h>
#include <stddef.h>


namespace is{
	class data;
	class node;

	class view;
	class frame;
	class layouter;
	class window;

	//S is root class.
	template <typename S, typename T>
	class class_object : class_object <S,S>{
		public:
		virtual S* get_instance(){ return new T;}
	};

	template <typename S>
	class class_object<S,S>{
		public:
		virtual S* get_instance(){ return new S;}
	};

	template <typename T = data>
	using data_class = class_object<data, T>;
	template <typename T = view>
	using view_class = class_object<view, T>;

	class data{
		public:
		//create and return new default view
		virtual view* default_view() { return NULL; }
	};

	class image_info:public data{
		size_t width;
		size_t height;
		size_t row_bytes;
		size_t bytes_per_pixel;
	};

	class port{
		node *holder;
		port *target;
		virtual data_class<>* type();
	};

	class node{
		uint32_t frag;
		enum node_frag{
			is_updated
		};

		virtual size_t input_count();
		virtual data_class<>* input_type_at(size_t idx);
		virtual void set_input(size_t idx, data data);

		virtual size_t output_count();
		virtual data_class<>* output_type_at(size_t idx);
		virtual void set_output(size_t idx, data data);
	};

	class filter:public node{
		virtual void apply();
	};

	class function_filter:public filter{
		void (*func)(data *in, data *out);
	};

/////////////////////////////////////////////////

	struct size{
		size_t w;
		size_t h;
	};

	class view{
		public:
		virtual size_t minWidth() { return 0; }
		virtual size_t maxWidth() { return 0; }
		virtual size_t minHeight() { return 0; }
		virtual size_t maxHeight() { return 0; }

		virtual void update(size &s, data *d) const { return; }

		//view never have output
		virtual size_t output_count() { return 0; }
		virtual data_class<>* output_type_at(size_t idx) { return NULL; }
		virtual void set_output(size_t idx, data data) { return; }
	};


	struct frame{
		size_t x;
		size_t y;
		size_t w;
		size_t h;
	};

	class layouter{
		virtual void layout();
	};

	class window{
		void update();
	};

	class core{
		std::vector<data*> data_list;
		//input container

		public:
		void add_data(data *d){
			data_list.push_back(d);
		}
		void update(size s);
		void run_GLUT(int argc, char** argv);
	};
}
