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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_keyboard.c $
$Author: chris_parker $
$Date: 2012-07-28 14:41:37 -0700 (Sat, 28 Jul 2012) $
$Rev: 23 $
*/

#include "xaut_keyboard.h"

/*
 * These masks are used to interpret current keyboard state.
 *
 * Note: I found these values through extensive testing and so they may only
 * work on my computer.  I can't find them documented anywhere.
 */
static unsigned long CAPS_LOCK_MASK = 1;
static unsigned long NUM_LOCK_MASK = 2;
static unsigned long SCROLL_LOCK_MASK = 0xFFFFE7FC;

/*
 * These masks are used to modify keyboard state
 *
 * Note: I found these values through extensive testing and so they may only
 * work on my computer.  I can't find them documented anywhere.
 */
static unsigned int CAPS_LOCK = 2;
static unsigned int NUM_LOCK = 16;
static unsigned int SCROLL_LOCK = 1;

BOOL caps_lock_on() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int neu = curr | CAPS_LOCK;
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL caps_lock_off() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int back_mask = ~ CAPS_LOCK;
    unsigned int neu = curr & back_mask;
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL num_lock_on() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int neu = curr | (NUM_LOCK);
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL num_lock_off() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int back_mask = ~ NUM_LOCK;
    unsigned int neu = curr & back_mask;
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL scroll_lock_on() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int neu = curr | (SCROLL_LOCK);
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL scroll_lock_off() {
    if (!_check_init()) {
        return 0;
    }
    unsigned int curr = _current_keyboard_state_mask();
    unsigned int back_mask = ~ SCROLL_LOCK;
    unsigned int neu = curr & back_mask;
    return (_alter_keyboard_state(neu) ? TRUE : FALSE);
}

BOOL is_caps_lock() {
    if (!_check_init()) {
        return 0;
    }
    XKeyboardState state;
    if(! XGetKeyboardControl(defaults->display, &state)) {
        return 0;
    }
    unsigned long mask = state.led_mask;
    return (mask & CAPS_LOCK_MASK ? TRUE : FALSE);
}

BOOL is_num_lock() {
    if (!_check_init()) {
        return 0;
    }
    XKeyboardState state;
    if(! XGetKeyboardControl(defaults->display, &state)) {
        return 0;
    }
    unsigned long mask = state.led_mask;
    return (mask & NUM_LOCK_MASK ? TRUE : FALSE);
}

BOOL is_scroll_lock() {
    if (!_check_init()) {
        return 0;
    }
    XKeyboardState state;
    if(! XGetKeyboardControl(defaults->display, &state)) {
        return 0;
    }
    unsigned long mask = state.led_mask;
    return (mask & SCROLL_LOCK_MASK ? TRUE : FALSE);
}

short key_down_delay(short delay) {
    if (!_check_init()) {
        return 0;
    }
    if(delay < 0) {
        logit(LOG_LEVEL_VERBOSE, "Leaving key down delay at %hu\n",
                defaults->key_down_delay);
        return defaults->key_down_delay;
    }
    logit(LOG_LEVEL_EXTRA_VERBOSE,
            "Keys used to stay down %hu milliseconds when clicked\n",
            defaults->key_down_delay);
    defaults->key_down_delay = delay;
    logit(LOG_LEVEL_EXTRA_VERBOSE,
            "Keys now stay down %hu milliseconds when clicked\n",
            defaults->key_down_delay);
    return defaults->key_down_delay;
}

short key_click_delay(short delay) {
    if (!_check_init()) {
        return 0;
    }
    if(delay < 0) {
        logit(LOG_LEVEL_VERBOSE, "Leaving key down delay at %hu\n",
                    defaults->key_click_delay);
        return defaults->key_click_delay;
    }
    logit(LOG_LEVEL_EXTRA_VERBOSE,
            "Keys used to wait %hu milliseconds between clicks\n",
            defaults->key_click_delay);

    defaults->key_click_delay = delay;
    logit(LOG_LEVEL_EXTRA_VERBOSE,
            "Keys now wait %hu milliseconds between clicks\n",
            defaults->key_click_delay);
    return defaults->key_click_delay;
}

BOOL key_click(unsigned int key) {
    if (!_check_init()) {
        return FALSE;
    }
    int success = key_down(key);
    if (success) {
        usleep(MILLI_MULTIPLIER * defaults->key_down_delay);
        success = key_up(key);
    }
    return success;
}

