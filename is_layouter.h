
namespace is{

	class Layouter{
		public:
		virtual size_t layout(Rect r, std::vector<View*> vs);
	};

	class Virtical_layouter:public Layouter{
		size_t layout(Rect r, std::vector<View*> vs);
	};

	class Virtical_layouter_v1:public  Layouter{
		size_t layout(Rect r, std::vector<View*> vs);
	};

}
