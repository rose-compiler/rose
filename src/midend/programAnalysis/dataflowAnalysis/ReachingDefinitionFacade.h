/* 
 * File:   ReachingDefinitionFacade.h
 * Author: rahman2
 *
 * Created on August 15, 2011, 3:55 PM
 * This class uses Facade design pattern to provide an easy interface for the Reaching Definition
 * Analysis. It hides the complex AstInterface, AliasAnalysis etc and provides a much easier and
 * cleaner access to ReachingDefinition Analysis using SgNode.
 *  */

#include "sage3basic.h"
#include "ReachingDefinition.h"
#include <DefUseChain.h>
#include <AstInterface_ROSE.h>

#ifndef REACHINGDEFINITIONFACADE_H
#define REACHINGDEFINITIONFACADE_H

class ReachingDefinitionFacade {

    SgFunctionDefinition *functionDefinition;
    DefaultDUchain *graph;

public:
    ReachingDefinitionFacade(SgNode * head);
    void run();
    DefaultDUchain* getGraph() { return graph; }
    void toDot(std::string);
    
};




#endif  /* REACHINGDEFINITIONFACADE_H */

