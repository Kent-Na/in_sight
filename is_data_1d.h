namespace is{
	
	template <typename T = double>
	class Data_1d_cache:public Data{
		public:
		typedef T Value_type;

		private:
		std::vector<Value_type> data;
		//T _min_value;
		//T _max_value;

		public:

		Data_1d_cache(std::vector<T> d){
			data.insert(data.begin(), d.begin(), d.end());
			//init_cache();
		}
		Data_1d_cache(T* d, size_t len){
			data.insert(data.begin(), d, d+len);
			//init_cache();
		}

		//void init_cache(){
			//_min_value = *std::min_element(data.begin(), data.end());
			//_max_value = *std::max_element(data.begin(), data.end());
		//}

		size_t size() const{
			return data.size();
		}
		double min() const{
			//return _min_value;
			return  *std::min_element(data.begin(), data.end());
		}
		double max() const{
			//return _max_value;
			return *std::max_element(data.begin(), data.end());
		}
		double operator [] (size_t idx) const{
			return data[idx];
		}

		//Be careful. It will make system broken.
		std::vector<T>& raw_data(){
			return data;
		}
	};


	template <typename T>
	class View_1d_graph_base:public Data_view{
		public:
		typedef T Data_source;
		typedef typename Data_source::Value_type Value_type;

		private:
		size_t _grid_interval;

		void init(){
			_grid_interval = 0;
			_scale_name = "x";
			_transposed = false;
			idx_start = 0;
			value_map(Value_map::min_to_max, 0, 0);
		}

		protected:
		std::string _scale_name;
		int32_t idx_start;
		Data_source *_data;
		Value_type _map_min;
		Value_type _map_max;
		Value_map _value_map;
		bool _transposed;

		public:

		View_1d_graph_base(Core *c, std::vector<Value_type> d){
			_data = new Data_source(d);
			init();
			c->add(this);
		}

		View_1d_graph_base(Core *c, Value_type* d, size_t len){
			_data = new Data_source(d, len);
			init();
			c->add(this);
		}

		View_1d_graph_base(Core* c, Data_source* d){
			_data = d;
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

		Data* data() const{
			return _data;
		}

		size_t grid_interval() const{
			return _grid_interval;
		}

		View_1d_graph_base* grid_interval(size_t value){
			_grid_interval = value;
			return this;
		}
		View_1d_graph_base* transpose(){
			_transposed = true;
			return this;
		}

		View_1d_graph_base* value_map(
				Value_map method, Value_type min, Value_type max){
			_map_min = min;
			_map_max = max;
			_value_map = method;
			
			Value_type max_value = _data->max();
			Value_type min_value = _data->min();
			if (method == Value_map::invalid)
				method = Value_map::min_to_max;
			if (method == Value_map::direct)
				method = Value_map::min_to_max;

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
				Value_type min_abs = min_value;
				if (min_value < 0)
					min_abs = -min_abs;
				Value_type s = std::max<Value_type>(min_abs,max_value);
				_map_min = -s;
				_map_max = s;
			}
			return this;
		}

		size_t min_contents_h(){
			if (_transposed)
				return 0;
			else
				return 32;
		}
		size_t min_contents_w(){
			if (!_transposed)
				return 0;
			else
				return 32;
		}
		size_t max_contents_w(){
			if (_transposed)
				return 200;
			else
				return _data->size();
		}
		size_t max_contents_h(){
			if (!_transposed)
				return 200;
			else
				return _data->size();
		}
		/*
		size_t min_w(){
			return 32;
		}
		size_t max_h(){
			return _data->size();
		}
		*/

		size_t idx_end(Size s) const{
			return std::min(idx_start+s.w, _data->size());
		}

		size_t visible_count(Size s) const{
			return std::min(s.w, _data->size() - idx_start);
		}

		void update_contents(Core *c){
			Rect f = this->contents_frame();
			//const size_t header_size = 14;

			
			if (_transposed){
				glPushMatrix();
				glTranslated(f.p.x,f.p.y,0);
				double m[16] = {
					0.0, 1.0, 0.0, 0.0,
					1.0, 0.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0,
				};
				glMultMatrixd(m);

				Size vs(f.s.h, f.s.w);
				this->update_grid(vs);
				this->update_invalid(vs);
				update_data(c, vs);
				glPopMatrix();
			}
			else{
				glPushMatrix();
				glTranslated(f.p.x,f.p.y,0);
				Size vs = f.s;
					//vs(f.s.w, f.s.h-header_size);
				this->update_grid(vs);
				this->update_invalid(vs);
				update_data(c, vs);
				glPopMatrix();
			}

			//glPushMatrix();
			//glTranslated(f.p.x,f.p.y+f.s.h-header_size,0);
			//this->update_header(c, f.s);
			//glPopMatrix();

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

		virtual void update_data(Core *c,Size s) const{
			//Overlode to customize.
		}

		void update_invalid(Size s) const{
			color::invalid();
			draw_rect(visible_count(s), 0, 
					s.w-visible_count(s), s.h);
		}

		void update_header(Core *c){
			Rect f = this->header_frame();
			Size s = f.s;
			glPushMatrix();
			glTranslated(f.p.x,f.p.y,0);

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
			glPopMatrix();
		}
		void update_seek_bar(Core *c, Size s) const{
			Size ss = {50,12};
			if (!_transposed){
				Point sp = {s.w - ss.w, 1};
				draw_seek_bar(sp, ss, _data->size(), idx_start, idx_end(s));
			}
			else{
				Point sp = {s.h - ss.h, 1};
				draw_seek_bar(sp, ss, _data->size(), idx_start, idx_end(s));
			}
		}

		void mouse_move(Core* c, Event* e){
			Rect f = contents_frame();
			if (not f.in_side(e->cursor()))
				return;

			Point p = cursor_in_view_coord(e);
			std::string s_name = _scale_name;
			if (_transposed){
				c->set_scale(s_name, idx_start+p.y);
			}
			else{
				c->set_scale(s_name, idx_start+p.x);
			}
		}
		void scroll(Core *c, Event *e, int32_t dx, int32_t dy, int32_t dz){
			Rect f = contents_frame();
			int32_t delta = dx;
			int32_t view_length = f.s.w;
			if (_transposed){
				delta = dy;
				view_length = f.s.h;
			}

			if ((int32_t)idx_start < -delta)
				idx_start = 0;
			else
				idx_start += delta;

			if (idx_start+view_length > _data->size()){
				if (_data->size()<view_length)
					idx_start = 0;
				else
					idx_start = _data->size()-view_length;
			}
			mouse_move(c, e);
			e->window()->scroll_to(_scale_name, idx_start);
		}
		void scroll_to(Core *c, Event *e, std::string name, int32_t value){
			Rect f = contents_frame();
			int32_t view_length = f.s.w;
			if (_transposed){
				view_length = f.s.h;
			}

			if (name != _scale_name)
				return;
			if (value>=0)
				idx_start = value;
			if (idx_start+view_length > _data->size()){
				if (_data->size()<view_length)
					idx_start = 0;
				else
					idx_start = _data->size()-view_length;
			}
		}
	};

	template<typename T>
	class View_1d_bar_graph:public View_1d_graph_base<T>{
		public:
		typedef typename View_1d_graph_base<T>::Value_type Value_type;
		typedef typename View_1d_graph_base<T>::Data_source Data_source;

		View_1d_bar_graph(Core *c, std::vector<Value_type> d):
			View_1d_graph_base<T>(c, d) { return; }
		View_1d_bar_graph(Core *c, Value_type* d, size_t len):
			View_1d_graph_base<T>(c, d, len) { return; }
		View_1d_bar_graph(Core *c, Data_source* d):
			View_1d_graph_base<T>(c, d) { return; }

		void update_data(Core *c,Size s) const{
			glTranslated(0.5,0,0);
			const size_t idx_end = this->idx_end(s);

			//const double max_value = this->_data->max_value();
			//const double min_value = this->_data->min_value();
			const double scale = 
				s.h/(double)(this->_map_max-this->_map_min);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<this->visible_count(s); i++){
				const double value = (*this->_data)[i+this->idx_start];
				const double scaled = (value-this->_map_min)*scale;
				glVertex2d(i, 0.0);
				glVertex2d(i, scaled);
			}
			glEnd();
		
			size_t forcused_idx = c->get_scale(this->_scale_name);
			glBegin(GL_LINES);
			if (forcused_idx >= this->idx_start && 
				forcused_idx <  idx_end){
				const double value = (*this->_data)[forcused_idx];
				const double scaled = (value-this->_map_min)*scale;
				const double idx_start = this->idx_start;

				color::hilight();
				glVertex2d(forcused_idx-idx_start, 0.0);
				glVertex2d(forcused_idx-idx_start, scaled);

				color::hilight_background();
				glVertex2d(forcused_idx-idx_start, scaled);
				glVertex2d(forcused_idx-idx_start, s.h);
			}
			glEnd();
			glTranslated(-0.5,0,0);
		}
		

	};

	template<typename T>
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
			Rect f = this->contents_frame();
			const size_t header_size = 14;

			glPushMatrix();
			glTranslated(f.p.x,f.p.y,0);
			this->update_header(c, f.s);
			glPopMatrix();
		}
	};

	template <typename T = double>
	using View_1d = View_1d_bar_graph<Data_1d_cache<T>>;
}
