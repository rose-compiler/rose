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
    //! Stored the Project Object
    SgProject *project;

public:
    InterProcDataFlowAnalysis(SgProject *_project) : project(_project){};
    //! Get the list of Function Declaration participating in DataFlow Analysis
    virtual void getFunctionDeclarations(std::vector<SgFunctionDeclaration*> &) = 0;
    
    //! Execute IntraProc DataFlow Analysis and check whether anything changed
    virtual bool runAndCheckIntraProcAnalysis(SgFunctionDeclaration *) = 0;
    
    //! Function which actually performs the DataFlowAnalyis
    virtual void run();
};



#endif  /* INTERPROCDATAFLOWANALYSIS_H */

