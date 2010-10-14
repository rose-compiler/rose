// we don't want functionState.h to be included until nodeState.h is finished
#define NO_FUNCTION_STATE_H
#include "nodeState.h"
// inclure functionState.h now
#undef NO_FUNCTION_STATE_H
#include "functionState.h"

using namespace std;

// Records that this analysis has initialized its state at this node
void NodeState::initialized(Analysis* analysis)
{
	#ifdef THREADED
	BoolMap::accessor wInit;
	initializedAnalyses.insert(wInit, (Analysis*)analysis);
	wInit->second = true;
	#else
	initializedAnalyses[(Analysis*)analysis] = true;
	#endif
}

// Returns true if this analysis has initialized its state at this node and false otherwise
bool NodeState::isInitialized(Analysis* analysis)
{
	#ifdef THREADED
	BoolMap::const_accessor rInit;
	return initializedAnalyses.find(rInit, (Analysis*)analysis);
	#else
	return initializedAnalyses.find((Analysis*)analysis) != initializedAnalyses.end();
	#endif
}

/*// adds the given lattice, organizing it under the given analysis and lattice name
void NodeState::addLattice(const Analysis* analysis, int latticeName, Lattice* l)
{
	addLattice_ex(dfInfoAbove, analysis, latticeName, l);
	addLattice_ex(dfInfoBelow, analysis, latticeName, l->copy());
}*/

// sets this node's lattices for this analysis
void NodeState::setLattices(const Analysis* analysis, vector<Lattice*>& lattices)
{
	vector<Lattice*> tmp;
	
	// Empty out the current mappings of analysis in dfInfoAbove and  dfInfoBelow
	#ifdef THREADED
		LatticeMap::accessor wA, wB;
	
		if(dfInfoAbove.find(wA, (Analysis*)analysis))
			wA->second.clear();
		else
			wA->second = tmp;
			
		if(dfInfoBelow.find(wB, (Analysis*)analysis))
			wB->second.clear();
		else
			wB->second = tmp;
	#else
		if(dfInfoAbove.find((Analysis*)analysis) != dfInfoAbove.end())
			dfInfoAbove[(Analysis*)analysis].clear();
		else
			dfInfoAbove[(Analysis*)analysis] = tmp;
	
		if(dfInfoBelow.find((Analysis*)analysis) != dfInfoBelow.end())
			dfInfoBelow[(Analysis*)analysis].clear();
		else
			dfInfoBelow[(Analysis*)analysis] = tmp;
	#endif
	
	// Set dfInfoAbove and  dfInfoBelow to lattices
	#ifdef THREADED
		// set dfInfoAbove to lattices
		wA->second = lattices;
		wA.release();
		
		// Now iterate through dfInfoAbove, copying each lattice in dfInfoAbove over to dfInfoBelow
		LatticeMap::const_accessor rA;
		dfInfoAbove.find(rA, (Analysis*)analysis);
		
		// copy dfInfoAbove to dfInfoBelow (including copies of all the lattices)
		for(vector<Lattice*>::const_iterator it = rA->second.begin(); 
		    it!=rA->second.end(); it++)
		{
			Lattice* l = (*it)->copy();
			//cout << "NodeState::setLattices pushing dfInfoBelow: "<<l->str("")<<"\n";
			wB->second.push_back(l);
		}
		rA.release();
		wB.release();
	#else
		// set dfInfoAbove to lattices
		dfInfoAbove[(Analysis*)analysis] = lattices;
		// copy dfInfoAbove to dfInfoBelow (including copies of all the lattices)
		for(vector<Lattice*>::iterator it = dfInfoAbove[(Analysis*)analysis].begin(); 
		    it!=dfInfoAbove[(Analysis*)analysis].end(); it++)
		{
			Lattice* l = (*it)->copy();
			//cout << "NodeState::setLattices pushing dfInfoBelow: "<<l->str("")<<"\n";
			dfInfoBelow[(Analysis*)analysis].push_back(l);
		}
	#endif
	
	/*printf("Lattices above:\n");
	for(vector<Lattice*>::iterator it = dfInfoAbove[(Analysis*)analysis].begin(); 
	    it!=dfInfoAbove[(Analysis*)analysis].end(); it++)
	{	
		cout << (*it)->str("    ") << "\n";
	}*/
	
	/*printf("Lattices below: state=%p, analysis=%p\n", this, analysis);
	for(vector<Lattice*>::iterator it = dfInfoBelow[(Analysis*)analysis].begin(); 
	    it!=dfInfoBelow[(Analysis*)analysis].end(); it++)
	{	
		cout << (*it)->str("    ") << "\n";
	}*/
	
	// Records that this analysis has initialized its state at this node
	initialized((Analysis*)analysis);
}

