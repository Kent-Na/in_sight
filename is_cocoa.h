
#ifdef __APPLE__

namespace is{
	namespace cocoa{

    class Window_internal;
	class Window: public is::Window{
        Window_internal* internal;
    public:
		Window(Core *c);
		~Window();
	};

	void init(int argc, char** argv);
	void run();
}}

#endif //__APPLE__
