
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

		//
		//contents
		virtual void update(Core *c){ return; }

		///
		//Events
		virtual View* view_at(Point p);
		virtual Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id);
		virtual void mouse_move(Core *c, Event* e); 
		virtual void 
			scroll(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz);
		virtual void 
			key_down(Core *c, Event *e, uint8_t key_name);

		Point cursor_in_view_coord(Event *e) const;

		///
		//For view list
		private:
		bool _is_target;
		bool _is_temporaly_visible;
		bool _is_visible;
		bool _is_expanded;
		bool _is_marked;

		View* _next;
		View* _back;

		std::string _name;

		public:
		View* next_list_element() const;
		View* back_list_element() const;
		View* next_visible_element() const;

		void mark() { _is_marked = true; }
		void unmark() { _is_marked = false; }
		void set_visible() { _is_visible = true; }
		void reset_visible() { _is_visible = false; }
		void set_temporaly_visible() { _is_temporaly_visible = true; }
		void reset_temporaly_visible() { _is_temporaly_visible = false; }
		bool is_visible() const
			{ return _is_visible || _is_temporaly_visible; };
		bool is_marked() const
			{ return _is_marked; };

		View* name(std::string);
		std::string name() const;

		friend class View_list;
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

		uint8_t mark_type;
		//0:ignore
		//1:force visible
		//2:force invisible

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
		void scroll_all
			(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz);

		size_t list_slot_count();
		void recalculate_list_begin();
		size_t slot_at(Event *e);
		View* view_at_slot(size_t slot_idx);

		void set_tmp_visible(size_t begin, size_t end);
		void reset_tmp_visible(size_t begin, size_t end);
		void set_visible(size_t begin, size_t end);
		void reset_visible(size_t begin, size_t end);
		void mark(size_t begin, size_t end);
		void unmark(size_t begin, size_t end);
		bool is_visible(View* v);

		void mark_as_visible(){mark_type = 1;};
		void mark_as_invisible(){mark_type = 2;};

		void layout();
		void update(Core *c);

		void draw_view(Core *c);
		void draw_list();
		void draw(View* v, size_t slot_idx);
	};

	class Data_view: public View{
		public:
		virtual Data* data() const {return NULL;}
	};

}
