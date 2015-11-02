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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_mouse.c $
$Author: chris_parker $
$Date: 2012-03-11 09:40:18 -0700 (Sun, 11 Mar 2012) $
$Rev: 20 $
*/

#include "xaut_mouse.h"

//Modifiers for defaults
short mouse_move_delay(short delay) {
    if (!_check_init()) {
        return -1;
    }
    if(delay < 0) {
        return defaults->mouse_move_delay;
    }
    if(defaults->mouse_move_delay > 0) {
        char *msg = "The mouse used to pause at each pixel for %hu"
            " milliseconds\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_move_delay);
    } else {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s", "The mouse used to move instantly");
    }
    defaults->mouse_move_delay = delay;
    if(defaults->mouse_move_delay > 0) {
        char *msg = "The mouse now pauses at each pixel for %hu"
            " milliseconds\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_move_delay);
    } else {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s", "The mouse now moves instantly");
    }
    return defaults->mouse_move_delay;
}

short mouse_down_delay(short delay) {
    if(!_check_init()) {
        return -1;
    }
    if(delay < 1) {
        return defaults->mouse_down_delay;
    }
    {
        char *msg = "The mouse button used to stay down for %hu"
            " milliseconds when clicked\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_down_delay);
    }
    defaults->mouse_down_delay = delay;
    {
        char *msg = "The mouse button now stays down for %hu"
            " milliseconds when clicked\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_down_delay);
    }
    return defaults->mouse_down_delay;
}

short mouse_click_delay(short delay) {
    if(!_check_init()) {
        return -1;
    }
    if(delay < 1) {
        return defaults->mouse_click_delay;
    }
    {
        char *msg = "The mouse button used to pause for %hu"
            " milliseconds between clicks\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_down_delay);
    }
    defaults->mouse_click_delay = delay;
    {
        char *msg = "The mouse button now pauses for %hu"
            " milliseconds between clicks\n";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, defaults->mouse_down_delay);
    }
    return defaults->mouse_click_delay;
}

BOOL mouse_click(unsigned short btn, unsigned short count) {
    if(btn < 1) {
        fprintf(stderr,
            "The first mouse button is #1, clicking '0' will not work\n");
        return FALSE;
    }
    if (!_check_init()) {
        return FALSE;
    }
    BOOL success = TRUE;
    unsigned short i;
    for (i = 0; i < count; i++) {
        success = mouse_down(btn);
        logit(LOG_LEVEL_VERBOSE, "Mouse button %d down ", btn);
        if (!success) {
            mouse_up(btn); //Just in case
            break;
        }
        usleep(MILLI_MULTIPLIER * defaults->mouse_down_delay);
        success = mouse_up(btn);
        logit(LOG_LEVEL_VERBOSE, "Mouse button %d up\n", btn);
        if (i < count - 1) {
            usleep(MILLI_MULTIPLIER * defaults->mouse_click_delay);
        }
    }
    return (success != 0);
}

BOOL mouse_down(unsigned short btn) {
    if (!_check_init()) {
        return FALSE;
    }
    BOOL success =
        XTestFakeButtonEvent(defaults->display, btn, TRUE, CurrentTime );
    XFlush(defaults->display);
    return (success != 0);
}

BOOL move_mouse(int newX, int newY, Window win) {
    int loop = 0;
    if (!_check_init()) {
        return FALSE;
    }
    BOOL success = TRUE;
    logit(LOG_LEVEL_VERBOSE, "Window handle: %lu\n", win);
    if (win) {
        success = _normalize_wincoords(&newX, &newY, win);
        if(success == FALSE) {
            return FALSE;
        }
    } else {
        _normalize_coords(&newX, &newY);
    }
    if (defaults->mouse_move_delay == ZERO) {
        return _do_movement(newX, newY);
    } else {
        int oldX = mouse_x(0L);
        int oldY = mouse_y(0L);
        while (oldX != newX || oldY != newY) {
            int movementX, movementY;
            _gradientMovement(newX, newY, &movementX, &movementY);
            // 1:1 is a little slow
            if ((fabs(movementX) == 1 && fabs(movementY) == 1) &&
                (fabs(newX - oldX) > 1 && fabs(newY - oldY) > 1) ) {
                movementX *= 2;
                movementY *= 2;
            }
            oldX += movementX;
            oldY += movementY;
            success &= _do_movement(oldX, oldY);
            if (!success) {
                fprintf(stderr, "Simulation of mouse movement failed.\n");
                break;
            }
            usleep(50 * defaults->mouse_move_delay);
            loop++;
            if (loop > 10000) {
                return FALSE;
            } //Probably something is wrong
        }
    }
    return success;
}

