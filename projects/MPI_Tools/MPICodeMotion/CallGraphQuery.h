#ifndef __CALL_GRAPH_QUERY_H__
#define __CALL_GRAPH_QUERY_H__

//#include "rose.h"
#include <CallGraph.h>

class CallGraphQuery
{
  public:
	CallGraphQuery(SgProject* project);
	~CallGraphQuery() { delete cgBuilder; }

  public:
	void initialize();
	bool pathExist(SgGraphNode* from, SgGraphNode* to);
	void constructFuncWithMPICallSet(std::list<SgExprStatement*> mpiCallList);
	SgIncidenceDirectedGraph* getCallGraphCreate() { return cgBuilder->getGraph(); }
	std::set<SgFunctionDeclaration*> getFuncWithMPICallSet() { return funcWithMPICallSet; }
	bool hasMPICall(SgExprStatement* callSite);

  protected:
	CallGraphBuilder* cgBuilder;
	std::set<SgFunctionDeclaration*> funcWithMPICallSet;

	void DFS(SgGraphNode* p, SgFunctionDeclaration* mpiCall);
};

#endif
