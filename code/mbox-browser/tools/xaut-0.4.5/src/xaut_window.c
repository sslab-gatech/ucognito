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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_window.c $
$Author: chris_parker $
$Date: 2012-09-22 17:17:57 -0700 (Sat, 22 Sep 2012) $
$Rev: 29 $
*/

#include <X11/extensions/XTest.h>
#include <regex.h>

#include "xaut.h"

BOOL activate_window(Window win) {
    char *event_name = "_NET_ACTIVE_WINDOW";
    if (!_check_init() || win < 1) {
        return FALSE;
    }
    if (!_is_supported_feature("_NET_ACTIVE_WINDOW")) {
        fprintf(stderr, "Cannot use '_NET_ACTIVE_WINDOW', "
            "so it is impossible to activate the window "
            "using this function.  Try alt-tabbing to it instead.");
    }
    long win_desk = window_desktop(win);
    if (win_desk < 0) {
        return FALSE;
    }
    long current_desktop = get_current_desktop();
    if (win_desk != current_desktop) {
        BOOL switched = set_current_desktop(win_desk);
        if (!switched) {
            return FALSE;
        }
    }
    XEvent event;
    memset(&event, 0, sizeof(XEvent));
    event.xclient.display = defaults->display;
    event.xclient.window = win;
    event.xclient.format = 32;

    //Toggling _NET_ACTIVE_WINDOW (event_name) with no other changes activates
    event.xclient.data.l[0] = 2L;           //We want to toggle the setting
    event.xclient.data.l[1] = CurrentTime;  //Now
    event.xclient.data.l[2] = 0L;           //These two flags are redundantly
    event.xclient.data.l[3] = 0L;           //saying "no other changes"
    event.xclient.message_type =
        XInternAtom(defaults->display, event_name, FALSE);

    event.type = ClientMessage;
    event.xclient.display = defaults->display;
    event.xclient.window = win;
    XWindowAttributes wattr;
    XGetWindowAttributes(defaults->display, win, &wattr );
    long mask = SubstructureNotifyMask | SubstructureRedirectMask;
    Status status =
        XSendEvent(defaults->display, wattr.screen->root , FALSE, mask, &event);
    XFlush(defaults->display);
    return (status != 0);
}

Window active_window() {
    if (!_check_init()) {
        return 0;
    }
    Window win = 0;
    int focus_state_ignored = 0;
    int success = XGetInputFocus(defaults->display, &win, &focus_state_ignored);
    Window ret = 0;
    if (success) {
        ret = win;
    }
    return ret;
}

int _is_not_valid_error_handler(Display *dsp, XErrorEvent *err) {
    return 0;
}

BOOL is_valid(Window win) {
    _check_init();
    XSetErrorHandler(_is_not_valid_error_handler);
    XWindowAttributes attribs;
    Status status = XGetWindowAttributes(defaults->display, win, &attribs);
    XSetErrorHandler(NULL);
    return status ? TRUE : FALSE;
}

Window find_outer_parent(Window win) {
    Window root_return;
    Window parent_return;
    Window *children_return_ignored;
    unsigned int num_children_return_ignored;

    Status status = XQueryTree(
        defaults->display,
        win,
        &root_return,
        &parent_return,
        &children_return_ignored,
        &num_children_return_ignored
    );
    if(!status) {
        fprintf(stderr, "Something went wrong trying to find parent\n");
        return 0;
    }
    if(root_return == parent_return) {
        return win;
    } else if(parent_return > root_return) {
        return find_outer_parent(parent_return);
    } else {
        fprintf(stderr, "Somehow missed the parent, returning null\n");
        return 0;
    }
}

