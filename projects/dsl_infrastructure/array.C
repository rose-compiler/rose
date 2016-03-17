#include "array.h"

// Default constructor (we need an implementation so that it
// can be used in the initialization of the dsl_attribute_map).
array::array()
   {
   }

// Assignment operator
array array::operator= (const array & X)
   {
     return *this;
   }

// Assignment operator
array array::operator= (int x)
   {
     return *this;
   }

// Simple array operator
array operator+ (const array & X, const array & Y)
   {
  // Wrong implementation, but we are not leveraging the runtime semantics.
     return array();
   }

#if 0
// These are not required.

// Copy constructor
array::array(const array & X)
   {
   }

#endif
