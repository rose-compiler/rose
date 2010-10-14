#include "genUID.h"

genUID::genUID()
{
	curUID=0;
	#ifdef THREADED
	uidAccessM = new tbb::mutex();
	#endif
}

genUID::genUID(unsigned long minUID)
{
	curUID=minUID;
	
	#ifdef THREADED
	uidAccessM = new tbb::mutex();
	#endif
}

unsigned long genUID::getUID()
{
	#ifdef THREADED
	// Construction of myLock acquires lock on uidAccessM
	tbb::mutex::scoped_lock myLock(*uidAccessM);
	#endif

	return ++curUID;
	// Destruction of myLock releases lock on uidAccessM
}
