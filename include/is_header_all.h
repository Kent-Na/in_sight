/*
 * "in_sight" is_header_all.h
 * 
 * Copyright 2012-2018 Kent Nakajima<kent@tuna-cat.com>
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>
#include <stddef.h>


namespace is{
	constexpr size_t header_size = 14;

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

		template <typename T> 
		static Image_info rgba(size_t w, size_t h){
			Image_info out;
			out.w = w;
			out.h = h;
			out.channel = 4;
			out.bytes_per_row = 4*w*sizeof(T);
			out.bytes_per_pixel = 4*sizeof(T);
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

	inline float abs(float x){
		return fabs(x);
	}
	inline double abs(double x){
		return ::abs(x);
	}
	inline uint8_t abs(uint8_t x){
		return x;
	}
	inline uint8_t abs(uint16_t x){
		return x;
	}
	inline size_t abs(size_t x){
		return x;
	}
}
