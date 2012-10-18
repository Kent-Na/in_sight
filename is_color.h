namespace is{
	namespace color{
		inline void background(){
			double l = 0.12;
			glColor4d(l, l, l, 1.0);
		}

		inline void light_background(){
			double l = 0.57;
			glColor4d(l, l, l*0.75, 1.0);
		}

		inline void light_text(){
			double l = 0.0;
			glColor4d(l, l, l, 1.0);
		}

		inline void light_hilight(){
			glColor4d(0.77, 0.62 ,0.0, 1.0);
		}

		inline void text(){
			double l = 1.0;
			glColor4d(l, l, l, 1.0);
		}

		inline void grid(){
			double l = 1.0;
			glColor4d(l, l, l, 0.3);
		}

		inline void value(){
			double l = 1.0;
			glColor4d(l, l, l, 1.0);
		}

		inline void invalid(){
			double l = 0.3;
			glColor4d(l, l, l, 1.0);
		}

		inline void hilight(){
			glColor4d(0.95, 0.25, 0.05, 1.0);
		}

		inline void hilight_background(){
			double l = 0.22;
			glColor4d(l, l, l, 1.0);
		}
	}
}
