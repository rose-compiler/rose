#include "rose.h"
#include <sageDoxygen.h>
#include <list>
#include <commandline_processing.h>
#include <string>

using namespace std;

class Nothing : public AstSimpleProcessing 
   {

     public:
          virtual void visit(SgNode *n) 
             {
             }
   };

int main( int argc, char * argv[] ) 
   {
     CommandlineProcessing::addCppSourceFileSuffix("docs");
     CommandlineProcessing::addCppSourceFileSuffix("h");

     char **newArgv = new char*[argc+2];
     newArgv[0] = argv[0];
     newArgv[1] = "-rose:collectAllCommentsAndDirectives";
     for (int i = 1; i <= argc; i++)
        {
          newArgv[i+1] = argv[i];
        }

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(argc+1,newArgv);

     Doxygen::annotate(sageProject);
     Nothing n;
     n.traverse(sageProject, preorder);
   }