// Sets this node's above lattices for this analysis to the given vector of lattices, 
// deleting any previous mapping (the previous Lattices are freed)
void NodeState::setLatticeAbove(const Analysis* analysis, vector<Lattice*>& lattices)
{
	// if the analysis currently has a mapping in dfInfoAbove
#ifdef THREADED
	LatticeMap::accessor w;
	if(dfInfoAbove.find(w, (Analysis*)analysis))
#else
	LatticeMap::iterator w;
	if((w = dfInfoAbove.find((Analysis*)analysis)) != dfInfoAbove.end())
#endif
	{
		// Empty out the current mapping of analysis in dfInfoAbove
		for(vector<Lattice*>::iterator it = w->second.begin(); 
		    it != w->second.end(); it++)
		{ delete *it; }
		w->second.clear();
		
		// Create the new mapping
		w->second = lattices;
	}
	else
	{
#ifdef THREADED		
		// Create the new mapping
		w->second = lattices;
#else
		// Create the new mapping
		dfInfoAbove[(Analysis*)analysis] = lattices;
#endif
	}
	
	/*printf("Lattices above:\n");
	for(vector<Lattice*>::iterator it = w->second.begin(); it!=w->second.end(); it++)
	{	
		cout << (*it)->str("    ") << "\n";
	}*/
	
	// Records that this analysis has initialized its state at this node
	initialized((Analysis*)analysis);
}

// Sets this node's below lattices for this analysis to the given vector of lattices, 
// deleting any previous mapping (the previous Lattices are freed)
void NodeState::setLatticeBelow(const Analysis* analysis, vector<Lattice*>& lattices)
{
	// if the analysis currently has a mapping in dfInfoBelow
#ifdef THREADED
	LatticeMap::accessor w;
	if(dfInfoBelow.find(w, (Analysis*)analysis))
#else
	LatticeMap::iterator w;
	if((w = dfInfoBelow.find((Analysis*)analysis)) != dfInfoBelow.end())
#endif
	{
		// Empty out the current mapping of analysis in dfInfoBelow
		for(vector<Lattice*>::iterator it = w->second.begin(); 
		    it != w->second.end(); it++)
		{ delete *it; }
		w->second.clear();
		
		// Create the new mapping
		w->second = lattices;
	}
	else
	{
#ifdef THREADED		
		// Create the new mapping
		w->second = lattices;
#else
		// Create the new mapping
		dfInfoBelow[(Analysis*)analysis] = lattices;
#endif
	}
	
	/*printf("Lattices below: state=%p, analysis=%p\n", this, analysis);
	for(vector<Lattice*>::iterator it = w->second.begin(); 
	    w->second.end(); it++)
	{	
		cout << (*it)->str("    ") << "\n";
	}*/
	
	// Records that this analysis has initialized its state at this node
	initialized((Analysis*)analysis);
}

static vector<Lattice*> emptyLatVec;

// returns the given lattice from above the node, which owned by the given analysis
Lattice* NodeState::getLatticeAbove(const Analysis* analysis, int latticeName) const
{
	getLattice_ex(dfInfoAbove, analysis, latticeName);
}

// returns the map containing all the lattices from above the node that are owned by the given analysis
// (read-only access)
const vector<Lattice*>& NodeState::getLatticeAbove(const Analysis* analysis) const
{
	#ifdef THREADED
		LatticeMap::const_accessor r;
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoAbove.find(r, (Analysis*)analysis))
			return r->second;
	#else
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoAbove.find((Analysis*)analysis)!=dfInfoAbove.end())
			return dfInfoAbove.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty vector
			return emptyLatVec;
}

// returns the map containing all the lattices from above the node that are owned by the given analysis
// (read/write access)
vector<Lattice*>& NodeState::getLatticeAboveMod(const Analysis* analysis)
{
	#ifdef THREADED
		LatticeMap::accessor r;
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoAbove.find(r, (Analysis*)analysis))
			return r->second;
	#else
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoAbove.find((Analysis*)analysis)!=dfInfoAbove.end())
			return dfInfoAbove.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty vector
			return emptyLatVec;
}

// returns the given lattice from below the node, which owned by the given analysis
Lattice* NodeState::getLatticeBelow(const Analysis* analysis, int latticeName) const
{
	getLattice_ex(dfInfoBelow, analysis, latticeName);
}

