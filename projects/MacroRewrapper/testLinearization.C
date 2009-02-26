// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <functional>
using namespace std;

bool VERBOSE_MESSAGES_OF_WAVE = false;
#include "macroRewrapper.h"
#include <vector>
int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     std::vector<SgNode*> linearizedSubtree = linearize_subtree(project);

     for (std::vector<SgNode*>::iterator it_sub =  linearizedSubtree.begin();
           it_sub != linearizedSubtree.end(); ++ it_sub){
            std::cout << (*it_sub)->class_name() << std::endl; 
     }


//     return backend(project);

   };

