/* 
 * File:   InterProcDataFlowAnalysis.h
 * Author: rahman2
 *
 * Created on August 15, 2011, 11:06 AM
 */
#ifndef INTERPROCDATAFLOWANALYSIS_H
#define INTERPROCDATAFLOWANALYSIS_H
#include <vector>
#include "sage3basic.h"

class InterProcDataFlowAnalysis {
protected:
    SgProject *project;

public:
    InterProcDataFlowAnalysis(SgProject *_project) : project(_project){};
    
    virtual void getFunctionDeclarations(std::vector<SgFunctionDeclaration*> &) = 0;
    
    virtual bool runAndCheckIntraProcAnalysis(SgFunctionDeclaration *) = 0;

    virtual void run();
};



#endif  /* INTERPROCDATAFLOWANALYSIS_H */

