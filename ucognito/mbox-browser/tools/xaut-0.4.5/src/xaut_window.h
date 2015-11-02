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
 $URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_window.h $
 $Author: chris_parker $
 $Date: 2012-09-22 17:17:57 -0700 (Sat, 22 Sep 2012) $
 $Rev: 29 $
*/

/**
 * Various ways of getting information about and manipulating windows.
 */

#ifndef XAUT_WINDOW_H_
#define XAUT_WINDOW_H_

#include <regex.h>

#include "xaut.h"
#include "xaut_types.h"

/**
 * Activates the Window pointed to by the handle.
 * "Activate" in this context means "bring to front and
 * get ready to accept input".
 *
 * Note that some window managers do not support
 * or will suppress this.  If that is the case, you
 * may wish to try "switch_to_window".
 * @param the win to the window
 * @return boolean indication of success
 */
BOOL activate_window(Window win);

/**
 * Returns a handle to the currently active window.
 * @return a handle to the active window.
 */
Window active_window();

/**
 * Returns 1 if the window handle passed is valid, 0 otherwise.
 * @param win the window handle to test
 * @return as described.
 */
BOOL is_valid(Window win);

/**
 * Finds the outer container parent of a window.
 * Typically, when one finds a window by name or
 * by getting the active window, what one gets is
 * the window that the user interacts with.  This
 * window almost always has an outer boundary
 * window where the title bar and borders go.  It
 * is almost always necessary to use the outer window when
 * calculating window size or xy position.  Otherwise
 * one gets the size of the container window.
 *
 * It is an implementation detail that this function
 * returns the final window just before the root window.
 * @param win the window which is likely the edit or use window
 * @param the outer parent window.
 */
Window find_outer_parent(Window win);

/**
 * Looks for a window with the title supplied.
 * It is an implementation detail that this function asks the current
 * window manager (e.g. KDM the KDE Window manager) for a list of
 * current windows and checks those.  This has the advantage that
 * there are fewer windows to test, but has the disadvantage that
 * there are many "windows" that the window manager does not "manage".
 * For example the volumn control in the task tray is managed by
 * X but is not managed by e.g. KDM.  Use this function if you
 * are interested only in traditional "window" objects,
 * otherwise try "search_for_window".
 *
 * The following criteria is used when multiple windows match:
 * First the distance from the user's current window is considered.
 * If one window is "closer" than another it is immediately selected.
 * If not, then the window with the lower ID is returned.  This usually -
 * but not always - means the older of the windows (sometimes window IDs
 * are reused).
 *
 * @param title a string to match against the title
 * @return the window handle, or NULL if none found
 */
Window find_window(char *title);

/**
 * Performs a deep search and possibly finds a window based on title.
 * Note that the title can contain regular expressions.
 * "Deep search" in this case means that the root X Window
 * tree is recursively searched for a match to the name supplied.
 * This search is extremely intensive, and can take a full second
 * or more to return.  This search is meant to be used for getting
 * the handle to task tray items and the like.  For example
 * a terminal window can normally be found via "find_window", while
 * the volumn control cannot - but it can most likely can be found via
 * this search.
 *
 * "find_window" and "search_for_window" use the same criteria for
 * deciding which window to return in the event multiple titles match.
 *
 * @param title a string to match against the title
 * @return the window handle, or NULL if none found
 */
Window* search_for_window(char *title);

/**
 * Moves a window to the coordinates supplied.
 *
 * Note that the boolean success return does not
 * necessarily mean that the move was successful,
 * it means that the X11 call did not generate an error.
 *
 * Use -1 for x, y, or desktop if you wish to leave that aspect
 * of the window alone (technically anything less than zero
 * will work).  Some examples:<pre>
 *
 * //Move window to 300x300 on current desktop
 * move_window( window, 300, 300, -1 );
 *
 * //Move window to desktop 3
 * move_window( window, -1, -1, 3 );
 *
 * //Move window up 10 pixels
 * move_window( window, -1, window_y() - 10, -1 );
 *
 * </pre>
 *
 * @param win a handle to the window to move
 * @param x the new x coordinate for the upper left corner of the window
 * @param y the new y coordinate for the upper left corner of the window
 * @param desktop the destination desktop.
 * @return boolean indication of success
 */
BOOL move_window(Window win, int x, int y, long desktop);

/**
 * Resizes a window to the size supplied.
 *
 * Note that the boolean success return does not
 * necessarily mean that the move was successful,
 * it means that the X11 call did not generate an error.
 *
 * Use 0 for w or h if you wish to leave that aspect of the window alone
 * (technically anything less than zero will work).  Some examples:<pre>
 *
 *  //Resize the window to 800x600
 *  resize_window( window, 800, 600);
 *
 *  //Set the window's width to exactly 800
 *  resize_window( window, 800, 0 );
 *
 *  //Make the window 10 pixels wider
 *  resize_window( window, window_w(window) + 10, 0 );
 *
 * @param win a handle to the window to resize
 * @param w the new width for the window
 * @param h the new height for the window
 * @return boolean indication of success
 */
BOOL resize_window(Window win, unsigned int w, unsigned int h);

