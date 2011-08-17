#include "InterProcDataFlowAnalysis.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH


void InterProcDataFlowAnalysis::run () {

     bool change;
     int iteration = 0;
    do {
            change = false;
            
            std::vector<SgFunctionDeclaration*> processingOrder;        

            getFunctionDeclarations(processingOrder);
           
            foreach (SgFunctionDeclaration* funcDecl, processingOrder) {

                change |= runAndCheckIntraProcAnalysis(funcDecl);
            
            }
            
            iteration++;
        } while (change);
        std::cout << "Total Interprocedural iterations: " << iteration << std::endl;
}