BOOL key_down(unsigned int key) {
    if (!_check_init()) {
        return FALSE;
    }

    int success = XTestFakeKeyEvent(defaults->display, key, TRUE, CurrentTime);
    XFlush(defaults->display);
    if (success) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "Pressed %d ", key);
    } else {
        fprintf(stderr, "Failure to press %d ", key);
    }
    return (success != 0);
}

BOOL key_up(unsigned int key) {
    if (!_check_init()) {
        return FALSE;
    }
    int release = XTestFakeKeyEvent(defaults->display, key, FALSE, CurrentTime);
    BOOL success = (release != 0);
    XFlush(defaults->display);
    if (success) {
        logit(LOG_LEVEL_EXTRA_VERBOSE, "Released %d", key);
    } else {
        fprintf(stderr, "Failure to release %d\n", key);
    }
    return (success != 0);
}

void interpret_meta_symbols(BOOL tf) {
    //"BOOL" is actually a short - so I'm going
    //to filter the value sent.
    {
        char *msg = "I was %sinterpreting meta symbols\n";
        char *yn = defaults->interpret_meta_symbols ? "" : "not ";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, yn);
    }
    defaults->interpret_meta_symbols = (tf) ? TRUE : FALSE;
    {
        char *msg = "Now I will %sinterpret meta symbols\n";
        char *yn = defaults->interpret_meta_symbols ? "" : "not ";
        logit(LOG_LEVEL_EXTRA_VERBOSE, msg, yn);
    }
}

void print_keycodes() {
    if (!_check_init()) {
        fprintf(stderr, "Unable to print keycodes");
        return;
    }
    printf("Code X11 key symbol name string        Shift?\n");
    printf("---- --------------------------------- -------\n");
    int min, max; //The minumum and maximum codes
    XDisplayKeycodes(defaults->display, &min, &max);
    int i;
    KeySym symL, symU; //Lower and upper symbols
    char *strL, *strU; //Lower and upper strings
    char *fmt = "%4d %-33s %-6s\n";
    for (i = (min - 1); i <= max; i++) {
        symL = XkbKeycodeToKeysym(defaults->display, i, 0, FALSE);
        symU = XkbKeycodeToKeysym(defaults->display, i, 0, TRUE);
        if (symL) {
            strL = XKeysymToString(symL);
        }
        if (symU) {
            strU = XKeysymToString(symU);
        }

        //If both shifted and unshifted have values
        if (symL && symU) {
            if (!strcmp(strL, strU)) {
                //If they're the same
                printf(fmt, i, strL, "Either");
            } else {
                //Otherwise
                printf(fmt, i, strL, "No");
                printf(fmt, i, strU, "Yes");
            }
        } else if (symL) {
            //Unshifted only (lower case)
            printf(fmt, i, strL, "No");
        } else if (symU) {
            //Shifted only (upper case)
            printf(fmt, i, strU, "Yes");
        }
    }
}

