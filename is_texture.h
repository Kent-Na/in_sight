
namespace is{

	GLuint texture_from_grayscale(uint8_t *img, size_t w, size_t h);

	class text_texture{
		FT_Library lib;
		FT_Face face;
		public:
		text_texture();
		~text_texture();
		GLuint generate(size &s, const char* text);
	};
}
