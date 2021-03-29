#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

#ifndef DATAFLOW_CFG_H
#define DATAFLOW_CFG_H

#include "genericDataflowCommon.h"
#include <map>
#include <string>
#include <vector>

namespace VirtualCFG {

// "Interesting" node and edge filters for use with dataflow analyses
class DataflowEdge;

bool defaultFilter (CFGNode cfgn);

class DataflowNode {
        public:
        CFGNode n;
        bool (*filter) (CFGNode cfgn); // a filter function to decide which raw CFG node to show (if return true) or hide (otherwise)           
        
        // We enforce the user codes (the framework) of DataflowNode to explicitly set filter, or provide default filter on their own
        DataflowNode(CFGNode n, bool (*f) (CFGNode)): n(n), filter(f) {}
        // By default, the default filter function is used unless otherwise specified
//        DataflowNode(CFGNode n, bool (*f) (CFGNode) = defaultFilter): n(n), filter(f) {}
        DataflowNode(const DataflowNode& dfn): n(dfn.n), filter (dfn.filter) {} 

        std::string toString() const {return n.toString();}
        std::string toStringForDebugging() const {return n.toStringForDebugging();}
        std::string id() const {return n.id();}
        SgNode* getNode() const {return n.getNode();}
        unsigned int getIndex() const {return n.getIndex();}
        std::vector<DataflowEdge> outEdges() const;
        std::vector<DataflowEdge> inEdges() const;
        bool isInteresting() const; 
        bool operator==(const DataflowNode& o) const {return n == o.n;}
        bool operator!=(const DataflowNode& o) const {return !(*this == o);}
        bool operator<(const DataflowNode& o) const {return n < o.n;}
        
        std::string str(std::string indent="") const;
};

typedef std::map<SgNode*, DataflowNode> m_AST2CFG;

class DataflowEdge {
        CFGPath p;
        bool (*filter) (CFGNode cfgn);
        
        public:
//        DataflowEdge(CFGPath p, bool (*f) (CFGNode) = defaultFilter): p(p), filter(f) {}
        DataflowEdge(CFGPath p, bool (*f) (CFGNode) ): p(p), filter(f) {}
        DataflowEdge(const DataflowEdge& dfe): p(dfe.p), filter(dfe.filter) {}

        std::string toString() const {return p.toString();}
        std::string toStringForDebugging() const {return p.toStringForDebugging();}
        std::string id() const {return p.id();}
        DataflowNode source() const {return DataflowNode(p.source(), filter);}
        DataflowNode target() const {return DataflowNode(p.target(), filter);}
        EdgeConditionKind condition() const {return p.condition();}
        SgExpression* caseLabel() const {return p.caseLabel();}
        SgExpression* conditionBasedOn() const {return p.conditionBasedOn();}
        std::vector<SgInitializedName*> scopesBeingExited() const {return p.scopesBeingExited();}
        std::vector<SgInitializedName*> scopesBeingEntered() const {return p.scopesBeingEntered();}
        bool operator==(const DataflowEdge& o) const {return p == o.p;}
        bool operator!=(const DataflowEdge& o) const {return p != o.p;}
        //bool operator<(const DataflowEdge& o) const {return p < o.p;}
};

//inline DataflowNode makeDataflowCfg(SgNode* start, bool (*f) (CFGNode) = defaultFilter) {
inline DataflowNode makeDataflowCfg(SgNode* start, bool (*f) (CFGNode) ) {
        // Returns CFG node for just before start
        return DataflowNode(cfgBeginningOfConstruct(start), f);
}

bool isDataflowInteresting(CFGNode cn);
}

#endif
#endif
