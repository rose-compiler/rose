/* 
 * File:   ReachingDefinitionFacadeTest.C
 * Author: rahman2
 *
 * Created on August 15, 2011, 4:16 PM
 */

#include "ReachingDefinitionFacade.h"

using namespace std;


void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-dot :generate DOT output \n";
}


/*
 * 
 */
int main(int argc, char** argv) {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }

    SgProject sageProject ( (int)argc,argv);
    SageInterface::changeAllLoopBodiesToBlocks(&sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);
    
    SgFunctionDeclaration *mainDecl = SageInterface::findMain(&sageProject);
    
    ROSE_ASSERT(mainDecl != NULL);
    
    SgFunctionDefinition *mainDef = mainDecl->get_definition();
    
    ROSE_ASSERT(mainDef != NULL);
    
    ReachingDefinitionFacade *reachDef = new ReachingDefinitionFacade(mainDef);
    reachDef->run();
    
    reachDef->toDot("ReachingDef.dot");
    
            

    
    
    return 0;
}