// returns the map containing all the lattices from below the node that are owned by the given analysis
// (read-only access)
const vector<Lattice*>& NodeState::getLatticeBelow(const Analysis* analysis) const
{	
	#ifdef THREADED
		LatticeMap::const_accessor r;
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoBelow.find(r, (Analysis*)analysis))
			return r->second;
	#else
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoBelow.find((Analysis*)analysis)!=dfInfoBelow.end())
			return dfInfoBelow.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty vector
			return emptyLatVec;
}

// returns the map containing all the lattices from below the node that are owned by the given analysis
// (read/write access)
vector<Lattice*>& NodeState::getLatticeBelowMod(const Analysis* analysis)
{
	#ifdef THREADED
		LatticeMap::accessor r;
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoBelow.find(r, (Analysis*)analysis))
			return r->second;
	#else
		// if this analysis has registered some lattices at this node, return their vector
		if(dfInfoBelow.find((Analysis*)analysis)!=dfInfoBelow.end())
			return dfInfoBelow.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty vector
			return emptyLatVec;
}

// deletes all lattices above this node associated with the given analysis
void NodeState::deleteLatticeAbove(const Analysis* analysis)
{
	#ifdef THREADED
		LatticeMap::accessor r;
		dfInfoAbove.find(r, (Analysis*)analysis);
		vector<Lattice*>& l = r->second;
	#else
		vector<Lattice*>& l = dfInfoAbove.find((Analysis*)analysis)->second;
	#endif

	// delete the individual lattices associated with this analysis
	for(vector<Lattice*>::iterator it = l.begin(); it!=l.end(); it++)
		delete *it;

	// delete the analysis' mapping in dfInfoAbove
	dfInfoAbove.erase((Analysis*)analysis);
}

// deletes all lattices below this node associated with the given analysis
void NodeState::deleteLatticeBelow(const Analysis* analysis)
{
	#ifdef THREADED
		LatticeMap::accessor r;
		dfInfoBelow.find(r, (Analysis*)analysis);
		vector<Lattice*>& l = r->second;
	#else
		vector<Lattice*>& l = dfInfoBelow.find((Analysis*)analysis)->second;
	#endif
	
	// delete the individual lattices associated with this analysis
	for(vector<Lattice*>::iterator it = l.begin(); it!=l.end(); it++)
		delete *it;

	// delete the analysis' mapping in dfInfoBelow
	dfInfoBelow.erase((Analysis*)analysis);
}

// returns true if the two lattices vectors are the same and false otherwise
bool NodeState::eqLattices(const vector<Lattice*>& latticesA,
                           const vector<Lattice*>& latticesB)
{
//	printf("    latticesA.size()=%d latticesB.size()=%d\n", latticesA.size(), latticesB.size());
	if(latticesA.size() != latticesB.size())
		return false;
	
	vector<Lattice*>::const_iterator itA, itB;
	for(itA = latticesA.begin(), itB = latticesB.begin();
	    itA != latticesA.end(), itB != latticesB.end();
	    itA++, itB++)
	{
		Lattice *lA = *itA;
		Lattice *lB = *itB;
/*		cout << "lA = "<<lA->str("    ")<<"\n";
		cout << "lB = "<<lB->str("    ")<<"\n";*/
		if(*itA != *itB) return false;
	}
	
	return true;
}

