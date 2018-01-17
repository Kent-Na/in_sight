/*
 * "in_sight" is_glut.cpp
 * 
 * Copyright 2012-2018 Kent Nakajima<kent@tuna-cat.com>
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_event.h"
#include "is_view.h"
#include "is_core.h"
#include "is_glut.h"


namespace is{
namespace GLUT{

	std::map<int,Window*> window_map;

	void display(){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		window->update(s);
		glutSwapBuffers();
	}

	void mouse(int Button, int State, int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		Point p = {(size_t)x, (size_t)s.h-y};

		if (State == GLUT_DOWN)
			window->mouse_down(s, p, Button);
		else if (State == GLUT_UP)
			window->mouse_up(s, p, Button);
		else
			printf("OMG!!\n");

		window->update(s);
		glutSwapBuffers();

		/*
		const int amount_of_schroll = 20;
		if (Button == 3)//up
			window->wheel_move(s, p, 0, amount_of_schroll);
		if (Button == 4)//down
			window->wheel_move(s, p, 0, -amount_of_schroll);
		if (Button == 5)//left
			window->wheel_move(s, p, amount_of_schroll, 0);
		if (Button == 6)//right
			window->wheel_move(s, p, -amount_of_schroll, 0);
			*/
	}

	void motion(int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		Point p = {(size_t)x, (size_t)s.h-y};

		window->mouse_drag(s, p);
		window->update(s);
		glutSwapBuffers();
	}

    int f_passive_motion;
	void passive_motion(int x, int y){
        if (f_passive_motion){
            f_passive_motion = 0;
            return;
        }
        else{
            f_passive_motion = 1;
        }
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		Point p = {(size_t)x, (size_t)s.h-y};

		window->mouse_move(s, p);
		window->update(s);
		glutSwapBuffers();
	}

	void keyboard(unsigned char Key, int x, int y){
		int window_id = glutGetWindow();
		Window *window = window_map[window_id];

		Size s;
		s.w = glutGet(GLUT_WINDOW_WIDTH);
		s.h = glutGet(GLUT_WINDOW_HEIGHT);

		window->key_down(s, Key);
		window->update(s);
		glutSwapBuffers();
	}

	void init(int argc, char** argv){
		glutInit(&argc, argv);
	}
	void run(){
		glutMainLoop();
	}

	//It is GLUT window.
	Window::Window(Core *c):is::Window(c){
		int old_window_id = glutGetWindow();

		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutInitWindowSize(1800, 1000);
		glutCreateWindow("In side of my brain");

		int window_id = glutGetWindow();
		window_map[window_id] = this;

		core = c;

		glutDisplayFunc(is::GLUT::display);
		glutKeyboardFunc(is::GLUT::keyboard);
		glutMouseFunc(is::GLUT::mouse);
		glutMotionFunc(is::GLUT::motion);
		glutPassiveMotionFunc(is::GLUT::passive_motion);
		glutSwapBuffers();
		if (old_window_id != 0)
			glutSetWindow(old_window_id);
	}
}}

