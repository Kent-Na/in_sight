#include "is_pch.h"

namespace is{
	void draw_rect(double x, double y, double w, double h){
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x+0,y+0);
		glVertex2f(x+0,y+h);
		glVertex2f(x+w,y+h);
		glVertex2f(x+w,y+0);
		glEnd();
	}
}
