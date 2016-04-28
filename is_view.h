
namespace is{

	class View_list;

	class View{
		Rect _frame;// in Window coord. Set by auto layouter.
		Rect _contents_frame;// in Window coord
		Rect _header_frame;// in Window coord
		bool _is_transposed;
		bool _has_header;

		public:
		View();
		virtual ~View(){ return; }
		virtual View* clone() { return NULL; }

		//In window grobal coordinate.
		View* frame(Rect r);
		Rect frame() const;

		//In view coordinate.
		View* header_frame(Rect r);
		Rect header_frame() const;

		//In view coordinate.
		View* contents_frame(Rect r);
		Rect contents_frame() const;
		
		///
		//Auto layout hints
		virtual size_t min_contents_h() { return 0; }
		virtual size_t max_contents_h() { return 200; }
		virtual size_t min_contents_w() { return 0; }
		virtual size_t max_contents_w() { return 200; }

		size_t min_h();
		size_t max_h();
		size_t min_w();
		size_t max_w();

		size_t header_height() const;
	
		///
		//Dimentions
		virtual size_t contents_dimension() { return 0; };
		virtual size_t contents_width() {return 0; };
		virtual size_t contents_height() {return 0; };

		///
		//Contents
		void update(Core *c);
		virtual void update_contents(Core *c){ return; }
		virtual void update_header(Core *c){ return; }

		///
		//Events
		virtual View* view_at(Point p);
		virtual Mouse_event_tracker* 
			begin_mouse_event(Core *c, Event *e, uint8_t button_id);
		virtual void mouse_move(Core *c, Event* e); 
		virtual void 
			scroll(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz);
		virtual void 
			scroll_to(Core *c, Event *e, 
				      std::string name, int32_t location);
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
		View* name(const char* format, ...);
		std::string name() const;

		View* set_transpose() {_is_transposed = true; return this; };
		View* reset_transpose() {_is_transposed = false; return this; };
		bool is_transposed() {return _is_transposed; };

		View* no_header(){_has_header = false; return this; };

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
		void scroll(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz);
		void scroll_to(Core *c, Event *e,
				std::string name, int32_t location);
		void active_view_list();
		void add(View* v);

		void select_all();
		void deselect_all();
		void scroll_all
			(Core *c, Event* e,int32_t dx, int32_t dy, int32_t dz);
		void scroll_to_all
			(Core *c, Event* e, std::string name, int32_t location);

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
