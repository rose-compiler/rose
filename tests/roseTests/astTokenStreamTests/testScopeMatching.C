// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include "linearizeAST.h"
#include "createMap.h"
#include "doCompleteMapping.h"

int main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
    std::vector<std::string> newArgv(argv,argv+argc);
    newArgv.push_back("-rose:wave");
    SgProject* project = frontend(newArgv);
	std::vector<SgNode*> macroVec2 = linearize_subtree(project);
 
    createMap map_tokenToAST(macroVec2,wave_tokenStream);
    map_tokenToAST.printNodeToTokenMap();
    map_tokenToAST.printTokenToNodeMap();

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

