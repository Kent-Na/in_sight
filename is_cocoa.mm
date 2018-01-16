/*
 * "in_sight" is_cocoa.mm
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

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"
#include "is_event.h"
#include "is_view.h"
#include "is_core.h"
#include "is_cocoa.h"

@interface IsCocoaOpenGLView : NSOpenGLView
@property(assign) is::cocoa::Window* is_window;
@end

@implementation IsCocoaOpenGLView
- (BOOL)acceptsFirstResponder{
    return YES;
}
-(void)viewDidMoveToWindow {
	[[self window] setAcceptsMouseMovedEvents:YES];
}

- (void)prepareOpenGL{

}
- (void)reshape{
	NSSize size=[self frame].size;
	[[self openGLContext] makeCurrentContext];
    is::Size s;
    s.w = size.width;
    s.h = size.height;
    glViewport(0,0,s.w,s.h);
    [self is_window]->update(s);
     
	glFlush();
	[[self openGLContext] flushBuffer];
}
- (void)update{
	[super update];
	[[self openGLContext] makeCurrentContext];
    
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
    [self is_window]->update(s);
    
	glFlush();
	[[self openGLContext] flushBuffer];
}


//mouse event
- (void)mouseDown:(NSEvent *)theEvent{
	[[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    is::Point p = {(size_t)pp.x, (size_t)pp.y};
    [self is_window]->mouse_down(s, p, [theEvent buttonNumber]);
    [self is_window]->update(s);
	glFlush();
	[[self openGLContext] flushBuffer];
}

- (void)mouseUp:(NSEvent *)theEvent{
	[[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    is::Point p = {(size_t)pp.x, (size_t)pp.y};
    [self is_window]->mouse_up(s, p, [theEvent buttonNumber]);
    [self is_window]->update(s);
	glFlush();
	[[self openGLContext] flushBuffer];
}

- (void)mouseMoved:(NSEvent *)theEvent{
	[[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    is::Point p = {(size_t)pp.x, (size_t)pp.y};
    [self is_window]->mouse_move(s, p);
    [self is_window]->update(s);
	glFlush();
	[[self openGLContext] flushBuffer];
}

- (void)mouseDragged:(NSEvent *)theEvent{
	[[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    is::Point p = {(size_t)pp.x, (size_t)pp.y};
    [self is_window]->mouse_drag(s, p);
    [self is_window]->update(s);
	glFlush();
	[[self openGLContext] flushBuffer];
}

- (void)scrollWheel:(NSEvent *)theEvent{
	[[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    is::Point p = {(size_t)pp.x, (size_t)pp.y};
    const double scale = 10.0;
    [self is_window]->wheel_move(s, p,
                                 -[theEvent deltaX]*scale,
                                 [theEvent deltaY]*scale);
    [self is_window]->update(s);
    //system->triggerMouseDown([theEvent buttonNumber]);
	glFlush();
	[[self openGLContext] flushBuffer];
}

//keybord event
- (void)keyDown:(NSEvent *)theEvent{
    [[self openGLContext] makeCurrentContext];
	NSSize size=[self frame].size;
    is::Size s;
    s.w = size.width;
    s.h = size.height;
	NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
    [self is_window]->key_down(s,[[theEvent characters]characterAtIndex:0]);
    [self is_window]->update(s);
	glFlush();
	[[self openGLContext] flushBuffer];
	//if (system)
	//	system->triggerKeyDown([theEvent keyCode]);
}

- (void)keyUp:(NSEvent *)theEvent{
	//if (system)
	//	system->triggerKeyUp([theEvent keyCode]);
}


@end

@interface IsCocoaApplicationDelegate :NSObject
@property(assign) is::cocoa::Window* is_window;
@end

@implementation IsCocoaApplicationDelegate 
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:
		(NSApplication *)theApplication{
	return YES;
}
@end

namespace is{
    namespace cocoa{

	class Window_internal{
		NSWindow* window;
		IsCocoaOpenGLView* view;
	public:
		Window_internal(Window *w);
	};

	Window_internal::Window_internal(Window *w){
		NSRect frame = NSMakeRect(0,0,1800,1100);
		window = [[NSWindow alloc] initWithContentRect:frame
											 styleMask:
				  NSTitledWindowMask|
				  NSClosableWindowMask|
				  NSMiniaturizableWindowMask|
				  NSResizableWindowMask
											   backing:
				  NSBackingStoreBuffered
												 defer:true];
		NSOpenGLPixelFormatAttribute attribute[]={
			NSOpenGLPFADoubleBuffer,
			//NSOpenGLPFABackingStore,
			NSOpenGLPFAWindow,
			NSOpenGLPFADepthSize, 16,
			0
		};

		NSOpenGLPixelFormat *format =
			[[[NSOpenGLPixelFormat alloc]initWithAttributes:attribute]autorelease];

		view = [[IsCocoaOpenGLView alloc] initWithFrame:frame pixelFormat:format];

		[[window contentView]addSubview:view];
		[[window contentView] setAutoresizesSubviews:YES];
		[view setAutoresizingMask:
			NSViewWidthSizable | NSViewHeightSizable];
		[view setIs_window:w];
		[window makeKeyAndOrderFront:nil];
	   
	}

	Window::Window(Core *c):is::Window(c){
		core = c;
		internal = new Window_internal(this);
	}
	Window::~Window(){
		delete internal;
	}

	//By Minimalist Cocoa programming
	//http://www.cocoawithlove.com/2010/09/minimalist-cocoa-programming.html
	void init(int argc, char** argv){
		id pool = [NSAutoreleasePool new];
		[NSApplication sharedApplication];
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		id menubar = [[NSMenu new] autorelease];
		id appMenuItem = [[NSMenuItem new] autorelease];
		[menubar addItem:appMenuItem];
		[NSApp setMainMenu: menubar];
		id appMenu = [[NSMenu new] autorelease];
		id appName = [[NSProcessInfo processInfo] processName];
		id quitTitle = [@"Quit " stringByAppendingString:appName];
		id quitMenuItem = [[[NSMenuItem alloc] 
			initWithTitle:quitTitle
			action:@selector(terminate:) 
			keyEquivalent:@"q"] autorelease];
		[appMenu addItem:quitMenuItem];
		[appMenuItem setSubmenu:appMenu];
		[NSApp setDelegate:[[IsCocoaApplicationDelegate alloc]init]];
		//[pool release];
	}
	void run(){
		[NSApp activateIgnoringOtherApps:YES];
		[NSApp run];
	}
}}

#endif //__APPLE__
