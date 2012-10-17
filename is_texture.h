
namespace is{

	GLuint texture_from_grayscale(uint8_t *img, size_t w, size_t h);
	void draw_texture(GLuint tex, double x, double y, double w, double h);

	class Text_texture{
		FT_Library lib;
		FT_Face face;
		public:
		Text_texture();
		~Text_texture();
		GLuint generate(Size &s, const char* text);
	};
}
