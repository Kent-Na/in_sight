/*
 * "in_sight" simple_2d_test.cpp
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

#include <math.h>
#include "in_sight.h"

int main(int argc, char** argv)
{
	is::GLUT::init(argc,argv);

	auto c = new is::Core;

	size_t w = 1173;
	size_t h = 720;
	uint8_t img[w*h];


	for (size_t y = 0; y<h; y++){
		for (size_t x = 0; x<w; x++){
			img[y*w+x] = (x/(double)w)*(y/(double)h)*255;
		}
	}

	is::Image_info info;
	info.w = w;
	info.h = h;
	info.channel = 1;
	info.bytes_per_pixel = 1;
	info.bytes_per_row = w;
	(new is::View_2d<uint8_t>(c, img, &info))
		->scale_name_x("x")
		->scale_name_y("y")
		->name("frame-1");

	new is::GLUT::Window(c);

	is::GLUT::run();

	return 0;
}
