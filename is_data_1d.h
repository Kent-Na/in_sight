namespace is{
	
	template <typename T = double>
	class Data_1d:public Data{
		std::vector<T> data;
		T _min_value;
		T _max_value;

		public:

		View* default_view();

		Data_1d(std::vector<T> d){
			data.insert(data.begin(), d.begin(), d.end());
			init_cache();
		}
		Data_1d(T* d, size_t len){
			data.insert(data.begin(), d, d+len);
			init_cache();
		}

		void init_cache(){
			_min_value = *std::min_element(data.begin(), data.end());
			_max_value = *std::max_element(data.begin(), data.end());
		}

		size_t size() const{
			return data.size();
		}
		double min_value() const{
			return _min_value;
		}
		double max_value() const{
			return _max_value;
		}
		double operator [] (size_t idx) const{
			return data[idx];
		}

		//Be careful. It will make system broken.
		std::vector<T>& raw_data(){
			return data;
		}
	};


	template <typename T = double>
	class View_1d_graph_base:public Data_view{
		size_t _grid_interval;

		void init(){
			_grid_interval = 0;
			_scale_name = "x";
			idx_start = 0;
		}

		protected:
		std::string _scale_name;
		size_t idx_start;
		Data_1d<T> *_data;

		public:

		View_1d_graph_base(Core *c, std::vector<T> d){
			_data = new Data_1d<T>(d);
			init();
			c->add(this);
		}

		View_1d_graph_base(Core *c, T* d, size_t len){
			_data = new Data_1d<T>(d, len);
			init();
			c->add(this);
		}

		///
		// seter and geter
		View_1d_graph_base* scale_name(std::string value){
			_scale_name = value;
			return this;
		}

		std::string scale_name() const{
			return _scale_name;
		}

		View_1d_graph_base* grid_interval(size_t value){
			_grid_interval = value;
			return this;
		}

		Data* data() const{
			return _data;
		}

		size_t grid_interval() const{
			return _grid_interval;
		}

		size_t min_h(){
			return 32;
		}

		size_t idx_end(Size s) const{
			return std::min(idx_start+s.w, _data->size());
		}

		size_t visible_count(Size s) const{
			return std::min(s.w, _data->size() - idx_start);
		}

		void update_grid(Size s) const{
			if (_grid_interval == 0)
				return;

			const size_t grid_start = 
				_grid_interval-idx_start%_grid_interval;

			color::grid();

			glBegin(GL_LINES);
			for (size_t i = grid_start; 
					i<visible_count(s); i+=_grid_interval){
				glVertex2d(i, 0);
				glVertex2d(i, s.h);
			}
			glEnd();
		}

		void update_invalid(Size s) const{
			color::invalid();
			draw_rect(visible_count(s), 0, 
					s.w-visible_count(s), s.h);
		}

		void update_header(Core *c, Size s) const{

			//const size_t header_size = 14;
			std::stringstream s_s;

			size_t forcused_idx = c->get_scale(_scale_name);
			std::string name = _data->name();
			if (name.empty()) name = View::name();

			s_s << name << "[" << _scale_name << ":"
				<< forcused_idx << "] = ";
			if (forcused_idx < _data->size()){
				const double value = (*_data)[forcused_idx];
				s_s << value;
			}
			else{
				s_s << "OOB";
			}
			
			s_s << " {" << idx_start << "-" << idx_end(s) << "}";

			std::string str = s_s.str();
			Text_texture *tex_gen = shared_text_texture();
			size_t tex_w = tex_gen->width(14, str.c_str())/4*4+4;
			Size tex_s = {tex_w,14};
			GLuint tex = tex_gen->generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, 0);

			update_seek_bar(c,s);
		}
		void update_seek_bar(Core *c, Size s) const{
			Size ss = {50,12};
			Point sp = {s.w - ss.w, 1};
			draw_seek_bar(sp, ss, _data->size(), idx_start, idx_end(s));
		}

		void mouse_move(Core* c, Event* e){
			Rect f = frame();
			if (not f.in_side(e->cursor()))
				return;

			Point p = cursor_in_view_coord(e);
			std::string s_name = _scale_name;
			c->set_scale(s_name, idx_start+p.x);
		}
		void scroll(Core *c, Event *e, int32_t dx, int32_t dy, int32_t dz){
			Rect f = frame();

			if ((int32_t)idx_start < -dx)
				idx_start = 0;
			else
				idx_start += dx;

			if (idx_start+f.s.w > _data->size()){
				if (_data->size()<f.s.w)
					idx_start = 0;
				else
					idx_start = _data->size()-f.s.w;
			}
			mouse_move(c, e);
		}
	};

	template<typename T = double>
	class View_1d_bar_graph:public View_1d_graph_base<T>{
		public:

		View_1d_bar_graph(Core *c, std::vector<T> d):
			View_1d_graph_base<T>(c, d) { return; }
		View_1d_bar_graph(Core *c, T* d, size_t len):
			View_1d_graph_base<T>(c, d, len) { return; }

		void update_data(Core *c,Size s) const{
			glTranslated(0.5,0,0);
			const size_t idx_end = this->idx_end(s);

			const double max_value = this->_data->max_value();
			const double min_value = this->_data->min_value();
			const double scale = s.h/(double)(max_value-min_value);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<this->visible_count(s); i++){
				const double value = (*this->_data)[i+this->idx_start];
				const double scaled = (value-min_value)*scale;
				glVertex2d(i, 0.0);
				glVertex2d(i, scaled);
			}
			glEnd();
		
			size_t forcused_idx = c->get_scale(this->_scale_name);
			if (forcused_idx >= this->idx_start && 
				forcused_idx <  idx_end){
				const double value = (*this->_data)[forcused_idx];
				const double scaled = (value-min_value)*scale;
				const double idx_start = this->idx_start;

				glBegin(GL_LINES);
				color::hilight();
				glVertex2d(forcused_idx-idx_start, 0.0);
				glVertex2d(forcused_idx-idx_start, scaled);

				color::hilight_background();
				glVertex2d(forcused_idx-idx_start, scaled);
				glVertex2d(forcused_idx-idx_start, s.h);
				glEnd();
			}
			glTranslated(-0.5,0,0);
		}
		
		void update(Core *c){
			Rect f = this->frame();
			const size_t header_size = 14;

			glPushMatrix();
			glTranslated(f.p.x,f.p.y,0);
			Size vs(f.s.w, f.s.h-header_size);
			this->update_grid(vs);
			this->update_invalid(vs);
			update_data(c,Size(f.s.w,f.s.h-header_size));
			glPopMatrix();

			glPushMatrix();
			glTranslated(f.p.x,f.p.y+f.s.h-header_size,0);
			this->update_header(c, f.s);
			glPopMatrix();
		}

	};

	template<typename T = double>
	class View_1d_label:public View_1d_graph_base<T>{
		public:

		View_1d_label(Core *c, std::vector<T> d):
			View_1d_graph_base<T>(c, d) { return; }
		View_1d_label(Core *c, T* d, size_t len):
			View_1d_graph_base<T>(c, d, len) { return; }

		size_t max_h(){
			return 14;
		}
		size_t min_h(){
			return 14;
		}

		void update(Core *c){
			Rect f = this->frame();
			const size_t header_size = 14;

			glPushMatrix();
			glTranslated(f.p.x,f.p.y,0);
			this->update_header(c, f.s);
			glPopMatrix();
		}
	};

	template <typename T = double>
	using View_1d = View_1d_bar_graph<T>;
}