Window find_window(char *title) {
    if(! _check_init()) {
        fprintf(stderr, "Failure to initialize");
        return 0;
    }
    regex_t *regx = calloc(1, sizeof(regex_t));
    if (!regx) {
        fprintf(stderr, "Unable to allocate space for a regular expression");
        return 0;
    }
    if (!_prepare_regex(title, &regx)) {
        return 0;
    }
    unsigned long count;
    Atom type_ret;
    long offset = 0L;
    long length = (~0L);
    BOOL delete = FALSE;
    int format_ret;
    Window bytes_after;
    unsigned char *results;

    int prop_ret_val = XGetWindowProperty(
            defaults->display,
            XDefaultRootWindow(defaults->display),
            XInternAtom(defaults->display, "_NET_CLIENT_LIST", FALSE),
            offset,
            length,
            delete,
            XA_WINDOW,
            &type_ret,
            &format_ret,
            &count,
            &bytes_after,
            &results
    );
    if(prop_ret_val != Success) {
        XFree(results);
        fprintf(stderr, "XGetWindowProperty returned %d", prop_ret_val);
        return 0;
    }
    unsigned long i;
    Window *windows = (Window *)results;
    Window *match_buffer = calloc(count, sizeof(Window)); //A safe size...
    if(!match_buffer) {
        fprintf(stderr, "Unable to allocate space for a window buffer\n");
        XFree(results);
        return 0;
    }
    unsigned int found = 0;
    for(i = 0; i < count; i++) {
        char *win_title = window_name(windows[i]);
        logit(LOG_LEVEL_EXTRA_VERBOSE, "Testing to see if '%s' matches '%s'\n",
                title, win_title);
        if (win_title && !regexec(regx, win_title, 0, NULL, 0)) {
            logit(LOG_LEVEL_VERBOSE, "Adding '%s' as a match\n", win_title);
            match_buffer[found] = windows[i];
            found++;
        }
        free(win_title);
    }
    Window final_ret = 0;
    if(found == 0) {
        logit(LOG_LEVEL_VERBOSE, "No matches found for '%s'\n", title);
    } else if(found == 1) {
        logit(LOG_LEVEL_VERBOSE, "Only one match found for '%s'\n", title);
        final_ret = match_buffer[0];
    } else {
        logit(LOG_LEVEL_VERBOSE, "%d matches found for '%s'\n", found, title);
        qsort(match_buffer, found, sizeof(Window), _window_search_comparator);
        final_ret = match_buffer[0];
    }
    regfree(regx);
    free(regx);
    XFree(results);
    free(match_buffer);
    return final_ret;
}

Window* search_for_window(char *title) {
    if(! _check_init()) {
        fprintf(stderr, "Failure to initialize");
        return 0;
    }
    regex_t *regx = calloc(1, sizeof(regex_t));
    if (!regx) {
        fprintf(stderr, "Unable to allocate space for a regular expression");
        return 0;
    }
    if (!_prepare_regex(title, &regx)) {
        return 0;
    }
    _window_search *search = calloc(1, sizeof(_window_search));
    if (!search) {
        fprintf(stderr, "Unable to allocate space for a buffer");
        return 0;
    }
    search->buffer = calloc(10, sizeof(Window));
    if (!search->buffer) {
        fprintf(stderr, "Unable to allocate space for a buffer");
        return 0;
    }
    search->buffer_size = 10;
    search->current = XDefaultRootWindow(defaults->display);
    search->found = 0;
    search->title = title;
    search->regx = &regx;
    XSetErrorHandler(_invalid_window_error_handler);
    _recursive_window_search(search);
    XSetErrorHandler(NULL);
    if(search->found == 0) {
        logit(LOG_LEVEL_VERBOSE, "No matches found for '%s'\n", title);
    } else if(search->found == 1) {
        logit(LOG_LEVEL_VERBOSE, "Only one match found for '%s'\n", title);
    } else {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%d matches found for '%s'\n",
                    search->found, title);
        qsort(search->buffer, search->found, sizeof(Window),
                _window_search_comparator);
    }
    Window* ret = calloc(search->found + 1, sizeof(Window));
    if(ret == NULL) {
        fprintf(stderr, "Unable to allocate space for the retured windows");
    } else {
        unsigned int i;
        for(i = 0; i < search->found; i++) {
            ret[i] = search->buffer[i];
        }
        ret[search->found] = 0;
    }
    regfree(regx);
    free(regx);
    free(search->buffer);
    free(search);
    return ret;
}

BOOL move_window(Window win, int x, int y, long desktop) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    if ((x < 0) && (y < 0) && (desktop < 0)) {
        //Why am I being called?
        fprintf(stderr, "All movement parameters were -1\nNothing to do\n");
        return TRUE;
    }
    BOOL moved = _move_window_to_desktop(win, desktop);
    if(moved == FALSE) {
        fprintf(stderr, "Unable to move window %lu to desktop %lu\n",
                win, desktop);
    }
    if((x < 0) && (y < 0)) {
        logit(LOG_LEVEL_VERBOSE, "%s",
                "Leaving window at it's current x y coordinates\n");
        return TRUE;
    } else if(x < 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s",
                "Using current window x coordinate as its destination\n");
        x = window_x(win);
    } else if(y < 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s",
                "Using current window y coordinate as its destination\n");
        y = window_y(win);
    }
    logit(LOG_LEVEL_VERBOSE, "Moving window %lu to %d X %d\n", win, x, y);
    XMoveWindow(defaults->display, win, x, y);
    XFlush(defaults->display);
    return TRUE;
}

