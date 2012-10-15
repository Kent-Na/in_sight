namespace is{
	class data;
	class view_1d_bar_graph;

	class data_1d:public data{
		std::vector<double> data;

		public:

		view* default_view();

		data_1d(core *c, double* d, size_t len){
			data.insert(data.begin(), d, d+len);
			c->add_data(this);
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
	};

	class view_1d_graph_base:public view{

		public:

		size_t grid_interval;	
		size_t idx_start;

		view_1d_graph_base(){
			grid_interval = 30;
			idx_start = 0;
		}

		size_t idx_end(size s, data* d) const{
			data_1d* data = (data_1d*)d;
			return std::min(idx_start+s.w, data->size());
		}

		size_t visible_count(size s, data* d) const{
			data_1d* data = (data_1d*)d;
			return std::min(s.w, data->size() - idx_start);
		}

		void update_grid(size s, data* d) const{
			if (grid_interval == 0)
				return;

			const size_t grid_start = idx_start%grid_interval;

			color::grid();

			glBegin(GL_LINES);
			for (size_t i = grid_start; 
					i<visible_count(s, d); i+=grid_interval){
				glVertex2d(i, 0);
				glVertex2d(i, s.h);
			}
			glEnd();
		}
	};

	class view_1d_bar_graph:public view_1d_graph_base{
		
		void update(size &s, data* d) const{
			const size_t idx_end = this->idx_end(s, d);

			update_grid(s, d);

			const data_1d* data = (data_1d*)d;

			const double max_value = data->max_value();
			const double min_value = data->min_value();
			const double scale = s.h/(double)(max_value-min_value);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<visible_count(s, d); i++){
				const double value = (*data)[i+idx_start];
				const double scaled = (value-min_value)*scale;
				glVertex2d(i, 0.0);
				glVertex2d(i, scaled);
			}
			glEnd();
			
			size_t forcused_idx = 20;
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
	};

	view* data_1d::default_view(){
		return new view_1d_bar_graph;
	}
}
