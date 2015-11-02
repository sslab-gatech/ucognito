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
$URL: https://xautomation.svn.sourceforge.net/svnroot/xautomation/trunk/src/xaut_clipboard.c $
$Author: chris_parker $
$Date: 2011-11-27 10:29:06 -0800 (Sun, 27 Nov 2011) $
$Rev: 18 $
*/

#include "xaut_clipboard.h"

static Atom text_atom = None;

Window _dummy_window() {
    if (!_check_init()) {
        return None;
    }

    //We use the root window to create a dummy window
    Window root = XDefaultRootWindow(defaults->display);

    //Now we create a dummy window
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int w = 1;
    unsigned int h = 1;
    unsigned int bw = 0;    //Border width
    unsigned long bc = 0;   //Black
    unsigned long bg = 0;
    Window window = XCreateSimpleWindow (
            defaults->display, root, x, y, w, h, bw, bc, bg);

    return window;
}

Time _get_timestamp() {
    if (!_check_init()) {
        return None;
    }

    //The X spec says that the format value can be 8, 16, or 32, but it must be
    //cast to unsigned char* if it's anything other than 8.  We're not changing
    //anything anyway, so we may as well choose the easy value.
    int format = 8;

    //No elements to change.
    int num_elements = 0;

    //We create a dummy window that we use for a zero element "change"
    Window window = _dummy_window();
    XSelectInput(defaults->display, window, PropertyChangeMask);

    //And we make the "change"
    XChangeProperty(
            defaults->display, window, XA_WM_NAME, XA_STRING, format,
            PropModeAppend, NULL, num_elements);

    //Nothing is actually changed in the dummy window, but now we can find out
    //_when_ the nothing change event occurred - and that's our timestamp.
    XEvent event;
    while(1) {
        XNextEvent(defaults->display, &event);
        //Loop until X tells us that the "change" is complete
        if(event.type == PropertyNotify) {
            //And return that timestamp
            return event.xproperty.time;
        }
    }
}

unsigned char* _get_incremental_selection(Atom selection, XSelectionEvent xse) {
    if (!_check_init()) {
        return (unsigned char*)0;
    }
    //We start by telling the server we are interested in the selection data.
    XSelectInput(xse.display, xse.requestor, PropertyChangeMask);

    //Next we tell X that we want incremental data.
    //According to the ICCM, we do that by deleting the incr property.
    XDeleteProperty(
            xse.display,
            xse.requestor,
            xse.property);

    //These hold the values we'll get back from XGetWindowProperty
    XEvent event;
    Atom target;
    int format;
    unsigned long num_items;
    unsigned long remaining_bytes = 1; //"1" jump starts the loop
    unsigned char* value = NULL;

    //How much data we ask for at once
    unsigned int request_amount = 1024;

    //Finally, we loop around asking for data until we get it all
    unsigned char* retval = NULL;
    BOOL want_data = TRUE;
    int prop_success;
    while(want_data) {
        XNextEvent (xse.display, &event);
        if(event.type == PropertyNotify) {
            if(event.xproperty.state != PropertyNewValue) {
                continue;
            }

            while(remaining_bytes > 0) {
                prop_success = XGetWindowProperty(
                        xse.display,            //Use the event's display
                        xse.requestor,          //Use the event's requestor [sic]
                        xse.property,           //Use the event's property
                        0L,                     //Start at beginning
                        request_amount,         //Request length
                        FALSE,                  //Do not delete
                        (Atom)AnyPropertyType,  //Give me anything
                        &target,                //Where to put the property
                        &format,                //What kind of property did we get
                        &num_items,             //How many items did we get?
                        &remaining_bytes,       //How much data is past request_amount
                        &value);                //What we got
                if(prop_success != Success) {
                    XFree(value);
                    value = NULL;
                    want_data = FALSE;
                    remaining_bytes = 0;
                }
                if(remaining_bytes > 0) {
                    //Add enough space to hold it all plus some breathing room
                    request_amount += remaining_bytes + 16;
                    //And clean up the mess
                    XFree(value);
                }
            }
            if(num_items == 0) {
                //We got it all
                want_data = FALSE;

            } else {
                //Add what we got to the running total
                size_t val_len = strlen((char *)value);
                if(! retval) {
                    retval = malloc(val_len);
                } else {
                    size_t ret_len = strlen((char *)retval);
                    retval = realloc(retval, val_len + ret_len);
                }
                if(! retval) {
                    //Oops, ran out of room
                    free(retval);
                    retval = NULL;
                    break;
                }
                strcat((char *)retval, (char *) value);
            }
        }
    }
    //Clean up request
    XDeleteProperty(xse.display, xse.requestor, xse.property);
    return retval;
}