BOOL resize_window(Window win, unsigned int w, unsigned int h) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    if((w == 0) && (h == 0)) {
        //Why am I being called?
        fprintf(stderr, "Both resize parameters were 0\nNothing to do\n");
        return TRUE;
    }
    win = find_outer_parent(win);
    if(w == 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s",
                "Using current window width as its new width\n");
        w = window_w(win);
    }
    if(h == 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s",
                "Using current window height as its new height");
        h = window_h(win);
    }
    logit(LOG_LEVEL_VERBOSE, "Resizing window %lu to %du X %du", win, w, h);
    XSetErrorHandler(_invalid_window_error_handler);
    int ret = XResizeWindow(defaults->display, win, w, h);
    XFlush(defaults->display);
    XSetErrorHandler(NULL);
    if(ret == 0) {
        fprintf(stderr, "Resizing window %lu to %du X %du generated an error\n",
                win, w, h);
    }
    return ret > 0;
}

BOOL iconify_window(Window win, BOOL tf) {
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    if(!_is_supported_feature("_NET_WM_ACTION_MINIMIZE")) {
        fprintf(stderr, "Apparently that window cannot be minimized\n");
        return FALSE;
    }

    Status status;
    XSetErrorHandler(_invalid_window_error_handler);
    XWindowAttributes attribs;
    status = XGetWindowAttributes(
        defaults->display,
        win,
        &attribs);
    if(!status) {
        fprintf(stderr, "%lu is not a valid window handle", win);
        return FALSE;
    }
    if(tf) {
        int screen = XScreenNumberOfScreen(attribs.screen);
        status = XIconifyWindow(defaults->display, win, screen);
    } else {
        XMapWindow(defaults->display, win);
    }
    XFlush(defaults->display);

    XSetErrorHandler(NULL);
    return (status != 0);
}

BOOL minimize_window(Window win, BOOL tf) {
    return iconify_window(win, tf);
}

BOOL maximize_window(Window win, BOOL tf) {
    return(maximize_window_horz(win, tf) &&
            maximize_window_vert(win, tf));
}

BOOL maximize_window_horz(Window win, BOOL tf) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    char *horz = "_NET_WM_STATE_MAXIMIZED_HORZ";
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    if(!_is_supported_feature(horz)) {
        fprintf(stderr, "Apparently that window can't be"
                " maximized horizontally" );
        return FALSE;
    }
    Atom datal1 = XInternAtom(defaults->display, horz, FALSE);
    Atom datal2 = 0;
    long action = (tf) ? 1 : 0; //1 means "set", 0 means "clear" flag
    return _alter_window_state(win, action, datal1, datal2);
}

BOOL maximize_window_vert(Window win, BOOL tf) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    char *vert = "_NET_WM_STATE_MAXIMIZED_VERT";
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    if (!_is_supported_feature(vert)) {
        fprintf(stderr, "Apparently that window can't be"
                " maximized vertically" );
        return FALSE;
    }
    Atom datal1 = XInternAtom(defaults->display, vert, FALSE);
    Atom datal2 = 0;
    long action = (tf) ? 1 : 0; //1 means "set", 0 means "clear" flag
    return _alter_window_state(win, action, datal1, datal2);
}

BOOL full_screen_window(Window win, BOOL tf) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    char *full = "_NET_WM_STATE_FULLSCREEN";
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    if(!_is_supported_feature(full)) {
        fprintf(stderr, "Apparently that window can't be made full screen" );
    }
    Atom datal1 = XInternAtom(defaults->display, full, FALSE);
    long action = (tf) ? 1 : 0; //1 means "set", 0 means "clear" flag
    return _alter_window_state(win, action, datal1, 0);
}

BOOL shade_window(Window win, BOOL tf) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    char *shade = "_NET_WM_STATE_SHADED";
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    if(!_is_supported_feature(shade)) {
        fprintf(stderr, "Apparently that window can't be shaded" );
    }
    Atom datal1 = XInternAtom(defaults->display, shade, FALSE);
    long action = (tf) ? 1 : 0; //1 means "set", 0 means "clear" flag
    return _alter_window_state(win, action, datal1, 0);
}

