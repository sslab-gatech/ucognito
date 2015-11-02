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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_keyboard.h $
$Author: chris_parker $
$Date: 2012-06-30 07:29:19 -0700 (Sat, 30 Jun 2012) $
$Rev: 22 $
*/

#ifndef XAUT_KEYBOARD_H_
#define XAUT_KEYBOARD_H_

#include "xaut.h"

/**
 * The following methods turn on or off the various lock keys.
 *
 * @return TRUE if the call succeeds
 */
BOOL caps_lock_on();
BOOL caps_lock_off();
BOOL num_lock_on();
BOOL num_lock_off();
BOOL scroll_lock_on();
BOOL scroll_lock_off();

/**
 * The following methods return the current lock states.
 */
BOOL is_caps_lock();
BOOL is_num_lock();
BOOL is_scroll_lock();

/**
 * How long the keyboard key stays down when "clicked" in milliseconds.
 * Default is 10 milliseconds, and usually works with most applications.
 * Note that 0 rarely works.  If you are trying to get a big speed boost
 * and are getting surprising results, try changing this to 1.
 * Does not make any changes if delay is less than zero.  Regardless,
 * this function always returns the current delay.
 *
 * @param delay the number of milliseconds
 * @return the delay which may or may not have changed
 */
short key_down_delay(short delay);

/**
 * The number of milliseconds delay between clicks in a multi- key-click event.
 * Default is 10 milliseconds, and usually works with most applications.
 * Note that 0 rarely works.  If you are trying to get a big speed boost
 * and are getting surprising results, try changing this to 1.
 * Does not make any changes if delay is less than zero.  Regardless,
 * this function always returns the current delay.
 *
 * @param delay the number of milliseconds
 * @return the delay which may or may not have changed
 */
short key_click_delay(short delay);

/**
 * Presses and releases a single key.
 * Convenience method for key_down followed by
 * key_up.  Note that this function "holds" the key
 * down for the amount of time specified in key_down_delay.
 *
 * Use print_keycodes() to get a listing of valid keycodes.  Use
 * one of the values from the column labeled <em>Code</em> as
 * a parameter.
 * @param the keycode
 * @return boolean indication of success
 */
BOOL key_click(unsigned int key);

/**
 * Simulates pressing down the key by key code.
 * Use print_keycodes() to get a listing of valid key codes.
 * Note that the key appears to usually releases by itself when
 * the calling program terminates - but this is not guaranteed
 * (at least not by me).
 *
 * Use print_keycodes() to get a listing of valid keycodes.  Use
 * one of the values from the column labeled <em>Code</em> as
 * a parameter.
 * @param the keycode
 * @return boolean indication of success
 */
BOOL key_down(unsigned int code);

/**
 * Releases a key previously presed down.
 * Note that this will not release an actual key, only a simulated
 * key.  Calling key_up without first calling key_down may or may not
 * work, and may have "interesting" side effects.  Fair warning.
 *
 * Use print_keycodes() to get a listing of valid keycodes.  Use
 * one of the values from the column labeled <em>Code</em> as
 * a parameter.
 * @param the keycode
 * @return boolean indication of success
 */
BOOL key_up(unsigned int code);

/**
 * A way to tell the program whether to interpret meta symbols.
 * If the text to be typed contains meta symbols (!#^+{}),
 * and you want those symbols sent as opposed to interpreted,
 * then set this to FALSE.  Otherwise set this to TRUE - this
 * is the default.
 * @tf a boolean indication of whether to interpret the symbols
 */
void interpret_meta_symbols(BOOL tf);

/**
 * Prints out a listing of keycodes on the local machine.
 * The code is the scan code of the key to simulate being pressed.
 * For example, on my keyboard code 9 is "Escape".  So using
 * key_down( 9 ) followed by key_up( 9 ) would simulate pressing
 * and releasing the "Esc" key.
 *
 * <ul>Note that every key lists whether shift is required or not.
 * <li><em>Either</em> This key works exactly the same regardless of
 * whether shift is pressed or not.</li>
 * <li><em>Yes</em> This code only works when the shift key is down.</li>
 * <li><em>No</em> This code only works when the shift key is up.</li>
 */
void print_keycodes();

