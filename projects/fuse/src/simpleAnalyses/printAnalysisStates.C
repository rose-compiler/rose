
/* GB - 2013-05-10 - Temporarily removing printAnalysisStates until we develop a good procedure
                     for iterating the Abstraction graph in an easy-to-read order
#include "printAnalysisStates.h"
#include "compose.h"

using namespace dbglog;
namespace fuse {

printAnalysisStates::printAnalysisStates(ComposedAnalysis* creator, vector<int>& factNames, vector<int>& latticeNames, ab latSide) : 
UnstructuredPassAnalysis(creator), latticeNames(latticeNames), factNames(factNames), latSide(latSide)
{ }

void printAnalysisStates::visit(const Function& func, PartPtr part, NodeState& state)
{
  ostringstream funcName; funcName<< "function "<<func.get_name().getString()<<"() part=["<<part->str()<<" state="<<(&state)<<" analysis="<<analysis->str();
  region reg(1, 1, region::midLevel, funcName.str());
  //const vector<Lattice*>* masterLat;
  //if(latSide==above) masterLat = &(state.getLatticeAbove(creator));
  //else         masterLat = &(state.getLatticeBelow(creator));
  //printf("    creator=%p, state=%p, masterLat.size()=%lu\n", creator, &state, (unsigned long)(masterLat->size()));
  for(vector<int>::iterator it = factNames.begin(); it!=factNames.end(); it++)
  {
    NodeFact* fact = state.getFact((Analysis*)analysis, *it);
    if(fact)
      dbg << "    fact"<<*it<<" = \n"<<fact->str("    ")<<"\n";
    else
      dbg << "    fact"<<*it<<" = None\n";
  }
  
  for(vector<int>::iterator it = latticeNames.begin(); it!=latticeNames.end(); it++)
  {
    Lattice* lat;
    if(latSide==above) lat = state.getLatticeAbove((Analysis*)analysis, *it);
    else         lat = state.getLatticeBelow((Analysis*)analysis, *it);
    if(lat)
      dbg << "    lattice"<<*it<<" = \n"<<lat->str("    ")<<"\n";
    else
      dbg << "    lattice"<<*it<<" = None\n";
  }
}
}; // namespace fuse
*/


