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

#include "ATPhone.hpp"
#include "BuiltIn.hpp"
#include "SerialConnection.hpp"
#include <iostream>
#include <string.h>

#define VERSION 0.1

bool verbose_g = false;

static char* device_g = "/dev/rfcomm0";
static bool keyboard_g = false;
static bool mouse_g = false;

static char* progName_g;

void version()
{
    std::cout << progName_g << " " << VERSION << std::endl;
}

void usage(const char* message)
{
    if (message)
	std::cerr << message << std::endl;
    std::cerr << progName_g << " [options]" << std::endl
	     << std::endl
	     << "Options:" << std::endl
	     << "  -d --device DEVICE    the device (defaults to /dev/rfcomm0)" << std::endl
	     << "  -m --mouse            control the mouse" << std::endl
	     << "  -k --keyboard         control the keyboard" << std::endl
	     << "  -v --verbose          activate verbose output" << std::endl
	     << "     --version          prints the version" << std::endl;
    exit(message?-1:0);
}

void parseArgs(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
	const char* arg = argv[i];
	if (arg[0] == '-' && arg[1] == '-')
	{
	    if (!strcmp("device", arg+2))
	    {
		if (i < (argc - 1))
		    device_g = argv[++i];
		else
		    usage("--device flag needs argument.");
	    } else if (!strcmp("mouse", arg+2)) {
		mouse_g = true;
	    } else if (!strcmp("keyboard", arg+2)) {
		keyboard_g = true;
	    } else if (!strcmp("usage", arg+2)) {
		usage(NULL);
	    } else if (!strcmp("help", arg+2)) {
		usage(NULL);
	    } else if (!strcmp("verbose", arg+2)) {
		verbose_g = true;
	    } else if (!strcmp("version", arg+2)) {
		version();
	    }
	} else if (arg[0] == '-') {
	    for (int j = 0; arg[j]; j++)
	    {
		switch (arg[j])
		{
		case 'm': mouse_g = true; break;
		case 'k': keyboard_g = true; break;
		case 'h': usage(NULL); break;
		case 'u': usage(NULL); break;
		case 'v': verbose_g = true; break;
		case 'd':
		    if (arg[j+1] || i + 1 >= argc)
		    {
			usage("the 'd' flag must not be followed by other flags,\n"
			      "and needs an additional parameter");
		    } else
			device_g = argv[++i];
		}
	    }
	} else
	    usage(NULL);
    }
}

int main(int argc, char* argv[])
{
    progName_g = argv[0];
    parseArgs(argc, argv);

    if (!device_g)
	usage(NULL);
    if (!keyboard_g && !mouse_g)
	usage(NULL);
    if (keyboard_g && mouse_g)
	usage("Keyboard and Mouse are mutually exclusive");
    try
    {
	SerialConnection serial(device_g);
	ATPhone phone(&serial);
	phone.startListening();
	if (keyboard_g)
	    BuiltIn::keyboard(&phone);
	else
	    BuiltIn::mouse(&phone);
	phone.stopListening();
    } catch (const char* message) {
	std::cerr << "ERROR: " << message << std::endl;
	exit(-1);
    }
    return 0;
}