BOOL mouse_up(unsigned short btn) {
    if (!_check_init()) {
        return FALSE;
    }
    BOOL success = XTestFakeButtonEvent(defaults->display, btn, FALSE,
            CurrentTime );
    XFlush(defaults->display);
    return (success != 0);
}

int mouse_x(Window win) {
    if(win > 0 && !is_valid(win)) {
        return INT_MIN;
    }
    int arr[2] = { 0, 0 };
    _mouse_xy(arr, win);
    int x = arr[0];
    return x;
}

int mouse_y(Window win) {
    if(win > 0 && !is_valid(win)) {
        return INT_MIN;
    }
    int arr[2] = { 0, 0 };
    _mouse_xy(arr, win);
    int y = arr[1];
    return y;
}

void _mouse_xy(int *ret, Window win) {
    if (!_check_init()) {
        return;
    }
    Window query_window;
    if(win == 0) {
        query_window = DefaultRootWindow(defaults->display);
        logit(LOG_LEVEL_EXTRA_VERBOSE, "%s", "Getting mouse xy against root window\n");
    } else {
        if(is_valid(win)) {
            query_window = find_outer_parent(win);
            logit(LOG_LEVEL_EXTRA_VERBOSE, "Getting mouse xy against window %lu\n", win);
        } else {
            fprintf(stderr, "Window %lu was not valid for xy -"
                    " returning absolute coordinates\n", win);
            _mouse_xy(ret, 0);
            return;
        }
    }

    //Both root xy and window xy come along for the ride.
    //Later we'll get the one we really want.
    Window root_return_ignored;
    Window child_return_ignored;
    int rootx;
    int rooty;
    int winx;
    int winy;
    unsigned int mask_return_ignored;
    int success =
        XQueryPointer(
            defaults->display,
            query_window,
            &root_return_ignored,
            &child_return_ignored,
            &rootx,
            &rooty,
            &winx,
            &winy,
            &mask_return_ignored);

    if(success) {
        if(is_valid(win)) {
            logit(LOG_LEVEL_EXTRA_VERBOSE,
                    "Mouse is at %d X %d relative to window %lu\n",
                    winx, winy, win);
            ret[0] = winx;
            ret[1] = winy;
        } else {
            logit(LOG_LEVEL_EXTRA_VERBOSE,
                    "Mouse is at %d X %d relative to the root window\n",
                    winx, winy);
            ret[0] = rootx;
            ret[1] = rooty;
        }
    } else {
        fprintf(stderr, "Query for mouse x and y failed.  Returning %d\n", INT_MIN);
        ret[0] = INT_MIN;
        ret[1] = INT_MIN;
    }
}

BOOL _do_movement(int x, int y) {
    logit(LOG_LEVEL_EXTRA_VERBOSE, "Moving mouse to %d X %d\n", x, y);
    int status = XTestFakeMotionEvent(defaults->display, -1, x, y, CurrentTime );
    XSync(defaults->display, FALSE);
    return status != 0;
}

void _gradientMovement(int targX, int targY, int *xResp, int *yResp) {
    int currX = mouse_x(0L);
    int currY = mouse_y(0L);
    int diffX = (int) (fabs(targX - currX));
    int diffY = (int) (fabs(targY - currY));
    if (diffX == 0 && diffY == 0) {
        //No movement at all - why are we here?
        //Theoretically this shouldn't happen, but at least it won't explode
        fprintf(stderr,
                "gradientMovement - target and destination"
                " coordinates are the same\n");
        *xResp = 0;
        *yResp = 0;
    } else if (diffX == 0) {
        //Up-down movement only
        *xResp = 0;
        *yResp = diffY;
    } else if (diffY == 0) {
        //Left-right movement only
        *xResp = diffX;
        *yResp = 0;
    } else {
        //Want a movement based on ratio
        if (diffX == diffY) {
            *xResp = 1;
            *yResp = 1;
        } else if (diffX > diffY) {
            //The following line is cast for clarity sake - I know I want int
            *xResp = (int) (diffX / diffY);
            *yResp = 1;
        } else {
            *xResp = 1;
            //The following line is cast for clarity sake - I know I want int
            *yResp = (int) (diffY / diffX);
        }
    }
    // Multiply by -1 if moving up or left
    if (targX < currX) {
        *xResp *= -1;
    }
    if (targY < currY) {
        *yResp *= -1;
    }
}
