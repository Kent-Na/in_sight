#include <ft2build.h>
#include FT_FREETYPE_H

#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"

namespace is{

	GLuint texture_from_grayscale(uint8_t *img, size_t w, size_t h){
		GLuint tex = 0;	
		glGenTextures(1, &tex);

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0,
				GL_ALPHA, GL_UNSIGNED_BYTE, img);
		return tex;
	}

	void draw_texture(GLuint tex, double x, double y, double w, double h){
		glBindTexture(GL_TEXTURE_2D, tex);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,1);	glVertex2f(x+0,y+0);
		glTexCoord2f(0,0);	glVertex2f(x+0,y+h);
		glTexCoord2f(1,0);	glVertex2f(x+w,y+h);
		glTexCoord2f(1,1);	glVertex2f(x+w,y+0);
		glEnd();
	}

	Text_texture::Text_texture(){
		FT_Error error;
		error = FT_Init_FreeType(&lib);
		const char* font_name[] = {
			"/usr/share/fonts/TTF/DejaVuSans.ttf",
			"/System/Library/Fonts/Helvetica.dfont",
		};
		for (int i=0; i<2; i++){
			error = FT_New_Face(lib, font_name[i], 0, &face);
			if (error == 0)
				return;
		}

		printf("no font\n");
		exit(0);
	}

	Text_texture::~Text_texture(){
		FT_Done_Face(face);
		FT_Done_FreeType(lib);
	}

	size_t Text_texture::width(size_t h,const char* text){
		//init freetype
		FT_Error error;

		error = FT_Set_Pixel_Sizes(face, 0, h*0.9);

		const char* itr = text;
		size_t x_0 = 0;
		size_t y_0 = h*0.8;
		while (*itr){
			FT_UInt glyph_idx = FT_Get_Char_Index(face, *itr);
			error = FT_Load_Glyph(face, glyph_idx, FT_LOAD_DEFAULT);
		
			x_0 += face->glyph->advance.x/64;
			y_0 += face->glyph->advance.y/64;
			itr++;
		}
		return x_0;
	}
	GLuint Text_texture::generate(Size &s, const char* text){
		//init freetype
		FT_Error error;

		error = FT_Set_Pixel_Sizes(face, 0, s.h*0.9);

		uint8_t *out_bitmap = (uint8_t*)malloc(s.h*s.w);
		memset(out_bitmap,0,s.h*s.w);

		const char* itr = text;
		size_t x_0 = 0;
		size_t y_0 = s.h*0.8;
		while (*itr){
			FT_UInt glyph_idx = FT_Get_Char_Index(face, *itr);
			error = FT_Load_Glyph(face, glyph_idx, FT_LOAD_DEFAULT);
			error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			FT_Bitmap &bitmap = face->glyph->bitmap;
		
			const size_t x_t = x_0 + face->glyph->bitmap_left;
			const size_t y_t = y_0 - face->glyph->bitmap_top;
			if (not (x_t<s.w)) break;
			const size_t w = std::min(x_t+bitmap.width, s.w)-x_t;
			const size_t h = std::min(y_t+bitmap.rows, s.h)-y_t;
			for (size_t y=0; y<h; y++){
				uint8_t *in_row = bitmap.buffer+bitmap.width*y;
				uint8_t *out_row = out_bitmap+(y+y_t)*s.w + x_t;
				if (out_row<out_bitmap) continue;
				for (size_t x=0; x<w; x++){
					out_row[x] += in_row[x];
				}
			}
			x_0 += face->glyph->advance.x/64;
			y_0 += face->glyph->advance.y/64;
			itr++;
		}
		
		GLuint tex = texture_from_grayscale(out_bitmap, s.w, s.h);
		free(out_bitmap);
		return tex;
	}
}
