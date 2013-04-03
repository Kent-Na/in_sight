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

	class Data_2d: public Data{
		uint8_t *data;
		Image_info info;
		std::string _scale_name_x;
		std::string _scale_name_y;

    public:
		GLuint tex;
		Data_2d(Core *c, uint8_t *d, Image_info *i){
			const size_t data_size = i->w*i->h*i->channel;
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
			if (info.channel == 1)
				tex = texture_from_grayscale(data, info.w, info.h);
			else if (info.channel == 3)
				tex = texture_from_rgb(data, info.w, info.h);
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
        size_t idx_start_x;
        size_t idx_start_y;
    public:
        View_2d(){
            idx_start_x = 0;
            idx_start_y = 0;
        }
        size_t min_h(){
			return 100;
		}
		size_t max_h(){
            const size_t header_size = 14;
			Data_2d *data = (Data_2d*)this->data;
			Size is = data->image_size();
			return is.h+header_size;
		}

        void update_image(Core *c, Size s) const{
			Data_2d *data = (Data_2d*)this->data;
			Size is = data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
            
			glColor4d(1,1,1,1);
			if (not data->tex)
				data->rebuild_texture();

            float tx_min = idx_start_x/(float)is.w;
            float ty_min = idx_start_y/(float)is.h;
            float tx_max = (idx_start_x+vs.w)/(float)is.w;
            float ty_max = (idx_start_y+vs.h)/(float)is.h;

            glBindTexture(GL_TEXTURE_2D, data->tex);
            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(tx_min,ty_min);	glVertex2f(0,0);
            glTexCoord2f(tx_min,ty_max);	glVertex2f(0,vs.h);
            glTexCoord2f(tx_max,ty_max);	glVertex2f(vs.w,vs.h);
            glTexCoord2f(tx_max,ty_min);	glVertex2f(vs.w,0);
            glEnd();

			//draw_texture(data->tex, -(double)idx_start_x, -(double)idx_start_y, is.w, is.h);
			//draw_texture(0, 0, 0, 100, 100);
        }

        void update_v_grid(Core *c, Size s) const{

        }
		void update(Core *c, Size s) const{
            const size_t header_size = 14;
			Data_2d *data = (Data_2d*)this->data;
			Size is = data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
			glColor4d(1,1,1,1);

			//draw_texture(data->tex, -(double)idx_start_x, -(double)idx_start_y, is.w, is.h);

            update_image(c, Size(s.w, s.h-header_size));

			std::string s_name_x = data->scale_name_x();
			std::string s_name_y = data->scale_name_y();
			size_t forcused_idx_vx = c->get_scale(s_name_x)-idx_start_x;
			size_t forcused_idx_vy = c->get_scale(s_name_y)-idx_start_y;

            double g_delta = 8;

            glBindTexture(GL_TEXTURE_2D, 0);
			glColor4d(0.8,0.7,0.1,0.4);

            glBegin(GL_LINES);
			glVertex2d(forcused_idx_vx,0);
			glVertex2d(forcused_idx_vx,vs.h-header_size);
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

            
            glPushMatrix();
			glTranslated(0,s.h-header_size,0);
			update_header(c, s);
            update_seek_bar(c, Size(s.w, s.h-header_size));
			glPopMatrix();
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
				 forcused_idx_y << "] = xxx" <<
            idx_start_x << "," << idx_start_y;
			/*
			if (forcused_idx >= idx_start && forcused_idx < idx_end(s)){
				const double value = (*data)[forcused_idx];
				s_s << value;
			}
			*/

			std::string str = s_s.str();
			Text_texture tex_gen;
            size_t tex_w = tex_gen.width(14, str.c_str())/4*4+4;
			Size tex_s = {tex_w,14};
			GLuint tex = tex_gen.generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
		}
		void update_seek_bar(Core *c, Size s) const{
			const Data_2d *data = (Data_2d*)this->data;
			Size ss = {50,12};
			Size is = data->image_size();
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
		void mouse_move(Core *c, Size s, Point p){
			Data_2d *data = (Data_2d*)this->data;
			std::string s_name_x = data->scale_name_x();
			std::string s_name_y = data->scale_name_y();
			c->set_scale(s_name_x, idx_start_x+p.x);
			c->set_scale(s_name_y, idx_start_y+p.y);
		}
        void scroll(Size s, int32_t delta_x, int32_t delta_y){
			if ((int32_t)idx_start_x < -delta_x)
				idx_start_x = 0;
			else
				idx_start_x += delta_x;

			if ((int32_t)idx_start_y < -delta_y)
				idx_start_y = 0;
			else
				idx_start_y += delta_y;

			const Data_2d* data = (Data_2d*)this->data;
			if (idx_start_x+s.w > data->image_size().w)
				idx_start_x = data->image_size().w-s.w;
            if (idx_start_y+s.h > data->image_size().h)
                idx_start_y = data->image_size().h-s.h;

			if (data->image_size().w<s.w)
				idx_start_x = 0;
			if (data->image_size().h<s.h)
				idx_start_y = 0;

		}
		void wheel_move(Core *c, Size s, Point p,
						int32_t dx, int32_t dy){
			scroll(s, -dx, -dy);
		}
		void event(Core *c, Size s, Event_code code){
			if (code == scroll_x_plus){
				scroll(s, 100, 0);
			}
			if (code == scroll_x_minus){
				scroll(s, -100, 0);
			}
			if (code == scroll_y_plus){
				scroll(s, 0, 100);
			}
			if (code == scroll_y_minus){
				scroll(s, 0, -100);
			}
		}
	};

	inline View* Data_2d::default_view(){
		return new View_2d;
	}
}