unsigned char* _get_selection(Atom selection, Atom type) {
    if (!_check_init()) {
        return (unsigned char*)0;
    }
    //Various things we need to do our job
    Window w = _dummy_window();
    Time timestamp = _get_timestamp();
    Atom incr = XInternAtom (defaults->display, "INCR", FALSE);
    Atom selected = XInternAtom(defaults->display, "XSEL_DATA", FALSE);
    Atom compound_text = XInternAtom(defaults->display, "COMPOUND_TEXT", FALSE);


    //Convert the selection to the type requested
    XConvertSelection(
            defaults->display, selection, type, selected, w, timestamp);
    XSync(defaults->display, FALSE);

    //These hold the values we'll get back from XGetWindowProperty
    XEvent event;
    Atom target;
    int format;
    unsigned long num_items;
    unsigned long remaining_bytes = 1; //"1" jump starts the loop
    unsigned char* value = NULL;

    //How much data we ask for at once
    unsigned int request_amount = 1024;

    //Keep looking at X events until we find the selection (clipboard) event
    BOOL waiting = TRUE;
    int prop_success;
    while(waiting) {
        XNextEvent(defaults->display, &event);
        if(event.type == SelectionNotify) {
            if (event.xselection.selection != selection) {
                continue;
            }

            while(remaining_bytes > 0) {

    //Outdented because it was getting too far over
    prop_success = XGetWindowProperty(
        event.xselection.display,   //Use the event's display
        event.xselection.requestor, //Use the event's requestor [sic]
        event.xselection.property,  //Use the event's property
        0L,                         //Start at beginning
        request_amount,             //Request length
        FALSE,                      //Do not delete
        (Atom)AnyPropertyType,      //Give me anything
        &target,                    //Where to put the property
        &format,                    //What kind of property did we get
        &num_items,                 //How many items did we get?
        &remaining_bytes,           //How much data is past request_amount
        &value);                    //What we got
    if(prop_success != Success) {
        XFree(value);
        value = NULL;
        waiting = FALSE;
        remaining_bytes = 0;
    }
    if(remaining_bytes > 0) {
        //Add enough space to hold it all plus some breathing room
        request_amount += remaining_bytes + 16;
        //And clean up the mess
        XFree(value);
    }

            }

            if(target == incr) {
                //We're getting the data a little at a time
                value = _get_incremental_selection(selection, event.xselection);
            } else if(target != text_atom &&
                    target != XA_STRING &&
                    target != compound_text) {
                //Something other than text is selected, and it won't convert
                waiting = FALSE;
            } else {
                //We have the whole selection
                waiting = FALSE;
            }
        }
    }
    return value;
}

unsigned char* _find_selection(Atom selection) {
    if (!_check_init()) {
        return (unsigned char*)0;
    }
    if(text_atom == None) {
        text_atom = XA_STRING;
    }
    unsigned char* data;

    data = _get_selection(selection, text_atom);
    if(data == NULL && text_atom != XA_STRING) {
        data = _get_selection(selection, XA_STRING);
    }
    return data;
}

/*
 * This gets called when we try to get a non-existent selection.
 * This is almost certain to be called when the program tries to get the
 * secondary selection, since almost no one uses that.
 */
int _no_selection_error_handler(Display *dsp, XErrorEvent *err) {
    return 0;
}

/*
 * This gets called when we can't put data onto the clipboard.
 */
int _cant_set_selection_error_handler(Display *dsp, XErrorEvent *err) {
    return 0;
}

BOOL _to_background() {
    // Clear old X11 values
    cleanup();
    pid_t pid;

    pid = fork();
    if(pid == -1) {
        fprintf (stderr, "Error with first fork.\n");
        return FALSE;
    } else if (pid > 0) {
        //Original thread does not exit
        return FALSE;
    }

    if (setsid () == -1) {
        fprintf (stderr, "Error with setsid\n");
    }

    pid = fork();
    if (pid == -1) {
        fprintf (stderr, "Error with second fork.\n");
        return FALSE;
    } else if (pid > 0) {
        //Second thread is superfluous
        _exit(0);
    }

    // Leave current partition just in case the window hangs around for a while
    if (chdir ("/") == -1) {
      fprintf(stderr, "Warning: unable to set current directory to /\n");
    }

    // This probably doesn't matter - but better safe than sorry
    umask(0);

    // I don't see any reason to redirect stdin, stdout, or stderr, since this isn't a "true" daemon

    //Get all new X11 values
    _check_init();
    return TRUE;
}

