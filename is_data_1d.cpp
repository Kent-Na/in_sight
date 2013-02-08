#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_core.h"
#include "is_color.h"
#include "is_graphics.h"
#include "is_data_1d.h"

namespace is{
	template <typename T>
	Class_object<View, View_1d_bar_graph<T>> 
		View_1d_bar_graph<T>::klass;
	template <typename T>
	Class_object<View, View_1d_label<T>> 
		View_1d_label<T>::klass;
	
	namespace klass_magic{
		auto magic_0 = &View_1d_bar_graph<double>::klass;
		auto magic_1 = &View_1d_label<double>::klass;
	}
}
