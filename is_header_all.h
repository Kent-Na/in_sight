#include <inttypes.h>
#include <stddef.h>


namespace is{

	class Core;
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
		Point(){ }
		Point(size_t x0, size_t y0){
			x = x0;
			y = y0;
		}
	};

	struct Rect{
		Point p;
		Size s;

		Rect(){}
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

		bool in_side(Point p0) const{
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


	class Data{
		std::string _name;

		public:
		Data(){
			//_name = "void";
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
	};


	struct Data_1d_info{
		size_t size;
		size_t channel;
	};

	struct Image_info{
		size_t w;//width
		size_t h;//height
		size_t channel;//1 for grayscale, 3 for RGB...
		size_t bytes_per_row;
		size_t bytes_per_pixel;

		void init_as_grayscale(size_t w, size_t h){
			this->w = w;
			this->h = h;
			this->channel = 1;
			this->bytes_per_row = w;
			this->bytes_per_pixel = 1;
		}

		void init_as_rgb(size_t w, size_t h){
			this->w = w;
			this->h = h;
			this->channel = 3;
			this->bytes_per_row = w*3;
			this->bytes_per_pixel = 3;
		}

		void init_as_double_grayscale(size_t w, size_t h){
			this->w = w;
			this->h = h;
			this->channel = 1;
			this->bytes_per_row = w*sizeof(double);
			this->bytes_per_pixel = sizeof(double);
		}
		void init_as_double_rgb(size_t w, size_t h){
			this->w = w;
			this->h = h;
			this->channel = 3;
			this->bytes_per_row = w*sizeof(double)*3;
			this->bytes_per_pixel = sizeof(double)*3;
		}

		template <typename T> 
		static Image_info grayscale(size_t w, size_t h){
			Image_info out;
			out.w = w;
			out.h = h;
			out.channel = 1;
			out.bytes_per_row = w*sizeof(T);
			out.bytes_per_pixel = sizeof(T);
			return out;
		}

		template <typename T> 
		static Image_info rgb(size_t w, size_t h){
			Image_info out;
			out.w = w;
			out.h = h;
			out.channel = 3;
			out.bytes_per_row = 3*w*sizeof(T);
			out.bytes_per_pixel = 3*sizeof(T);
			return out;
		}

	};

	enum struct Value_map{
		//
		//val_max = max(data)
		//val_min = min(data)
		
		//map (val_min, val_max)
		min_to_max,

		//map (0, val_max)
		zero_to_max,
		
		//map (val_min, 0)
		min_to_zero,

		//map (-s,+s)
		//where s = max(abs(val_min),val_max)
		balanced,

		//map (a, b)
		//where a and b is user defined value.
		custom,

		//map (0, 255)
		direct,

		//For internal. Don't use this.
		invalid,
	};
}
