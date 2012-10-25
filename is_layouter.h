
namespace is{
	class Layouter{
		protected:
		std::vector<View*> views;
		std::vector<Frame> frames;

		public:
		void add_view(View* view);
		virtual void layout(Size s);
		void update(Core *c,Size s);
		void mouse_move(Core *c,Size s, Point p);
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
