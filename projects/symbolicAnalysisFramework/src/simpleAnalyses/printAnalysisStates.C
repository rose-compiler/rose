#include "printAnalysisStates.h"

printAnalysisStates::printAnalysisStates(Analysis* creator, vector<int>& factNames, vector<int>& latticeNames, ab latSide, string indent="")
{
//printf("printAnalysisStates::printAnalysisStates() creator=%p\n", creator);
	this->creator = creator;
	this->factNames = factNames;
	this->latticeNames = latticeNames;
	this->latSide = latSide;
	this->indent = indent;
}

void printAnalysisStates::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	printf("%sfunction %s() node=<%s | %s | %d> state=%p n=%p sgn=%p creator=%p\n", indent.c_str(), func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str(), n.getIndex(), &state, &n, n.getNode(), creator);
	//const vector<Lattice*>* masterLat;
	//if(latSide==above) masterLat = &(state.getLatticeAbove(creator));
	//else               masterLat = &(state.getLatticeBelow(creator));
	//printf("    creator=%p, state=%p, masterLat.size()=%lu\n", creator, &state, (unsigned long)(masterLat->size()));
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
		Lattice* lat;
		if(latSide==above) lat = state.getLatticeAbove(creator, *it);
		else               lat = state.getLatticeBelow(creator, *it);
		if(lat)
			printf("%s    lattice%d = \n%s\n", indent.c_str(), *it, lat->str(indent+"    ").c_str());
		else
			printf("%s    lattice%d = None\n", indent.c_str(), *it);
	}
	//printf("    creator=%p, masterLat.size()=%lu\n", creator, (unsigned long)(masterLat->size()));
}
