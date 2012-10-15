namespace is{
	namespace color{
		void background(){
			double l = 0.12;
			glColor4d(l, l, l, 1.0);
		}

		void grid(){
			double l = 1.0;
			glColor4d(l, l, l, 0.3);
		}

		void value(){
			double l = 1.0;
			glColor4d(l, l, l, 1.0);
		}

		void hilight(){
			glColor4d(0.95, 0.25, 0.05, 1.0);
		}

		void hilight_background(){
			double l = 0.22;
			glColor4d(l, l, l, 1.0);
		}
	}
}
