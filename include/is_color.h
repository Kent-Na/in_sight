/*
 * "in_sight" is_color.h
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

namespace is{
	namespace color{
		inline void background(){
			double l = 0.12;
			glColor4d(l, l, l, 1.0);
		}

		inline void light_background(){
			double l = 0.57;
			//glColor4d(l, l, l*0.75, 1.0);
			glColor4d(l, l, l, 1.0);
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