BOOL restore_window(Window win) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    char *vert = "_NET_WM_STATE_MAXIMIZED_VERT";
    char *horz = "_NET_WM_STATE_MAXIMIZED_HORZ";
    char *shade = "_NET_WM_STATE_SHADED";
    char *full = "_NET_WM_STATE_FULLSCREEN";
    if(!_check_init() || win < 1) {
        return FALSE;
    }
    BOOL ret = 0;
    long action = 0; //Remove the bit
    if(_is_supported_feature(shade)) {
        Atom datal1 = XInternAtom(defaults->display, shade, FALSE);
        ret |= _alter_window_state(win, action, datal1, 0);
    }
    if(_is_supported_feature(vert)) {
        Atom datal1 = XInternAtom(defaults->display, vert, FALSE);
        ret |= _alter_window_state(win, action, datal1, 0);
    }
    if(_is_supported_feature(horz)) {
        Atom datal1 = XInternAtom(defaults->display, horz, FALSE);
        ret |= _alter_window_state(win, action, datal1, 0);
    }
    if(_is_supported_feature(full)) {
        Atom datal1 = XInternAtom(defaults->display, full, FALSE);
        ret |= _alter_window_state(win, action, datal1, 0);
    }
    XMapWindow(defaults->display, win);
    XFlush(defaults->display);
    return ret;
}

int window_x(Window win) {
    if (!_check_init()) {
        return INT_MIN;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return INT_MIN;
    }
    Window parent = find_outer_parent(win);
    if(!parent) {
        return INT_MIN;
    }
    XWindowAttributes attribs;
    int status = XGetWindowAttributes(defaults->display, parent, &attribs);
    int ret = -1;
    if (status) {
        int junk_y = 0; //Y coordinate not used here
        Window junk_win;
        (void) XTranslateCoordinates(
                defaults->display,
                parent,
                attribs.root,
                -attribs.border_width,
                -attribs.border_width,
                &ret,
                &junk_y,
                &junk_win);
    } else {
        fprintf(stderr,"Unable to retrieve x attribute for %lu\n", win);
    }
    return ret;
}

int window_y(Window win) {
    if (!_check_init()) {
        return INT_MIN;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return INT_MIN;
    }
    Window parent = find_outer_parent(win);
    if(!parent) {
        return INT_MIN;
    }
    XWindowAttributes attribs;
    int status = XGetWindowAttributes(defaults->display, parent, &attribs);
    int ret = -1;
    if (status) {
        int junk_x = 0; //X coordinate not used here
        Window junk_win;
        (void) XTranslateCoordinates(
                defaults->display,
                parent,
                attribs.root,
                -attribs.border_width,
                -attribs.border_width,
                &junk_x,
                &ret,
                &junk_win);
    } else {
        fprintf(stderr,"Unable to retrieve y attribute for %lu\n", win);
    }
    return ret;
}

int window_w(Window win) {
    if (!_check_init()) {
        return INT_MIN;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return INT_MIN;
    }
    Window parent = find_outer_parent(win);
    if(!parent) {
        return INT_MIN;
    }
    XWindowAttributes attribs;
    int status = XGetWindowAttributes(defaults->display, parent, &attribs);
    int ret = -1;
    if (status) {
        ret = attribs.width;
    } else {
        fprintf(stderr,"Unable to retrieve width attribute for %lu\n", win);
    }
    return ret;
}

int window_h(Window win) {
    if (!_check_init()) {
        return -1;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return INT_MIN;
    }
    Window parent = find_outer_parent(win);
    if(!parent) {
        return INT_MIN;
    }
    XWindowAttributes attribs;
    int status = XGetWindowAttributes(defaults->display, parent, &attribs);
    int ret = -1;
    if (status) {
        ret = attribs.height;
    } else {
        fprintf(stderr,"Unable to retrieve height attribute for %lu\n", win);
    }
    return ret;
}

char *window_name(Window win) {
    if (!_check_init()) {
        return NULL;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return NULL;
    }
    //I don't know if it will cause problems freeing this outside X -
    //so I'll make a copy and return that.
    char *xname = "";
    XFetchName(defaults->display, win, &xname);
    if(xname == NULL) {
        return NULL;
    }
    char *name;
    name = calloc(strlen(xname) + 1, sizeof(char));
    if(name == NULL) {
        fprintf(stderr, "Unable to allocate space for a window name");
    } else {
        strncpy(name, xname, strlen(xname));
    }
    XFree(xname);
    return name;
}

