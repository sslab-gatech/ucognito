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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_types.h $
$Author: chris_parker $
$Date: 2010-05-01 17:17:37 -0700 (Sat, 01 May 2010) $
$Rev: 5 $
*/

#ifndef XAUT_TYPES_H_
#define XAUT_TYPES_H_
#include <regex.h>

/**
 * Structure of global values.
 * <ul>
 * <li>display is a pointer to the current display.</li>
 * <li>dsp_width is a constant value of the display width.</li>
 * <li>dsp_height is a constant value of the display height.</li>
 *
 * <li>mouse_move_delay is how long to wait between at each pixel movement in milliseconds.
 *      0 means move the mouse instantly.  10 is the default, and moves the mouse fairly
 *      rapidly.  100 is the maximum, and moves the mouse very slowly.</li>
 * <li>mouse_down_delay is how long the mouse button stays down when "clicked" in milliseconds</li>
 * <li>mouse_click_delay is the number of milliseconds delay between clicks in a multi-click event</li>
 * <li>key_down_delay is how long a key stays down in milliseconds when "pressed"</li>
 * <li>key_click_delay is the number of milliseconds between keystrokes when multiple keys are sent</li>
 * <li>interpret_meta_symbols is a boolean indication of whether to interpret
 *      key meta symbols like !#^+{}.</li>
 * <li>window_switch_command the keystroke command combination used to
 *      switch windows (defaults to !{Tab})</li>
 *
 * <li>verbose output status messages</li>
 * <li>extra_verbose output LOTS of messages</li>
 * </ul>
 */
typedef struct {
    // These remain constant while program in use
    Display *display;
    int dsp_width;
    int dsp_height;

    //These can be changed by the user
    unsigned short mouse_move_delay;
    unsigned short mouse_down_delay;
    unsigned short mouse_click_delay;
    unsigned short key_down_delay;
    unsigned short key_click_delay;
    BOOL interpret_meta_symbols;
    unsigned short log_level;
} xautpy_defaults_t;

/**
 * A structure of ascii codes which can be sent directly.
 * This is basically anything that can be typed at the keyboard
 * without using any strange Vulcan neck pinch key stroke combos.
 * For example "a" can be sent, while the Unicode symbol for the
 * British pound sign cannot (at least not on EN_US keyboards).
 *
 * If it is necessary to use a Vulcan neck pinch key combo to put the
 * character in a document, then that key combo should be simulated
 * using {KEY_SYMBOL} methodology.
 */
typedef struct {
    char ascii;
    KeyCode code;
    BOOL shifted;
} xautpy_ascii_t;

/**
 * A structure which contains information about an extracted meta key.
 * The intention is that the meta key will be between braces.  The
 * count will be between the braces, after a space.  So for example the
 * meta entry {Tab 3} means three tabs.  Note that the name is case
 * sensitive.
 */
typedef struct {
    char *name;
    KeyCode keycode;
    KeySym keysym;
    BOOL shifted;
    unsigned short count;
} xautpy_meta_t;

/**
 * Contains the parameters for recursive window searches.
 * Used only internally.  Makes it easier to have
 * some values remain constant through a recursive
 * function.<ul>
 * <li>current - the current window we are looking at</li>
 * <li>buffer_size - the number of elements the buffer can hold</li>
 * <li>found - the nuber of elements found</li>
 * <li>buffer - a buffer of windows which meet our search criteria</li>
 */
typedef struct {
    Window current;
    unsigned int buffer_size;
    unsigned int found;
    char *title;
    regex_t **regx;
    Window *buffer;
} _window_search;

//Used internally so that I don't have a bunch of "1000"s around
static const int MILLI_MULTIPLIER = 1000;

// Globals - not published
xautpy_defaults_t *defaults;
xautpy_ascii_t *ascii_codes;

// Codes for meta keys
KeySym SHIFT_L;
KeySym CONTROL_L;
KeySym ALT_L;
KeySym META_L;

#endif /* XAUT_TYPES_H_ */
