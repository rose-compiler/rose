//-*-Mode: C++;-*-
#ifndef _arrayInfo_h_
#define _arrayInfo_h_

#include "supported_dimensions.h"
#include "arrayReference.h"
#include "tlist.h"
#include "bool.h"


//
// CW: this class stores all the data which is
// needed for the array padding transformations.
// Basically it stores all information which is related to
// an array: sizes, dimension, references to array, etc.
//
class arrayInfo
{
  public:

	// simplified constructor for 2D case
    arrayInfo(int elemSize,unsigned long address,const List<const int>& columnSizes, const List<const int>& bounds);
    arrayInfo(const arrayInfo&);
    ~arrayInfo();
    
    arrayInfo& operator=(const arrayInfo&);
    Boolean operator==(const arrayInfo& rhs) const;
    Boolean operator!=(const arrayInfo& rhs) const;
    
    // CW: returns size of an element of the array in bytes
    int getElementSize() const { return elementSize; };
    int getNumberOfDimensions() const { return dimensions; };

	// CW: size of col for dimension dim (starts from 1)
    int getColSize(int dim) const;
    void setColSize(int dim, int size);
	// CW: get size of array in bytes
	unsigned long getSize() const;
	
	// CW: return reference to List which contains constants for
	// all references to that array. The references are read only!
	const List<arrayReference>& getReferences() const { return references; };
	int getNumberOfReferences() const { return references.getLength();};
	// CW: add a new reference to the array
	void addReference(const arrayReference& newRef);
	
	// CW: return and set base address of array:
	unsigned long getBaseAddress() const { return baseAddress; };
	void setBaseAddress(unsigned long address) { baseAddress=address; };
	
	// CW: returns lower bound of array for dimension dim
	int getLowerBound(int dim) const;

  private:

	// CW: I should not use int here...
	unsigned long baseAddress;						// CW: base address of array
	int elementSize;								// CW: size of one element of the array
	int dimensions;									// CW: dimension of array
	
	// CW: remove int arrays and use vector instead
	// channge implementation of get/set functions
	int colSize[ROSE_MAX_ARRAY_DIMENSION];			// CW: size of each column
	int lowerBound[ROSE_MAX_ARRAY_DIMENSION];		// CW: lowerBounds of the array

	// CW: references hve to be uniformy generated
	// Hence, we can store them by remembering the constants
	List<arrayReference> references;

	// CW: internal functions:
};

#endif
