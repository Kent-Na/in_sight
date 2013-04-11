
namespace is{

	class View_list;

	class View{
		Rect _frame;// in Window coord

		public:
		View();
		virtual ~View(){ return; }
		virtual View* clone() { return NULL; }

		View* frame(Rect r);
		Rect frame() const;
		
		///
		//Auto layout hints
		virtual size_t min_h() { return 0; }
		virtual size_t max_h() { return 200; }
		virtual size_t min_w() { return 0; }
		virtual size_t max_w() { return 200; }
	
		///
		//Dimentions
		virtual size_t contents_dimension() { return 0; };
		virtual size_t contents_width() {return 0; };
		virtual size_t contents_height() {return 0; };

		///
		//Events
		virtual View* view_at(Point p){ return this; }
		virtual void update(Core *c){ return; }
		virtual Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id)
				{ return NULL; }
		virtual void mouse_move(Core *c, Event* e) { return; }
		virtual void 
			scroll(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz) 
				{ return; }
		virtual void 
			key_down(Core *c, Event *e, uint8_t key_name) { return; }

		Point cursor_in_view_coord(Event *e) const;

		///
		//For view list
		private:
		bool _is_target;
		bool _is_temporaly_visible;
		bool _is_visible;
		bool _is_expanded;

		View* _next;
		View* _back;

		std::string _name;

		public:
		View* next_list_element() const;
		View* back_list_element() const;
		View* next_visible_element() const;

		bool is_visible() const{ return _is_visible; };

		View* name(std::string);
		std::string name() const;

		friend View_list;
	};

	class Red_view:public View{
		virtual void update(Core *c);
	};
	class Blue_view:public View{
		virtual void update(Core *c);
	};

	class View_list:public View{
		View* first_view;
		View* last_view;

		View* list_begin;
		Core *core;
		Layouter *layouter;

		//visual parms
		size_t list_width;
		size_t slot_height;

		size_t active_view_count;

		public: 
		View_list();

		virtual View* view_at(Point p);
		Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id);
		void key_down(Core *c, Event *e, uint8_t key_name);
		void active_view_list();
		void add(View* v);

		void select_all();
		void deselect_all();

		size_t list_slot_count();
		void recalculate_list_begin();
		size_t slot_at(Event *e);
		View* view_at_slot(size_t slot_idx);

		void layout();
		void update(Core *c);

		void draw_view(Core *c);
		void draw_list();
		void draw(View* v, size_t slot_idx);
	};

	class View_deselect_tracker:public Mouse_event_tracker{

	};

	class View_select_tracker:public Mouse_event_tracker{

	};

	class Data_view: public View{
		public:
		virtual Data* data() const {return NULL;}
	};

}
