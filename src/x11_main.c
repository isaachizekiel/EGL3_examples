// x11_main.c


#include <EGL/egl.h>
#include <X11/X.h>
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#include "egl_utils.h"

static Display *x_display = NULL;
static Atom s_wmDeleteMessage;


// initializes the native x11 display and window for egl
EGLBoolean create_window(struct egl_context *context, const char *title) {
  Window root;
  XSetWindowAttributes swa;
  XSetWindowAttributes swa2;
  Atom wm_state;
  XWMHints hints;
  XEvent xev;
  EGLConfig config;
  EGLint num_configs;
  Window win;

  // X11 native window initialization
  x_display = XOpenDisplay(NULL);
  if (x_display == NULL) {
    return EGL_FALSE;
  }

  root = DefaultRootWindow(x_display);

  swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
  win = XCreateWindow(
		      x_display,
		      root, 0, 0, context->width, context->height, 0,
		      CopyFromParent, InputOutput,
		      CopyFromParent, CWEventMask, &swa);
  s_wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(x_display, win, &s_wmDeleteMessage, 1);

  swa2.override_redirect = False;
  XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &swa2);

  hints.input = True;
  hints.flags = InputHint;
  XSetWMHints(x_display, win, &hints);

  // make the window visible on the screen
  XMapWindow(x_display, win);
  XStoreName(x_display, win, title);

  // get identifiers for the provided atom name string
  wm_state = XInternAtom(x_display, "_NET_WM_STATE", False);

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = False;
  XSendEvent(x_display, DefaultRootWindow(x_display), False, SubstructureNotifyMask, &xev);  
  
  // todo shall we cast
  context->egl_native_window = win;
  context->egl_native_display = x_display;
  
  return GL_TRUE;
}

// reads from x11 event loop and interupt program if there is a keypress
// or window close action
EGLBoolean user_interupt(struct egl_context * context) {
  XEvent xev;
  KeySym key;
  EGLBoolean usr_interupt;
  char txt;

  // pump all messages from X server. Keypress are directed to keyfunc (if defined)
  while (XPending(x_display)) {
    XNextEvent(x_display, &xev);
    if(xev.type == KeyPress) {
      if (XLookupString(&xev.xkey, &txt,  1, &key, 0) == 1) {
	if (context->key_cb != NULL) {
	  context->key_cb(context, txt, 0, 0);
	}
      }      
    }
    if (xev.type == ClientMessage) {
      if (xev.xclient.data.l[0] == s_wmDeleteMessage) {
	usr_interupt = EGL_FALSE;
      }
    }

    if (xev.type == DestroyNotify) {
      usr_interupt = EGL_TRUE;
    }
  }
  return usr_interupt;
}

// start mai window loop
void window_loop(struct egl_context * context) {
  struct timeval t1, t2;
  struct timezone tz;
  float deltatime;

  gettimeofday(&t1, &tz);
  
  while(user_interupt(context) == EGL_FALSE) {
    // calculate fps
    gettimeofday(&t2, &tz);
    deltatime = (t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
    t1 = t2;

    if (context->update_cb != NULL) {
      context->update_cb(context, deltatime);
    }
    
    if(context->draw_cb != NULL) {
      context->draw_cb(context);
    }

    eglSwapBuffers(context->egl_display, context->egl_surface);
  }
}

int main() {
  struct egl_context * context = malloc(sizeof(struct egl_context));

  // memset(context, 0, sizeof(struct egl_context));

  //window_loop(context);

  if (context->shutdown_cb != NULL) context->shutdown_cb(context);
  
  return 0;
}
