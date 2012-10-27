namespace is{
	class Data;
	class View_1d_bar_graph;

	class Data_1d:public Data{
		std::vector<double> data;
		std::string _scale_name;

		public:

		View* default_view();

		Data_1d(Core *c, double* d, size_t len){
			data.insert(data.begin(), d, d+len);
			c->add_data(this);
		}

		Data_1d* scale_name(std::string value){
			_scale_name = value;
			return this;
		}

		std::string scale_name() const{
			return _scale_name;
		}

		size_t size() const{
			return data.size();
		}
		double min_value() const{
			return *std::min_element(data.begin(), data.end());
		}
		double max_value() const{
			return *std::max_element(data.begin(), data.end());
		}
		double operator [] (size_t idx) const{
			return data[idx];
		}

		//Be careful. It will make system broken.
		std::vector<double>& raw_data(){
			return data;
		}
	};

	//in_place filter
	inline void filter_1d_MA_LPF(
			double *data, size_t length, size_t kernel_size){
		const size_t end = length-kernel_size;
		double s = 1.0/kernel_size;
		for (size_t i=0; i<end; i++){
			double sum = 0;
			for (size_t j = 0; j<kernel_size; j++){
				sum += data[i+j];
			}
			data[i]=sum*s;
		}
	}

	//in_place filter
	inline void filter_1d_MA_HPF(
			double *data, size_t length, size_t kernel_size){
		const size_t end = length-kernel_size;
		double s = 1.0/kernel_size;
		size_t half_kernel_size = kernel_size/2;
		for (size_t i=0; i<end; i++){
			double sum = 0;
			for (size_t j = 0; j<kernel_size; j++){
				sum += data[i+j];
			}
			data[i]=data[i+half_kernel_size]-sum*s;
		}
	}

	class View_1d_graph_base:public View{

		public:

		size_t grid_interval;	
		size_t idx_start;

		View_1d_graph_base(){
			grid_interval = 30;
			idx_start = 0;
		}

		size_t min_h(){
			return 32;
		}

		size_t idx_end(Size s) const{
			const Data_1d* data = (Data_1d*)this->data;
			return std::min(idx_start+s.w, data->size());
		}

		size_t visible_count(Size s) const{
			const Data_1d* data = (Data_1d*)this->data;
			return std::min(s.w, data->size() - idx_start);
		}

		void update_grid(Size s) const{
			if (grid_interval == 0)
				return;

			const size_t grid_start = idx_start%grid_interval;

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
			const Data_1d* data = (Data_1d*)this->data;

			const size_t header_size = 14;
			std::stringstream s_s;

			std::string s_name = data->scale_name();
			size_t forcused_idx = c->get_scale(s_name);

			s_s << s_name << "[" << forcused_idx << "] = ";
			if (forcused_idx >= idx_start && forcused_idx < idx_end(s)){
				const double value = (*data)[forcused_idx];
				s_s << value;
			}

			std::string str = s_s.str();
			Text_texture tex_gen;
			Size tex_s = {128,14};
			GLuint tex = tex_gen.generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
		}

		void mouse_move(Core *c,Size s, Point p) const{
			const Data_1d* data = (Data_1d*)this->data;
			std::string s_name = data->scale_name();
			c->set_scale(s_name, idx_start+p.x);
		}
	};

	class View_1d_bar_graph:public View_1d_graph_base{
		public:
		void update_data(Core *c,Size s) const{
			const size_t idx_end = this->idx_end(s);
			const Data_1d* data = (Data_1d*)this->data;

			const double max_value = data->max_value();
			const double min_value = data->min_value();
			const double scale = s.h/(double)(max_value-min_value);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<visible_count(s); i++){
				const double value = (*data)[i+idx_start];
				const double scaled = (value-min_value)*scale;
				glVertex2d(i+1, 0.0);
				glVertex2d(i+1, scaled);
			}
			glEnd();
		
			std::string s_name = data->scale_name();
			size_t forcused_idx = c->get_scale(s_name);
			if (forcused_idx >= idx_start && forcused_idx < idx_end){
				const double value = (*data)[forcused_idx];
				const double scaled = (value-min_value)*scale;

				glBegin(GL_LINES);
				color::hilight();
				glVertex2d(forcused_idx-idx_start, 0.0);
				glVertex2d(forcused_idx-idx_start, scaled);

				color::hilight_background();
				glVertex2d(forcused_idx-idx_start, scaled);
				glVertex2d(forcused_idx-idx_start, s.h);
				glEnd();
			}
		}
		
		void update(Core *c,Size s) const{
			Size vs(s.w, s.h-14);
			update_grid(vs);
			update_invalid(vs);
			update_data(c,Size(s.w,s.h-14));

			glPushMatrix();
			glTranslated(0,s.h-14,0);
			update_header(c, s);
			glPopMatrix();
		}

	};

	inline View* Data_1d::default_view(){
		return new View_1d_bar_graph;
	}

	class Filter_1d_MA_LPF:public Data{
		Data_1d *input;
		Data_1d *output;

		size_t kernel_size;
		public:

		View* default_view();

		void apply(){
			std::vector<double> &input_data = input->raw_data();
			std::vector<double> &output_data = output->raw_data();
			output_data.clear();
			output_data.insert(output_data.begin(), input_data.begin(), 
					input_data.end());
			filter_1d_MA_LPF(&output_data.front(), output_data.size(), 12);
		}
	};

	class View_filter_1d_MA_LPF:public View{

		void update(Core *c, Size s) const{
			const size_t header_size = 14;

			View_1d_bar_graph data_view;
			Size data_view_size = {s.w, s.h-header_size};
			data_view.update(c, data_view_size);
		}
	};

	class View_param_bar:public View{

		size_t min_h(){
			return 14;
		}

		size_t max_h(){
			return 14;
		}

		void update(Core *c, Size s) const{
			const size_t header_size = 14;
			std::stringstream s_s;
			s_s << "size = " << 100;
			std::string str = s_s.str();
			Text_texture tex_gen;
			Size tex_s = {128,14};
			GLuint tex = tex_gen.generate(tex_s, str.c_str());
			color::text();
			draw_texture(tex, 0, 0, tex_s.w, tex_s.h);
			glDeleteTextures(1, &tex);
		}
	};

	inline View* Filter_1d_MA_LPF::default_view(){
		return new View_filter_1d_MA_LPF;
	}

}
