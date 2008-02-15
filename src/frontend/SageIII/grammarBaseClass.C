
/* Make boolean syntax work even when boolean is broken. */
#ifdef BOOL_IS_BROKEN
#ifndef BOOL_IS_TYPEDEFED
typedef int bool;
#define BOOL_IS_TYPEDEFED 1
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif

#include "rose.h"
#include "grammarBaseClass.h"

ROSE_BaseGrammar::ROSE_BaseGrammar ()
   {
   }


bool
ROSE_BaseGrammar::isRootGrammar() const
   {
     return (parentGrammar == NULL) ? false : true;
   }

void
ROSE_BaseGrammar::setParentGrammar ( ROSE_BaseGrammar* Xptr )
   {
     parentGrammar = Xptr;
   }

ROSE_BaseGrammar*
ROSE_BaseGrammar::getParentGrammar () const
   {
     return parentGrammar;
   }

