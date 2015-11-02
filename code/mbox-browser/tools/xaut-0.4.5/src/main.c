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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/main.c $
$Author: chris_parker $
$Date: 2012-09-22 17:17:57 -0700 (Sat, 22 Sep 2012) $
$Rev: 29 $
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <assert.h>

#include "xaut.h"

char *testing = "Testing %s\n";
char *done = "Finished testing %s\n";
//The following are tests - plus they enable me to
// test the program with valgrind

const int CapsLock = 2, NumLock = 16, ScrollLock = 1;

void setLeds(int leds) {
    Display *dpy = XOpenDisplay(0);
    XKeyboardControl values;
    values.led_mode = leds & ScrollLock ? LedModeOn : LedModeOff;
    values.led = 3;
    XChangeKeyboardControl(dpy, KBLedMode, &values);
    XkbLockModifiers(dpy, XkbUseCoreKbd, CapsLock | NumLock, leds & (CapsLock | NumLock) );
    XFlush(dpy);
    XCloseDisplay(dpy);
}

void TEST_cleanup() {
    char *routine = "TEST_cleanup";
    printf(testing, routine);
    assert(_check_init()); //In case this hasn't been called
    assert(defaults);
    cleanup();
    assert(defaults == NULL);
    assert(_check_init()); //So that I don't have to call this elsewhere
    printf(done, routine);
}
void TEST_is_valid() {
    Window win = active_window();
    assert(is_valid(win));
    assert(is_valid(20) == FALSE);
}
void TEST_verbose() {
    char *routine = "TEST_verbose";
    printf(testing, routine);
    verbose(FALSE);
    assert(!(defaults->log_level & LOG_LEVEL_VERBOSE));
    verbose(TRUE);
    assert(defaults->log_level & LOG_LEVEL_VERBOSE);
    verbose(FALSE);
    printf(done, routine);
}
void TEST_extra_verbose() {
    char *routine = "TEST_extra_verbose";
    printf(testing, routine);
    extra_verbose(FALSE);
    assert(!(defaults->log_level & LOG_LEVEL_EXTRA_VERBOSE));
    extra_verbose(TRUE);
    assert(defaults->log_level & LOG_LEVEL_EXTRA_VERBOSE);
    verbose(FALSE);
    extra_verbose(FALSE);
    printf(done, routine);
}
void TEST_init_defaults() {
    char *routine = "TEST_init_defaults";
    printf(testing, routine);
    assert(init_defaults());
    printf(done, routine);
}
void TEST_display_h() {
    char *routine = "TEST_display_h";
    printf(testing, routine);
    init_defaults();
    assert(defaults->dsp_height);
    printf(done, routine);
}
void TEST_display_w() {
    char *routine = "TEST_display_w";
    printf(testing, routine);
    init_defaults();
    assert(defaults->dsp_width);
    printf(done, routine);
}
void TEST_desktop_count() {
    char *routine = "TEST_desktop_count";
    printf(testing, routine);
    assert(desktop_count());
    printf(done, routine);
}
void TEST_get_current_desktop() {
    char *routine = "TEST_get_current_desktop";
    printf(testing, routine);
    assert(get_current_desktop());
    printf(done, routine);
}
void TEST_set_current_desktop() {
    char *routine = "TEST_set_current_desktop";
    printf(testing, routine);
    long max = desktop_count();
    if(max == 1) {
        return;
    }
    long before = get_current_desktop();
    if(before < max) {
        assert(set_current_desktop(max));
        sleep(1);
        assert(desktop_count() == get_current_desktop());
    } else {
        assert(set_current_desktop(max -1));
        assert(desktop_count() != get_current_desktop());
    }
    set_current_desktop(before);
    printf(done, routine);
}
void TEST_key_down_delay() {
    char *routine = "TEST_key_down_delay";
    printf(testing, routine);
    unsigned short before = defaults->key_down_delay;
    key_down_delay(before + 5);
    unsigned short after = defaults->key_down_delay;
    assert(before != after);
    key_down_delay(before);
    printf(done, routine);
}
void TEST_key_click_delay() {
    char *routine = "TEST_key_click_delay";
    printf(testing, routine);
    unsigned short before = defaults->key_click_delay;
    key_click_delay(before + 5);
    unsigned short after = defaults->key_click_delay;
    assert(before != after);
    key_click_delay(before);
    printf(done, routine);
}
void TEST_key_click() {
    char *routine = "TEST_key_click";
    printf(testing, routine);
    //"36" happens to be the code for "Return" on my computer
    assert(key_click(36));
    printf(done, routine);
}
void TEST_key_down() {
    char *routine = "TEST_key_down";
    printf(testing, routine);
    //It's necessary to send both down and up commands
    //"100" happens to be the code for "Left" on my computer
    assert(key_down(100));
    assert(key_up(100));
    printf(done, routine);
}
void TEST_key_up() {
    char *routine = "TEST_key_up";
    printf(testing, routine);
    //It's necessary to send both down and up commands
    //"100" happens to be the code for "Left" on my computer
    assert(key_down(100));
    assert(key_up(100));
    printf(done, routine);
}
void TEST_interpret_meta_symbols() {
    char *routine = "TEST_interpret_meta_symbols";
    printf(testing, routine);
    unsigned short before = defaults->interpret_meta_symbols;
    unsigned short neu = before ? FALSE : TRUE;
    interpret_meta_symbols(neu);
    unsigned short after = defaults->interpret_meta_symbols;
    assert(before != after);
    interpret_meta_symbols(before);
    printf(done, routine);
}
void TEST_print_keycodes() {
    char *routine = "TEST_print_keycodes";
    printf(testing, routine);
    //I can't think of anything to "test".  This is here
    //to give valgrind something to look at while checking
    //for memory leaks.
    print_keycodes();
    printf(done, routine);
}
void TEST_type() {
    char *routine = "TEST_type";
    printf(testing, routine);
    assert(type("{Return}"));
    printf(done, routine);
}
void TEST_mouse_move_delay() {
    char *routine = "TEST_mouse_move_delay";
    printf(testing, routine);
    unsigned short before = defaults->mouse_move_delay;
    mouse_move_delay(before + 5);
    unsigned short after = defaults->mouse_move_delay;
    assert(before != after);
    mouse_move_delay(before);
    printf(done, routine);
}
void TEST_mouse_down_delay() {
    char *routine = "TEST_mouse_down_delay";
    printf(testing, routine);
    unsigned short before = defaults->mouse_down_delay;
    mouse_down_delay(before + 5);
    unsigned short after = defaults->mouse_down_delay;
    assert(before != after);
    mouse_down_delay(before);
    printf(done, routine);
}
void TEST_mouse_click_delay() {
    char *routine = "TEST_mouse_click_delay";
    printf(testing, routine);
    unsigned short before = defaults->mouse_click_delay;
    mouse_click_delay(before + 5);
    unsigned short after = defaults->mouse_click_delay;
    assert(before != after);
    mouse_click_delay(before);
    printf(done, routine);
}
void TEST_mouse_click() {
    char *routine = "TEST_mouse_click";
    printf(testing, routine);
    assert(mouse_click(1, 1));
    printf(done, routine);
}
void TEST_mouse_down() {
    char *routine = "TEST_mouse_down";
    printf(testing, routine);
    //It is necessary to include both down and up
    assert(mouse_down(1));
    assert(mouse_up(1));
    printf(done, routine);
}
void TEST_mouse_up() {
    char *routine = "TEST_mouse_up";
    printf(testing, routine);
    //It is necessary to include both down and up
    assert(mouse_down(1));
    assert(mouse_up(1));
    printf(done, routine);
}
void TEST_move_mouse() {
    char *routine = "TEST_move_mouse";
    printf(testing, routine);
    //First we test with no window
    int before_x = mouse_x(0);
    int before_y = mouse_y(0);

    //Let's exercise a bug recently found by Beat Master Bob.  The
    //bug is manifested when the mouse is moved by exactly one
    //pixel in both the x and y directions simultaneously.
    assert(move_mouse(before_x + 1, before_y + 1, 0));

    assert(move_mouse(0, 0, 0));
    int after_x = mouse_x(0);
    int after_y = mouse_y(0);
    assert(after_x == 0 && after_y == 0);

    //Now we try it with a valid window
    Window win = active_window();
    activate_window(win);
    assert(move_mouse(10, 10, win));
    after_x = mouse_x(win);
    after_y = mouse_y(win);
    assert(after_x == 10 && after_y == 10);

    //Finally, let's make sure it doesn't bomb with an invalid window
    //The correct action is to leave the mouse where it is
    assert(move_mouse(20, 20, 20) == FALSE);
    assert(mouse_x(0) != 20 && mouse_y(0) != 20);

    move_mouse(before_x, before_y, 0);
    printf(done, routine);
}
void TEST_mouse_x() {
    char *routine = "TEST_mouse_x";
    printf(testing, routine);

    //There's no way to know if the values we receive are "correct",
    //so we can only test to make sure it doesn't cause problems

    //First with no window
    mouse_x(0);

    //Then with a valid window
    Window win = active_window();
    mouse_x(win);

    //Finally with an invalid window - which should return INT_MIN
    int invalid_x = mouse_x(20);
    assert(invalid_x == INT_MIN);
    printf(done, routine);
}
void TEST_mouse_y() {
    char *routine = "TEST_mouse_y";
    printf(testing, routine);

    //There's no way to know if the values we receive are "correct",
    //so we can only test to make sure it doesn't cause problems

    //First with no window
    mouse_y(0);

    //Then with a valid window
    Window win = active_window();
    mouse_y(win);

    //Finally with an invalid window - which should return INT_MIN
    int invalid_y = mouse_y(20);
    assert(invalid_y == INT_MIN);
    printf(done, routine);
}
void TEST_activate_window() {
    char *routine = "TEST_activate_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    BOOL tf = activate_window(win);
    if(! tf) {
        fprintf(stderr, "WARNING: Window may not have activated properly.\n"
                "This may not be a valid test.\n");
    }

    //Then with an invalid window
    assert(activate_window(20) == FALSE);
    printf(done, routine);
}
void TEST_active_window() {
    char *routine = "TEST_active_window";
    printf(testing, routine);
    assert(active_window());
    printf(done, routine);
}
void TEST_find_window() {
    char *routine = "TEST_find_window";
    printf(testing, routine);
    //Note that this fails when run in Eclipse.
    //Apparently it also fails in "terminal"
    Window active = active_window();
    char *name = window_name(active);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }
    Window found = find_window(name);
    assert(found == active);
    free(name);
    printf(done, routine);
}
void TEST_search_for_window() {
    char *routine = "TEST_search_for_window";
    printf(testing, routine);
    Window active = active_window();
    char *name = window_name(active);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }
    Window* found = search_for_window(name);
    //Note that this fails when run in Eclipse...
    assert(found);
    free(name);
    free(found);
    printf(done, routine);
}
void TEST_maximize_window() {
    char *routine = "TEST_maximize_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(maximize_window(win, TRUE));
    assert(maximize_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(maximize_window(20, TRUE) == FALSE);
    printf(done, routine);
}
void TEST_maximize_window_horz() {
    char *routine = "TEST_maximize_horz";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(maximize_window_horz(win, TRUE));
    assert(maximize_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(maximize_window_horz(20, TRUE) == FALSE);
    printf(done, routine);
}
void TEST_maximize_window_vert() {
    char *routine = "TEST_maximize_vert";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(maximize_window_vert(win, TRUE));
    assert(maximize_window_vert(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(maximize_window_vert(20, TRUE) == FALSE);
    printf(done, routine);
}
void TEST_minimize_window() {
    char *routine = "TEST_minimize_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(minimize_window(win, TRUE));
    assert(minimize_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(minimize_window(20, TRUE) == FALSE);
    assert(minimize_window(20, FALSE) == FALSE);
    printf(done, routine);
}
void TEST_iconify_window() {
    char *routine = "TEST_iconify_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(iconify_window(win, TRUE));
    assert(iconify_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(iconify_window(20, TRUE) == FALSE);
    assert(iconify_window(20, FALSE) == FALSE);
    printf(done, routine);
}
void TEST_full_screen_window() {
    char *routine = "TEST_full_screen_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(full_screen_window(win, TRUE));
    assert(full_screen_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(full_screen_window(20, TRUE) == FALSE);
    assert(full_screen_window(20, FALSE) == FALSE);
    printf(done, routine);
}
void TEST_shade_window() {
    char *routine = "TEST_shade_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(shade_window(win, TRUE));
    assert(shade_window(win, FALSE));
    activate_window(win);

    //Then with an invalid window
    assert(shade_window(20, TRUE) == FALSE);
    assert(shade_window(20, FALSE) == FALSE);
    printf(done, routine);
}
void TEST_restore_window() {
    char *routine = "TEST_restore_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    assert(restore_window(win));
    activate_window(win);

    //Then with an invalid window
    assert(restore_window(20) == FALSE);
    printf(done, routine);
}
void TEST_move_window() {
    char *routine = "TEST_move_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    char *name = window_name(win);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }
    int before_x = window_x(win);
    int before_y = window_y(win);
    assert(move_window(win, before_x + 10, before_y + 10, -1));
    int after_x = window_x(win);
    int after_y = window_y(win);

    //"Snap to grid" makes this a bit hard to test.
    //What if the grid isn't "10"?  If it's e.g. 12,
    //then we'll be off by 2 pixels.
    assert(before_x != after_x);
    assert(before_y != after_y);
    move_window(win, before_x, before_y, -1);
    activate_window(win);

    //Now with an invalid window
    assert(move_window(20, 0, 0, -1) == FALSE);
    printf(done, routine);
}
void TEST_resize_window() {
    char *routine = "TEST_resize_window";
    printf(testing, routine);
    //First with a valid window
    Window win = active_window();
    char *name = window_name(win);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }

    int before_w = window_w(win);
    int before_h = window_h(win);
    assert(resize_window(win, before_w + 10, before_h + 10));
    int after_w = window_w(win);
    int after_h = window_h(win);

    assert(before_w != after_w);
    assert(before_h != after_h);
    resize_window(win, before_w, before_h);
    activate_window(win);


    //Now with an invalid window
    assert(resize_window(20, 10, 10) == FALSE);
    printf(done, routine);
}
void TEST_window_x() {
    char *routine = "TEST_window_x";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    int x = window_x(win);
    assert(x > INT_MIN);

    //Then an invalid window
    assert(window_x(20) == INT_MIN);
    printf(done, routine);
}
void TEST_window_y() {
    char *routine = "TEST_window_y";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    int y = window_y(win);
    assert(y > INT_MIN);

    //Then an invalid window
    assert(window_y(20) == INT_MIN);
    printf(done, routine);
}
void TEST_window_w() {
    char *routine = "TEST_window_w";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    int w = window_w(win);
    assert(w > INT_MIN);

    //Then an invalid window
    assert(window_w(20) == INT_MIN);
    printf(done, routine);
}
void TEST_window_h() {
    char *routine = "TEST_window_h";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    int h = window_h(win);
    assert(h > INT_MIN);

    //Then an invalid window
    assert(window_h(20) == INT_MIN);
    printf(done, routine);
}
void TEST_window_name() {
    char *routine = "TEST_window_name";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    char *name = window_name(win);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }
    //This fails when run from Eclipse
    assert(name);
    free(name);

    //Now an invalid window
    char *bad = window_name(20);
    assert(bad == NULL);
    free(bad);  //Just in case it's not null
    printf(done, routine);
}
void TEST_window_desktop() {
    char *routine = "TEST_window_desktop";
    printf(testing, routine);
    //First a valid window
    Window win = active_window();
    char *name = window_name(win);
    if(name == NULL) {
        printf("Aborting %s - conditions are invalid\n", routine);
        return;
    }
    long desk = window_desktop(win);
    assert(desk > -1);

    //Then an invalid window
    long bad = window_desktop(20);
    assert(bad == LONG_MIN);
    printf(done, routine);
}
void TEST_get_primary() {
    char *routine = "TEST_get_primary";
    printf(testing, routine);
    char* clip = get_primary();
    assert(strcmp(routine, (char*)clip) == 0);
    printf(done, routine);
}
void TEST_get_secondary() {
    char *routine = "TEST_get_secondary";
    printf(testing, routine);
    char* clip = get_secondary();
    assert(strcmp(routine, (char*)clip) == 0);
    printf(done, routine);
}
void TEST_get_clipboard() {
    char *routine = "TEST_get_clipboard";
    printf(testing, routine);
    char* clip = get_clipboard();
    assert(strcmp(routine, (char*)clip) == 0);
    printf(done, routine);
}

int main(int argc, char* argv[]) {
    Window win = active_window();
    printf("Running validation tests...\n");
    printf("For best results, run this from a prompt in a window that's");
    printf(" on it's own desktop.\n");
    printf("Also, make sure the mouse starts in the middle");
    printf(" of the title bar.\n");

    sleep(5);

    TEST_cleanup();
    TEST_is_valid();
    TEST_verbose();
    TEST_extra_verbose();
    TEST_init_defaults();
    TEST_display_h();
    TEST_display_w();
    TEST_desktop_count();
    TEST_get_current_desktop();
    TEST_set_current_desktop();
    sleep(1);
    TEST_key_down_delay();
    TEST_key_click_delay();
    TEST_key_click();
    sleep(1);
    TEST_key_down();
    TEST_key_up();
    sleep(1);
    TEST_interpret_meta_symbols();
    TEST_print_keycodes();
    TEST_type();
    sleep(1);
    TEST_mouse_move_delay();
    TEST_mouse_down_delay();
    TEST_mouse_click_delay();
    TEST_mouse_click();
    activate_window(win);
    sleep(1);
    TEST_mouse_down();
    activate_window(win);
    sleep(1);
    TEST_mouse_up();
    activate_window(win);
    sleep(1);
    TEST_move_mouse();
    TEST_mouse_x();
    TEST_mouse_y();
    sleep(1);
    TEST_activate_window();
    sleep(1);
    TEST_active_window();
    sleep(1);
    TEST_find_window();
    sleep(1);
    TEST_search_for_window();
    sleep(1);
    TEST_maximize_window();
    sleep(1);
    TEST_maximize_window_horz();
    sleep(1);
    TEST_maximize_window_vert();
    sleep(1);
    TEST_minimize_window();
    sleep(1);
    TEST_iconify_window();
    sleep(1);
    TEST_full_screen_window();
    sleep(1);
    TEST_shade_window();
    sleep(1);
    TEST_restore_window();
    sleep(1);
    TEST_move_window();
    sleep(1);
    TEST_resize_window();
    TEST_window_x();
    TEST_window_y();
    TEST_window_w();
    TEST_window_h();
    TEST_window_name();
    TEST_window_desktop();
    sleep(1);
    put_primary("TEST_get_primary");
    sleep(1);
    TEST_get_primary();
    put_secondary("TEST_get_secondary");
    sleep(1);
    TEST_get_secondary();
    put_clipboard("TEST_get_clipboard");
    sleep(1);
    TEST_get_clipboard();

    cleanup();
	return 0;
}
