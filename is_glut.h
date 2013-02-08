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

