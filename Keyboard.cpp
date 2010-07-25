/*
 * Blues55 - use your S55 or compatible phone as a remote for your PC.
 * Copyright (C) 2006
 * Florian Loitsch <blues55 at florian dot loitsch dot com>
 * 
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#include "Keyboard.hpp"
#include "XDisplay.hpp"
#include "verbose.hpp"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <iostream>

static
KeyCode
keysymStr2keycode(Display* disp,
		  const std::string& keysymStr)
{
    KeySym ks = XStringToKeysym(keysymStr.c_str());
    if(ks == NoSymbol)
    {
	// TODO: log, and handle error correctly
	vprint("couldn't map keysymbol");
	return (keysymStr2keycode(disp, "space"));
    }
    return XKeysymToKeycode(disp, ks);
}

void
Keyboard::downUp(const std::string& keysymStr, bool press)
{
    XTestGrabControl(disp(), true);
    KeyCode kc = keysymStr2keycode(disp(), keysymStr);
    XTestFakeKeyEvent(disp(), kc, press, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
}
