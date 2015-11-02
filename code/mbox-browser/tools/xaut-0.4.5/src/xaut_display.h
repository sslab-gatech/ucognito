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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_display.h $
$Author: chris_parker $
$Date: 2010-05-01 17:17:37 -0700 (Sat, 01 May 2010) $
$Rev: 5 $
*/

#ifndef XAUT_DISPLAY_H_
#define XAUT_DISPLAY_H_

#include "xaut.h"

/**
 * Returns the height of the screen in pixels.
 * @return the display height of the screen in pixels
 */
int display_h();

/**
 * Returns the width of the screen in pixels.
 * @return the display width of the screen in pixels
 */
int display_w();

/**
 * Returns the number of virtual desktops.
 * @return the desktop count.
 */
long desktop_count();

/**
 * Sets/gets the desktop.
 * If desk > -1, then switches to the desktop.
 * Regardless, the function always returns the current
 * desktop.
 * @param desk the desired desktop or -1 if no change required
 * @return the current desktop
 */
long desktop(long desk);

/**
 * Returns the current desktop.
 * @return the current desktop.
 */
long get_current_desktop();

/**
 * Sets the current desktop.
 * @param zero based desktop number
 * @return boolean indication of success.
 */
BOOL set_current_desktop(long window_desktop);

extern void logit(int, char*, ...);
#endif /* XAUT_DISPLAY_H_ */
