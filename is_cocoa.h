

namespace is{
	namespace cocoa{

    class Window_internal;
	class Window: public is::Window{
        Window_internal* internal;
    public:
		Window(Core *c);
		~Window();
	};
}}
