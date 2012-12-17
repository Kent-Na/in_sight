
namespace is{
	class Layouter{
		protected:
		std::vector<View*> views;
		std::vector<Frame> frames;

		Frame* frame_at(Point p);

		public:
		virtual ~Layouter();	
		void add_view(View* view);

		//Remove all view from layouter whithout delete it.
		void detach_all_view();

		//Number of views
		size_t size() const;

		//View at index
		View* view(size_t index) const;

		virtual void layout(Size s);
		void update(Core *c,Size s);
		void mouse_move(Core *c, Size s, Point p);
		void wheel_move(Core *c, Size s, Point p, int32_t dx, int32_t dy);
	};

	class Virtical_layouter: public Layouter{
		public:
		void layout(Size s);
	};

	class Virtical_layouter_v1: public Layouter{
		public:
		void layout(Size s);
	};
}
