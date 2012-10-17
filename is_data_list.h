
namespace is{


	class Data_list:public View{
		public:
		
		void update(Core *c, Size s){
			const size_t text_size = 14;
			const size_t w = s.w;

			const char* str[] = {
				"data1",
				"data2",
				"data3 yjq",
				"data4",
				"jdata5",
				"data6",
			};

			Text_texture tex_gen;
			Size tex_size = {w, text_size};

			color::light_background();
			draw_rect(0, 0, w, s.h);

			std::vector<Data*> data_list = c->get_data_list();

			for (int i = 0; i<data_list.size(); i++){
				Data* d = data_list[i];
				GLuint tex  = tex_gen.generate(tex_size, d->name.c_str());
				if (i != 2){
					color::light_text();
					draw_texture(tex, 0, i*text_size, w, text_size);
				}
				else{
					color::light_hilight();
					draw_texture(0, 0, i*text_size, w, text_size);
					color::light_text();
					draw_texture(tex, 0, i*text_size, w, text_size);
				}
			}
		}

	};


}
