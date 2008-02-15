//-*-Mode: C++;-*-

#ifndef _cacheInfo_h_
#define _cacheInfo_h_

#include <iostream.h>

// CW: this class defines a container which stores information
// of the cache structure of the current architecture. It is used
// for parametrizing the array padding transformations

// CW: forward declaration
class cacheInfo;

ostream & operator<<(ostream& os, const cacheInfo& c);

// CW: the class is defines only the minimum information needed
// for my application. If there is a generall purpose class
// available which describes a cache structure we might to
// use that class instead.
class cacheInfo
{
  public:
  
    cacheInfo(int size, int lineSize);
    ~cacheInfo();

	// CW : returns size of the cache in bytes
	int getCacheSize() const { return cacheSize; };
	// CW: returns size of a cache line in bytes
	int getCacheLineSize() const { return cacheLineSize; }; 

  private:
  
	int cacheSize;			// CW: size of cache in bytes
	int cacheLineSize;		// CW: size of a cache line in bytes
};

#endif
