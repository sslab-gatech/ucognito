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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut.c $
$Author: chris_parker $
$Date: 2012-07-28 14:41:37 -0700 (Sat, 28 Jul 2012) $
$Rev: 23 $
*/

#include "xaut.h"
#include <stdarg.h>

// --- Published subroutines, functions and structures

void cleanup() {
    if (defaults->display == NULL) {
        fprintf(stderr, "cleanup: Display is null, no need to release it\n");
    } else {
        XFlush(defaults->display);
        XCloseDisplay(defaults->display);
        defaults->display = NULL;
    }
    free(defaults);
    free(ascii_codes);
    defaults = NULL;
    ascii_codes = NULL;
}

void verbose(BOOL verbose) {
    if (_check_init()) {
        if(verbose) {
            defaults->log_level |= LOG_LEVEL_VERBOSE;
        } else {
            defaults->log_level &= (! LOG_LEVEL_VERBOSE);
        }
    }
}

void extra_verbose(BOOL extra_verbose) {
    if (_check_init()) {
        if(extra_verbose) {
            defaults->log_level |= LOG_LEVEL_EXTRA_VERBOSE;
        } else {
            defaults->log_level &= (! LOG_LEVEL_EXTRA_VERBOSE);
        }
    }
}

void logit(int level, char *msg, ...) {
    if(defaults->log_level >= level) {
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
    }
}

// --- Unpublished

void _add_code(unsigned short code, BOOL shifted) {
    ascii_codes[code].ascii = (char) code;
    ascii_codes[code].code = XKeysymToKeycode(defaults->display, code);
    ascii_codes[code].shifted = shifted;
}

BOOL _check_init() {
    if (!defaults || !defaults->dsp_width) {
        _initialize();
    }
    if (!ascii_codes) {
        _init_ascii();
    }
    if (defaults->dsp_width) {
        return TRUE;
    } else {
        fprintf(stderr, "Cannot properly initialize defaults - exiting\n");
        return FALSE;
    }
}

#ifdef EN_US
BOOL _init_ascii() {
    ascii_codes = malloc(128 * sizeof(xautpy_ascii_t));
    if (ascii_codes == NULL) {
        fprintf(stderr, "init: Unable to allocate space for ascii codes");
        return FALSE;
    }
    memset(ascii_codes, 0, 128 * sizeof(xautpy_ascii_t));

    // These are easy - regular letters are ( lower case == uppercase | 32 )
    unsigned short i;
    for (i = 'A'; i <= 'Z'; i++) {
        _add_code(i, TRUE); //Upper
        _add_code((i | 32), FALSE); //Lower
    }

    // Numbers
    for (i = '0'; i <= '9'; i++) {
        _add_code(i, FALSE);
    }

    // These are arbitrary keys that will just work.  They would be too
    // weird to try to enter in a loop, seeing as how consecutive ascii
    // codes sometimes require shift, and sometimes do not.
    // This is just me running my fingers across my QWERTY US keyboard.
    _add_code(' ', FALSE); //Space
    _add_code('\t', FALSE); //Tab
    _add_code('`', FALSE);
    _add_code('~', TRUE);
    _add_code('!', TRUE);
    _add_code('@', TRUE);
    _add_code('#', TRUE);
    _add_code('$', TRUE);
    _add_code('%', TRUE);
    _add_code('^', TRUE);
    _add_code('&', TRUE);
    _add_code('*', TRUE);
    _add_code('(', TRUE);
    _add_code(')', TRUE);
    _add_code('-', FALSE);
    _add_code('_', TRUE);
    _add_code('=', FALSE);
    _add_code('+', TRUE);
    _add_code('[', FALSE);
    _add_code('{', TRUE);
    _add_code(']', FALSE);
    _add_code('}', TRUE);
    _add_code('\\', FALSE);
    _add_code('|', TRUE);
    _add_code(';', FALSE);
    _add_code(':', TRUE);
    _add_code('\'', FALSE);
    _add_code('"', TRUE);
    _add_code(',', FALSE);
    _add_code('<', TRUE); //Strangely, this doesn't appear to work.  "+," does though.
    _add_code('.', FALSE);
    _add_code('>', TRUE);
    _add_code('/', FALSE);
    _add_code('?', TRUE);

    SHIFT_L = XKeysymToKeycode(defaults->display, XStringToKeysym("Shift_L"));
    CONTROL_L = XKeysymToKeycode(defaults->display,
            XStringToKeysym("Control_L"));
    ALT_L = XKeysymToKeycode(defaults->display, XStringToKeysym("Alt_L"));
    META_L = XKeysymToKeycode(defaults->display, XStringToKeysym("Super_L"));

#ifdef LESS_THAN_FIX
    //Note that we leave the ascii code and shift alone, so in
    //essence we are sending +, or shift + comma.
    ascii_codes['<'].code = ascii_codes[','].code; //Corrects the "<" problem
#endif // LESS_THAN_FIX
    return TRUE;
}
#else
BOOL _init_ascii() {
    fprintf( stderr, "init: No init_ascii function enabled" );
    return FALSE;
}
#endif //EN_US

