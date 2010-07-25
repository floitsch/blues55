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

#include "Mouse.hpp"
#include "XDisplay.hpp"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>

void Mouse::downUp(ButtonEnum button, bool press)
{
    XTestGrabControl(disp(), true);
    XTestFakeButtonEvent(disp(), button, press, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
}

void Mouse::click(ButtonEnum button)
{
    XTestGrabControl(disp(), true);
    XTestFakeButtonEvent(disp(), button, true, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
    XTestGrabControl(disp(), true);
    XTestFakeButtonEvent(disp(), button, false, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
}

void Mouse::doubleClick()
{
    click(BUTTON_left);
    usleep(1000);
    click(BUTTON_left);
}

void Mouse::move(int x, int y)
{
    XTestGrabControl(disp(), true);
    XTestFakeMotionEvent(disp(), -1, x, y, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
}

void Mouse::relMove(int x, int y)
{
    XTestGrabControl(disp(), true);
    XTestFakeRelativeMotionEvent(disp(), x, y, CurrentTime);
    XTestGrabControl(disp(), false);
    XFlush(disp());
}
