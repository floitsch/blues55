Blues55 transforms a Siemens S55 cell-phone into a remote control for your computer. It allows to control the keyboard or mouse with the cellphone.
It is basically a [Bluemote](http://swik.net/Bluemote) light for bluetooth enabled Siemens phones (at least for the S55) and other compatible phones.

Siemens phones don't have the same functionality as Sony Ericsson phones, though. It's for instance not possible to send userface elements onto the phone. Blues55 just sniffs every key-press on the phone. It's therefore best to go into key-lock state of the phone before launching Blues55.

Usage:
  * Establish a serial bluetooth-connection with the phone (usually `sudo rfcomm bind 0 00:11:22:33:44:55`)
  * Go into the keylock-state of the phone.
  * Launch Blues55: `./blues55 -k` (-k for keyboard control; -m for mouse).

M. Fedotov (author of [anyRemote](http://anyremote.sourceforge.net/)) provided a patch to make Blues55 work with Motorola V500 phones.