long window_desktop(Window win) {
    if (!_check_init()) {
        return LONG_MIN;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return LONG_MIN;
    }

    if (!_is_supported_feature("_NET_WM_DESKTOP")) {
        fprintf(stderr,
        "Apparently your window manager doesn't support "
            "_NET_CURRENT_DESKTOP so I'm returning 1.\n");
        return 1;
    }

    Atom type;
    int size = 0;
    long item_count = 0;
    Atom request = XInternAtom(defaults->display, "_NET_WM_DESKTOP", FALSE);

    unsigned char *window_desktop = _window_property((Window) win, request,
            &item_count, &type, &size);

    long desktop = 0;
    if (item_count > 0) {
        desktop = *((long*) window_desktop);
    } else {
        desktop = -2;
    }
    if(desktop == -1) {
        logit(LOG_LEVEL_VERBOSE, "The window %lu is currently on all desktops\n", win);
    } else {
        logit(LOG_LEVEL_VERBOSE, "The window %lu is currently on desktop %lu\n",
                win, (desktop + 1));
    }
    if(window_desktop) {
        XFree(window_desktop);
    }
    desktop++; //Force desktop to be one based rather than zero based
    return desktop;
}

BOOL _alter_window_state(Window win, long action, long datal1, long datal2) {
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;
    XEvent event;
    Atom message_type = XInternAtom(defaults->display, "_NET_WM_STATE", FALSE);
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = TRUE;
    event.xclient.message_type = message_type;
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = action;
    event.xclient.data.l[1] = datal1;
    event.xclient.data.l[2] = datal2;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;
    Status s = XSendEvent(
            defaults->display,
            DefaultRootWindow(defaults->display),
            FALSE,
            mask,
            &event);
    XFlush(defaults->display);
    return (s != 0);
}


BOOL _ensure_window_buffer_size(_window_search *search) {
    if (search->buffer_size == search->found) {
        logit(LOG_LEVEL_EXTRA_VERBOSE,
                "Growing window search results buffer from %d ",
                search->buffer_size);
        search->buffer_size += 20;
        logit(LOG_LEVEL_EXTRA_VERBOSE, "to %d elements\n",
                search->buffer_size);
        search->buffer = realloc(search->buffer, search->buffer_size
                * sizeof(Window));
        if (search->buffer) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return TRUE;
    }
}

BOOL _move_window_to_desktop(Window win, long desktop) {
    logit(LOG_LEVEL_EXTRA_VERBOSE,
            "Received desktop value %ld for window %lu\n", desktop, win);
    char *event_name = "_NET_WM_DESKTOP";
    if (desktop < 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE,
                "Desktop %ld means 'leave it where it is'\n", desktop);
        return TRUE;
    } else if(desktop == 0) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s", "Desktop 0 means 'put it on all desktops'\n");
    } else {
        logit(LOG_LEVEL_EXTRA_VERBOSE,
                "Moving window %lu to desktop %ld\n",
                win, desktop);
    }
    if (!_check_init()) {
        return FALSE;
    }
    if(is_valid(win) == FALSE) {
        fprintf(stderr, "Window %lu is invalid\n", win);
        return FALSE;
    }

    if (!_is_supported_feature("_NET_WM_DESKTOP")) {
        fprintf(stderr,
        "Apparently your window manager doesn't support "
            "_NET_CURRENT_DESKTOP so I'm leaving the window "
            "where it is.\n");
        return FALSE;
    }
    if (desktop == window_desktop(win)) {
        logit(LOG_LEVEL_VERBOSE, "Window is already on the target desktop\n");
        return TRUE;
    }
    desktop--; //Force desktop to be one based rather than zero based
    logit(LOG_LEVEL_VERBOSE,
            "*Actual* final destination desktop of window %lu is %ld\n",
            win, desktop);
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xclient.format = 32;
    event.xclient.data.l[0] = desktop;
    event.xclient.data.l[1] = CurrentTime;
    event.type = ClientMessage;
    event.xclient.display = defaults->display;
    event.xclient.window = win;
    event.xclient.message_type =
        XInternAtom(defaults->display, event_name, FALSE);
    XWindowAttributes wattr;
    XGetWindowAttributes(defaults->display, win, &wattr );
    long mask = SubstructureNotifyMask | SubstructureRedirectMask;
    Status status =
        XSendEvent(defaults->display, wattr.screen->root , FALSE, mask, &event);
    XFlush(defaults->display);
    return (status != 0);
}

