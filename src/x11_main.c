// x11_main.c


#include <X11/X.h>
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

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
EGLBoolean user_interupt(struct egl_context * context) { return GL_TRUE; }

// start mai window loop
void window_loop(struct egl_context * context) {
  
}

int main() {
  return 0;}
