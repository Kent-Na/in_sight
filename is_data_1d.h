namespace is{
	
	template <typename T = double>
	class Data_1d:public Data{
		std::vector<T> data;
		T _min_value;
		T _max_value;
		std::string _scale_name;
		size_t _grid_interval;

		public:

		View* default_view();

		Data_1d(Core *c, std::vector<T> d){
			data.insert(data.begin(), d.begin(), d.end());
			init_cache();
			c->add_data(this);
            _grid_interval = 0;
		}
		Data_1d(Core *c, T* d, size_t len){
			data.insert(data.begin(), d, d+len);
			init_cache();
			c->add_data(this);
            _grid_interval = 0;
		}

		void init_cache(){
			_min_value = *std::min_element(data.begin(), data.end());
			_max_value = *std::max_element(data.begin(), data.end());
		}

		Data_1d* scale_name(std::string value){
			_scale_name = value;
			return this;
		}

		std::string scale_name() const{
			return _scale_name;
		}

		Data_1d* grid_interval(size_t value){
			_grid_interval = value;
			return this;
		}
		size_t grid_interval() const{
			return _grid_interval;
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
	class View_1d_graph_base:public View{

		public:
		size_t idx_start;

		View_1d_graph_base(){
			idx_start = 0;
		}

		size_t min_h(){
			return 32;
		}

		size_t idx_end(Size s) const{
			const Data_1d<T>* data = (Data_1d<T>*)this->data;
			return std::min(idx_start+s.w, data->size());
		}

		size_t visible_count(Size s) const{
			const Data_1d<T>* data = (Data_1d<T>*)this->data;
			return std::min(s.w, data->size() - idx_start);
		}

		void update_grid(Size s) const{
            const Data_1d<T>* data = (Data_1d<T>*)this->data;
            auto grid_interval = data->grid_interval();
			if (grid_interval == 0)
				return;

			const size_t grid_start = 
				grid_interval-idx_start%grid_interval;

			color::grid();

			glBegin(GL_LINES);
			for (size_t i = grid_start; 
					i<visible_count(s); i+=grid_interval){
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
			const Data_1d<T>* data = (Data_1d<T>*)this->data;

			//const size_t header_size = 14;
			std::stringstream s_s;

			std::string s_name = data->scale_name();
			size_t forcused_idx = c->get_scale(s_name);

			s_s << data->name() << "[" << s_name << ":"
				<< forcused_idx << "] = ";
			if (forcused_idx < data->size()){
				const double value = (*data)[forcused_idx];
				s_s << value;
			}
			
			s_s << " [" << idx_start << ":" << idx_end(s) << "]";

			std::string str = s_s.str();
			Text_texture tex_gen;
			size_t tex_w = tex_gen.width(14, str.c_str())/4*4+4;
			Size tex_s = {tex_w,14};
			GLuint tex = tex_gen.generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, 0);

			update_seek_bar(c,s);
		}
		void update_seek_bar(Core *c, Size s) const{
			const Data_1d<T>* data = (Data_1d<T>*)this->data;

			Size ss = {50,12};
			Point sp = {s.w - ss.w, 1};
			draw_seek_bar(sp, ss, data->size(), idx_start, idx_end(s));
		}

		void mouse_move(Core *c,Size s, Point p){
			const Data_1d<T>* data = (Data_1d<T>*)this->data;
			std::string s_name = data->scale_name();
			c->set_scale(s_name, idx_start+p.x);
		}
		void scroll(Size s, int32_t delta){
			if ((int32_t)idx_start < -delta)
				idx_start = 0;
			else
				idx_start += delta;

			const Data_1d<T>* data = (Data_1d<T>*)this->data;
			if (idx_start+s.w > data->size()){
				if (data->size()<s.w)
					idx_start = 0;
				else
					idx_start = data->size()-s.w;
			}
		}
		void wheel_move(Core *c, Size s, Point p,
						int32_t dx, int32_t dy){
			scroll(s, -dx);
		}
		void event(Core *c, Size s, Event_code code){
			if (code == scroll_x_plus){
				scroll(s, 100);
			}
			if (code == scroll_x_minus){
				scroll(s, -100);
			}
		}
	};

	template<typename T = double>
	class View_1d_bar_graph:public View_1d_graph_base<T>{
		public:

		static Class_object<View, View_1d_bar_graph<T>> klass;

		void update_data(Core *c,Size s) const{
			glTranslated(0.5,0,0);
			const size_t idx_end = this->idx_end(s);
			const Data_1d<T>* data = (Data_1d<T>*)this->data;

			const double max_value = data->max_value();
			const double min_value = data->min_value();
			const double scale = s.h/(double)(max_value-min_value);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<this->visible_count(s); i++){
				const double value = (*data)[i+this->idx_start];
				const double scaled = (value-min_value)*scale;
				glVertex2d(i, 0.0);
				glVertex2d(i, scaled);
			}
			glEnd();
		
			std::string s_name = data->scale_name();
			size_t forcused_idx = c->get_scale(s_name);
			if (forcused_idx >= this->idx_start && 
				forcused_idx <  idx_end){
				const double value = (*data)[forcused_idx];
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
		
		void update(Core *c,Size s) const{
			Size vs(s.w, s.h-14);
			this->update_grid(vs);
			this->update_invalid(vs);
			update_data(c,Size(s.w,s.h-14));

			glPushMatrix();
			glTranslated(0,s.h-14,0);
			this->update_header(c, s);
			glPopMatrix();
		}

	};

	template<typename T = double>
	class View_1d_label:public View_1d_graph_base<T>{
		public:

		static Class_object<View, View_1d_label<T>> klass;

		size_t max_h(){
			return 14;
		}
		size_t min_h(){
			return 14;
		}
		void update(Core *c,Size s) const{
			this->update_header(c, s);
		}

	};

	template <typename T>
	inline View* Data_1d<T>::default_view(){
		if (view_class())
			return view_class()->new_instance();
		return new View_1d_bar_graph<T>;
	}
}
