#ifndef MAPPINGWS_H
#define MAPPINGWS_H "MappingWS.h"


#include <A++.h>

#include "OvertureTypes.h"     // define real to be float or double

#include "wdhdefs.h"           // some useful defines and constants

//===========================================================================
//===========================================================================
class MappingWorkSpace
{
 public:
  realArray x0;      // new copy needed so we can shift x in the periodic case
  realArray r0;      // or if the inverse is "blocked"
  realArray rx0; 
  intArray index0;   // index pointer from r0 back to r
  bool index0IsSequential;
  Index I0;

  MappingWorkSpace( )
  {
    index0IsSequential=TRUE;
  }
  ~MappingWorkSpace()
  {
  }

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

};

#endif   // MAPPINGWS_H