BOOL _prepare_regex(char *search, regex_t **regx) {
    if (regcomp(*regx, search, 0)) {
        fprintf(stderr, "Unable to compile title regex %s", search);
        return FALSE;
    }
    return TRUE;
}

void _recursive_window_search(_window_search *search) {
    unsigned long desk_count = desktop_count();
    Window root_return_ignored = 0;
    Window parent_return_ignored = 0;
    Window *children;
    unsigned int count = 0;
    Status status = XQueryTree(defaults->display, search->current,
            &root_return_ignored, &parent_return_ignored, &children, &count);
    if (count == 0 || !status) {
        return;
    }
    unsigned int i;
    for (i = 0; i < count; i++) {
        if (!children[i]) {
            return;
        }
        char *title = window_name(children[i]);
        if (title) {
            logit(LOG_LEVEL_EXTRA_VERBOSE,
                    "Testing to see if '%s' matches '%s'\n",
                    search->title, title);
            if (!regexec(*search->regx, title, 0, NULL, 0)) {
                unsigned long win_desk = window_desktop(children[i]);
                if(win_desk > desk_count){
                    logit(LOG_LEVEL_VERBOSE,
                            "Skipping %lu because its hidden\n",
                            children[i]);
                }
                logit(LOG_LEVEL_VERBOSE, "Adding '%s' as a match\n", title);
                if (_ensure_window_buffer_size(search)) {
                    Window res = children[i];
                    search->buffer[search->found] = res;
                    search->found++;
                } else {
                    fprintf(stderr,
                            "Unable to allocate space for a window buffer");
                }
            }
            free(title);
        }
        if (children[i]) {
            search->current = children[i];
            _recursive_window_search(search);
        }
    }
    XFree(children);
}

int _window_search_comparator(const void *l, const void *r) {
    //First sort is desktop - closest wins
    Window winL = *((Window*) l);
    Window winR = *((Window*) r);
    long ldesk = window_desktop(winL);
    long rdesk = window_desktop(winR);
    if (ldesk != rdesk) {
        long curr = get_current_desktop();
        long difL = fabs(curr - ldesk);
        long difR = fabs(curr - rdesk);
        if (difL < difR) {
            char *msg = "'%s' comes before '%s' because its"
                " desktop %ld is closer to %ld than %l\n";
            logit(LOG_LEVEL_EXTRA_VERBOSE, msg,
                    window_name(winL),
                    window_name(winR),
                    ldesk,
                    curr,
                    rdesk);
            return -1;
        } else if (difL > difR) {
            char *msg = "'%s' comes before '%s' because its"
                " desktop %ld is closer to %ld than %l\n";
            logit(LOG_LEVEL_EXTRA_VERBOSE, msg,
                    window_name(winR),
                    window_name(winL),
                    rdesk,
                    curr,
                    ldesk);
            return 1;
        }
    }

    //If we're here, we sort by id
    if (winL < winR) {
        char *msg = "'%s' comes before '%s' because its"
            " id %ld is lower than %ld\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg,
                window_name(winL),
                window_name(winR),
                winL,
                winR);
        return -1;
    } else if (winR < winL) {
            char *msg = "'%s' comes before '%s' because its"
                    " id %ld is lower than %ld\n";
            logit(LOG_LEVEL_EXTRA_VERBOSE, msg,
                    window_name(winR),
                    window_name(winL),
                    winR,
                    winL);
        return 1;
    } else {
        return 0;
    }
}

int _invalid_window_property_error_handler(Display *dsp, XErrorEvent *err) {
    return BadWindow;
}
unsigned char * _window_property(
        Window win,
        Atom property,
        long *item_count_ret,
        Atom *type,
        int *size) {
    long offset = 0L;
    long length = (~0L);
    BOOL delete = FALSE;
    Atom type_ret;
    int format_ret;
    Window items_found;
    Window bytes_after; /* unused */
    int status;
    unsigned char *x_prop;

    XSetErrorHandler(_invalid_window_property_error_handler);
    status = XGetWindowProperty(
            defaults->display,
            win, property, offset,
            length, delete, AnyPropertyType, &type_ret, &format_ret,
            &items_found, &bytes_after, &x_prop);


    if (status == BadWindow) {
        fprintf(stderr, "%ld is a bad window handle - returning null\n", win);
        return NULL;
    }
    if (status != Success) {
        fprintf(stderr, "XGetWindowProperty failed - returning null\n");
        return NULL;
    }

    *item_count_ret = items_found;
    *type = type_ret;
    *size = format_ret;
    return x_prop;
}
