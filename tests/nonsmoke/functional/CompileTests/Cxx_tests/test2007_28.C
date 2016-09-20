/*
Hi Dan,
compiling the following code
#include "rose.h"
#include <stdarg.h>

int main(){
   va_list ap;
   va_end(ap);
}

using the translator:

int main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     VariantVector vv1(V_SgAddressOfOp);
     std::list<SgNode*> memoryPoolTraversal =
NodeQuery::queryMemoryPool(vv1);
     for(std::list<SgNode*>::iterator it_beg =
memoryPoolTraversal.begin(); it_beg != memoryPoolTraversal.end();
                ++it_beg){
             ROSE_ASSERT((*it_beg)->get_parent()!=NULL);
     }
     return backend(project);
   }

gives the following error:

/home/andreas/test-parent-NULL.c l 5 c 4
/home/andreas/REPOSITORY-SRC/ROSE/January-2b-2007/NEW_ROSE/tutorial/identityTranslator.C:11:
int main(int, char**): Assertion "(*it_beg)->get_parent()!=__null" failed.
Aborted

which means the the parent is not set for the SgAddressOfOp in this case
where the constructs is buildin.
*/

#include <stdarg.h>

int main()
   {
     va_list ap;
     va_end(ap);
   }

