/*
 * "in_sight" is_histgram.h
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
	inline View_1d<int>* histgram_of(Core* c, uint8_t *d, Image_info *i){
		int histgram[256];
		for (int i=  0; i< 256; i++)
			histgram[i] = 0;

		for (size_t y = 0; y<i->h; y++){
			const uint8_t *row_adr = d+y*i->bytes_per_row;
			for (size_t x = 0; x<i->w; x++){
				const uint8_t *pixel_adr = 
					row_adr+x*i->bytes_per_pixel;
				for (size_t ch = 0; ch<i->channel; ch++){
					 histgram[pixel_adr[ch]]++;
				}
			}
		}
/*
		double histgram_d[256];
		double sum = 0;
		for (int i=  0; i< 256; i++){
			sum += histgram[i];
			histgram_d[i] = sum;

			//histgram_d[i] = histgram[i];
		}
		return new is::View_1d<double>(c, histgram_d, 256);
		*/

		return new is::View_1d<int>(c, histgram, 256);
	}
}
