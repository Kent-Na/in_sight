#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_event.h"
//#include "is_color.h"
//#include "is_graphics.h"
#include "is_texture.h"
#include "is_view.h"
#include "is_layouter.h"

namespace is{
	size_t Layouter::layout(Rect r, std::vector<View*> vs){
		return 0;
	}
	size_t Virtical_layouter::layout(Rect r, std::vector<View*> vs){
		double cell_h = r.s.h/(double)vs.size();
		for (size_t i=0; i<vs.size(); i++){
			View *v = vs[i];
			Rect f;
			f.p.x = r.p.x;
			f.s.w = r.s.w;
			f.p.y = r.p.y+ceil(cell_h*i);
			f.s.h = ceil(cell_h*(i+1)) - ceil(cell_h*i);
			v->frame(f);
		}
		return vs.size();
	}

	size_t Virtical_layouter_v1::layout(Rect r, std::vector<View*> vs){
		Size s = r.s;
		size_t sum_min_h = 0.0;
		size_t visible_count = 0;

		for (size_t i=0; i<vs.size(); i++){
			size_t next_sum = sum_min_h + vs[i]->min_h();
			if (next_sum>s.h)
				break;
			sum_min_h = next_sum;
			visible_count = i+1;
		}

		struct View_info{
			View* v;
			size_t idx;
			size_t h;
			bool maxed;
		} info[visible_count];

		size_t remain = s.h;

		for (size_t i=0; i<visible_count; i++){
			info[i].idx = i;
			info[i].v = vs[i];
			info[i].h = vs[i]->min_h();
			info[i].maxed = false;
			remain -= info[i].h;
		}
			
		auto comp_info_max = [](View_info l, View_info r)->bool{
			return l.v->max_h()<r.v->max_h();
		};

		std::sort(info, info+visible_count, comp_info_max);

		for (size_t i=0; i<visible_count; i++){
			size_t uncalculated_count = visible_count-i; 
			double remain_par_view = remain/(double)uncalculated_count;
			
			size_t new_h = info[i].h + remain_par_view;
			size_t max_h = info[i].v->max_h();
			if (max_h<new_h){
				remain -= max_h-info[i].h;
				info[i].h = max_h;
			}
			else{
				remain -= new_h-info[i].h;
				info[i].h = new_h;
			}
		}

		auto comp_info_idx = [](View_info l, View_info r)->bool{
			return l.idx<r.idx;
		};
		
		std::sort(info, info+visible_count, comp_info_idx);
		size_t v_pos = 0;
		for (size_t i=0; i<visible_count; i++){
			Rect f;
			f.p.x = r.p.x;
			f.s.w = s.w;
			f.p.y = r.p.y+v_pos;
			f.s.h = info[i].h;
			info[i].v->frame(f);
			v_pos += info[i].h;
		}
		return visible_count;
	}

	size_t Horizontial_layouter_v1::layout(Rect r, std::vector<View*> vs){
		Size s = r.s;
		size_t sum_min_w = 0.0;
		size_t visible_count = 0;

		for (size_t i=0; i<vs.size(); i++){
			size_t next_sum = sum_min_w + vs[i]->min_w();
			if (next_sum>s.w)
				break;
			sum_min_w = next_sum;
			visible_count = i+1;
		}

		struct View_info{
			View* v;
			size_t idx;
			size_t w;
			bool maxed;
		} info[visible_count];

		size_t remain = s.w;

		for (size_t i=0; i<visible_count; i++){
			info[i].idx = i;
			info[i].v = vs[i];
			info[i].w = vs[i]->min_w();
			info[i].maxed = false;
			remain -= info[i].w;
		}
			
		auto comp_info_max = [](View_info l, View_info r)->bool{
			return l.v->max_w()<r.v->max_w();
		};

		std::sort(info, info+visible_count, comp_info_max);

		for (size_t i=0; i<visible_count; i++){
			size_t uncalculated_count = visible_count-i; 
			double remain_par_view = remain/(double)uncalculated_count;
			
			size_t new_w = info[i].w + remain_par_view;
			size_t max_w = info[i].v->max_w();
			if (max_w<new_w){
				remain -= max_w-info[i].w;
				info[i].w = max_w;
			}
			else{
				remain -= new_w-info[i].w;
				info[i].w = new_w;
			}
		}

		auto comp_info_idx = [](View_info l, View_info r)->bool{
			return l.idx<r.idx;
		};
		
		std::sort(info, info+visible_count, comp_info_idx);
		size_t v_pos = 0;
		for (size_t i=0; i<visible_count; i++){
			Rect f;
			f.p.y = r.p.y;
			f.s.h = s.h;
			f.p.x = r.p.x+v_pos;
			f.s.w = info[i].w;
			info[i].v->frame(f);
			v_pos += info[i].w;
		}
		return visible_count;
	}
}