void _initialize() {
    if (defaults && defaults->display) {
        return;
    }
    if (!init_defaults()) {
        fprintf(stderr, "Failure initialize defaults\n");
        return;
    }
    if (!_init_ascii()) {
        fprintf(stderr, "Failure to initialize ascii keys\n");
        return;
    }
}

BOOL init_defaults() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "init: Unable to get default display\n");
        return FALSE;
    }
    defaults = malloc(sizeof(xautpy_defaults_t));
    if (defaults == NULL) {
        fprintf(stderr, "init: Unable to allocate space for defaults");
        return FALSE;
    }
    memset(defaults, 0, sizeof(xautpy_defaults_t));
    defaults->display = display;
    int defScreen = XDefaultScreen(defaults->display);
    defaults->dsp_width = XDisplayWidth(defaults->display, defScreen);
    defaults->dsp_height = XDisplayHeight(defaults->display, defScreen);
    defaults->mouse_move_delay = 10;
    defaults->mouse_down_delay = 10;
    defaults->mouse_click_delay = 10;
    defaults->key_down_delay = 10;
    defaults->key_click_delay = 5;
    defaults->log_level = LOG_LEVEL_NONE;
    defaults->interpret_meta_symbols = TRUE;
    return TRUE;
}

BOOL _is_supported_feature(char *feature) {
    Atom type = 0;
    long item_count = 0L;
    int size = 0;
    Atom *results = NULL;
    long i = 0;

    Atom request = XInternAtom(defaults->display, "_NET_SUPPORTED", FALSE);
    Atom feature_atom = XInternAtom(defaults->display, feature, FALSE);
    Window root = RootWindow(defaults->display, 0);

    results = (Atom *) _window_property(
        root, request, &item_count, &type, &size);
    BOOL ret = FALSE;
    for (i = 0L; i < item_count; i++) {
        if (results[i] == feature_atom)
            ret = TRUE;
    }
    XFree(results);
    return ret;
}

char* _stringmid(const char *str, size_t first, size_t last) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str);
    if (len == 0 || len < first || last < first) {
        return NULL;
    }
    if (len < last) {
        last = len;
    }
    size_t count = last - first + 1;
    char *ret = malloc((count + 1) * sizeof(char));
    if (ret == NULL) {
        fprintf(stderr, "Unable to create string of %zu characters\n",
                (count + 1));
        return NULL;
    }
    memset(ret, '\0', (count + 1) * sizeof(char));
    ret = strncpy(ret, str + first, count);
    return ret;
}

void _normalize_coords(int *x, int *y) {
    if (!_check_init()) {
        return;
    }
    if (*x < 0) {
        *x = 0;
    } else if (*x > defaults->dsp_width) {
        x = &defaults->dsp_width;
    }
    if (*y < 0) {
        *y = 0;
    } else if (*y > defaults->dsp_height) {
        y = &defaults->dsp_height;
    }
    logit(LOG_LEVEL_VERBOSE, "Final mouse location %d X %d\n", *x, *y);
}

BOOL _normalize_wincoords(int *x, int *y, unsigned long win) {
    if (!_check_init()) {
        return FALSE;
    }
    XWindowAttributes *attribs = calloc(1, sizeof(XWindowAttributes));
    if(attribs == NULL) {
        fprintf(stderr, "Unable to allocate space for XWindowAttributes");
        return FALSE;
    }
    XSetErrorHandler(_invalid_window_error_handler);
    int status = XGetWindowAttributes(defaults->display, win, attribs);
    XSetErrorHandler(NULL);
    BOOL ret;
    if (status) {
        int winx = window_x(win);
        int winy = window_y(win);
        *x = winx + *x;
        *y = winy + *y;
        logit(LOG_LEVEL_VERBOSE, "Window at %d X %d\n", winx, winy);
        logit(LOG_LEVEL_VERBOSE, "Relative location %d X %d\n", *x, *y);
        ret = TRUE;
    } else {
        fprintf(stderr, "Window handle %lu appears to be invalid\n", win);
        fprintf(stderr, "Not moving the mouse\n");
        *x = mouse_x(0);
        *y = mouse_y(0);
        ret = FALSE;
    }
    free(attribs);
    //In case the window is partially off the screen
    _normalize_coords(x, y);
    return ret;
}

/* Typically we don't need to do anything other than return
 * a status of zero.  That is sufficient for these functions
 */
int _invalid_window_error_handler(Display *dsp, XErrorEvent *err) {
    return 0;
}
