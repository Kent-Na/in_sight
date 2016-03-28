namespace is{
	

	class Core{
		std::map<std::string,size_t> scale;

		public:
		View_list* default_view;
		Window* main_window;

		Core();

		size_t get_scale(std::string name);
		void set_scale(std::string name, size_t value);

		void add(View *v);

		//opengl state related

		private:
		bool gl_state_initialized;
		GLuint shader_program_argb;
		GLuint shader_program_plain;

		public:
		void argb_mode();
		void plain_mode();
		void load_shader();
	};

	class Window{
		void setup_GL_option();
		void setup_matrix(Size s);
		View_list* root_view;
		Event* event;
		std::map<uint8_t, Mouse_event_tracker*> m_tracker;

		View* view_at(Point p);

		public:
		Core *core;
		Window(Core* c);
		virtual ~Window();

		void add(View *v);

		//raw UID events
		void update(Size s);
		void mouse_down(Size s, Point p, uint8_t button_id);
		void mouse_drag(Size s, Point p);
		void mouse_up(Size s, Point p, uint8_t button_id);
		void mouse_move(Size s, Point p);
		void wheel_move(Size s, Point p, uint32_t dx, uint32_t dy);
		void key_down(Size s, uint8_t key);
		void key_up(Size s, uint8_t key);

		//extra events
		void scroll_to(std::string name, int32_t location);
	};
}

