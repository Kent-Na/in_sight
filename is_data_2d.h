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
				for (size_t ch = 0; ch<input_info->channel; ch++){
					*(itr++) = pixel_adr[ch];
				}
			}
		}
	}

	template<typename T_IN, typename T_OUT>
	void capture_image_ex(T_IN *in, Image_info *input_info,
					   T_OUT *out, Image_info *output_info){
		uint8_t *itr = out;
		for (size_t y = 0; y<input_info->h; y++){
			const uint8_t *row_adr = in+y*input_info->bytes_per_row;
			for (size_t x = 0; x<input_info->w; x++){
				const uint8_t *pixel_adr = 
					row_adr+x*input_info->bytes_per_pixel;
				for (size_t ch = 0; ch<input_info->channel; ch++){
					*(itr++) = pixel_adr[ch];
				}
			}
		}
	}

	class Data_2d_tile{
		uint8_t *img_data;
		GLuint texture;
		size_t w;
		size_t h;

		public:
		void load_texture();
		void unload_texture();
		void draw(
				size_t src_x, size_t src_y, size_t src_w, size_t src_h,
				size_t dst_x, size_t dst_y, size_t dst_w, size_t dst_h);
	};

	class Data_2d: public Data{
		uint8_t *data;
		Image_info info;

    public:
		GLuint tex;
		Data_2d(uint8_t *d, Image_info *i){
			const size_t data_size = i->w*i->h*i->channel;
			data = (uint8_t*)malloc(data_size);
			capture_image(d, i, data, Size(i->w, i->h));
			tex = 0;
			this->info = *i;
		}
		~Data_2d(){
			free(data);
			if (tex)
				glDeleteTextures(1, &tex);
		}

		void rebuild_texture(){
			if (tex)
				glDeleteTextures(1, &tex);
			if (info.channel == 1)
				tex = texture_from_grayscale(data, info.w, info.h);
			else if (info.channel == 3)
				tex = texture_from_rgb(data, info.w, info.h);
		}

		Size image_size() const{
			return Size(info.w, info.h);
		}


	};

	class View_2d:public View{
        size_t idx_start_x;
        size_t idx_start_y;
	protected:
		std::string _scale_name_x;
		std::string _scale_name_y;
		Data_2d* _data;
    public:
		
		void init(){
            idx_start_x = 0;
            idx_start_y = 0;
			_scale_name_x = "x";
			_scale_name_y = "y";
		}

        View_2d(Core* c, uint8_t *d, Image_info *i){
			_data = new Data_2d(d, i);
			init();
			c->add(this);
        }

		View_2d* scale_name_x(std::string value){
			_scale_name_x = value;
			return this;
		}
		std::string scale_name_x() const{
			return _scale_name_x;
		}

		View_2d* scale_name_y(std::string value){
			_scale_name_y = value;
			return this;
		}
		std::string scale_name_y() const{
			return _scale_name_y;
		}

		Data* data() const{
			return _data;
		}

        size_t min_h(){
			return 100;
		}
		size_t max_h(){
            const size_t header_size = 14;
			Size is = _data->image_size();
			return is.h+header_size;
		}

        void update_image(Core *c, Size s) const{
			Size is = _data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
            
			glColor4d(1,1,1,1);
			if (not _data->tex)
				_data->rebuild_texture();

            float tx_min = idx_start_x/(float)is.w;
            float ty_min = idx_start_y/(float)is.h;
            float tx_max = (idx_start_x+vs.w)/(float)is.w;
            float ty_max = (idx_start_y+vs.h)/(float)is.h;

            glBindTexture(GL_TEXTURE_2D, _data->tex);
            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(tx_min,ty_min);	glVertex2f(0,0);
            glTexCoord2f(tx_min,ty_max);	glVertex2f(0,vs.h);
            glTexCoord2f(tx_max,ty_max);	glVertex2f(vs.w,vs.h);
            glTexCoord2f(tx_max,ty_min);	glVertex2f(vs.w,0);
            glEnd();

			//draw_texture(data->tex, -(double)idx_start_x, -(double)idx_start_y, is.w, is.h);
			//draw_texture(0, 0, 0, 100, 100);
        }
		void update_forcus(Core *c, Size s){
            const size_t header_size = 14;

			Size is = _data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
			glColor4d(1,1,1,1);
			std::string &s_name_x = _scale_name_x;
			std::string &s_name_y = _scale_name_y;
			size_t forcused_idx_vx = c->get_scale(s_name_x)-idx_start_x;
			size_t forcused_idx_vy = c->get_scale(s_name_y)-idx_start_y;

            double g_delta = 8;

            glBindTexture(GL_TEXTURE_2D, 0);
			glColor4d(0.8,0.7,0.1,0.4);

            glBegin(GL_LINES);
			glVertex2d(forcused_idx_vx,0);
			glVertex2d(forcused_idx_vx,vs.h);
			glVertex2d(0,forcused_idx_vy);
			glVertex2d(vs.w,forcused_idx_vy);
            for (size_t i=1 ; i<=12; i++){
                if (forcused_idx_vx+g_delta*i<0 ||
                    forcused_idx_vx+g_delta*i>s.w-header_size)
                    continue;
                glVertex2d(forcused_idx_vx+g_delta*i,0);
                glVertex2d(forcused_idx_vx+g_delta*i,vs.h);
            }
			glEnd();

		}

		void update(Core *c){
            const size_t header_size = 14;
			Rect f = frame();

			//draw_texture(data->tex, -(double)idx_start_x, -(double)idx_start_y, is.w, is.h);

			glPushMatrix();
			glTranslated(f.p.x, f.p.y, 0);
            update_image(c, Size(f.s.w, f.s.h-header_size));
            update_forcus(c, Size(f.s.w, f.s.h-header_size));
			glPopMatrix();
            
            glPushMatrix();
			glTranslated(f.p.x,f.p.y+f.s.h-header_size,0);
			update_header(c, f.s);
            update_seek_bar(c, Size(f.s.w, f.s.h-header_size));
			glPopMatrix();
			//draw_texture(0, 0, 0, 100, 100);
		}
		void update_header(Core *c, Size s) const{

			//const size_t header_size = 14;
			std::stringstream s_s;

			const std::string &s_name_x = _scale_name_x;
			const std::string &s_name_y = _scale_name_y;
			size_t forcused_idx_x = c->get_scale(s_name_x);
			size_t forcused_idx_y = c->get_scale(s_name_y);

			s_s << s_name_x << "," << s_name_y <<
				"[" << forcused_idx_x << "," <<
				 forcused_idx_y << "] = xxx" <<
            idx_start_x << "," << idx_start_y;
			/*
			if (forcused_idx >= idx_start && forcused_idx < idx_end(s)){
				const double value = (*data)[forcused_idx];
				s_s << value;
			}
			*/

			std::string str = s_s.str();
			Text_texture *tex_gen = shared_text_texture();
            size_t tex_w = tex_gen->width(14, str.c_str())/4*4+4;
			Size tex_s = {tex_w,14};
			GLuint tex = tex_gen->generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
		}
		void update_seek_bar(Core *c, Size s) const{
			Size ss = {50,12};
			Size is = _data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
            glBindTexture(GL_TEXTURE_2D, 0);
			//x
			{
				Point sp = {s.w - ss.w-52, 1};
				draw_seek_bar(sp, ss, is.w, idx_start_x, idx_start_x+vs.w);
			}
			//y
			{
				Point sp = {s.w - ss.w, 1};
				draw_seek_bar(sp, ss, is.h, idx_start_y, idx_start_y+vs.h);
			}
		}
		void mouse_move(Core *c, Event* e){
			Point p = cursor_in_view_coord(e);
			std::string &s_name_x = _scale_name_x;
			std::string &s_name_y = _scale_name_y;
			c->set_scale(s_name_x, idx_start_x+p.x);
			c->set_scale(s_name_y, idx_start_y+p.y);
		}
        void scroll(Core* c, Event* e, int32_t dx, int32_t dy, int32_t dz){
			Rect f = frame();

			if ((int32_t)idx_start_x < -dx)
				idx_start_x = 0;
			else
				idx_start_x += dx;

			if ((int32_t)idx_start_y < -dy)
				idx_start_y = 0;
			else
				idx_start_y += dy;

			Size is = _data->image_size();

			if (idx_start_x+f.s.w > is.w)
				idx_start_x = is.w-f.s.w;
            if (idx_start_y+f.s.h > is.h)
                idx_start_y = is.h-f.s.h;

			if (is.w<f.s.w)
				idx_start_x = 0;
			if (is.h<f.s.h)
				idx_start_y = 0;

		}
	};

	//inline View* Data_2d::default_view(){
	//	return new View_2d;
	//}
}
