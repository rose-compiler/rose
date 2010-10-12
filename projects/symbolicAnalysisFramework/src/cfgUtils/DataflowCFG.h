#ifndef DATAFLOW_CFG_H
#define DATAFLOW_CFG_H

#include "common.h"

namespace VirtualCFG {

// "Interesting" node and edge filters for use with dataflow analyses
class DataflowEdge;

class DataflowNode {
	public:
	CFGNode n;
	
	DataflowNode(CFGNode n): n(n) {}
	DataflowNode(const DataflowNode& dfn): n(dfn.n) {}
	std::string toString() const {return n.toString();}
	std::string toStringForDebugging() const {return n.toStringForDebugging();}
	std::string id() const {return n.id();}
	SgNode* getNode() const {return n.getNode();}
	unsigned int getIndex() const {return n.getIndex();}
	std::vector<DataflowEdge> outEdges() const;
	std::vector<DataflowEdge> inEdges() const;
	bool isInteresting() const {return true;}
	bool operator==(const DataflowNode& o) const {return n == o.n;}
	bool operator!=(const DataflowNode& o) const {return !(*this == o);}
	bool operator<(const DataflowNode& o) const {return n < o.n;}
	
	std::string str(std::string indent="") const;
};

typedef map<SgNode*, DataflowNode> m_AST2CFG;

class DataflowEdge {
	CFGPath p;
	
	public:
	DataflowEdge(CFGPath p): p(p) {}
	DataflowEdge(const DataflowEdge& dfe): p(dfe.p) {}
	std::string toString() const {return p.toString();}
	std::string toStringForDebugging() const {return p.toStringForDebugging();}
	std::string id() const {return p.id();}
	DataflowNode source() const {return DataflowNode(p.source());}
	DataflowNode target() const {return DataflowNode(p.target());}
	EdgeConditionKind condition() const {return p.condition();}
	SgExpression* caseLabel() const {return p.caseLabel();}
	SgExpression* conditionBasedOn() const {return p.conditionBasedOn();}
	std::vector<SgInitializedName*> scopesBeingExited() const {return p.scopesBeingExited();}
	std::vector<SgInitializedName*> scopesBeingEntered() const {return p.scopesBeingEntered();}
	bool operator==(const DataflowEdge& o) const {return p == o.p;}
	bool operator!=(const DataflowEdge& o) const {return p != o.p;}
	//bool operator<(const DataflowEdge& o) const {return p < o.p;}
};

inline DataflowNode makeDataflowCfg(SgNode* start) {
	// Returns CFG node for just before start
	return DataflowNode(cfgBeginningOfConstruct(start));
}

bool isDataflowInteresting(CFGNode cn);
}

#endif
