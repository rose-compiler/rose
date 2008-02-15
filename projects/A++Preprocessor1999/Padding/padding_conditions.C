#include <stdlib.h>
#include <assert.h>

#include "padding_conditions.h"

//
// CW: the algoritms are documented in 
// Gabriel Rivera, Chau-Wen Tseug
// "Data Transformation for Eliminating Conflict Misses"
// Department of Computer Science, University of Maryland
//

// CW: utility functions used by condition functions...
// firstConflict1 resp. firstContact2 is used by linpad1 resp. linpad2
// condition_linpad is used by condition_linpad1 and condition_linpad2
// calcDistance is used by condition_intrapad and condition inter_pad
static int firstConflict1(int r,int r2,int c,int c2,int size);
static int firstConflict2(int r,int r2,int c,int c2,int size);
static bool condition_linpad(const cacheInfo& cInfo, const arrayInfo& aInfo, int (*conflict)(int,int,int,int,int));
static unsigned long calcDistance(const arrayInfo& aInfo, const arrayReference& r, const arrayReference& s);
static unsigned long calcDistance(const arrayInfo& aInfo, const arrayReference& r, const arrayInfo& bInfo, const arrayReference& s);

// ---------------------------------
// CW: intra array padding functions
// ---------------------------------

bool condition_linpad1(const cacheInfo& cInfo, const arrayInfo& aInfo)
{
	return condition_linpad(cInfo,aInfo,firstConflict1);
}

bool condition_linpad2(const cacheInfo& cInfo, const arrayInfo& aInfo)
{
	return condition_linpad(cInfo,aInfo,firstConflict2);
}

// CW: assertions/restrictions:
// - cacheSize > 3*PADLITE_M
// function is able to handle dimensions >2!
bool condition_intrapadlite(const cacheInfo& cInfo, const arrayInfo& aInfo)
{
	const int cacheSize=cInfo.getCacheSize();
	assert(cacheSize>3*PADLITE_M);

	const int dimension = aInfo.getNumberOfDimensions();
	assert(dimension>0);

	// no intra padding for 1D!
	if(dimension==1) return FALSE;
	
	int subPlaneSize=aInfo.getElementSize();

	for(int i=1;i<dimension;i++)
	{
		// CW: calculate size of subplane
		subPlaneSize *= aInfo.getColSize(i);

		// CW: calculate conflict distance of two adjacent sub planes
		int distance1 = subPlaneSize % cacheSize;
		if (distance1 > cacheSize/2) distance1 -= cacheSize;
		// CW: calculate conflict distance of two sub planes seperated
		// by one subplane
		int distance2 = (2*subPlaneSize) % cacheSize;
		if (distance2 > cacheSize/2) distance2 -= cacheSize;

		// CW: are they conflicting?
		if((abs(distance1)<PADLITE_M) || (abs(distance2)<PADLITE_M))
			return TRUE;

	}
	// padding condition is FALSE for all subplanes
	return FALSE;
}

// CW: no restrictions but will only give accurate answers if the array info
// contains references
bool condition_intrapad(const cacheInfo& cInfo, const arrayInfo& aInfo)
{
	const int cacheLineSize = cInfo.getCacheLineSize();
	const int cacheSize = cInfo.getCacheSize();

	const List<arrayReference>& references = aInfo.getReferences();
	const int noReferences = aInfo.getNumberOfReferences();

	// CW: if we have no references or only one we do not
	// have conflicting references in a llop nest. Hnece,
	// we do no padding
	if((noReferences==0) || (noReferences==1)) return FALSE;

	// CW: for every combination of two references to a test
	// padding condition
	for(int i=0 ; i< noReferences-1; i++)
	{
		arrayReference ref1 = references[i];
		for(int j=i+1; j < noReferences; j++)
		{
			arrayReference ref2 = references[j];

			const int distance = calcDistance(aInfo,ref1,ref2);
			// CW: if the two referenced memory locations are not
			// adjacent and are matched to the same cache line then
			// the padding condition is TRUE
			if( (distance>cacheLineSize)
				&& (distance%cacheSize)<cacheLineSize) return TRUE;
		}
	}
	return FALSE;
}


// ---------------------------------
// CW: inter array padding functions
// ---------------------------------

// CW: no restrictions, arrays must not be conforming
bool condition_interpadlite(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo)
{
	const int cacheSize = cInfo.getCacheSize();
	unsigned long distance = (bInfo.getBaseAddress()-aInfo.getBaseAddress()) % cacheSize;
	
	if (distance > cacheSize/2) distance -= cacheSize;
	bool b = (labs(distance)<PADLITE_M);

	return b;
}

