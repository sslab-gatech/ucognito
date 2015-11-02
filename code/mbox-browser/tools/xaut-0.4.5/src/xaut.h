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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut.h $
$Author: chris_parker $
$Date: 2012-07-28 14:41:37 -0700 (Sat, 28 Jul 2012) $
$Rev: 23 $
*/

#ifndef XAUT_H_
#define XAUT_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <limits.h>

#include "xaut_clipboard.h"
#include "xaut_types.h"
#include "xaut_display.h"
#include "xaut_keyboard.h"
#include "xaut_mouse.h"
#include "xaut_window.h"


// --- Published subroutines, functions and structures

/**
 * Releases any resources created by this code.
 */
void cleanup();

/**
 * Print information about what's going on.
 */
void verbose(BOOL verbose);
void extra_verbose(BOOL extra_verbose);

// --- End of published stuff

/**
 * Add a code to the list of ascii codes.
 * @param code the ascii code to add
 * @param shifted does the shift key need to be down to reach this
 */
void _add_code(unsigned short code, BOOL shifted);

/**
 * Checks to see if init() has been called, and calls it if not.
 * Will return false only if init() cannot initialize values.  So
 * can be used as a simple test.
 */
BOOL _check_init();

/**
 * Initializes the program.
 */
void _initialize();

/**
 * Initializes the list of allowed ascii characters.
 * These represent any character which can be sent directly, as opposed
 * to requiring being sent in a brace.  So it's possible to send e.g. 'A',
 * but not the control key.  That would require "{Control_L}".
 * Note that the following keys have special meaning: !^+{}.  Respectively
 * they mean "Alt_L", "Control_L", "Shift_L", and "whatever is inside
 * is a special key code".
 * Note that this has some overlap with init_keys().
 */
BOOL _init_ascii();

/**
 * Initializes only the defaults.
 * The structure "defaults" is filled with values.
 * @return boolean indication of success.
 */
BOOL init_defaults();

/**
 * Indication of whether the requested feature is available.
 * Not all implementations of X11 support all possible things.
 * This helps keep things portable.
 * @param the feature to find
 * @return boolean indication of whether the requested feature is possible.
 */
BOOL _is_supported_feature(char *feature);

/**
 * Performs screen coordinate normalization.
 * Normalizes the coordinates supplied so that
 * they are at least equal to zero, and
 * no greater than the maximum coordinate.
 * Prevents stupid things from happening if e.g.
 * mouse_move( 10000, 10000 ) is called.
 * @param x the x coordinate to normalize
 * @param y the y coordinate to normalize
 */
void _normalize_coords(int *x, int *y);

/**
 * Performs screen coordinate normalization as regards a window.
 * Normalizes the coordinates such that they make sense with the
 * window that is identified by the window handle.  For example,
 * if the window is at 100x100, and the coordinates supplied to mouse_move are
 * -10, -10, then the mouse will be moved to 90x90.  If the
 * coordinates supplied for the same window are 10, 10, then the
 * mouse will be moved to 110x110.  Note that if the window
 * handle is not valid, the coordinates will be normalized to the
 * whole screen using <code>normalize_coords</code>.
 *
 * Note that if the window is partially off the screen, the mouse
 * will move as close as possible to the desired location.  The
 * mouse will not move off the screen.
 * @param x the x coordinate to normalize
 * @param y the y coordinate to normalize
 * @param win a handle to use as a reference point
 * @return will return FALSE if window handle passed is invalid
 */
BOOL _normalize_wincoords(int *x, int *y, unsigned long win);

/**
 * Extracts a substring from a string.
 * <ul>Notes:
 * <li>You <em>must</em> release the substring when done.</li>
 * <li>The values for begin and end are inclusive.<li>
 * <li>The function will return NULL if the string parameter is NULL.</li>
 * <li>The function will return NULL if the string is empty.</li>
 * <li>The function will return NULL if the first parameter is greater
 *      than the length of the string.</li>
 * <li>The end parameter will be reduced to the last character in the
 *      string if end is greater than the end of the string.</li>
 * </ul>
 *
 * @param str the string
 * @param first the first character to extract
 * @param last the last character to extract
 * @return the substring, or null if something went wrong.
 */
char* _stringmid(const char *str, size_t first, size_t last);

/**
 * Called by X in the event of an error.
 * X is notified that it should use this function usually just
 * before a user provided window handle is used.  There are a
 * number of reasons such a call might fail.  It may be that
 * the user closed the window, it may be that a transient
 * window has closed by itself.  Whatever.  The point
 * is that without an error handler, the entire program will
 * exit.
 */
int _invalid_window_error_handler(Display *dsp, XErrorEvent *err);

#endif /* XAUT_H_ */
