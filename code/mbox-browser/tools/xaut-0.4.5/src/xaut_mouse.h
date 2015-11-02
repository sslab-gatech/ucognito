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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_mouse.h $
$Author: chris_parker $
$Date: 2010-05-01 17:17:37 -0700 (Sat, 01 May 2010) $
$Rev: 5 $
*/

/**
 * Various ways of getting information about and manipulating the mouse.
 */

#ifndef XAUT_MOUSE_H_
#define XAUT_MOUSE_H_

#include <X11/extensions/XTest.h>

#include "xaut.h"

/**
 * How long to wait between at each pixel movement in milliseconds.
 * 0 means move the mouse instantly.
 * 10 is the default, and moves the mouse fairly rapidly.
 * Anything greater than about 100 will start being annoying.
 * Does not make any changes if delay is less than zero.  Regardless,
 * this function always returns the current delay.
 *
 * @param delay the number of milliseconds
 * @return the delay which may or may not have changed
 */
short mouse_move_delay(short delay);

/**
 * How long the mouse button stays down when "clicked" in milliseconds.
 * Default is 10 milliseconds, and usually works with most applications.
 * Does not make any changes if delay is less than zero.  Regardless,
 * this function always returns the current delay.
 *
 * @param delay the number of milliseconds
 * @return the delay which may or may not have changed
 */
short mouse_down_delay(short delay);

/**
 * The number of milliseconds delay between clicks in a multi-click event.
 * Default is 10 milliseconds, and usually works with most applications.
 * Does not make any changes if delay is less than zero.  Regardless,
 * this function always returns the current delay.
 *
 * @delay the number of milliseconds
 * @return the delay which may or may not have changed
 */
short mouse_click_delay(short delay);

/**
 * Performs a mouse click.
 * In pseudo-code this performs the following:<pre>
 * for i = 1 to count
 *     mouse_down(btn)
 *     sleep_millis(mouse_down_delay)
 *     mouse_up(btn)
 *     if count > i
 *         sleep_millis(mouse_click_delay)
 *     endif
 * next</pre>
 * @param btn the button to click
 * @param count the number of clicks
 * @return boolean indication of success
 */
BOOL mouse_click(unsigned short btn, unsigned short count);

/**
 * Presses down and does not release the button pointed to by btn.
 * Note that this MUST be followed by a call to mouse_up, otherwise
 * the mouse will stay down until a sequence of events causes X11 to
 * reset mouse status.
 * @param btn the button to press down
 * @return boolean indication of success
 */
BOOL mouse_down(unsigned short btn);

/**
 * Moves the mouse to the coordinates supplied.
 * Note that impossible coordinates are "normalized" before use.
 * If win is NULL or an invalid handle, the mouse is moved relative to
 * the display.  If win resolves to a valid window, then the mouse moves
 * relative to the coordinates based on the upper left of the supplied
 * window.
 *
 * @param newX the desired new X coordinate
 * @param newY the desired new Y coordinate
 * @param win the window the movement is relative to (optional in scripts)
 * @return a boolean indication of success
 */
BOOL move_mouse(int newX, int newY, Window win);

/**
 * Releases a previously pressed mouse button.
 * This call MUST follow a call to mouse_down, otherwise
 * the mouse will stay down until a sequence of events causes X11 to
 * reset mouse status.
 * @param btn the button to release
 * @return boolean indication of success
 */
BOOL mouse_up(unsigned short btn);

/**
 * Finds the current x coordinate of the mouse relative to the display.
 * Note that if the window is non-zero, the function
 * makes the coordinates relative to the window.
 * @param win the window the coordinates are relative to
 * @return the current x coordinate in pixels
 */
int mouse_x(Window win);

/**
 * Finds the current y coordinate of the mouse relative to the display.
 * Note that if the window is non-zero, the function
 * makes the coordinates relative to the window.
 * @param win the window the coordinates are relative to
 * @return the current y coordinate in pixels
 */
int mouse_y(Window win);

/**
 * Performs the actual mouse movement.
 * Moves the mouse immediately to the coordinates
 * supplied.
 * @param newX the desired new X coordinate
 * @param newY the desired new Y coordinate
 * @return a boolean indication of success
 */
BOOL _do_movement(int newX, int newY);

/**
 * Calculates a gradient movement for the mouse.
 * @param targX the x coordinate of where we want the mouse to go
 * @param targY the y  coordinate of where we want the mouse to go
 * @param xResp what the next x movement should be
 * @param yResp what the next y movement should be
 */
void _gradientMovement(int targX, int targY, int *xResp, int *yResp);

/**
 * Puts both the x and y coordinates into the array supplied.
 * MUST be at least a two element array.  Puts the
 * values into the first two elements.  Note that if
 * the window is non-zero, the function makes the coordinates
 * relative to the window.
 * @param ret the place to put the values.
 * @param win the window the coordinates are relative to
 */
void _mouse_xy(int *ret, Window win);

#endif /* XAUT_MOUSE_H_ */
