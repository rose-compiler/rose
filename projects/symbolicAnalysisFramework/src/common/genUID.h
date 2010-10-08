#ifndef GEN_UID_H
#define GEN_UID_H

#ifdef THREADED
#include "tbb/mutex.h"
#endif

// Generates unique unsigned long ids, with a user-specified minimum id
class genUID
{
	unsigned long curUID;
	#ifdef THREADED
	tbb::mutex* uidAccessM;
	#endif
	
	public:
	// minimum ID defaults to 0
	genUID();
	// minimum ID will be minUID
	genUID(unsigned long minUID);
	
	unsigned long getUID();
};

#endif
