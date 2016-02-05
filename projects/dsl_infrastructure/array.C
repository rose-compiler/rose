#include "array.h"

// default constructor
array::array()
   {
   }

// copy constructor
array::array(const array & X)
   {
   }

// Assignment operator
array array::operator= (const array & X)
   {
     return *this;
   }

// simple array operator
array operator+ (const array & X, const array & Y)
   {
  // Wrong implementation, but we are not leveraging the runtime semantics.
     return array();
   }