/**
 * Sends the sequence of characters in the string parameter.
 * Note that there are a number of rules to keep in mind:
 * <ol>
 * <li>Only valid characters are sent.  So for example, character 7
 * (the bell) cannot be sent.</li>
 *
 * <li>The braces {} signify a special key code.  The code contained
 * inside must correspond to a X11 key symbol name string.  The brace
 * can also contain a number, which indicates the number of times to
 * press the key.  E.g. {Tab} presses the tab key.  {Tab 3} presses
 * the tab key three times.  Use print_keycodes() to get a listing
 * of valid keycodes.  You will want to use a value from the
 * column called <em>X11 key symbol name string</em>.  The symbols
 * are case sensitive - they must match what is printed <em>exactly</em>.</li>
 *
 * <li>Note that all valid X11 codes are valid inside braces.</li>
 *
 * <li>The exclamation point (!) signifies press the alt key for the next
 * letter.  So type( "!a" ) would press the alt key down, then press
 * and release the a key, and then finally release the alt key.  Use
 * {exclam} to send the literal exclamation point.</li>
 *
 * <li>The plus sign (+) signifies press the shift key for the next
 * letter - similar to alt.  So type( "+a" ) would press down
 * the shift key down, press and release the a key, and then finally release
 * the shift key.</li>
 *
 * <li>The carat - or circumflex - (^) signifies press the control key
 * for the next letter - similar to shift and alt.  So type( "^a" ) would
 * press the control key, then press and release the a key, and then finally
 * release the control key.</li>
 *
 * <li>The number sign - or pound symbol - (#) signifies press the meta key
 * for the next letter - similar to shift, control, and alt.  So type( "#a" )
 * would press the meta key, then press and release the a key, and then finally
 * release the meta key.  Note that the "meta" key is sometimes named after
 * a certain software company :)</li>
 *
 * <li>The previous special keys can be combined.  So type( "+^#!a" ) would
 * press and hold down the shift, control, meta, and alt keys, press and release the
 * a, and then release the alt, meta, control, and shift keys.</li>
 *
 * <li>Use the string name for a key if you wish to send a literal !, +, or ^.
 * E.g. {exclam}{plus}{asciicircum} would send !+^ .</li>
 *
 * <li>Sending upper case letters as well as many symbols actually cause a
 * shift and letter/symbol, e.g. A and +a are the same.  For this reason,
 * one should use lower case letters in combination with control keys
 * to avoid confusion.  ^a is not the same as ^A - ^A is precisely
 * the same as ^+a .</li>
 *
 * <li>Invalid characters encountered (if any) are printed to the error
 * stream and then ignored.</li>
 *
 * <li>Modifier keys (alt, control, and shift) are released after each
 * keycode in a brace, and after the next letter when not in a brace.
 * If you wish to use multiple letters in combination with modifier keys,
 * you'll need to construct the correct sequence yourself using the
 * correct codes and the key_down key_up functions.</li>
 *
 * </ol>
 *
 * @param the key sequence to type
 * @return boolean indication of success
 */
BOOL type(char *str);

/**
 * Changes the state of the three "lock" keys.
 * The three lock keys being caps lock, num lock,
 * and scroll lock.
 *
 * All three lock keys are changed at once based on the mask
 * where (starting at LSB) bit[0] determines scroll lock,
 * bit[1] determines caps lock, and bit[4] determines num lock.
 * If the respective bits are set, then the locks are set.  If
 * the respective bits are clear, then the locks are cleared.
 */
int _alter_keyboard_state(unsigned int mask);

/**
 * Creates a mask that can be passed to _alter_state which will NOT alter state.
 * The idea is that one gets a mask that keeps the status quo,
 * and then changes just the one thing that needs to be changed by
 * toggling the one bit.
 */
unsigned int _current_keyboard_state_mask();

/**
 * Extracts a meta key and count from a string.
 * Presumably the string is the inside of a set
 * of braces, and the keycode is on the left,
 * the count is on the right or not present.
 * E.g. (ignoring the braces) {Tab} returns
 * "Tab" and 1.  {Tab 3} returns "Tab" and 3.
 *
 * Note that it is the caller's responsibility
 * to free the memory used by the struct.
 *
 * @param input the string which contains the meta key.
 * @return a ASCII structure, note that the ascii code will
 *     be null
 */
xautpy_meta_t* _extract_metakey(char *input);

/**
 * Looks at the name and key code and determines if it needs a shift key.
 * Sets the value of meta->shifted to the correct shift value.
 * Note that this routine leaves shifted at 0 and prints to
 * the error stream if something goes terribly wrong.
 * @param meta a nearly complete xautpy_meta_t
 */
void _extract_metakey_shifted(xautpy_meta_t *meta);

/**
 * Separates the meta key name from the count.
 * Places both values into the meta key structure.
 * @param meta a meta key without a key name or count
 * @param str the string which contains the space
 * @param space_index where to find the space
 */
void _extract_metakey_with_count(xautpy_meta_t *meta, char *str,
        size_t space_index);

/**
 * Extracts and types the keycode between the braces.
 * The comparison is NOT case sensitive, which means
 * that single letters will probably not work as expected.
 * E.g. {a} and {A} both resolve to the code for "a", and
 * no shift key will be sent.  So {A} will type "a".  There's
 * little reason to send single keys, so it's best to avoid this
 * problem.  If you truly wish to use {A}, then the following
 * would work: +{a}.
 *
 * This function will operate the shift key if it is necessary
 * to achieve the correct result.  It is not necessary to preface
 * a code that ordinarly requires a shift key, with the exception
 * of the single characters as mentioned in the previous paragraph.
 *
 * Note that it is possible to send multiple keys by placing a
 * number inside the braces, after the keycode.  E.g. {TAB 5} will
 * send 5 different tab keys.
 * @param index index of the opening brace
 * @param str the string which contains the code
 * @return the index of the closing brace -
 *      or the length of the string if none found
 */
size_t _handle_keycode(size_t index, char *str);

#endif /* XAUT_KEYBOARD_H_ */
