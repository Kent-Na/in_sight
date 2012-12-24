namespace is{
	
	class Core{
		std::map<std::string,size_t> scale;
		//std::vector<Data*> data_list;
		Data_list data_list;
		Text_texture tt;
		//input container

		public:
		void add_data(Data *d){
			data_list.push_back(d);
		}
		Data_list& get_data_list(){
			return data_list;
		}
		void update(Size s);

		size_t get_scale(std::string name){
			if (scale.count(name))
				return scale[name];
			return 0;
		}
		void set_scale(std::string name, size_t value){
			scale[name] = value;
		}

		void run_GLUT(int argc, char** argv);
	};

	class Window{
		void setup_GL_option();
		void setup_matrix(Size s);
		Layouter *layouter; 
		public:
		Core *core;
		Window();
		//load all data from core and put those to window
		void reload();
		void update(Size s);
		void mouse(Size s, Point p);
		void mouse_move(Size s, Point p);
		void wheel_move(Size s, Point p, uint32_t dx, uint32_t dy);
		void key_down(Size s, uint8_t key);
	};
}

namespace is{
	namespace GLUT{

	//is::GLUT::Window
	class Window: public is::Window{
		public:
		Window(Core *c);
	};

	void init(int argc, char** argv);
	void run();
}}

