namespace is{
	class Core{
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
		void run_GLUT(int argc, char** argv);
	};

	class Window{
		void setup_GL_option();
		void setup_matrix(Size s);
		public:
		Core *core;
		void update(Size s);
		void mouse(Size s, Point p);
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

