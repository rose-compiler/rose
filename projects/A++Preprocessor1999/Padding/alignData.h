//-*-Mode: C++;-*-
#ifndef _alignData_h_
#define _alignData_h_

#define DATA_ALIGNMENT_VALUE 4

// CW: if address is an aligned adress it returns the address
// otherwise it returns the next aligned address following that
// address.
unsigned long alignData(unsigned long address);

#endif
