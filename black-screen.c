#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


Cursor create_invisible_cursor(Display *d, Window win) {
    Pixmap blank;
    XColor dummy;
    char data[1] = { 0 };
    Cursor cursor;

    blank = XCreateBitmapFromData(d, win, data, 1, 1);
    cursor = XCreatePixmapCursor(d, blank, blank, &dummy, &dummy, 0, 0);
    XFreePixmap(d, blank);
    return cursor;
}


int main(void) {
    Display *d;
    Window root, win;
    XSetWindowAttributes attrs;
    int screen;
    unsigned long valuemask = CWBackPixel | CWOverrideRedirect;

    if ((d = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Failed to open X display.\n");
        return 1;
    }

    screen = DefaultScreen(d);
    root = RootWindow(d, screen);

    attrs.override_redirect = True;
    attrs.background_pixel = BlackPixel(d, screen);

    int width = DisplayWidth(d, screen);
    int height = DisplayHeight(d, screen);

    win = XCreateWindow(d, root,
                        0, 0, width, height,
                        0, CopyFromParent, InputOutput,
                        CopyFromParent, valuemask, &attrs);

    XDefineCursor(d, win, create_invisible_cursor(d, win));
    XSelectInput(d, win, KeyPressMask | ButtonPressMask | FocusChangeMask);
    XMapWindow(d, win);
    XRaiseWindow(d, win);
    XFlush(d);

    bool running = true;
    XEvent e;

    while (running) {
        XSetInputFocus(d, win, RevertToParent, CurrentTime);
        XNextEvent(d, &e);

        if (e.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&e.xkey, 0);

            if (((e.xkey.state & ControlMask) && keysym == XK_c) || keysym == XK_Escape)
                running = false;
        }
    }

    XDestroyWindow(d, win);
    XCloseDisplay(d);
    return 0;
}
