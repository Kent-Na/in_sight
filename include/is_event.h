/*
 * "in_sight" is_event.h
 * 
 * Copyright 2012-2018 Kent Nakajima<kent@tuna-cat.com>
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
		Event* window(Window* w){ 
			_window = w;
			return this;
		};
		Window* window(){ return _window; };
	};

	class Event_responder{
		//renturned tracker are managed by Event class.
		virtual Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id);
		virtual void move_cursor(Core *c, Event *e);
		virtual void scroll(Core *c, Event *e,
				int32_t dx, int32_t dy, int32_t dz);
		virtual void scroll_to(Core *c, Event *e,
				std::string name, int32_t location);
		virtual void pless_key(Core *c, Event *e,
				uint8_t key_name);

	};
}
