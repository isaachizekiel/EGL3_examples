// x11_main.c


#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#include "egl_utils.h"

static Display *x_display = NULL;
static Atom s_wmDeleteMessage;


// initializes the native x11 display and window for egl
EGLBoolean create_window(struct egl_context *context, const char *title) {  
  return GL_TRUE;
}

// reads from x11 event loop and interupt program if there is a keypress
// or window close action
EGLBoolean user_interupt(struct egl_context * context) { return GL_TRUE; }

// start mai window loop
void window_loop(struct egl_context * context) {
  
}

int main() {
  return 0;
}