// Creates a copy of all the dataflow state (Lattices and Facts) associated with
// analysis srcA and associates this copied state with analysis tgtA.
void NodeState::cloneAnalysisState(const Analysis* srcA, const Analysis* tgtA)
{
	// Copy srcA's facts
	//const map <int, NodeFact*>& srcFacts = getFacts(srcA);
	const vector<NodeFact*>& srcFacts = getFacts(srcA);
	//printf("srcFacts.size()=%d\n", srcFacts.size());
	
	vector<NodeFact*> tgtFacts;
	for(vector<NodeFact*>::const_iterator it = srcFacts.begin();
	    it != srcFacts.end(); it++)
	{
		//printf("NodeState::cloneAnalysisState it->first=%d, it->second=%p\n", it->first, it->second);
		//tgtFacts[it->first] = it->second->copy();
		tgtFacts.push_back((*it)->copy());
	}
	//printf("tgtFacts.size()=%d\n", tgtFacts.size());
	// Associate analysis tgtA with the copied facts
	setFacts(tgtA, tgtFacts);
	
	// Copy srcA's lattices into tgtLatAbv and tgtLatBel
	const vector<Lattice*>& srcLatAbv = getLatticeAbove(srcA);
	const vector<Lattice*>& srcLatBel = getLatticeBelow(srcA);
	vector<Lattice*> tgtLatAbv, tgtLatBel;
	//printf("srcLatAbv.size()=%d  srcLatBel.size()=%d\n", srcLatAbv.size(), srcLatBel.size());
	for(vector<Lattice*>::const_iterator it = srcLatAbv.begin();
	    it != srcLatAbv.end(); it++)
	{ 
		//cout << "srcLatAbv: "<<(*it)->str("")<<"\n";
		tgtLatAbv.push_back((*it)->copy()); }
	for(vector<Lattice*>::const_iterator it = srcLatBel.begin();
	    it != srcLatBel.end(); it++)
	{ 
		//cout << "tgtLatBel: "<<(*it)->str("")<<"\n";
		tgtLatBel.push_back((*it)->copy()); }
	//printf("tgtLatAbv.size()=%d  tgtLatBel.size()=%d\n", tgtLatAbv.size(), tgtLatBel.size());
	
	// Associated analysis tgtA with the copied lattices
	setLatticeAbove(tgtA, tgtLatAbv);
	setLatticeBelow(tgtA, tgtLatBel);
}

// Given a set of analyses, one of which is designated as a master, unions together the 
// lattices associated with each of these analyses. The results are associated on each 
// CFG node with the master analysis.
void NodeState::unionLattices(set<Analysis*>& unionSet, const Analysis* master)
{
	vector<Lattice*>& masterLatAbv = getLatticeAboveMod(master);
	vector<Lattice*>& masterLatBel = getLatticeBelowMod(master);

	//printf("    unionLattices() unionSet.size()=%d, master=%p, this=%p\n", unionSet.size(), master, this);
	//printf("        masterLatAbv.size()=%d, masterLatBel.size()=%d\n", masterLatAbv.size(), masterLatBel.size());
	for(set<Analysis*>::iterator it = unionSet.begin(); it!=unionSet.end(); it++)
	{
		Analysis* curA = *it;
		//printf("        curA=%p\n", curA);
		if(curA != master)
		{
			const vector<Lattice*>& curLatAbv = getLatticeAbove(curA);
			const vector<Lattice*>& curLatBel = getLatticeBelow(curA);
			
			//printf("        curLatAbv.size()=%d, curLatBel.size()=%d\n", masterLatAbv.size(), masterLatBel.size());
			
			// All the analyses in unionSet must have the same number of lattices
			ROSE_ASSERT(masterLatAbv.size() == curLatAbv.size());
			ROSE_ASSERT(masterLatBel.size() == curLatBel.size());
			
			// Union the Above lattices 
			vector<Lattice*>::const_iterator curIt  = curLatAbv.begin();
			vector<Lattice*>::iterator       mstrIt = masterLatAbv.begin();
			for(; (curIt != curLatAbv.end()) && (mstrIt != masterLatAbv.end()); 
			    curIt++, mstrIt++)
			{ 
				//cout <<"        master lattice = "<<(*mstrIt)->str("")<<"\n";
				//cout <<"        other lattice = "<<(*curIt)->str("")<<"\n";
				(*mstrIt)->meetUpdate(*curIt);
			}
			
			// Union the Below lattices
			curIt  = curLatBel.begin();
			mstrIt = masterLatBel.begin();
			for(; (curIt != curLatBel.end()) && (mstrIt != masterLatBel.end()); 
			    curIt++, mstrIt++)
			{ (*mstrIt)->meetUpdate(*curIt); }
		}
	}
}

/*void NodeState::removeLattice(const Analysis* analysis, int latticeName)
{
	removeLattice_ex(dfInfoAbove, analysis, latticeName);
	removeLattice_ex(dfInfoBelow, analysis, latticeName);
}

// adds the given lattice to the given dfInfo structure (dfInfoAbove or dfInfoBelow), 
// organizing it under the given analysis and lattice name
void NodeState::addLattice_ex(map <Analysis*, vector<Lattice*> >& dfMap, 
                              const Analysis* analysis, int latticeName, Lattice* l)
{
	map <Analysis*, vector<Lattice*> >::iterator dfLattices;
	// if this analysis has registered some Lattices at this node
	if((dfLattices = dfMap.find((Analysis*)analysis)) != dfMap.end())
	{
		//printf("NodeState::addLattice_ex() found lattice %s\n", dfLattices->second.at(latticeName).str(""));
		
		// delete the old lattice (if any) and set it to the new lattice
		if(dfLattices->second.at(latticeName) != dfLattices->second.end())
		{
			//printf("NodeState::addLattice_ex() deleting old lattice\n");
			delete dfLattices->second.at(latticeName);
		}
		dfLattices->second.at(latticeName) = l;
	}
	else
	{
		//printf("NodeState::addLattice_ex() didn't find lattice\n");
		vector<Lattice*> newMap;
		newMap.at(latticeName) = l;
		dfMap[(Analysis*)analysis] = newMap;
	}
	//printf("NodeState::addLattice_ex() dfMap.size()=%d\n", dfMap.size());
}*/

