#ifndef INDEX_ARG_H
#define INDEX_ARG_H "IndexArg.h"

#include "A++.h"

//==================================================================================
//  This class is used as the argument to the operator() instead of using an Index, 
//  Range or int.
//  This class is used to reduce the number of () operators that need to be written
//  This class knows how to convert an int, Index or Range to a member of this class
//
//==================================================================================
class IndexArg
{
public:
  Index I;
  Range R;
  
  IndexArg(){ }
  IndexArg( const int i )   // convert an int to an IndexArg
  {
    I=Range(i,i); R=Range(i,i);
  }
  IndexArg( const Index & I0 )   // convert an Index to an IndexArg
  {
    I=I0; R=Range(I0.getBase(),I0.getBound());
  }
  IndexArg( const Range & R0 )   // convert a Range to an IndexArg
  {
    I=R0; R=R0;
  }
  ~IndexArg(){}
};


#endif