#ifdef EN_US
BOOL type(char *str) {
    if (!_check_init()) {
        return FALSE;
    }
    logit(LOG_LEVEL_VERBOSE, "Sending '%s'\n", str);
    BOOL success = TRUE;

    size_t index = 0;
    int down = 0;
    int meta[] = { 0, 0, 0, 0 };
    BOOL shft_pressed = FALSE;
    BOOL ctl_pressed = FALSE;
    BOOL alt_pressed = FALSE;
    BOOL meta_pressed = FALSE;
    unsigned short key = 0;
    while (str[index] != '\0') {
        switch (str[index]) {
        case ('+'):
            if(defaults->interpret_meta_symbols) {
                if (!shft_pressed) {
                    shft_pressed = TRUE;
                    logit(LOG_LEVEL_VERBOSE, "%s", "Shift_L down ");
                    success &= key_down(SHIFT_L);
                    meta[down] = SHIFT_L;
                    down++;
                }
                break;
            }
        case ('^'):
            if(defaults->interpret_meta_symbols) {
                if (!ctl_pressed) {
                    ctl_pressed = TRUE;
                    logit(LOG_LEVEL_VERBOSE, "%s", "Control_L down ");
                    success &= key_down(CONTROL_L);
                    meta[down] = CONTROL_L;
                    down++;
                }
                break;
            }
        case ('!'):
            if(defaults->interpret_meta_symbols) {
                if (!alt_pressed) {
                    alt_pressed = TRUE;
                    success &= key_down(ALT_L);
                    logit(LOG_LEVEL_VERBOSE, "%s", "Alt_L down ");
                    meta[down] = ALT_L;
                    down++;
                }
                break;
            }
        case ('#'):
            if(defaults->interpret_meta_symbols) {
                if (!meta_pressed) {
                    meta_pressed = TRUE;
                    success &= key_down(META_L);
                    logit(LOG_LEVEL_VERBOSE, "%s", "META_L down ");
                    meta[down] = META_L;
                    down++;
                }
                break;
            }
        case ('{'):
            if( defaults->interpret_meta_symbols) {
                index = _handle_keycode(index, str);
                if (down > 0) {
                    logit(LOG_LEVEL_VERBOSE, "%s", "Meta keys released ");
                }
                while (down > 0) {
                    down--;
                    success &= key_up(meta[down]);
                    meta[down] = 0;
                }
                shft_pressed = FALSE;
                ctl_pressed = FALSE;
                alt_pressed = FALSE;
                logit(LOG_LEVEL_VERBOSE, "%s", "\n");
                break;
            }
        default:
            key = ((unsigned short) (str[index]));
            if (ascii_codes[key].ascii) {
                int code = ascii_codes[key].code;
                if (ascii_codes[key].shifted && !shft_pressed) {
                    shft_pressed = TRUE;
                    logit(LOG_LEVEL_VERBOSE, "Shift_L down ");
                    success &= key_down(SHIFT_L);
                    meta[down] = SHIFT_L;
                    down++;
                }
                if (str[index] == ' ') {
                    logit(LOG_LEVEL_VERBOSE, "%s", "Typing (space) ");
                } else if (str[index] == '\t') {
                    logit(LOG_LEVEL_VERBOSE, "%s", "Typing (tab) ");
                } else {
                    logit(LOG_LEVEL_VERBOSE, "%s", "Typing %c ", str[index]);
                }
                success &= key_click(code);
                if (down > 0) {
                    logit(LOG_LEVEL_VERBOSE, "%s", "Meta keys released ");
                }
                while (down > 0) {
                    down--;
                    success &= key_up(meta[down]);
                    meta[down] = 0;
                }
                shft_pressed = FALSE;
                ctl_pressed = FALSE;
                alt_pressed = FALSE;
                logit(LOG_LEVEL_VERBOSE, "%s", "\n");
                XFlush(defaults->display);

                if (str[index + 1]) {
                    usleep(MILLI_MULTIPLIER * defaults->key_click_delay);
                }
            } else {
                fprintf(stderr, "Unable to print %c", str[index]);
            }
            break;
            // <- Switch ends
        }
        index++;
    }
    return success;
}
#else
BOOL type( char *str ) {
    fprintf( stderr, "No language defined - skipping" );
    return FALSE;
}
#endif


int _alter_keyboard_state(unsigned int mask) {
    if (!_check_init()) {
        return 0;
    }
    XKeyboardControl values;
    values.led_mode = mask & SCROLL_LOCK ? LedModeOn : LedModeOff;
    values.led = 3;
    if(! XChangeKeyboardControl(defaults->display, KBLedMode, &values)) {
        return 0;
    }
    if(! XkbLockModifiers(defaults->display,
                XkbUseCoreKbd,
                CAPS_LOCK | NUM_LOCK,
                mask & (CAPS_LOCK | NUM_LOCK))) {
        return 0;
    }
    return XFlush(defaults->display);
}

unsigned int _current_keyboard_state_mask() {
    if (!_check_init()) {
        return 0;
    }
    XKeyboardState state;
    if(! XGetKeyboardControl(defaults->display, &state)) {
        return 0;
    }
    unsigned long mask = state.led_mask;
    unsigned int ret = 0;
    if(mask & CAPS_LOCK_MASK) {
        ret += CAPS_LOCK;
    }
    if(mask & NUM_LOCK_MASK) {
        ret += NUM_LOCK;
    }
    if(mask & SCROLL_LOCK_MASK) {
        ret += SCROLL_LOCK;
    }
    return ret;
}

