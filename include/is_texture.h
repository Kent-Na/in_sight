/*
 * "in_sight" is_texture.h
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

	GLuint texture_from_grayscale(uint8_t *img, size_t w, size_t h);
	GLuint texture_from_alpha(uint8_t *img, size_t w, size_t h);
	GLuint texture_from_rgb(uint8_t *img, size_t w, size_t h);
	GLuint texture_from_rgba(uint8_t *img, size_t w, size_t h);
	void draw_texture(GLuint tex, double x, double y, double w, double h);

#ifdef IS_USE_FREETYPE
	class Text_texture{
		FT_Library lib;
		FT_Face face;
		public:
		Text_texture();
		~Text_texture();
		size_t width(size_t h, const char* text);
		GLuint generate(Size &s, const char* text);
	};
#endif

#ifdef IS_USE_COCOA

    class Text_texture{
    public:
		Text_texture();
		~Text_texture();
		size_t width(size_t h, const char* text);
		GLuint generate(Size &s, const char* text);
    };
#endif

	Text_texture* shared_text_texture();
}