// CW: restriction:
// - arrays must be conforming
bool condition_interpad(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo)
{
	assert(aInfo.getNumberOfDimensions()==bInfo.getNumberOfDimensions());
	assert(aInfo.getElementSize()==bInfo.getElementSize());
#ifndef NDEBUG
	for(int k=1;k<aInfo.getNumberOfDimensions();k++)
	{
		// CW: column sizes must be identical except of the
		// size of the column of the highest dimension
		assert(aInfo.getColSize(k)==bInfo.getColSize(k));
	}
#endif

	const int cacheLineSize = cInfo.getCacheLineSize();
	const int cacheSize = cInfo.getCacheSize();

	const unsigned long baseAddressOfA = aInfo.getBaseAddress();
	const unsigned long baseAddressOfB = bInfo.getBaseAddress();

	const List<arrayReference>& referencesA = aInfo.getReferences();
	const int noReferencesA = aInfo.getNumberOfReferences();
	const List<arrayReference>& referencesB = bInfo.getReferences();
	const int noReferencesB = bInfo.getNumberOfReferences();

	// CW: if there are no references to a or b then we don't
	// have conflicting references
	if(noReferencesA == 0 ||  noReferencesB == 0) return FALSE;

	for(int i=0 ; i< noReferencesA; i++)
	{
		arrayReference ref1 = referencesA[i];
		for(int j=0; j < noReferencesB; j++)
		{
			arrayReference ref2 = referencesB[j];

			unsigned long distance = calcDistance(aInfo,ref1,bInfo,ref2);
			// CW: if the to referenced memory locations are not
			// adjacent and are matched to same cache line then
			// the padding condition is TRUE
			if( (distance>cacheLineSize)
				&& (distance%cacheSize)<cacheLineSize) return TRUE;
		}
	}
	return FALSE;
}


// ---------------------
// CW: utility functions
// ---------------------

// CW: restrictions:
// if elemSize>cacheLineSize and elemSize%cacheLineSize==0 it is very likely that the condition
// is always true. I'm not sure when exactly this happens. If the above condition is
// TRUE you should not use linpad together with other conditions because if linpad is always
// TRUE no good padding will be found nevertheless the other conditions specify
static bool condition_linpad(const cacheInfo& cInfo, const arrayInfo& aInfo, int (*conflict)(int,int,int,int,int))
{
	const int dimension=aInfo.getNumberOfDimensions();

	// CW: no padding needed/possible for 1D arrarys
	if(dimension==1) return FALSE;

	int subPlaneSize=aInfo.getElementSize();

	for(int i=1;i<dimension;i++)
	{
		// CW: calculate size of subplane
		subPlaneSize *= aInfo.getColSize(i);
		
		int rowSize=1;
		for(int k=i+1;k<=dimension;k++)
		{
			rowSize*=aInfo.getColSize(k);
		}

		// CW: collect cache information
		int cacheSize = cInfo.getCacheSize();
		int cacheLineSize = cInfo.getCacheLineSize();

		// CW: determine a j which is tolerable for us
		int jTolerable = LINPAD_MAX_J;
		if(cacheSize/cacheLineSize < jTolerable) jTolerable=cacheSize/cacheLineSize;
		if(rowSize < jTolerable) jTolerable = rowSize;

		// CW: I have added this since it doesn't seem to be reasonable
		// to do padding if the tolerable amount of columns fits in the
		// cache to check whether the padding condition is TRUE!
		if( jTolerable*subPlaneSize > cacheSize)
		{
			const int j=conflict(cacheSize,subPlaneSize,0,1,cacheLineSize);	

			// if j is to small than that padding condition is true and
			// we want padding to be applied
			if (j<jTolerable) return TRUE;
		}
	}
	return FALSE;
}

// CW: determine first conflicting j with j*Col_s = 0 | mod C_s
static int firstConflict1(int r,int r2,int c,int c2,int size)
{
	if (r2==0) return c2;
	else return firstConflict1(r2,r%r2,c2,((int)r/r2)*c2+c,size);
}

// CW: determine first conflicting j with j*Col_s < L_s | mod C_s
static int firstConflict2(int r,int r2,int c,int c2,int size)
{
	if (r2<size) return c2;
	else return firstConflict2(r2,r%r2,c2,((int)r/r2)*c2+c,size);
}


static unsigned long calcDistance(const arrayInfo& aInfo, const arrayReference& r, const arrayReference& s)
{
	unsigned long product=1;
	unsigned long distance = 0;

	for(int j=1; j <= aInfo.getNumberOfDimensions() ; j++)
	{
		distance += (r(j-1) - s(j-1))*product;
		product *= aInfo.getColSize(j);
	}
	distance *= aInfo.getElementSize();	
	
	return labs(distance);
}

static unsigned long calcDistance(const arrayInfo& aInfo, const arrayReference& r, const arrayInfo& bInfo, const arrayReference& s)
{
	unsigned long product = 1;
	unsigned long distance = 0;

	unsigned long baseOfA = aInfo.getBaseAddress();
	unsigned long baseOfB = bInfo.getBaseAddress();

	for(int j=1; j <= aInfo.getNumberOfDimensions() ; j++)
	{
		const int baseDiff = bInfo.getLowerBound(j) - aInfo.getLowerBound(j);
		distance += (r(j-1) - s(j-1) + baseDiff)*product;
		product *= aInfo.getColSize(j);
	}
	distance *= aInfo.getElementSize();	
	distance += baseOfA-baseOfB;
	
	return labs(distance);	
}
