/*
 * "in_sight" is_core.h
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
#include "is_color.h"
#include "is_graphics.h"
#include "is_event.h"
#include "is_view.h"
#include "is_core.h"


namespace is{

	Core::Core(){
		default_view = new View_list;
		main_window = NULL;

		gl_state_initialized = false;
		shader_program_argb = 0;
		shader_program_plain = 0;
	}

	size_t Core::get_scale(std::string name){
		if (scale.count(name))
			return scale[name];
		return 0;
	}
	void Core::set_scale(std::string name, size_t value){
		scale[name] = value;
	}
	void Core::add(View *v){
		if (default_view){
			default_view->add(v);
		}
		else if (main_window){
			main_window->add(v);
		}
		else{
			printf("WTF\n");
		};
	}

	void Core::argb_mode(){
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(shader_program_argb);
	}
	void Core::plain_mode(){
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUseProgram(shader_program_plain);
		glUseProgram(0);
	}

	void Core::load_shader(){
		if (gl_state_initialized == true)
			return;
		gl_state_initialized = true;

		glCreateShader(GL_FRAGMENT_SHADER);
		const char* argb_fragment_code= 
"\
uniform sampler2D tex;\
\
void main()\
{\
gl_FragColor = gl_Color * texture2D(tex, gl_TexCoord[0].xy).bgra;\
}\
";

		const char* plain_fragment_code= 
"\
uniform sampler2D tex;\
\
void main()\
{\
gl_FragColor = gl_Color * texture2D(tex, gl_TexCoord[0].xy);\
}\
";
		const char* vertex_code = 
"\
void main()\
{\
	gl_FrontColor = gl_Color;\
	gl_TexCoord[0] = gl_MultiTexCoord0;\
	gl_Position = ftransform();\
}\
";
		auto shader_from_code = [](const char* code, GLuint type)->GLuint{
			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &code, NULL);
			glCompileShader(shader);
			return shader;
		};

		GLuint f_shader_argb = 
			shader_from_code(argb_fragment_code, GL_FRAGMENT_SHADER);
		GLuint f_shader_plain = 
			shader_from_code(plain_fragment_code, GL_FRAGMENT_SHADER);
		GLuint v_shader= 
			shader_from_code(vertex_code, GL_VERTEX_SHADER);

		shader_program_argb = glCreateProgram();
		glAttachShader(shader_program_argb, f_shader_argb);
		glAttachShader(shader_program_argb, v_shader);
		glLinkProgram(shader_program_argb);

		shader_program_plain = glCreateProgram();
		glAttachShader(shader_program_plain, f_shader_plain);
		glAttachShader(shader_program_plain, v_shader);
		glLinkProgram(shader_program_plain);

		//glUseProgram(shader_program_plain);
		glUniform1i(glGetUniformLocation(shader_program_plain,"tex"),0);
		glUniform1i(glGetUniformLocation(shader_program_argb,"tex"),0);

		glActiveTexture(GL_TEXTURE0);
	}

	Window::Window(Core* c):core(c){
		event = (new Event)->window(this);
		root_view = c->default_view;
		c->main_window = this;
		c->default_view = NULL;
	}

	Window::~Window(){
		delete event;
	}

	void Window::setup_matrix(Size s){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_VIEWPORT);
		glLoadIdentity();
		glOrtho(0, s.w, 0, s.h, -100, 100);
	}

	void Window::add(View *v){
		root_view->add(v);
	}

	void Window::setup_GL_option(){
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		core->load_shader();
	}


	void Window::update(Size s){

		setup_matrix(s);
		setup_GL_option();

		glBindTexture(GL_TEXTURE_2D,0);
		color::background();
		draw_rect(0,0,s.w,s.h);

		if (root_view){
			root_view->frame(Rect(0,0,s.w, s.h));
			root_view->update(core);
		}

	}

	View* Window::view_at(Point p){
		return root_view;
	}

	void Window::mouse_down(Size s, Point p, uint8_t button_id){
		event->cursor(p);
		if (m_tracker.count(button_id))
			printf("bad thing happen\n");

		View* target_view = view_at(p);
		Mouse_event_tracker* tracker = 
			target_view->begin_mouse_event(core, event, button_id);
		if (tracker){
			tracker->button_id(button_id);
			tracker->mouse_down(core, event);
			m_tracker[button_id] = tracker;
		}
	}

	void Window::mouse_drag(Size s, Point p){
		event->cursor(p);
		for (auto itr = m_tracker.begin();
				itr != m_tracker.end(); itr++){
			Mouse_event_tracker* tracker = itr->second;
			tracker->mouse_drag(core, event);
		}
	}

	void Window::mouse_up(Size s, Point p, uint8_t button_id){
		event->cursor(p);
		if (m_tracker.count(button_id)){
			Mouse_event_tracker* tracker = m_tracker[button_id];
			tracker->mouse_up(core, event);
			m_tracker.erase(button_id);
			delete tracker;
		}
	}

	void Window::mouse_move(Size s, Point p){
		event->cursor(p);
		View* target_view = view_at(p);
		if (target_view)
			target_view->mouse_move(core, event);

	}
	void Window::wheel_move(Size s, Point p, uint32_t dx, uint32_t dy){
		event->cursor(p);
		View* target_view = view_at(p);
		if (target_view)
			target_view->scroll(core, event, dx, dy, 0);
	}
	void Window::key_down(Size s, uint8_t key){
		if (key == 27){
			exit(0);
		}
		root_view->key_down(core, event, key);

		View* target_view = view_at(event->cursor());
		if (!target_view) return;
	
		int32_t amount = 64;

		if (key == 'n'){
			target_view->scroll(core, event, amount, 0, 0);
		}
		if (key == 'h'){
			target_view->scroll(core, event, -amount, 0, 0);
		}
		if (key == 'c'){
			target_view->scroll(core, event, 0, amount, 0);
		}
		if (key == 't'){
			target_view->scroll(core, event, 0, -amount, 0);
		}
	}
	void Window::key_up(Size s, uint8_t key){

	}

	////
	//extra events
	void Window::scroll_to(std::string name, int32_t location){
		root_view->scroll_to(core, event, name, location);
	}
}


