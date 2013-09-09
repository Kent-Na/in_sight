
namespace is{
	using namespace is;

	template <typename T = uint8_t>
	void capture_image(T *in, Image_info *input_info,
					   T *out){
		T *itr = out;
		for (size_t y = 0; y<input_info->h; y++){
			const uint8_t *row_adr = 
				(uint8_t*)in+y*input_info->bytes_per_row;
			for (size_t x = 0; x<input_info->w; x++){
				const T *pixel_adr = 
					(T*)(row_adr+x*input_info->bytes_per_pixel);
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

	template<typename T = uint8_t>
	class Data_2d: public Data{
		T* _data;
		Image_info _info;

    public:

		Data_2d(T *d, Image_info *i){
			const size_t data_size = i->w*i->h*i->channel*sizeof(*d);
			_data = (T*)malloc(data_size);
			capture_image(d, i, _data);
			this->_info = *i;
			_info.bytes_per_row   = _info.w*_info.channel*sizeof(T);
			_info.bytes_per_pixel = _info.channel*sizeof(T);
		}
		~Data_2d(){
			free(data);
		}

		Size image_size() const{
			return Size(_info.w, _info.h);
		}
		T* data() const{
			return _data;
		}
		T min() const{
			T min = *_data;
			uint8_t *in = (uint8_t*)_data;
			for (int y= 0; y<_info.h; y++){
				uint8_t *row_in = in+_info.bytes_per_row*y;
				for (int x= 0; x<_info.w; x++){
					uint8_t *pixel_in = 
						row_in+_info.bytes_per_pixel*x;
					for (int i = 0; i<_info.channel; i++){
						auto &v = ((T*)pixel_in)[i];
						if (v<min) min = v;
					}
				}
			}
			return min;
		}
		T max() const{
			T max = *_data;
			uint8_t *in = (uint8_t*)_data;
			for (int y= 0; y<_info.h; y++){
				uint8_t *row_in = in+_info.bytes_per_row*y;
				for (int x= 0; x<_info.w; x++){
					uint8_t *pixel_in = 
						row_in+_info.bytes_per_pixel*x;
					for (int i = 0; i<_info.channel; i++){
						auto &v = ((T*)pixel_in)[i];
						if (v>max) max = v;
					}
				}
			}
			return max;
		}
		T value_at(size_t x, size_t y, size_t ch){
			const auto &i = _info;
			uint8_t* ptr = (uint8_t*)_data;
			ptr += i.bytes_per_row    *y;
			ptr += i.bytes_per_pixel  *x;
			return ((T*)ptr)[ch];
		}
		const Image_info& info() const{
			return _info;
		}
	};

	class Data_2d_tile{
		GLuint texture;
		size_t w;
		size_t h;
		size_t ch;

		public:
		static constexpr size_t max_tile_size = 512;

		Data_2d_tile(){
			texture = 0;
		}
		~Data_2d_tile(){
			unload_texture();
		}
		void load_texture(uint8_t* img_data){
			if (texture)
				unload_texture();
			if (ch == 1)
				texture = texture_from_grayscale(img_data, w, h);
			else if (ch == 3)
				texture = texture_from_rgb(img_data, w, h);
		}
		void unload_texture(){
			if (texture)
				glDeleteTextures(1, &texture);
			texture = 0;
		}
		void draw(
				size_t src_x, size_t src_y, size_t src_w, size_t src_h,
				size_t dst_x, size_t dst_y, size_t dst_w, size_t dst_h){
            glBindTexture(GL_TEXTURE_2D, texture);
            glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2f  ((src_x      )/(float)w, (src_y      )/(float)h);	
			glVertex2f   (dst_x,       dst_y      );
            glTexCoord2f  ((src_x+src_w)/(float)w, (src_y      )/(float)h);	
			glVertex2f   (dst_x+dst_w, dst_y      );
            glTexCoord2f  ((src_x      )/(float)w, (src_y+src_h)/(float)h);	
			glVertex2f   (dst_x,       dst_y+dst_h);
            glTexCoord2f  ((src_x+src_w)/(float)w, (src_y+src_h)/(float)h);	
			glVertex2f   (dst_x+dst_w, dst_y+dst_h);
            glEnd();
		}

		bool inited(){
			return texture != 0;
		}

		template<typename T>
		void init_with(Data_2d<T> *data, size_t tx, size_t ty,
				Value_map c_map, T m_min, T m_max){
			const Image_info &i_info = data->info();
			const size_t t_size = Data_2d_tile::max_tile_size;
			size_t w  = std::min(i_info.w-tx*t_size, t_size);
			size_t h  = std::min(i_info.h-ty*t_size, t_size);
			size_t ch = i_info.channel;

			uint8_t *img_data = (uint8_t*)malloc(w*h*ch);
			this->w = w;
			this->h = h;
			this->ch = ch;

			uint8_t *in = (uint8_t*)data->data();
			uint8_t *out = img_data;
			uint8_t *out_itr = out;

			if (c_map == Value_map::direct){
				for (int y= 0; y<h; y++){
					uint8_t *row_in = in+i_info.bytes_per_row*(y+ty*t_size);
					for (int x= 0; x<w; x++){
						uint8_t *pixel_in = 
							row_in+i_info.bytes_per_pixel*(x+tx*t_size);
						for (int i = 0; i<ch; i++){
							*(out_itr++) = ((T*)pixel_in)[i];
						}
					}
				}
			}
			else{
				float s = 255.0f/(m_max-m_min);
				for (int y= 0; y<h; y++){
					uint8_t *row_in = in+i_info.bytes_per_row*(y+ty*t_size);
					for (int x= 0; x<w; x++){
						uint8_t *pixel_in = 
							row_in+i_info.bytes_per_pixel*(x+tx*t_size);
						for (int i = 0; i<ch; i++){
							auto v = ((T*)pixel_in)[i];
							*(out_itr++) = (v-m_min)*s;
						}
					}
				}
			}
			load_texture(img_data);
			free(img_data);
		}

	};

	struct Tile_2d_loc{
		size_t x;
		size_t y;
		
		Tile_2d_loc(){}
		Tile_2d_loc(size_t x, size_t y):x(x), y(y){}
	};

	inline bool operator < (const Tile_2d_loc &l,const Tile_2d_loc &r){
		if (l.x != r.x)
			return l.x<r.x;
		return l.y<r.y;
	}

	template<typename T = uint8_t>
	class View_2d:public View{
        int32_t idx_start_x;
        int32_t idx_start_y;
	protected:
		std::string _scale_name_x;
		std::string _scale_name_y;
		Data_2d<T>* _data;
		std::map<Tile_2d_loc, Data_2d_tile> tiles;

		Value_map _value_map;
		T _map_min;
		T _map_max;
    public:
		
		void init(){
            idx_start_x = 0;
            idx_start_y = 0;
			_scale_name_x = "x";
			_scale_name_y = "y";
			_value_map = Value_map::invalid;
		}

        View_2d(Core* c, T* d, Image_info *i){
			_data = new Data_2d<T>(d, i);
			init();
			c->add(this);
        }

		View_2d* scale_name(std::string x_value, std::string y_value){
			_scale_name_x = x_value;
			_scale_name_y = y_value;
			return this;
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

		View_2d* value_map(Value_map method, T min, T max){
			_map_min = min;
			_map_max = max;
			_value_map = method;
			
			T max_value = _data->max();
			T min_value = _data->min();

			if (method == Value_map::min_to_max){
				_map_min = min_value;
				_map_max = max_value;
			}
			if (method == Value_map::zero_to_max){
				_map_min = 0;
				_map_max = max_value;
			}
			if (method == Value_map::min_to_zero){
				_map_min = min_value;
				_map_max = 0;
			}
			if (method == Value_map::balanced){
				T s = std::max<T>(fabs(min_value),max_value);
				_map_min = -s;
				_map_max = s;
			}
			if (method == Value_map::direct){
				_map_min = 0;
				_map_max = 255;
			}
			return this;
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
        size_t min_w(){
			return 100;
		}
		size_t max_w(){
			Size is = _data->image_size();
			return is.w;
		}

        void update_image(Core *c, Size s){
			Size is = _data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
            
			glColor4d(1,1,1,1);

            const int ix_min = idx_start_x;
            const int iy_min = idx_start_y;
            const int ix_max = idx_start_x+vs.w;
            const int iy_max = idx_start_y+vs.h;

			const int t_size = Data_2d_tile::max_tile_size;

			const int tx_min = ix_min/t_size;
			const int tx_max = ix_max/t_size+1;
			const int ty_min = iy_min/t_size;
			const int ty_max = iy_max/t_size+1;

			for (int tx = tx_min; tx<tx_max; tx++){
				for (int ty = ty_min; ty<ty_max; ty++){
					Tile_2d_loc loc(tx, ty);
					auto &tile = tiles[loc];
					if (not tile.inited()){
						if (_value_map == Value_map::invalid){
							value_map(Value_map::direct,0,0);
						}
						tile.template init_with<T>(
								_data, tx, ty,
								_value_map, _map_min, _map_max);
					}
					using std::max;
					using std::min;
					const int lx_min = max(ix_min-tx*t_size, 0);
					const int lx_max = min(ix_max-tx*t_size, t_size);
					const int ly_min = max(iy_min-ty*t_size, 0);
					const int ly_max = min(iy_max-ty*t_size, t_size);

					const int w = lx_max-lx_min;
					const int h = ly_max-ly_min;
					tile.draw(
							lx_min, ly_min, w, h,
							lx_min+tx*t_size-ix_min, 
							ly_min+ty*t_size-iy_min, 
							w, h);
				}
			}
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
		}
		void update_header(Core *c, Size s) const{
			Size is = _data->image_size();
			Size vs(std::min(s.w,is.w),std::min(s.h,is.h));
			Size is_end(idx_start_x+vs.w, idx_start_y+vs.h);
			std::stringstream s_s;

			auto name = _data->name();
			if (name.empty()) name = View::name();
			const std::string &s_name_x = _scale_name_x;
			const std::string &s_name_y = _scale_name_y;
			size_t forcused_idx_x = c->get_scale(s_name_x);
			size_t forcused_idx_y = c->get_scale(s_name_y);

			s_s << name
				<< "[" << s_name_x << ":" << forcused_idx_x
				<< "," << s_name_y << ":" << forcused_idx_y
				<< "] = ";

			if (forcused_idx_x < is.w && forcused_idx_y < is.h){
				const double value = _data->value_at(
						forcused_idx_x, forcused_idx_y, 0);
				s_s << value;
			}
			else{
				s_s << "OOB";
			}

            s_s << "{" << idx_start_x << "-" << is_end.w 
				<< "," << idx_start_y << "-" << is_end.h
				<< "}";

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
			Rect f = frame();
			if (not f.in_side(e->cursor()))
				return;

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

			mouse_move(c, e);

			e->window()->scroll_to(_scale_name_x, idx_start_x);
			e->window()->scroll_to(_scale_name_y, idx_start_y);
		}
		void scroll_to(Core *c, Event *e, std::string name, int32_t value){
			Rect f = frame();
			Size is = _data->image_size();
			if (name == _scale_name_x){
				idx_start_x = value;
				if (idx_start_x < 0)
					idx_start_x = 0;
				if (idx_start_x+f.s.w > is.w){
					if (is.w<f.s.w)
						idx_start_x = 0;
					else
						idx_start_x = is.w-f.s.w;
				}
			}
			if (name == _scale_name_y){
				idx_start_y = value;
				if (idx_start_y < 0)
					idx_start_y = 0;
				if (idx_start_y+f.s.h > is.h){
					if (is.h<f.s.h)
						idx_start_y = 0;
					else
						idx_start_x = is.h-f.s.h;
				}
			}
		}
	};

	View_1d<double>* histgram_of(Core* c, uint8_t *d, Image_info *i){
		int histgram[256];
		for (int i=  0; i< 256; i++)
			histgram[i] = 0;

		for (size_t y = 0; y<i->h; y++){
			const uint8_t *row_adr = d+y*i->bytes_per_row;
			for (size_t x = 0; x<i->w; x++){
				const uint8_t *pixel_adr = 
					row_adr+x*i->bytes_per_pixel;
				for (size_t ch = 0; ch<i->channel; ch++){
					 histgram[pixel_adr[ch]]++;
				}
			}
		}

		double histgram_d[256];
		double sum = 0;
		for (int i=  0; i< 256; i++){
			sum += histgram[i];
			histgram_d[i] = sum;

			//histgram_d[i] = histgram[i];
		}
		return new is::View_1d<double>(c, histgram_d, 256);
	}

}
