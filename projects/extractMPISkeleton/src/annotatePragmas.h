#ifndef __annotatePragmas_H_LOADED__
#define __annotatePragmas_H_LOADED__

#include "rose.h"

typedef enum { pragma_undef,        // no annotation
               pragma_preserve,     // annotated for preservation
               pragma_remove        // annotated for removal
             }
  PresRemUnd;

PresRemUnd getPresRemUnd (SgStatement *s);

void annotatePragmas (SgProject *project);
  // Must be called before 'skeletonizeCode'


#endif
