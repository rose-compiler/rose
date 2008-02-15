#include "alignData.h"
#include <assert.h>

// CW: if address is an aligned adress it returns the address
// otherwise it returns the next aligned address following that
// address.
unsigned long alignData(unsigned long address)
{
	// CW: I don't really have an idea how to implement that
	// the only problem is that the compiler will do alignment
	// of arrays and the padding is useless if it places data
	// on not aligned addresses since the compiler will use another
	// address. I'm not sure whether this is a major problem
	assert(address!=0);
	
	unsigned long alignment = address % DATA_ALIGNMENT_VALUE;
	return address+alignment;
};
