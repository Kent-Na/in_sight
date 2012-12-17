namespace is{
	class Data;
	class View_2d;

	void capture_image(uint8_t *in, Image_info *input_info,
					   uint8_t *out, Size size){
		uint8_t *itr = out;
		for (size_t y = 0; y<input_info->h; y++){
			const uint8_t *row_adr = in+y*input_info->bytes_per_row;
			for (size_t x = 0; x<input_info->w; x++){
				const uint8_t *pixel_adr = 
					row_adr+x*input_info->bytes_per_pixel;
				*(itr++) = *pixel_adr;
			}
		}
	}

	class Data_2d: public Data{
		uint8_t *data;
		Image_info info;
		std::string _scale_name_x;
		std::string _scale_name_y;

		public:
		GLuint tex;
		Data_2d(Core *c, uint8_t *d, Image_info *i){
			const size_t data_size = i->w*i->h;
			data = (uint8_t*)malloc(data_size);
			capture_image(d, i, data, Size(i->w, i->h));
			tex = 0;
			this->info = *i;
			c->add_data(this);
		}
		~Data_2d(){
			free(data);
			if (tex)
				glDeleteTextures(1, &tex);
		}

		View* default_view();
		void rebuild_texture(){
			if (tex)
				glDeleteTextures(1, &tex);
			tex = texture_from_grayscale(data, info.w, info.h);
		}

		Size image_size() const{
			return Size(info.w, info.h);
		}

		Data_2d* scale_name_x(std::string value){
			_scale_name_x = value;
			return this;
		}
		std::string scale_name_x() const{
			return _scale_name_x;
		}

		Data_2d* scale_name_y(std::string value){
			_scale_name_y = value;
			return this;
		}
		std::string scale_name_y() const{
			return _scale_name_y;
		}

	};

	class View_2d:public View{
		size_t min_h(){
			return 100;
		}
		size_t max_h(){
			return 720;
		}
		void update(Core *c, Size s) const{
			Data_2d *data = (Data_2d*)this->data;
			Size is = data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
			glColor4d(1,1,1,1);
			if (not data->tex)
				data->rebuild_texture();
			draw_texture(data->tex, 0, 0, vs.w, vs.h);
			update_header(c, s);
			//draw_texture(0, 0, 0, 100, 100);
		}
		void update_header(Core *c, Size s) const{
			const Data_2d* data = (Data_2d*)this->data;

			//const size_t header_size = 14;
			std::stringstream s_s;

			std::string s_name_x = data->scale_name_x();
			std::string s_name_y = data->scale_name_y();
			size_t forcused_idx_x = c->get_scale(s_name_x);
			size_t forcused_idx_y = c->get_scale(s_name_y);

			s_s << s_name_x << "," << s_name_y <<
				"[" << forcused_idx_x << "," <<
				 forcused_idx_y << "] = xxx";
			/*
			if (forcused_idx >= idx_start && forcused_idx < idx_end(s)){
				const double value = (*data)[forcused_idx];
				s_s << value;
			}
			*/

			std::string str = s_s.str();
			Text_texture tex_gen;
			Size tex_s = {128,14};
			GLuint tex = tex_gen.generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
		}
		void mouse_move(Core *c, Size s, Point p){
			Data_2d *data = (Data_2d*)this->data;
			std::string s_name_x = data->scale_name_x();
			std::string s_name_y = data->scale_name_y();
			c->set_scale(s_name_x, p.x);
			c->set_scale(s_name_y, p.y);
		}
	};

	inline View* Data_2d::default_view(){
		return new View_2d;
	}
}
