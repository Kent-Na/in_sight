namespace is{
	class Core{
		std::vector<Data*> data_list;
		Text_texture tt;
		//input container

		public:
		void add_data(Data *d){
			data_list.push_back(d);
		}
		std::vector<Data*>& get_data_list(){
			return data_list;
		}
		void update(Size s);
		void run_GLUT(int argc, char** argv);
	};
}