void _handle_selection_request(XEvent e, unsigned char* clip, Time ts) {
    if (!_check_init()) {
        return;
    }
    Atom multiple_atom = XInternAtom (defaults->display, "MULTIPLE", False);
    XSelectionRequestEvent req = e.xselectionrequest;
    if (req.property == None && req.target != multiple_atom) {
        // Requested by an obsolete client (ICCM 2.2)
        fprintf(stderr, "Obsolete client\n");
        req.property = req.target;
    }

    Atom timestamp_atom = XInternAtom (defaults->display, "TIMESTAMP", False);
    Atom targets_atom = XInternAtom(defaults->display, "TARGETS", False);

    //We handle four types of data - all are variations on the text type
    Atom types[4];
    types[0] = XInternAtom(defaults->display, "text/plain", False);
    types[1] = XInternAtom(defaults->display, "TEXT", False);
    types[2] = XA_STRING;
    if(XInternAtom (defaults->display, "UTF8_STRING", True) != None) {
        types[3] = XInternAtom (defaults->display, "UTF8_STRING", True);
    } else {
        types[3] = XA_STRING;
    }


    XEvent resp;
    resp.xselection.type = SelectionNotify;
    resp.xselection.display = e.xselection.display;
    resp.xselection.requestor = req.requestor;
    resp.xselection.selection = req.selection;
    resp.xselection.time = req.time;
    resp.xselection.target = req.target;

    if(req.time != CurrentTime && req.time < ts) {
        /* We are making sure we should service the request:
         *
         * RE: req.time != CurrentTime
         * Some clipboard clients are broken, and will request using
         * "CurrentTime".  We want those to work, although I can't imagine
         * what will happen when time wraps to 0 on Jan 19, 2038 on 32 bit
         * systems. :^)
         *
         * RE: req.time < ts
         * With correctly working systems, we check to make sure they requested
         * the clipboard after we took control.  If the request is from before
         * we took control, then we supply None.
         */

        //If we're here, it's not our request
        resp.xselection.property = None; //This means "no"
    } else if (req.target == timestamp_atom) {
        //The client simply wants to know when we took control.
        resp.xselection.property = req.property;
        XChangeProperty(
                resp.xselection.display,    //Display value is sent to
                resp.xselection.requestor,  //Who gets the value
                resp.xselection.property,   //What kind of value are we sending
                req.target,                 //We're giving them what they want
                8,                          //Use 8 bit value type
                PropModeReplace,            //Replace value in property
                (unsigned char *)&ts,       //Value to put into response
                1                           //There's only one value
        );
    } else if(req.target == targets_atom) {
        //Client wants to know what kind of data we can supply.
        resp.xselection.property  = req.property;
        XChangeProperty(
                resp.xselection.display,    //Display value is sent to
                resp.xselection.requestor,  //Who gets the value
                resp.xselection.property,   //What kind of value are we sending
                XA_ATOM,                    //Predefined atom from X11
                32,                         //Use 32 bit data type
                PropModeReplace,            //Replace value in property
                (unsigned char *)&types[0], //Pointer to first type
                4                           //There are four values
        );
    } else if(
            req.target == types[0] ||
            req.target == types[1] ||
            req.target == types[2] ||
            req.target == types[3]) {
        //Supported target type
        resp.xselection.property = req.property;
        XChangeProperty(
                resp.xselection.display,    //Display value is sent to
                resp.xselection.requestor,  //Who gets the value
                resp.xselection.property,   //What kind of value are we sending
                req.target,                 //We're giving them what they want
                8,                          //Use 8 bit value type
                PropModeReplace,            //Replace value in property
                clip,                       //Value to put into response
                strlen((char *)clip)        //The length to send
        );
    } else {
        //Unsupported target type
        resp.xselection.property  = None;
        XChangeProperty(
                resp.xselection.display,    //Display value is sent to
                resp.xselection.requestor,  //Who gets the value
                resp.xselection.property,   //What kind of value are we sending
                req.target,                 //We're giving them what they want
                8,                          //Use 8 bit value type
                PropModeReplace,            //Replace value in property
                (unsigned char *)None,      //Value to put into response
                1                           //There's only one value
        );
    }
    XSendEvent(req.display, req.requestor, True, 0, &resp);
}

