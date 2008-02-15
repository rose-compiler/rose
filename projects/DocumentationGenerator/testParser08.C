#include "rose.h"
#include <copy_unparser.h>

#if 0
class Nothing : public AstSimpleProcessing 
   {

     public:
          virtual void visit(SgNode *n) 
             {
             }
   };
#endif

int main( int argc, char * argv[] ) 

   {
     SgProject* sageProject = frontend(argc,argv);
#if 0

     Nothing n;
     n.traverse(sageProject, preorder);
#endif

#if 0
     CopyUnparser copy(sageProject->get_file(0));
     sageProject->get_file(0).unparse(NULL, &copy);
#else
     CopyUnparseProject(sageProject);
#endif
   }
