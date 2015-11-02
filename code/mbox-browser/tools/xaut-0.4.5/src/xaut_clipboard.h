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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_clipboard.h $
$Author: chris_parker $
$Date: 2011-11-27 10:29:06 -0800 (Sun, 27 Nov 2011) $
$Rev: 18 $
*/

#ifndef XAUT_CLIPBOARD_H_
#define XAUT_CLIPBOARD_H_

#include "xaut.h"
#include <X11/Xlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/*
 * There are three main clipboard selections in X Windows:  PRIMARY, SECONDARY,
 * and CLIPBOARD.  It's possible to extend this with additional selections,
 * however, the additional selections would be environment specific and so
 * would not be reachable for a "normal" program.
 *
 * PRIMARY: This is the clip selection you get when text is selected.  For
 * example, select text in a terminal window, but don't hit any key
 * combinations or use any menu choices to tell X Windows that you want the
 * text selected.  Now middle click inside the same window and the text
 * selected will be pasted.  You can also try middle clicking in another
 * window, the text will sometimes paste across windows depending on how well
 * behaved the programs are.  This is of less utility than CLIPBOARD, and is
 * probably not what you will want under most circumstances.
 *
 * SECONDARY: This is not ordinarily reachable by a user.  This is more of a
 * place for programs to stash data that won't affect PRIMARY or CLIPBOARD.
 * This will be of limited utility to most users, but is here for completeness
 * sake.
 *
 * CLIPBOARD: This corresponds to what most users think of when thinking of the
 * "clipboard".  Data is made available here, and can be retrieved in another
 * program.  This is most likely the selection type that should be used.
 */


/**
 * Attempt to retrieve the data in the primary selection.
 * Note that the data returned may or may not be text.  It is the user's
 * responsibility to ensure that the data is the proper type for the
 * intended use.
 * <br><br>Most likely this is not the selection you wish to use.  Most
 * likely you want clipboard.
 *
 * @return the data in the primary selection, or null if there isn't any.
 */
char* get_primary();

/**
 * Attempt to retrieve the data in the secondary selection.
 * Note that the data returned may or may not be text.  It is the user's
 * responsibility to ensure that the data is the proper type for the
 * intended use.
 * <br><br>
 * Note also that it is <em>very</em> unusual to use the secondary
 * selection.  Most likely you want clipboard.
 *
 * @return the data in the secondary selection, or null if there isn't any.
 */
char* get_secondary();

/**
 * Attempt to retrieve the data in the clipboard.
 * Note that the data returned may or may not be text.  It is the user's
 * responsibility to ensure that the data is the proper type for the
 * intended use.
 * <br><br>
 * Most likely this is the selection you will want to use.
 *
 * @return the data in the clipboard, or null if there isn't any.
 */
char* get_clipboard();

/**
 * Attempt to put data into the primary selection.
 * Note that there can only be one selection at a time.  Often times a program
 * which has selected text will remove the selection when another program asks
 * to own the selection.  It's a little hard to be clear about that: Basically,
 * highlight some text in a program, then switch to a console and run an xaut
 * script which sets the primary selection.  The other program will often times
 * remove the highlight.  The text will not go away, just the highlight.
 * <br><br>
 *
 * @param data the data to put into the primary selection.
 */
void put_primary(char* clip);

/**
 * Attempt to put data into the secondary selection.
 * Note that the secondary selection is not ordinarily reachable by the user,
 * and that it is <em>very</em> unusual to use the secondary selection.
 * Most likely you want clipboard.
 *
 * @param clip the data to put into the secondary selection.
 */
void put_secondary(char* clip);

/**
 * Attempt to put data into the clipboard selection.
 * Most likely this is the selection you will want to use.
 *
 * @param clip the data to put into the clipboard selection.
 */
void put_clipboard(char* clip);

#endif /* XAUT_CLIPBOARD_H_ */
