//-*-Mode: C++;-*-
#ifndef _arrayReference_h_
#define _arrayReference_h_

#include "supported_dimensions.h"

//
// CW: the arrayReference class stores all of the constants which
// define a uniform generated array reference
//
class arrayReference
{
  public:

	// CW: initialize all constants to 0
	arrayReference()
	{
		for(int i=0;i<ROSE_MAX_ARRAY_DIMENSION;i++) refConstant[i]=0;
	};

	// CW: copy constants from another reference
	arrayReference(const arrayReference& rhs)
	{
		for(int i=0;i<ROSE_MAX_ARRAY_DIMENSION;i++) refConstant[i]=rhs.refConstant[i];
	};

	~arrayReference() {};

	// CW: return constant for dimension read only
	int operator()(int elem) const { return refConstant[elem]; };

	// CW: return constant read and writeable
	int& operator[](int elem) { return refConstant[elem]; };

  private:

	// CW: here we store the constants which define unified generated
	// references
	int refConstant[ROSE_MAX_ARRAY_DIMENSION];
};

#endif
