namespace is{
	class Event;

	class Mouse_event_tracker{
		uint8_t _button_id;

		public:
		virtual ~Mouse_event_tracker(){ };

		uint8_t button_id() const { return _button_id; }
		void button_id(uint8_t id){ _button_id = id; }

		virtual void mouse_down(Core *c, Event *e){};
		virtual void mouse_drag(Core *c, Event *e){};
		virtual void mouse_up(Core *c, Event *e){};
	};

	class Event{
		Window* _window;
		uint8_t _key_state[32];
		uint8_t _mouse_button_state;
		Point _cursor_location;


		public:
		bool key_state(uint8_t key_name);

		void cursor(Point p){ _cursor_location = p; }
		Point cursor() const{ return _cursor_location; }
	};

	class Event_responder{
		//renturned tracker are managed by Event class.
		virtual Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id);
		virtual void move_cursor(Core *c, Event *e);
		virtual void scroll(Core *c, Event *e,
				int32_t dx, int32_t dy, int32_t dz);
		virtual void pless_key(Core *c, Event *e,
				uint8_t key_name);

	};
}