// returns the given lattice, which owned by the given analysis
Lattice* NodeState::getLattice_ex(const LatticeMap& dfMap, 
                                  const Analysis* analysis, int latticeName) const
{
	#ifdef THREADED
		LatticeMap::const_accessor dfLattices;
		// if this analysis has registered some Lattices at this node
		if(dfMap.find(dfLattices, (Analysis*)analysis))
		{
			//printf("dfLattices->second.size()=%d\n", dfLattices->second.size());
			if(dfLattices->second.size()>latticeName)
				return dfLattices->second.at(latticeName);
			else
				return NULL;
		}
	#else
		//printf("getLattice_ex() analysis=%p, dfMap.size()=%d\n", analysis, dfMap.size());
		map <Analysis*, vector<Lattice*> >::const_iterator dfLattices;
		// if this analysis has registered some Lattices at this node
		if((dfLattices = dfMap.find((Analysis*)analysis)) != dfMap.end())
		{
			//printf("dfLattices->first=%p, dfLattices->second.size()=%d\n", dfLattices->first, dfLattices->second.size());
			if(dfLattices->second.size()>latticeName)
				return dfLattices->second.at(latticeName);
			else
				return NULL;
		}
	#endif
	return NULL;
}

/*// removes the given lattice, owned by the given analysis
// returns true if the given lattice was found and removed and false if it was not found
bool NodeState::removeLattice_ex(map <Analysis*, vector<Lattice*> >& dfMap, 
                                 const Analysis* analysis, int latticeName)
{
	map <Analysis*, vector<Lattice*> >::iterator dfLattices;
	// if this analysis has registered some Lattices at this node
	if((dfLattices = dfMap.find((Analysis*)analysis)) != dfMap.end())
	{
		vector<Lattice*>::iterator it;
		if((it = dfLattices->second.find(latticeName)) != dfLattices->second.end())
		{
			delete *it;
			dfLattices->second.erase(it);
			return true;
		}
	}
	return false;
}*/
// associates the given analysis/fact name with the given NodeFact, 
// deleting any previous association (the previous NodeFact is freed)
void NodeState::addFact(const Analysis* analysis, int factName, NodeFact* f)
{
	#ifdef THREADED
		NodeFactMap::accessor factsIt;
		// if this analysis has registered some facts at this node
		if(facts.find(factsIt, (Analysis*)analysis))
	#else
		NodeFactMap::iterator factsIt;
		// if this analysis has registered some facts at this node
		if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
	#endif
	{
		// delete the old fact (if any) and set it to the new fact
		//if(factsIt->second.find(factName) != factsIt->second.end())
		if(factName < factsIt->second.size())
		{
			delete factsIt->second[factName];
			factsIt->second[factName] = f;
		}
		else
		{
			for(int i=factsIt->second.size(); i<(factName-1); i++)
				factsIt->second.push_back(NULL);
			factsIt->second.push_back(f);
		}
	}
	else
	{
		vector<NodeFact*> newVec;
		for(int i=0; i<(factName-1); i++)
			newVec.push_back(NULL);
		newVec.push_back(f);
		#ifdef THREADED
			NodeFactMap::accessor w;
			facts.insert(w, (Analysis*)analysis);
			w->second = newVec;
		#else
			facts[(Analysis*)analysis] = newVec;
		#endif
		
		
	}
}

