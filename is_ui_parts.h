

namespace is{
	inline void draw_seek_bar(Point p, Size s, 
			size_t total, size_t begin, size_t end){

		size_t active_begin = begin/(double)total*s.w;
		size_t active_end = end/(double)total*s.w;
		if (active_begin == active_end)
			active_end++;

		color::grid();
		draw_rect(p.x, p.y, active_begin, s.h);
		draw_rect(p.x+active_end, p.y, s.w-active_end, s.h);
		color::hilight();
		draw_rect(p.x+active_begin, p.y, active_end-active_begin, s.h);
	}
}
