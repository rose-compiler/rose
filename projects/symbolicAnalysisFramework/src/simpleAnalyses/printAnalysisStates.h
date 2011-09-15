#ifndef PRINT_ANALYSIS_STATES_H
#define PRINT_ANALYSIS_STATES_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "lattice.h"

#include <vector>
#include <string>

class printAnalysisStates : public UnstructuredPassIntraAnalysis
{
        public:
        Analysis* creator;
        std::vector<int> latticeNames;
        std::vector<int> factNames;
        std::string indent;
        typedef enum {above=0, below=1} ab;
        ab latSide; // Records whether we should print lattices above or below each node.
                
        printAnalysisStates(Analysis* creator, std::vector<int>& factNames, std::vector<int>& latticeNames, ab latSide, std::string indent);
        
        void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

#endif