// associates the given analysis with the given map of fact names to NodeFacts
// deleting any previous association (the previous NodeFact is freed)
void NodeState::setFacts(const Analysis* analysis, const vector<NodeFact*>& newFacts)
{
	#ifdef THREADED
		NodeFactMap::accessor factsIt;
		// if this analysis has registered some facts at this node
		if(facts.find(factsIt, (Analysis*)analysis))
	#else
		NodeFactMap::iterator factsIt;
		// if this analysis has registered some facts at this node
		if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
	#endif
	{
		// delete the old facts (if any) and associate the analysis with the new set of facts
		for(vector<NodeFact*>::iterator it = factsIt->second.begin();
		    it != factsIt->second.end(); it++)
		//{ delete it->second; }
		{ delete *it; }
		factsIt->second.clear();
		factsIt->second = newFacts;
	}
	else
	{
		// Associate newFacts with the analysis
		#ifdef THREADED
			NodeFactMap::accessor w;
			facts.insert(w, (Analysis*)analysis);
			w->second = newFacts;
		#else
			facts[(Analysis*)analysis] = newFacts;
		#endif
	}
	
	// Records that this analysis has initialized its state at this node
	initialized((Analysis*)analysis);
}

// returns the given fact, which owned by the given analysis
NodeFact* NodeState::getFact(const Analysis* analysis, int factName) const
{
	#ifdef THREADED
		NodeFactMap::const_accessor factsIt;
		// if this analysis has registered some facts at this node
		if(facts.find(factsIt, (Analysis*)analysis))
	#else
		NodeFactMap::const_iterator factsIt;
		// if this analysis has registered some Lattices at this node
		if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
	#endif
	{
		vector<NodeFact*>::const_iterator it;
		//printf("NodeState::getFact() factName=%d factsIt->second.size()=%d\n", factName, factsIt->second.size());
		//if((it = factsIt->second.find(factName)) != factsIt->second.end())
		if(factName < factsIt->second.size())
		{
			//return it->second;
			//return *it;
			return (factsIt->second)[factName];
		}
	}
	return NULL;
}

static vector<NodeFact*> emptyFactsMap;
// returns the map of all the facts owned by the given analysis at this NodeState
// (read-only access)
const vector<NodeFact*>& NodeState::getFacts(const Analysis* analysis) const
{
	#ifdef THREADED
		NodeFactMap::const_accessor factsIt;
		// if this analysis has registered some facts at this node, return their map
		if(facts.find(factsIt, (Analysis*)analysis))
			return factsIt->second;
	#else
		//printf("NodeState::getFacts facts.find(%p)==facts.end()=%d\n", analysis, facts.find((Analysis*)analysis)==facts.end());
		// if this analysis has registered some facts at this node, return their map
		if(facts.find((Analysis*)analysis)!=facts.end())
			return facts.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty map
			return emptyFactsMap;
}

// returns the map of all the facts owned by the given analysis at this NodeState
// (read/write access)
vector<NodeFact*>& NodeState::getFactsMod(const Analysis* analysis)
{
	#ifdef THREADED
		NodeFactMap::accessor factsIt;
		// if this analysis has registered some facts at this node, return their map
		if(facts.find(factsIt, (Analysis*)analysis))
			return factsIt->second;
	#else
		// if this analysis has registered some facts at this node, return their map
		if(facts.find((Analysis*)analysis)!=facts.end())
			return facts.find((Analysis*)analysis)->second;
	#endif
		else
			// otherwise, return an empty map
			return emptyFactsMap;
}

// removes the given fact, owned by the given analysis
// returns true if the given fact was found and removed and false if it was not found
/*bool NodeState::removeFact(const Analysis* analysis, int factName)
{
	#ifdef THREADED
		NodeFactMap::accessor factsIt;
		// if this analysis has registered some facts at this node
		if(facts.find(factsIt, (Analysis*)analysis))
	#else
		NodeFactMap::iterator factsIt;
		// if this analysis has registered some Lattices at this node
		if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
	#endif
	{
		vector<NodeFact*>::iterator it;
		//if((it = factsIt->second.find(factName)) != factsIt->second.end())
		if(factName < factsIt->second.size())
		{
			//delete it->second;
			delete *it;
			factsIt->second.erase(it);
			return true;
		}
	}
	return false;
}*/

// deletes all facts at this node associated with the given analysis
void NodeState::deleteFacts(const Analysis* analysis)
{
	vector<NodeFact*>& f = getFactsMod(analysis);
	
	// delete the individual facts associated with this analysis
	for(vector<NodeFact*>::iterator it = f.begin(); it!=f.end(); it++)
		//delete it->second;
		delete *it;

	// delete the analysis' mapping in facts
	facts.erase((Analysis*)analysis);
}

// delete all state at this node associated with the given analysis
void NodeState::deleteState(const Analysis* analysis)
{
	deleteLatticeAbove(analysis);
	deleteLatticeBelow(analysis);
	deleteFacts(analysis);
}

