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

#include "BuiltIn.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "MutexLock.hpp"
#include "verbose.hpp"
#include <map>
#include <string>
#include <pthread.h>
#include <unistd.h>

void BuiltIn::run(BuiltInEnum builtIn, IPhone* phone) {
    switch (builtIn)
    {
    case BUILT_IN_ENUM_keyboard: keyboard(phone); break;
    case BUILT_IN_ENUM_mouse: mouse(phone); break;
    }
}

/* TODO: remove static map */
static std::map<char, std::string> key2keysymMap_g;
static bool isKeysymMapInitialized_g = false;

static void fillMap()
{
    if (isKeysymMapInitialized_g) return;
    key2keysymMap_g['1'] = "Home";
    key2keysymMap_g['2'] = "Up";
    key2keysymMap_g['3'] = "Prior";
    key2keysymMap_g['4'] = "Left";
    key2keysymMap_g['5'] = "space";
    key2keysymMap_g['6'] = "Right";
    key2keysymMap_g['7'] = "End";
    key2keysymMap_g['8'] = "Down";
    key2keysymMap_g['9'] = "Next";
    key2keysymMap_g['^'] = "Up";
    key2keysymMap_g['v'] = "Down";
    key2keysymMap_g['V'] = "Down";
    key2keysymMap_g['<'] = "Left";
    key2keysymMap_g['>'] = "Right";
    key2keysymMap_g['d'] = "XF86AudioLowerVolume";
    key2keysymMap_g['u'] = "XF86AudioRaiseVolume";
    key2keysymMap_g['-'] = "XF86AudioLowerVolume";
    key2keysymMap_g['+'] = "XF86AudioRaiseVolume";
    isKeysymMapInitialized_g = true;
}

void BuiltIn::keyboard(IPhone* phone) {
    // we are already in listening mode.
    /* phone->startListening() */

    vprint("Controlling keyboard");
    // TODO: Hack (lazy initialization using globals).
    if (!isKeysymMapInitialized_g)
	fillMap();
    
    Keyboard keyboard;
    while(true)
    {
	KeyEvent key = phone->readKey();
	// TODO: how to get out of loop? (and verify current solution.)
	if (key.c == 'E') return; // cancel

	if (key2keysymMap_g.find(key.c) != key2keysymMap_g.end())
	{
	    if (verbose_g)
	    {
		std::cerr << "simulating keypress for key <"
			  << key2keysymMap_g[key.c] << ">." << std::endl;
	    }
	    keyboard.downUp(key2keysymMap_g[key.c], key.isPressed);
	} else {
	    vprint("No keycode associated to pressed key.");
	}
    }
}

static int deltaX_g;
static int deltaY_g;
static bool stopMoving_g;
static pthread_mutex_t mouseLock_g = PTHREAD_MUTEX_INITIALIZER;

void* moveMouse(void* pmouse)
{
    Mouse& mouse = *((Mouse*)pmouse);
    while(true)
    {
	{
	    MutexLock lock(&mouseLock_g);
	    if (stopMoving_g)
		return NULL;
	    
	    if (deltaX_g != 0 || deltaY_g != 0)
		mouse.relMove(deltaX_g, deltaY_g);
	}
	usleep(2500); // wait quarter of a second
    }
}

/**
 * mouse movement.
 * 2 ways:
 * - either wait for phone-events only for a predefined intervall, and
 *   move the mouse, if the time-out has been reached (and repeat polling).
 * - start a new thread, that continuously moves the mouse (until a key-up
 *   event has been received).
 * - don't move mouse continuously (not really a correct way).
 */
// ATM it's the 'not really correct' way.
void BuiltIn::mouse(IPhone* phone)
{
    vprint("Controlling mouse");
    Mouse mouse;
    
    // start another thread, that will move the mouse periodically
    deltaX_g = 0;
    deltaY_g = 0;
    stopMoving_g = false;
    pthread_t mouseMoverThread;
    pthread_create(&mouseMoverThread, NULL, &moveMouse, &mouse);

    const int mdelta = 1;

    // we are already in listening mode.
    /* phone->startListening() */

    bool done = false;
    while(!done)
    {
	KeyEvent key = phone->readKey();

	MutexLock lock(&mouseLock_g);
	switch(key.c)
	{
	case 27: //Simply assume it's joy up coz T610 key report is buggy
	case '^':
	case '5':
	    deltaY_g = key.isPressed? -mdelta: 0;
	    break;
	case 'v':
	case 'V':
	case '8':
	    deltaY_g = key.isPressed? mdelta: 0;
	    break;
	case '<':
	case '4':
	case '7':
	    deltaX_g = key.isPressed? -mdelta: 0;
	    break;
	case '>':
	case '6':
	case '9':
	    deltaX_g = key.isPressed? mdelta: 0;
	    break;

	case '1':
	case '[':
	    vprint("Left Click");
	    mouse.downUp(BUTTON_left, key.isPressed);
	    break;
	case '2':
	    vprint("Middle Click");
	    mouse.downUp(BUTTON_middle, key.isPressed);
	    break;
	case ']':
	case '3':
	    vprint("Right Click");
	    mouse.downUp(BUTTON_right, key.isPressed);
	    break;
	case '*':
	    vprint("Double Click");
	    if (key.isPressed) mouse.doubleClick();
	    break;
	    
	default:
	    done = true;
	}
    }
    stopMoving_g = true;
    // wait till other thread finished
    void* status_ptr;
    pthread_join(mouseMoverThread, &status_ptr);
}
