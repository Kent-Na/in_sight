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