// ====== STATIC ======
map<DataflowNode, vector<NodeState*> > NodeState::nodeStateMap;
bool NodeState::nodeStateMapInit = false;

// returns the NodeState object associated with the given dataflow node.
// index is used when multiple NodeState objects are associated with a given node
// (ex: SgFunctionCallExp has 3 NodeStates: entry, function body, exit)
NodeState* NodeState::getNodeState(const DataflowNode& n, int index)
{
	// if we haven't assigned a NodeState for every dataflow node
	if(!nodeStateMapInit)
		initNodeStateMap();
	
	return nodeStateMap[n][index];
}

// returns a vector of NodeState objects associated with the given dataflow node.
const vector<NodeState*> NodeState::getNodeStates(const DataflowNode& n)
{
	// if we haven't assigned a NodeState for every dataflow node
	if(!nodeStateMapInit)
		initNodeStateMap();
	
	return nodeStateMap[n];
}

// returns the number of NodeStates associated with the given DataflowNode
int NodeState::numNodeStates(DataflowNode& n)
{
	// if we haven't assigned a NodeState for every dataflow node
	if(!nodeStateMapInit)
		initNodeStateMap();
	
	return nodeStateMap[n].size();
}

// initializes the nodeStateMap
void NodeState::initNodeStateMap()
{
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	
	// iterate over all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)
	{
		const Function& func = (*it)->func;
		DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
		DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());
		
		// iterate over all the dataflow nodes in this function
		for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::dataflow::end(); it++)
		{
			DataflowNode n = *it;
			SgNode* sgn = n.getNode();
			//printf("NodeState::initNodeStateMap() sgn=<%s | %s>\n", sgn->unparseToString().c_str(), sgn->class_name().c_str());
	
			// the number of NodeStates associated with the given dataflow node
			int numStates=1;
			
			/*// if this is a function call, it has 3 states: one for the call, one for the body and one for the return
			if(isSgFunctionCallExp(n.getNode()))
				numStates=3;*/
			
			for(int i=0; i<numStates; i++)
			{
				//nodeStateMap[n][i] = new NodeState(/*n*/);
				//nodeStateMap[n].insert(make_pair(i, new NodeState(n)));
				nodeStateMap[n].push_back(new NodeState(/*n*/));
				//printf("                              nodeStateMap[n][i]=%p\n", nodeStateMap[n][i]);
			}
			//printf("                              nodeStateMap[n].size()=%d\n", nodeStateMap[n].size());
		}
	}
	
	nodeStateMapInit = true;
}

/*// copies the facts from that to this
void NodeState::copyFacts(NodeState &that)
{
	// empty facts
	for(NodeFactMap::iterator it1 = facts.begin(); it1!=facts.end(); it1++)
	{
		for(map <int, NodeFact*>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
		{
			delete it2->second;
		}
		it->second.clear();
	}
	facts.clear();
	
	// copy facts
	for(NodeFactMap::it1 = that.facts.begin(); it1!=that.facts.end(); it++)
	{
		map <int, NodeFact*> newMap;
		for(map <int, NodeFact*>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
			newMap[it2->first] = it2->second;
		facts[(Analysis*)analysis] = newMap;
	}
}

// copies the dfInfoAbove lattices from that to this
void NodeState::copyLatticesAbove(NodeState &that)
{
	copyLattices(dfInfoAbove, that.dfInfoAbove);
}

// copies the dfInfoBelow lattices from that to this
void NodeState::copyLatticesBelow(NodeState &that)
{
	copyLattices(dfInfoBelow, that.dfInfoBelow);
}

// copies the dfInfoAbove lattices from the given map to this
void NodeState::copyLatticesAbove(const map <Analysis*, vector<Lattice*> >& thatInfo)
{
	copyLattices(dfInfoBelow, thatInfo);
}

// copies the dfInfoBelow lattices from the given map to this
void NodeState::copyLatticesBelow(const map <Analysis*, vector<Lattice*> >& thatInfo)
{
	copyLattices(dfInfoAbove, thatInfo);
}

// copies the dfInfoAbove or dfInfoBelow lattices from that to this
void NodeState::copyLattices(const map <Analysis*, vector<Lattice*> >& dfInfo, 
                             const map <Analysis*, vector<Lattice*> >& thatInfo)
{
	// empty lattices
	map <Analysis*, vector<Lattice*> >::iterator it, itThat;
	for(it = dfInfo.begin(); it!=dfInfo.end(); it++)
	{
		vector<Lattice*>::iterator itL;
		for(itL = it->second.begin(); itL != it->second.end(); itL++)
			delete *itL;
		it->second.clear();
	}
	dfInfo.clear();
	
	// copy lattices
	for(it = dfInfo.begin(), itThat = thatInfo.begin(); 
	    it!=dfInfo.end() && itThat!=thatInfo.end(); 
	    it++, itThat++)
	{
		vector<Lattice*> newInfo;
		vector<Lattice*>::iterator itL;
		for(itL = itThat->second.begin(); itL != itThat->second.end(); itL)
			newInfo.push_back((*itL)->copy());
		dfInfo[it->first] = newInfo;
	}
}*/

