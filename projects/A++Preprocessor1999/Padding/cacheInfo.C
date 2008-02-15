#include "cacheInfo.h"
#include <assert.h>
#include <iostream.h>

ostream & operator<<(ostream& os, const cacheInfo& c)
{
	os << "cache size : " << c.getCacheSize();
	os << ", line size: " << c.getCacheLineSize() << " ";
	
	return os;
}

cacheInfo::cacheInfo(int size, int lineSize)
: cacheSize(size), cacheLineSize(lineSize)
{
	// CW: cache size should be a multiple of cache line size
	assert(cacheSize%cacheLineSize==0);
}

cacheInfo::~cacheInfo()
{}
