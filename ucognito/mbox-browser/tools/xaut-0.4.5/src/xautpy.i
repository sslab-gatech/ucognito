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
 $URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xautpy.i $
 $Author: chris_parker $
 $Date: 2012-09-22 17:17:57 -0700 (Sat, 22 Sep 2012) $
 $Rev: 29 $
*/

%module xautpy

//Maps a pointer to a null terminated array of unsigned longs
%typemap(out) unsigned long* {
    int i;
    int len = 0;
    while($1[len]) {
        len++;
    }
    $result = PyList_New(len);
    for(i = 0; i < len; i++) {
        PyObject *o = PyLong_FromLong((long)$1[i]);
        PyList_SetItem($result, i, o);
    }
}

%{
/** Found in xaut.h **/
extern void cleanup();
extern void verbose(unsigned short);
extern void extra_verbose(unsigned short);
extern unsigned short init_defaults();

/** Found in xaut_clipboard.h **/
extern char* get_primary();
extern char* get_secondary();
extern char* get_clipboard();
extern void put_primary(char*);
extern void put_secondary(char*);
extern void put_clipboard(char*);

/** Found in xaut_display.h **/
extern int display_h();
extern int display_w();
extern long desktop_count();
extern long desktop(long);

/** Found in xaut_keyboard.h  **/
extern short key_down_delay(short);
extern short key_click_delay(short);
extern unsigned short key_click(unsigned int);
extern unsigned short key_down(unsigned int);
extern unsigned short key_up(unsigned int);
extern void interpret_meta_symbols(unsigned short);
extern void print_keycodes();
extern unsigned short type(char*);

/** Found in xaut_mouse.h **/
extern short mouse_move_delay(short);
extern short mouse_down_delay(short);
extern short mouse_click_delay(short);
extern unsigned short mouse_click(unsigned short, unsigned short);
extern unsigned short mouse_down(unsigned short);
extern unsigned short move_mouse(int, int, unsigned long);
extern unsigned short mouse_up(unsigned short);
extern int mouse_x(unsigned long);
extern int mouse_y(unsigned long);

/** Found in xaut_window.h **/
extern unsigned short activate_window(unsigned long);
extern unsigned long active_window();
extern unsigned long find_outer_parent(unsigned long);
extern unsigned long find_window(char*);
extern unsigned long* search_for_window(char*);
extern unsigned short maximize_window(unsigned long, unsigned short);
extern unsigned short maximize_window_horz(unsigned long, unsigned short);
extern unsigned short maximize_window_vert(unsigned long, unsigned short);
extern unsigned short minimize_window(unsigned long, unsigned short);
extern unsigned short iconify_window(unsigned long, unsigned short);
extern unsigned short full_screen_window(unsigned long, unsigned short);
extern unsigned short shade_window(unsigned long, unsigned short);
extern unsigned short restore_window(unsigned long);
extern unsigned short move_window(unsigned long, int, int, long);
extern unsigned short resize_window(unsigned long, unsigned int, unsigned int);
extern int window_x(unsigned long);
extern int window_y(unsigned long);
extern int window_w(unsigned long);
extern int window_h(unsigned long);
extern char* window_name(unsigned long);
extern long window_desktop(unsigned long);
extern unsigned short is_valid(unsigned long);

%}

/** Found in xaut.h **/
extern void cleanup();
extern void verbose(unsigned short);
extern void extra_verbose(unsigned short);
extern unsigned short init_defaults();

/** Found in xaut_clipboard.h **/
extern char* get_primary();
extern char* get_secondary();
extern char* get_clipboard();
extern void put_primary(char*);
extern void put_secondary(char*);
extern void put_clipboard(char*);

/** Found in xaut_display.h **/
extern int display_h();
extern int display_w();
extern long desktop_count();
extern long desktop(long);

/** Found in xaut_keyboard.h  **/
extern short key_down_delay(short);
extern short key_click_delay(short);
extern unsigned short key_click(unsigned int);
extern unsigned short key_down(unsigned int);
extern unsigned short key_up(unsigned int);
extern void interpret_meta_symbols(unsigned short);
extern void print_keycodes();
extern unsigned short type(char*);

/** Found in xaut_mouse.h **/
extern short mouse_move_delay(short);
extern short mouse_down_delay(short);
extern short mouse_click_delay(short);
extern unsigned short mouse_click(unsigned short, unsigned short);
extern unsigned short mouse_down(unsigned short);
extern unsigned short move_mouse(int, int, unsigned long);
extern unsigned short mouse_up(unsigned short);
extern int mouse_x(unsigned long);
extern int mouse_y(unsigned long);

/** Found in xaut_window.h **/
extern unsigned short activate_window(unsigned long);
extern unsigned long active_window();
extern unsigned long find_outer_parent(unsigned long);
extern unsigned long find_window(char*);
extern unsigned long* search_for_window(char*);
extern unsigned short maximize_window(unsigned long, unsigned short);
extern unsigned short maximize_window_horz(unsigned long, unsigned short);
extern unsigned short maximize_window_vert(unsigned long, unsigned short);
extern unsigned short minimize_window(unsigned long, unsigned short);
extern unsigned short iconify_window(unsigned long, unsigned short);
extern unsigned short full_screen_window(unsigned long, unsigned short);
extern unsigned short shade_window(unsigned long, unsigned short);
extern unsigned short restore_window(unsigned long);
extern unsigned short move_window(unsigned long, int, int, long);
extern unsigned short resize_window(unsigned long, unsigned int, unsigned int);
extern int window_x(unsigned long);
extern int window_y(unsigned long);
extern int window_w(unsigned long);
extern int window_h(unsigned long);
extern char* window_name(unsigned long);
extern long window_desktop(unsigned long);
extern unsigned short is_valid(unsigned long);
