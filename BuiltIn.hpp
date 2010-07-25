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

#pragma once

#include "IPhone.hpp"

enum BuiltInEnum
{
    BUILT_IN_ENUM_keyboard,
    BUILT_IN_ENUM_mouse
};

class BuiltIn
{
public:
    static void run(BuiltInEnum builtIn, IPhone* phone);
    
    static void keyboard(IPhone* phone);
    static void mouse(IPhone* phone);
    // shutdown, ........
    
private: // not to be instanciated
    BuiltIn() {}
    ~BuiltIn() {}
};
