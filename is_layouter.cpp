#include "is_pch.h"

#include "is_header_all.h"
#include "is_layouter.h"

namespace is{

	void Layouter::add_view(View* view){
		views.push_back(view);
	}
	void Layouter::layout(Size s){
		return; 
	}
	void Layouter::update(Core *c, Size s){
		for (auto itr = frames.begin(); itr != frames.end(); itr++){
			itr->update(c);
		}
	}
	void Layouter::mouse_move(Core *c,Size s, Point p){
		for (auto itr = frames.begin(); itr != frames.end(); itr++){
			Rect r(itr->x, itr->y, itr->w, itr->h);
			if (r.in_side(p)){
				itr->mouse_move(c,p);
				return;
			}
		}
	}

	void Virtical_layouter::layout(Size s){
		double cell_h = s.h/(double)views.size();
		for (size_t i=0; i<views.size(); i++){
			Frame f;
			f.x = 0;
			f.w = s.w;
			f.y = ceil(cell_h*i);
			f.h = ceil(cell_h);
			f.view = views[i];
			frames.push_back(f);
		}
	}

	struct View_info{
		View* v;
		size_t idx;
		size_t h;
	} info;

	void Virtical_layouter_v1::layout(Size s){
		size_t sum_min_h = 0.0;
		size_t visible_count = 0;

		for (size_t i=0; i<views.size(); i++){
			size_t next_sum = sum_min_h + views[i]->min_h();
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
			info[i].v = views[i];
			info[i].h = views[i]->min_h();
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
			Frame f;
			f.x = 0;
			f.w = s.w;
			f.y = v_pos;
			f.h = info[i].h;
			f.view = info[i].v;
			v_pos += info[i].h;
			frames.push_back(f);
		}
	}
}
