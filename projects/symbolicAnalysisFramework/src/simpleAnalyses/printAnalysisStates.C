#include "printAnalysisStates.h"

printAnalysisStates::printAnalysisStates(Analysis* creator, vector<int>& factNames, vector<int>& latticeNames, string indent="")
{
printf("printAnalysisStates::printAnalysisStates() creator=%p\n", creator);
	this->creator = creator;
	this->factNames = factNames;
	this->latticeNames = latticeNames;
	this->indent = indent;
}

void printAnalysisStates::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	printf("%sfunction %s() node=<%s | %s | %d> state=%p\n", indent.c_str(), func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str(), n.getIndex(), &state);
	const vector<Lattice*>& masterLatBel = state.getLatticeBelow(creator);
	printf("    creator=%p, state=%p, masterLatBel.size()=%d\n", creator, &state, masterLatBel.size());
	for(vector<int>::iterator it = factNames.begin(); it!=factNames.end(); it++)
	{
		NodeFact* fact = state.getFact(creator, *it);
		if(fact)
			printf("%s    fact%d = \n%s\n", indent.c_str(), *it, fact->str(indent+"    ").c_str());
		else
			printf("%s    fact%d = None\n", indent.c_str(), *it);
	}
	
	for(vector<int>::iterator it = latticeNames.begin(); it!=latticeNames.end(); it++)
	{
		Lattice* lat = state.getLatticeBelow(creator, *it);
		if(lat)
			printf("%s    lattice%d = \n%s\n", indent.c_str(), *it, lat->str(indent+"    ").c_str());
		else
			printf("%s    lattice%d = None\n", indent.c_str(), *it);
	}
	printf("    creator=%p, masterLatBel.size()=%d\n", creator, masterLatBel.size());
}
