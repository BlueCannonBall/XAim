#include "config.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <stdbool.h>
#include <stdio.h>

void allow_input_passthrough(Display* dpy, Window w) {
    XserverRegion region = XFixesCreateRegion(dpy, NULL, 0);

    XFixesSetWindowShapeRegion(dpy, w, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion(dpy, w, ShapeInput, 0, 0, region);

    XFixesDestroyRegion(dpy, region);
}

int main() {
    Display* dpy;
    if (!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, "Failed to open display, terminating\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);

    XWindowAttributes root_attrs;
    XGetWindowAttributes(dpy, root, &root_attrs);

    XSetWindowAttributes attrs;
    attrs.override_redirect = true;

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(dpy, DefaultScreen(dpy), 32, TrueColor, &vinfo)) {
        fprintf(stderr, "No visual found supporting 32 bit color, terminating\n");
        return 1;
    }
    attrs.colormap = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    // Window XCreateWindow(
    //     Display *display, Window parent,
    //     int x, int y, unsigned int width, unsigned int height, unsigned int border_width,
    //     int depth, unsigned int class,
    //     Visual *visual,
    //     unsigned long valuemask, XSetWindowAttributes *attributes
    // );
    Window overlay = XCreateWindow(
        dpy, root, 0, 0, root_attrs.width, root_attrs.height, 0, vinfo.depth, InputOutput, vinfo.visual, CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs);
    GC gc = XCreateGC(dpy, overlay, 0, 0);

    XStoreName(dpy, overlay, "XAim");
    XClassHint* class_hint = XAllocClassHint();
    if (class_hint) {
        class_hint->res_name = "XAim";
        class_hint->res_class = "xaim";
        XSetClassHint(dpy, overlay, class_hint);
        XFree(class_hint);
    }

    XSelectInput(dpy, overlay, ExposureMask);
    allow_input_passthrough(dpy, overlay);
    XMapWindow(dpy, overlay);

    XColor color;
    color.red = RED * 256;
    color.green = GREEN * 256;
    color.blue = BLUE * 256;
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(dpy, attrs.colormap, &color);
    XSetForeground(dpy, gc, color.pixel);
    XSetLineAttributes(dpy, gc, WIDTH, LineSolid, CapButt, JoinMiter);

    XEvent ev;
    while (true) {
        XNextEvent(dpy, &ev);

        if (ev.type == Expose) {
            XDrawArc(dpy, overlay, gc, root_attrs.width / 2 - RADIUS, root_attrs.height / 2 - RADIUS, RADIUS * 2, RADIUS * 2, 0, 360 * 64);
        }
    }
}
