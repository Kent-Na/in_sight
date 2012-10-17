namespace is{
	class Data;
	class View_1d_bar_graph;

	class Data_1d:public Data{
		std::vector<double> data;

		public:

		View* default_view();

		Data_1d(Core *c, double* d, size_t len){
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

	class View_1d_graph_base:public View{

		public:

		size_t grid_interval;	
		size_t idx_start;

		View_1d_graph_base(){
			grid_interval = 30;
			idx_start = 0;
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
	};

	class View_1d_bar_graph:public View_1d_graph_base{
		
		void update(Size s) const{
			const size_t idx_end = this->idx_end(s);

			update_grid(s);
			update_invalid(s);

			const Data_1d* data = (Data_1d*)this->data;

			const double max_value = data->max_value();
			const double min_value = data->min_value();
			const double scale = s.h/(double)(max_value-min_value);

			color::value();

			glBegin(GL_LINES);
			for (size_t i = 0; i<visible_count(s); i++){
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

	View* Data_1d::default_view(){
		return new View_1d_bar_graph;
	}
}
