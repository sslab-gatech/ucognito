/***************************************************************************
 *   Copyright (C) 2009 by Chris Parker                                    *
 *   chrsprkr3@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the Python License version 2.5 or later.        *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

/*
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_display.c $
$Author: chris_parker $
$Date: 2010-05-01 17:17:37 -0700 (Sat, 01 May 2010) $
$Rev: 5 $
*/

#include "xaut_display.h"
#include "xaut_window.h"

int display_h() {
    if (!_check_init()) {
        return -1;
    }
    return defaults->dsp_height;
}

int display_w() {
    if (!_check_init()) {
        return -1;
    }
    return defaults->dsp_width;
}

long desktop_count() {
    if (!_check_init()) {
        return -1;
    }
    if (!_is_supported_feature("_NET_NUMBER_OF_DESKTOPS")) {
        fprintf(stderr,
            "Your window manager does not support _NET_NUMBER_OF_DESKTOPS "
            "so I'm returning 1 since you obviously have one desktop");
        return 1;
    }

    Atom type;
    int size = 0;
    long item_count = 0;
    long ret = 1;

    Window root = XDefaultRootWindow(defaults->display);
    Atom request =
        XInternAtom(defaults->display, "_NET_NUMBER_OF_DESKTOPS", FALSE);

    unsigned char *data =
        _window_property(root, request, &item_count, &type, &size);

    if (item_count > 0) {
        ret = *((long*) data);
    }
    if(data) {
        XFree(data);
    }
    return ret;
}

long desktop(long desk) {
    if (!_check_init()) {
        return -1;
    }
    if(desk > -1) {
        set_current_desktop(desk);
    }
    return get_current_desktop();
}

long get_current_desktop() {
    Atom type;
    int size = 0;
    long item_count = 0;
    long desktop = 1;

    if (!_is_supported_feature("_NET_CURRENT_DESKTOP")) {
        fprintf(stderr,
            "Apparently your window manager doesn't support "
            "_NET_CURRENT_DESKTOP so I'm returning 1.\n");
        return 1;
    }

    Atom request =
            XInternAtom(defaults->display, "_NET_CURRENT_DESKTOP", FALSE);
    Window root = XDefaultRootWindow(defaults->display);

    unsigned char *desktops =
        _window_property(root, request, &item_count, &type, &size);

    if (item_count > 0) {
        desktop = *((long*) desktops);
    } else {
        desktop = -2; //Signals a problem as opposed to "I don't do that"
    }
    if(desktop) {
        XFree(desktops);
    }
    desktop++; //Force desktop to be one based rather than zero based
    return desktop;
}

BOOL set_current_desktop(long desktop) {
    if (!_check_init()) {
        return FALSE;
    }

    if (!_is_supported_feature("_NET_WM_DESKTOP")) {
        fprintf(stderr,
            "Apparently your window manager doesn't support "
            "_NET_WM_DESKTOP so I'm leaving you where you are.\n");
        return FALSE;
    }

    logit(LOG_LEVEL_VERBOSE, "Switching to desktop %ld\n", desktop);
    desktop--; //Force desktop to be one based rather than zero based
    if(desktop < 0) {
        char *msg = "This software starts desktop indexes at 1, so your"
            " request for desktop %ld failed\n";
        fprintf(stderr, msg, (desktop +1));
        return FALSE;
    }
    if((desktop +1) > desktop_count()) {
        char *msg = "You have specified desktop %ld, but you have only %ld"
            " desktops available\n";
        fprintf(stderr, msg, (desktop +1), desktop_count());
        return FALSE;
    }

    Window root = RootWindow(defaults->display, 0);
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xclient.format = 32;
    event.xclient.data.l[0] = desktop;
    event.xclient.data.l[1] = CurrentTime;
    event.type = ClientMessage;
    event.xclient.display = defaults->display;
    event.xclient.window = root;
    event.xclient.message_type =
        XInternAtom(defaults->display, "_NET_CURRENT_DESKTOP", FALSE);
    long mask = SubstructureNotifyMask | SubstructureRedirectMask;
    Status status =
        XSendEvent(defaults->display, root, FALSE, mask, &event );
    XFlush(defaults->display);
    return (status != 0);
}