void _set_selection(Atom clipboard, unsigned char* clip) {
    if (!_check_init()) {
        return;
    }
    if(! _to_background()) {
        return;
    }

    Window window = _dummy_window();
    Time ts = _get_timestamp();
    XSetSelectionOwner(defaults->display, clipboard, window, ts);
    Window owner = XGetSelectionOwner(defaults->display, clipboard);
    if(window != owner) {
        // Couldn't gain ownership
        return;
    }

    XEvent event;
    while(TRUE) {
        XFlush(defaults->display);
        XNextEvent(defaults->display, &event);
        if(event.type == SelectionClear &&
                event.xselectionclear.selection == clipboard) {
            // Someone else wants to take over this clipboard
            XSync(defaults->display, FALSE);
            exit(0);
        } else if(event.type == SelectionRequest &&
                event.xselectionrequest.selection == clipboard) {
            // Someone wants our data
            _handle_selection_request(event, clip, ts);
        }
    }

}

/*
Here's the basic methodology for putting data into the clipboard:
    1) Fork into the background (see http://www.steve.org.uk/Reference/Unix/faq_2.html#SEC16).
    2) Get a dummy window.
    3) Ask X if that dummy window can "own" the selection in question (primary, secondary, or clipboard)
    4) If yes, then let the daemon watch for X events regarding the clipboard
    5) When someone asks for the contents, the daemon sends them the data
    6) When someone asks for control of the selection, then the daemon thread exits
*/
void put_primary(char* clip) {
    if (!_check_init()) {
        return;
    }
    XSetErrorHandler(_cant_set_selection_error_handler);
    Atom primary = XInternAtom(defaults->display, "PRIMARY", FALSE);
    _set_selection(primary, (unsigned char*)clip);
    XSetErrorHandler(NULL);
}

void put_secondary(char* clip) {
    if (!_check_init()) {
        return;
    }
    XSetErrorHandler(_cant_set_selection_error_handler);
    Atom secondary = XInternAtom(defaults->display, "SECONDARY", FALSE);
    _set_selection(secondary, (unsigned char*)clip);
    XSetErrorHandler(NULL);
}

void put_clipboard(char* clip) {
    if (!_check_init()) {
        return;
    }
    XSetErrorHandler(_cant_set_selection_error_handler);
    Atom clipboard = XInternAtom(defaults->display, "CLIPBOARD", FALSE);
    _set_selection(clipboard, (unsigned char*)clip);
    XSetErrorHandler(NULL);
}

char* get_primary() {
    if (!_check_init()) {
        return '\0';
    }
    unsigned char* clip;
    XSetErrorHandler(_no_selection_error_handler);
    if(text_atom == None) {
        text_atom = XInternAtom(defaults->display, "UTF8_STRING", TRUE);
    }
    Atom primary = XInternAtom(defaults->display, "PRIMARY", FALSE);
    clip = _find_selection(primary);
    XSetErrorHandler(NULL);
    return (char*)clip;
}

char* get_secondary() {
    if (!_check_init()) {
        return '\0';
    }
    unsigned char* clip;
    XSetErrorHandler(_no_selection_error_handler);
    if(text_atom == None) {
        text_atom = XInternAtom(defaults->display, "UTF8_STRING", TRUE);
    }
    Atom secondary = XInternAtom(defaults->display, "SECONDARY", FALSE);
    clip = _find_selection(secondary);
    XSetErrorHandler(NULL);
    return (char*)clip;
}

char* get_clipboard() {
    if (!_check_init()) {
        return '\0';
    }
    unsigned char* clip;
    XSetErrorHandler(_no_selection_error_handler);
    if(text_atom == None) {
        text_atom = XInternAtom(defaults->display, "UTF8_STRING", TRUE);
    }
    Atom clipboard = XInternAtom(defaults->display, "CLIPBOARD", FALSE);
    clip = _find_selection(clipboard);
    XSetErrorHandler(NULL);
    return (char*)clip;
}
