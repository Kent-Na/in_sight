#include <Cocoa/Cocoa.h>
#include "is_config.h"
#include "is_pch.h"
#include "is_header_all.h"
#include "is_texture.h"
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
    [self is_window]->mouse(s, p);
    [self is_window]->update(s);
    //system->triggerMouseDown([theEvent buttonNumber]);
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
    //system->triggerMouseDown([theEvent buttonNumber]);
	glFlush();
	[[self openGLContext] flushBuffer];
}

- (void)mouseDragged:(NSEvent *)theEvent{
	//NSPoint pp=[self convertPoint:[theEvent locationInWindow]fromView:nil];
	//if (system)
	//	system->moveMouse(*(BnTvector*)&pp);
}

- (void)mouseUp:(NSEvent *)theEvent{
	//if (system)
	//	system->triggerMouseUp([theEvent buttonNumber]);
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
                                 [theEvent deltaX]*scale,
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
    //system->triggerMouseDown([theEvent buttonNumber]);
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

namespace is{
    namespace cocoa{



        class Window_internal{
            NSWindow* window;
            IsCocoaOpenGLView* view;
        public:
            Window_internal(Window *w);
        };

        Window_internal::Window_internal(Window *w){
            NSRect frame = NSMakeRect(0,0,800,600);
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

        Window::Window(Core *c){
            core = c;
            internal = new Window_internal(this);
        }
        Window::~Window(){
            delete internal;
        }
}}