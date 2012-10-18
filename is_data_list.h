
namespace is{

	class Data_list_view:public View{
		const static size_t text_size = 14;
		public:
		
		void update(Data_list &data_list, Size s){
			const size_t w = s.w;

			Text_texture tex_gen;
			Size tex_size = {w, text_size};

			color::light_background();
			draw_rect(0, 0, w, s.h);

			//Data_list data_list = c->get_data_list();

			for (size_t i = 0; i<data_list.size(); i++){
				Data* d = data_list[i].data;
				uint32_t f = data_list[i].flag;
				GLuint tex  = tex_gen.generate(tex_size, d->name.c_str());
				if (f&visible){
					color::light_hilight();
					draw_texture(0, 0, i*text_size, w, text_size);
					color::light_text();
					draw_texture(tex, 0, i*text_size, w, text_size);
				}
				else{
					color::light_text();
					draw_texture(tex, 0, i*text_size, w, text_size);
				}
			}
		}
		void select(Data_list &data_list, Size s, Point p){
			size_t idx = p.y/text_size;
			
			if (idx >= 0 && idx<data_list.size()){
				data_list[idx].flag ^= visible;
			}
		}
	};


}
