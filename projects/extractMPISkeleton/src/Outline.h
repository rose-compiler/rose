#ifndef __Outline_H_LOADED__
#define __Outline_H_LOADED__

#include "APISpec.h"

/*******************************************************************************
 skeletonizeCode - if 'outline' then outline the non-skeleton, otherwise
                   just remove.
 Preconditions:
   All statements have been properly annotated with their APIDep attributes.
*/

void skeletonizeCode (APISpecs *specs, SgProject *proj, bool outline,
                      bool genPDF);

#endif