/**
 * Iconify or deiconify a window - a synonym for minimize_window.
 *
 * Note that the boolean success return does not
 * necessarily mean that the minimization was successful,
 * it means that the X11 call did not generate an error.
 * @param win a handle to the window to iconify
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL iconify_window(Window win, BOOL tf);

/**
 * Minimize a window - a synonym for iconify_window.
 *
 * Note that the boolean success return does not
 * necessarily mean that the minimization was successful,
 * it means that the X11 call did not generate an error.
 * @param win a handle to the window to minimize
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL minimize_window(Window win, BOOL tf);

/**
 * Maximize a window.
 * It is an implementation detail that this function
 * does not perform any work, but instead calls
 * maximize_window_vert and maximize_window_horz to
 * maximize in both directions.  It's possible to succeed
 * in one direction and fail in the other.
 *
 * Note that the boolean success return does not
 * necessarily mean that the maximization was successful,
 * it means that the X11 call did not generate an error.
 *
 * @param win the window to set to maximize
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL maximize_window(Window win, BOOL tf);

/**
 * Maximize a window horizontally.
 * This function only makes the window fill the screen from
 * side to side, it does not expand top to bottom.
 *
 * Note that the boolean success return does not
 * necessarily mean that the maximization was successful,
 * it means that the X11 call did not generate an error.
 *
 * @param win the window to set to maximize
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL maximize_window_horz(Window win, BOOL tf);

/**
 * Maximize a window vertically.
 * This function only makes the window fill the screen from
 * top to bottom, it does not expand side to side.
 *
 * Note that the boolean success return does not
 * necessarily mean that the maximization was successful,
 * it means that the X11 call did not generate an error.
 *
 * @param win the window to set to maximize
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL maximize_window_vert(Window win, BOOL tf);

/**
 * Makes a window full screen.
 * This is different than maximized - full screen also removes
 * the title bar and tools, and the window covers any tool bars.
 *
 * @param win the window to set to full screen
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL full_screen_window(Window win, BOOL tf);

/**
 * Shades a window.
 * "Shading" is when the window has a title bar that is still
 * on the screen, but the main part of the window is rolled up.
 *
 * @param win the window to set to full screen
 * @param tf take the action if tf resolves to true, otherwise undo it
 * @return boolean indication of success
 */
BOOL shade_window(Window win, BOOL tf);

/**
 * Restore a window.
 * In this context "restore" means remove the full screen,
 * maximized, iconified, shaded, and other flags.  Removes
 * them ALL so that the window becomes a regular window again.
 *
 * Note that the boolean success return does not
 * necessarily mean that the restore was successful,
 * it means that the X11 call did not generate an error.
 *
 * @param win the window to set to full screen
 * @return boolean indication of success
 */
BOOL restore_window(Window win);

/**
 * Returns basic window information.
 * <ul>
 * <li>window_x returns the window's upper left X coordinate</li>
 * <li>window_y returns the window's upper left Y coordinate</li>
 * <li>window_w returns the width of the window</li>
 * <li>window_h returns the height of the window</li>
 * <li>window_name returns name of the window (usually the text on the title bar)</li>
 *
 * @param win a window handle
 * @return the appropriate value for the function, or -1 or NULL if the handle
 *      does not point to a real window or something else goes wrong.
 */
int window_x(Window win);
int window_y(Window win);
int window_w(Window win);
int window_h(Window win);
char* window_name(Window win);

/**
 * Gets the current desktop number for the window in question.
 * @param win the handle of the window to find
 */
long window_desktop(Window win);

/**
 * Common code for maximizing, shading and etc.
 * <ul>Note that action must be one of three actions:
 * <li>0 = Remove attribute</li>
 * <li>1 = Add attribute</li>
 * <li>2 = Toggle attribute</li></ul>
 * The action is normalized to "Toggle" if the
 * action passed does not match one of the three
 * previous actions.
 *
 * @param win a handle to the window to alter
 * @param action as described
 * @param data1 an atom of first change
 * @param data2 an atom of the second change, or 0 if only one change rquired
 * @return boolean indication of success
 */
BOOL _alter_window_state(Window win, long action, long data1, long data2);
/**
 * Use this to make sure the "results buffer" is large enough.
 * @param search a pointer to a window_search struct
 * @return boolean indication of success
 */
BOOL _ensure_window_buffer_size(_window_search *search);

/**
 * Moves the window to the specified desktop.
 * @param win the handle to the window to move
 * @param desktop the desktop to move the window to
 * @return boolean indication of success
 */
BOOL _move_window_to_desktop(Window win, long desktop);

/**
 * Prepares a regular expression for search.
 * @param search_term what to look for
 * @param regx a place to put the search regular expression
 */
BOOL _prepare_regex(char *title, regex_t **regx);


/**
 * Performs the window search.
 * Fills the buffer in search->buffer with handles to windows
 * that match the search term supplied.
 */
void _recursive_window_search(_window_search *search);

/**
 * The comparator used by qsort.
 *
 * First sort is by absolute distance from the current desktop.
 * For example if the current user is on desktop three,
 * and a matching window in on desktop two and one is on
 * desktop one, then the one on desktop two sorts higher.
 *
 * The second sort is by handle number.  Presumably an older
 * window will have a lower (numerically speaking) id than
 * a newer window.
 */
int _window_search_comparator(const void *l, const void *r);

/**
 * Gets the requested property for the window.
 * Used internally to get properties for things like which desktop the
 * window resides on.
 *
 * @param window the window in question
 * @param property the X11 atom of the desired property
 * @param item_count_ret pointer to a place to put the count of the items returned
 * @param type the X11 atom describing the property type
 * @param size
 */
unsigned char * _window_property(Window window, Atom property,
        long *item_count_ret, Atom *type, int *size);

#endif /* XAUT_WINDOW_H_ */