xautpy_meta_t *_extract_metakey(char *input) {
    size_t i = 1;
    BOOL space_char_found = FALSE;
    xautpy_meta_t *meta = malloc(sizeof(xautpy_meta_t));
    if (meta == NULL) {
        fprintf(stderr, "Unable to allocate space for metakey");
        return 0;
    }
    memset(meta, 0, sizeof(xautpy_meta_t));
    while (input[i] != '\0') {
        //First we check to see if there is a space in the string
        if (input[i] == ' ') {
            space_char_found = TRUE;
            break;
        }
        i++;
    }
    if (space_char_found) {
        //Note that i points directly at the space character
        _extract_metakey_with_count(meta, input, i);
    } else {
        meta->name = _stringmid(input, 0, strlen(input));
        meta->count = 1;
    }
    if (meta->name) { //Might be empty, since name is allocated
        meta->keysym = XStringToKeysym(meta->name);
        if (meta->keysym) {
            meta->keycode = XKeysymToKeycode(defaults->display, meta->keysym);
            _extract_metakey_shifted(meta);
        }
    }
    return meta;
}

void _extract_metakey_shifted(xautpy_meta_t *meta) {
    //Try no shift key first, since if it doesn't matter
    // to the code, why worry about it
    KeySym sym = XkbKeycodeToKeysym(defaults->display, meta->keycode, 0, FALSE);
    if (sym == meta->keysym) {
        meta->shifted = FALSE;
    } else {
        sym = XkbKeycodeToKeysym(defaults->display, meta->keycode, 0, TRUE);
        if (sym == meta->keysym) {
            meta->shifted = TRUE;
        } else {
            fprintf(stderr, "Something went wrong with keystring %s\n",
                    meta->name);
            meta->shifted = FALSE;
        }
    }
}

void _extract_metakey_with_count(xautpy_meta_t *meta, char *input,
        size_t space_index) {
    //We have to try to extract a string from the left, and
    // a number from the right.
    meta->name = _stringmid(input, 0, space_index - 1);
    char *numStr = _stringmid(input, space_index + 1, strlen(input));
    int num = atoi(numStr);
    if (num < 0) {
        num = 0;
    }
    meta->count = (unsigned short) num;
    free(numStr);
}

#ifdef EN_US
size_t _handle_keycode(size_t curr, char *str) {
    size_t index = curr;
    BOOL closing_brace_found = FALSE;
    xautpy_meta_t *meta;
    unsigned short count = 0; //No point in going past 100 characters
    while (str[index] != '\0' && count < 100) {
        if (str[index] == '}') {
            closing_brace_found = TRUE;
            break;
        }
        count++;
        index++;
    }
    if (closing_brace_found) {
        //Note: curr points at opening brace {
        // and index points at closing brace }
        char *keycode = _stringmid(str, curr + 1, index - 1);
        meta = _extract_metakey(keycode);
        free(keycode);
    } else {
        fprintf(stderr, "Unable to find closing brace in '%s'\n", str + curr);
        meta = malloc(sizeof(xautpy_meta_t));
        if (meta == NULL) {
            return index;
        }
        memset(meta, 0, sizeof(xautpy_meta_t));
        index = curr; //Point index back at opening brace
        meta->count = 1;
        meta->keycode = ascii_codes['{'].code;
        meta->shifted = ascii_codes['{'].shifted;
        meta->name = _stringmid("{", 0, 1);
    }
    if (meta && meta->keycode) {
        unsigned short i;
        if (meta->shifted) {
            key_down(SHIFT_L);
            logit(LOG_LEVEL_VERBOSE, "%s", "Shift_L down ");
        }
        for (i = 0; i < meta->count; i++) {
            key_click(meta->keycode);
            if (meta->keycode == ascii_codes['{'].code) {
                logit(LOG_LEVEL_VERBOSE, "%s", "Typing { ");
            } else {
                logit(LOG_LEVEL_VERBOSE, "Typing metakey %s ", meta->name);
            }
            if (i + 1 < meta->count) {
                usleep(MILLI_MULTIPLIER * defaults->key_click_delay);
            }
        }
        if (meta->shifted) {
            key_up(SHIFT_L);
            logit(LOG_LEVEL_VERBOSE, "%s", "Shift_L up ");
        }
    }
    free(meta->name);
    free(meta);
    return index;
}
#else
unsigned long _handle_keycode( size_t index, char *str ) {
    //Skips the remainder if not EN_US
    while( str[index] != '\0' )
    index++;
    return index;
}
#endif
