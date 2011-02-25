#include "createMap.h"

bool VERBOSE_MESSAGES_OF_WAVE = false;

int main( int argc, char * argv[] )
{
        // Build the AST used by ROSE

        SgProject* project = frontend(argc,argv);
        std::vector<SgNode*> macroVec2 = linearize_subtree(project);
 
    createMap map_tokenToAST(macroVec2,wave_tokenStream);
    map_tokenToAST.printNodeToTokenMap();
    map_tokenToAST.printTokenToNodeMap();


//    mapSeparators(wave_tokenStream, macroVec2); 
};

