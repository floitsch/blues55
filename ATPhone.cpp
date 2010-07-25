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
#include "ISerialConnection.hpp"
#include "IPhone.hpp"
#include "types.hpp"
#include "verbose.hpp"
#include <stdio.h>
#include <string.h>

ATPhone::ATPhone(ISerialConnection* con)
    : con_(con)
{
    buffer_[0]='\0';
    init();
}

ATPhone::~ATPhone() {}

void ATPhone::init()
{
    const char* AT_RESET = "ATZ\r";
    const char* AT_DISABLE_ECHO = "ATE0\r";
    const char* AT_DISABLE_ECHO_ALT = "ATE=0\r";

    writeOK(AT_RESET);
    try {
	writeOK(AT_DISABLE_ECHO);
	writeOK(AT_DISABLE_ECHO);
    } catch (...) { // TODO: verify exception
	// try alternativ form
	writeOK(AT_DISABLE_ECHO_ALT);
	writeOK(AT_DISABLE_ECHO_ALT);
    }
    vprint("initted");
}

void ATPhone::startListening()
{
    const char* AT_START_LISTENING = "AT+CMER=3,2\r";
    const char* AT_START_LISTENING_ALT = "AT+CMER=3,2,,,\r";
    try {
	writeOK(AT_START_LISTENING);
    } catch (...) { // TODO: verify exception
	// try alternativ form
	writeOK(AT_START_LISTENING_ALT);
    }
    vprint("listening");
}

void ATPhone::stopListening()
{
    const char* AT_STOP_LISTENING = "AT+CMER=0,0\r";
    writeOK(AT_STOP_LISTENING);
    vprint("stopping listening");
}

KeyEvent ATPhone::readKey()
{
    vprint("reading key");
    const char* AT_KEY_EVENT_START = "+CKEV: ";

    // a key-event is of the following form:
    // for the up-key:
    // down-event:
    // +CKEV: ^,1
    // release-event:
    // +CKEV: ^,0
    // and sometimes both together (at least on my S55). I think
    // the whitespace is a newline, but I don't care:
    // +CKEV: ^,1 +CKEV: ^,0
    // Motorola V500 sends events slightly different: +CKEV: "1",1

    // if a previous call left an event, we can use it now.
    // otherwise we read until we get a new event.
    while (!strstr(buffer_, AT_KEY_EVENT_START))
    {
	con_->read(buffer_, MAX_LENGTH);

	if (strstr(buffer_, "ERROR") != NULL)
	    throw "received ERROR";
    }

    // we could do it in while loop, but this is more explicit.
    char* eventStart = strstr(buffer_, AT_KEY_EVENT_START);

    // destroy the eventStart-pattern right away, so we won't use it again in the
    // next call to this function:
    eventStart[0] = 'X';

    KeyEvent event;
    char* statusChars = strchr(eventStart, ',');
    event.isPressed = (statusChars[1] != '0');

    char* keyPos = eventStart + 7;

    // Motorola sends +CKEV: "1",1
    if(keyPos[0] == '"')
	keyPos++;
    
    // Motorola can also send +CKEV: ":X",1
    if(keyPos[0] == ':')
	keyPos++;

    // Correct for bug in T610
    if(keyPos[0] == 0x1B && keyPos[1] == 0x14)
	event.c = '^';
    else
	event.c = keyPos[0];

    if (verbose_g)
	std::cerr << "read key: " << event.c << std::endl;
    
    return event;
}

void ATPhone::waitFor(const char* sequence, uint nbRetries)
{
    char buffer[MAX_LENGTH];
    uint retries = 0;
    while (retries < nbRetries)
    {
	retries++;
	con_->read(buffer, MAX_LENGTH);
	// we don't care where exactly the sequence is.
	if (strstr(buffer, sequence) != NULL)
	    return;
#ifdef DEBUG
	else
	    printf("waiting failed with %s (waiting for %s)\n", buffer, sequence);
#endif
    }
    throw "wait failed";
}

void ATPhone::waitForOK(uint nbRetries)
{
    waitFor("OK", nbRetries);
}

void ATPhone::writeOK(const char* buffer)
{
    con_->write(buffer);
    waitForOK();
}

#ifdef TEST

class TestConnection : public ISerialConnection {
public:
    TestConnection()
	: round_(0)
	{}

    void write(const char* buffer)
	{
	    printf("writing to device: %s\n", buffer);
	}
    
    uint read(char* buffer, uint maxlen)
	{
	    const char* returns[] = {
		"ATZ OK",
		"ATE0 OK",
		"OK",
		"OK",
		"+CKEV: ^,1",
		"+CKEV: ^,0 +CKEV: 2,1 +CKEV: 2,0",
		"+CKEV: 2,0\n+CKEV: 3,1\n+CKEV: 3,0",
		"ERROR" };

	    strcpy(buffer, returns[round_]);
	    printf("receiving from device: %s\n", buffer);
	    int length = strlen(returns[round_]);
	    round_++;
	    return length;
	}
private:
    int round_;
};

int main(int argc, char** argv)
{
    try {
	TestConnection con;
	ATPhone phone(&con);
	phone.startListening();
	while(true) {
	    KeyEvent event = phone.readKey();
	    printf("received char: %c (%s)\n",
		   event.c,
		   event.isPressed?"pressed":"released");
	}
    } catch(...) {
	printf("in catch\n");
    }
    return 0;
}

#endif