// copies from's above lattices for the given analysis to to's above lattices for the same analysis
void NodeState::copyLattices_aEQa(Analysis* analysis, NodeState& to, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoAbove.find(wTo, analysis);
	LatticeMap::const_accessor rFrom; from.dfInfoAbove.find(rFrom, analysis);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoAbove.find(analysis)->second, from.dfInfoAbove.find(analysis)->second);
	#endif
}

// copies from's above lattices for analysisA to to's above lattices for analysisB
void NodeState::copyLattices_aEQa(Analysis* analysisA, NodeState& to, Analysis* analysisB, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoAbove.find(wTo, analysisA);
	LatticeMap::const_accessor rFrom; from.dfInfoAbove.find(rFrom, analysisB);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoAbove.find(analysisA)->second, from.dfInfoAbove.find(analysisB)->second);
	#endif
}

// copies from's above lattices for the given analysis to to's below lattices for the same analysis
void NodeState::copyLattices_bEQa(Analysis* analysis, NodeState& to, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoBelow.find(wTo, analysis);
	LatticeMap::const_accessor rFrom; from.dfInfoAbove.find(rFrom, analysis);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoBelow.find(analysis)->second, from.dfInfoAbove.find(analysis)->second);
	#endif
}

// copies from's above lattices for analysisA to to's below lattices for analysisB
void NodeState::copyLattices_bEQa(Analysis* analysisA, NodeState& to, Analysis* analysisB, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoBelow.find(wTo, analysisA);
	LatticeMap::const_accessor rFrom; from.dfInfoAbove.find(rFrom, analysisB);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoBelow.find(analysisA)->second, from.dfInfoAbove.find(analysisB)->second);
	#endif
}

// copies from's below lattices for the given analysis to to's below lattices for the same analysis
void NodeState::copyLattices_bEQb(Analysis* analysis, NodeState& to, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoBelow.find(wTo, analysis);
	LatticeMap::const_accessor rFrom; from.dfInfoBelow.find(rFrom, analysis);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoBelow.find(analysis)->second, from.dfInfoBelow.find(analysis)->second);
	#endif
}

// copies from's below lattices for the given analysis to to's above lattices for the same analysis
void NodeState::copyLattices_aEQb(Analysis* analysis, NodeState& to, const NodeState& from)
{
	#ifdef THREADED
	LatticeMap::accessor       wTo;   to.dfInfoAbove.find(wTo, analysis);
	LatticeMap::const_accessor rFrom; from.dfInfoBelow.find(rFrom, analysis);
	copyLattices(wTo->second, rFrom->second);
	#else
	copyLattices(to.dfInfoAbove.find(analysis)->second, from.dfInfoBelow.find(analysis)->second);
	#endif
}

// makes dfInfoX a copy of dfInfoY
void NodeState::copyLattices(vector<Lattice*>& dfInfoX, const vector<Lattice*>& dfInfoY)
{
/*	// empty lattices
	for(vector<Lattice*>::iterator itX = dfInfoX.begin(); itX!=dfInfoX.end(); itX++)
	{
		printf("NodeState::copyLattices() deleting %p\n", *itX);
		cout << "    *itX = " << (*itX)->str("") << "\n";
		delete *itX;
	}
	dfInfoX.clear();*/
	
	// copy lattices
	vector<Lattice*>::const_iterator itX, itY;
	for(itX = dfInfoX.begin(), itY = dfInfoY.begin(); 
	   itX!=dfInfoX.end() && itY!=dfInfoY.end(); 
	   itX++, itY++)
		(*itX)->copy(*itY);
		//dfInfoX.push_back((*itY)->copy());
}

/*void NodeState::operator=(NodeState& that)
{
	parentNode = that.parentNode;
	
	copyFacts(that);
	copyLatticesAbove(&that);
	copyLatticesBelow(&that);
}*/
