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

#include "SerialConnection.hpp"
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define BAUDRATE B115200    

SerialConnection::SerialConnection(const char* device)
{
    openPort(device);
}

SerialConnection::~SerialConnection()
{
    closePort();
}

// TODO: handle timeout.
uint
SerialConnection::read(char* buffer, unsigned int maxChars)
{
    int cnt;

    cnt = ::read(comPort, buffer, maxChars-1);

    if(cnt==-1) throw -errno;
    if(!cnt) return 0;

    buffer[cnt]=0;
    return cnt;
}

void
SerialConnection::write(const char* buffer, uint nbChars)
{
    int cnt;

    cnt = ::write(comPort, buffer, nbChars);
    if(cnt == -1) throw "Couldn't write data to port";
}

void
SerialConnection::openPort(const char* device)
{
    // comPort is class-field.
    comPort = open(device, O_RDWR | O_NOCTTY ); 
    if (comPort <0) throw "Couldn't open comport";

    tcgetattr(comPort, &oldTio); /* save current serial port settings */

    termios newTio;
    bzero(&newTio, sizeof(newTio)); /* clear struct for new port settings */

    newTio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newTio.c_cflag &= ~ECHO;
    newTio.c_iflag = IGNCR; //IGNPAR | ICRNL;
    newTio.c_oflag = 0;
    newTio.c_lflag = 0; //ICANON;
    newTio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newTio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */

    tcflush(comPort, TCIOFLUSH);
    if(tcsetattr(comPort, TCSANOW, &newTio) == -1)
	throw "Couldn't set port attributes";
}

void
SerialConnection::closePort()
{
  /* restore the old port settings */
    tcsetattr(comPort, TCSANOW, &oldTio);
    close(comPort);
}
