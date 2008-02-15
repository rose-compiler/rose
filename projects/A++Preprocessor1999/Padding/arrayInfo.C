#include "arrayInfo.h"
#include <assert.h>
#include <limits.h>

// CW: constructor to generate an array info:
// elemSize: size of an array element
// address: base address of array
// columnSizes: size of all columns (list element 0 is size of dimension 1 and so on)
// bound: lower bound (base) of the array for each dimension (list element 0 is lower bound of dimension 1 and so on)
arrayInfo::arrayInfo(int elemSize,unsigned long address,const List<const int>& columnSizes, const List<const int>& bounds)
: elementSize(elemSize),baseAddress(address),dimensions(columnSizes.getLength())
{
	assert(columnSizes.getLength()==dimensions);
	assert(bounds.getLength()==dimensions);
	assert(elemSize>0);
	
	for(int i=0;i<dimensions;i++)
	{
		colSize[i]=columnSizes[i];
		lowerBound[i]=bounds[i];
	}
}

arrayInfo::arrayInfo(const arrayInfo& rhs)
:baseAddress(rhs.baseAddress),elementSize(rhs.elementSize),dimensions(rhs.dimensions),
references(rhs.references)
{
	assert(elementSize>0);
	assert(dimensions>0);

	for(int i=0;i<dimensions;i++)
	{
		colSize[i]=rhs.colSize[i];
		lowerBound[i]=rhs.lowerBound[i];
	}
}

arrayInfo& arrayInfo::operator=(const arrayInfo& rhs)
{
	if(this==&rhs) return *this;
	
	baseAddress=rhs.baseAddress;
	elementSize=rhs.elementSize;
	dimensions=rhs.dimensions;
	references=rhs.references;

	assert(elementSize>0);
	assert(dimensions>0);

	for(int i=0;i<dimensions;i++)
	{
		colSize[i]=rhs.colSize[i];
		lowerBound[i]=rhs.lowerBound[i];
	}
	
	return *this;
}

// CW: nothing to do so far
arrayInfo::~arrayInfo()
{}

// CW: return column size: dim is dimension starting from 1
int arrayInfo::getLowerBound(int dim) const
{
	assert(dim <= dimensions);
	return lowerBound[dim-1];
}


// CW: return column size: dim is dimension starting from 1
int arrayInfo::getColSize(int dim) const
{
	assert(dim <= dimensions);
	return colSize[dim-1];
}

// CW: set column size: dim is dimension starting from 1
void arrayInfo::setColSize(int dim, int size)
{
	assert(dim <= dimensions);
	assert(size>0);
	
	colSize[dim-1]=size;
}

// CW: returns size of array in bytes
unsigned long arrayInfo::getSize() const
{
	unsigned long result=colSize[0];
	for(int i=1; i<getNumberOfDimensions(); i++)
	{
		assert(result< ULONG_MAX/colSize[i]);
		
		result *= colSize[i];
	}
	
	assert(result < ULONG_MAX/getElementSize());
	return result * getElementSize();
}

// CW: add information of a new reference to array
void arrayInfo::addReference(const arrayReference& newRef)
{
	references.addElement(newRef);
}

Boolean arrayInfo::operator==(const arrayInfo& rhs) const
{
	if(this==&rhs) return TRUE;
	else{
		if(baseAddress!=rhs.baseAddress) return FALSE;
		if(elementSize!=rhs.elementSize) return FALSE;
		if(dimensions!=rhs.dimensions) return FALSE;

		for(int i=0;i<dimensions;i++){
			if(colSize[i]!=rhs.colSize[i]) return FALSE;
			if(lowerBound[i]!=rhs.lowerBound[i]) return FALSE;
		}
		// CW: all values are identical!
		return TRUE;	
	}
}

Boolean arrayInfo::operator!=(const arrayInfo& rhs) const
{
	if((this->operator==(rhs)) == TRUE ) return FALSE;
	else return TRUE;
}
