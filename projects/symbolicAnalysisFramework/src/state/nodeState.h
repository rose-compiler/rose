#ifndef NODE_STATE_H
#define NODE_STATE_H

#include "DataflowCFG.h"
class NodeFact;
class NodeState;

#include "lattice.h"
#include "analysis.h"
#include <map>

#ifdef THREADED
#include "tbb/concurrent_hash_map.h"
#include "tbb/atomic.h"
#endif


using namespace std;

//template<class factType>
/************************************************
 ***               NodeFact                   ***
 *** A fact associated with a CFG node by     ***
 *** some analysis thatis not evolved as part ***
 *** of a dataflow analysis (i.e. it should   ***
 *** stay constant throughout the analysis).  ***
 ************************************************/
// A fact associated with a CFG node that is not part of a dataflow analysis. In other words, 
// it is not a lattice and is not meant to evolve during the course of a dataflow analysis.
class NodeFact: public printable
{
	public:
	
	// The string that represents this object.
	// Every line of this string must be prefixed by indent.
	// The last character of the returned string must not be '\n', even if it is a multi-line string.
	//virtual string str(string indent="")=0;
	
		// returns a copy of this node fact
	virtual NodeFact* copy() const=0;
	
/*	void* fact;
	
	public:
	NodeFact(void* fact)
	{
		this->fact = fact;
	}
	
	/*NodeFact(factType* fact)
	{
		this->fact = *fact;
	}* /
	
	void* getFact()
	{
		return fact;
	}*/
};


/**********************************************
 ***               NodeState                ***
 *** The state of all the Lattice elements  ***
 *** associated by dataflow analyses with a ***
 *** given node. This state will evolve as  ***
 *** a result of the dataflow analysis.     ***
 **********************************************/
#ifdef THREADED
class NodeStateHashCompare
{ 
	public:
	NodeStateHashCompare() {}
	NodeStateHashCompare(const NodeStateHashCompare & that) {}
	
	~NodeStateHashCompare(){}
	
	static bool equal(const Analysis* & j, const Analysis* & k )
	{ return j==k; }
	
	static bool equal(const Analysis* const & j, const Analysis* const & k )
	{ return j==k; }
	
	static size_t hash( const Analysis* k ) { return (size_t) k; }
};
#endif

class NodeState
{
	#ifdef THREADED
	typedef tbb::concurrent_hash_map <Analysis*, vector<Lattice*>, NodeStateHashCompare > LatticeMap;
	//typedef tbb::concurrent_hash_map <Analysis*, map <int, NodeFact*>, NodeStateHashCompare > NodeFactMap;
	typedef tbb::concurrent_hash_map <Analysis*, vector<NodeFact*>, NodeStateHashCompare > NodeFactMap;
	typedef tbb::concurrent_hash_map <Analysis*, bool, NodeStateHashCompare  > BoolMap;	
	#else
	typedef map <Analysis*, vector<Lattice*> > LatticeMap;
	//typedef map <Analysis*, map <int, NodeFact*> > NodeFactMap;
	typedef map <Analysis*, vector<NodeFact*> > NodeFactMap;
	typedef map <Analysis*, bool > BoolMap;
	#endif
	
	// the dataflow information Above the node, for each analysis that 
	// may be interested in the current node
	LatticeMap dfInfoAbove;
	
	// the Analysis information Below the node, for each analysis that 
	// may be interested in the current node
	LatticeMap dfInfoBelow;

	// the facts that are true at this node, for each analysis that 
	// may be interested in the current node
	NodeFactMap facts;
	
	// Contains all the Analyses that have initialized their state at this node. It is a map because
	// TBB doesn't provide a concurrent set.
	BoolMap initializedAnalyses;
	
	// the dataflow node that this NodeState object corresponds to
	//DataflowNode parentNode;
	
	public:
	/*NodeState(DataflowNode& parentNode) : parentNode(parentNode)
	{}
	
	NodeState(CFGNode& parentNode) : parentNode(parentNode)
	{}
	
	NodeState(CFGNode parentNode) : parentNode(parentNode)
	{}*/
	
	NodeState()
	{}
	
/*	void initialize(Analysis* analysis, int latticeName)
	{
		initDfMap(dfInfoAbove);
		initDfMap(dfInfoBelow);
	}
	
	private:
	// initializes the given lattice owned by the given analysis in the given map
	// dfMap may be either dfInfoAbove or dfInfoBelow
	void initDfMap(map <Analysis*, vector<Lattice*> >& dfMap)
	{
		map <Analysis*, vector<Lattice*> >::iterator dfLattices;
		// if this analysis has registered some Lattices at this node
		if((dfLattices = dfMap.find(analysis)) != dfInfoAbove.end())
		{
			map <int, Lattice>::iterator it;
			// if the given lattice name was registered by this analysis
			if((it = (*dfLattices).find(latticeName) != (*dfLattices).end())
			{
				(*it)->initialize();
			}
			else
			{
				(*dfLattices)[latticeName] = new Lattice();
			}
		}
		else
		{
			map <int, Lattice> newMap;
			Lattice newLattice;
			newMap[latticeName] = newLattice;
			dfMap[analysis] = newMap;
		}
	}*/
	
	public:
	// Records that this analysis has initializedAnalyses its state at this node
	void initialized(Analysis* analysis);
	
	// Returns true if this analysis has initialized its state at this node and false otherwise
	bool isInitialized(Analysis* analysis);
		
	// adds the given lattice, organizing it under the given analysis and lattice name
	//void addLattice(const Analysis* analysis, int latticeName, Lattice* l);
	
	// sets this node's lattices for this analysis
	void setLattices(const Analysis* analysis, vector<Lattice*>& lattices);
	
	// Sets this node's above lattices for this analysis to the given vector of lattices, 
	// deleting any previous mapping (the previous Lattices are freed)
	void setLatticeAbove(const Analysis* analysis, vector<Lattice*>& lattices);
	
	// Sets this node's below lattices for this analysis to the given vector of lattices, 
	// deleting any previous mapping (the previous Lattices are freed)
	void setLatticeBelow(const Analysis* analysis, vector<Lattice*>& lattices);
	
	// returns the given lattice from above the node that is owned by the given analysis
	Lattice* getLatticeAbove(const Analysis* analysis, int latticeName) const;
	// returns the given lattice from below the node that is owned by the given analysis
	Lattice* getLatticeBelow(const Analysis* analysis, int latticeName) const;
	
	// returns the map containing all the lattices from above the node that are owned by the given analysis
	// (read-only access)
	const vector<Lattice*>& getLatticeAbove(const Analysis* analysis) const;
	// returns the map containing all the lattices from below the node that are owned by the given analysis
	// (read-only access)
	const vector<Lattice*>& getLatticeBelow(const Analysis* analysis) const;

	// returns the map containing all the lattices from above the node that are owned by the given analysis
	// (read/write access)
	vector<Lattice*>& getLatticeAboveMod(const Analysis* analysis);
	// returns the map containing all the lattices from below the node that are owned by the given analysis
	// (read/write access)
	vector<Lattice*>& getLatticeBelowMod(const Analysis* analysis);
	
	// deletes all lattices above this node associated with the given analysis
	void deleteLatticeAbove(const Analysis* analysis);
	
	// deletes all lattices below this node associated with the given analysis
	void deleteLatticeBelow(const Analysis* analysis);
	
	// returns true if the two lattices vectors are the same and false otherwise
	static bool eqLattices(const vector<Lattice*>& latticesA,
	                       const vector<Lattice*>& latticesB);
	
	// Creates a copy of all the dataflow state (Lattices and Facts) associated with
	// analysis srcA and associates this copied state with analysis tgtA.
	void cloneAnalysisState(const Analysis* srcA, const Analysis* tgtA);
	
	// Given a set of analyses, one of which is designated as a master, unions together the 
	// lattices associated with each of these analyses. The results are associated on each 
	// CFG node with the master analysis.
	void unionLattices(set<Analysis*>& unionSet, const Analysis* master);
	
	//void removeLattice(const Analysis* analysis, int latticeName);
	
	private:
	/*// adds the given lattice to the given dfInfo structure (dfInfoAbove or dfInfoBelow), 
	// organizing it under the given analysis and lattice name
	void addLattice_ex(map <Analysis*, vector<Lattice*> >& dfMap, 
	                  const  Analysis* analysis, int latticeName, Lattice* l);
	*/
	// returns the given lattice, which owned by the given analysis
	Lattice* getLattice_ex(const LatticeMap& dfMap, 
                          const Analysis* analysis, int latticeName) const;
	
	/*// removes the given lattice, owned by the given analysis
	// returns true if the given lattice was found and removed and false if it was not found
	bool removeLattice_ex(LatticeMap& dfMap, 
	                      const Analysis* analysis, int latticeName);
	*/
	public:
	// associates the given analysis/fact name with the given NodeFact, 
	// deleting any previous association (the previous NodeFact is freed)
	void addFact(const Analysis* analysis, int factName, NodeFact* f);
	
	// associates the given analysis with the given map of fact names to NodeFacts, 
	// deleting any previous association (the previous NodeFacts are freed)
	//void setFacts(const Analysis* analysis, const map <int, NodeFact*>& newFacts);
	void setFacts(const Analysis* analysis, const vector<NodeFact*>& newFacts);
	
	// returns the given fact, which owned by the given analysis
	NodeFact* getFact(const Analysis* analysis, int factName) const ;
	
	// returns the map of all the facts owned by the given analysis at this NodeState
	// (read-only access)
	//const map <int, NodeFact*>& getFacts(const Analysis* analysis) const;
	const vector<NodeFact*>& getFacts(const Analysis* analysis) const;
	
	// returns the map of all the facts owned by the given analysis at this NodeState
	// (read/write access)
	//map <int, NodeFact*>& getFactsMod(const Analysis* analysis);
	vector<NodeFact*>& getFactsMod(const Analysis* analysis);
	
	// removes the given fact, owned by the given analysis
	// returns true if the given fact was found and removed and false if it was not found
	//bool removeFact(const Analysis* analysis, int factName);
	
	// deletes all facts at this node associated with the given analysis
	void deleteFacts(const Analysis* analysis);
	
	// delete all state at this node associated with the given analysis
	void deleteState(const Analysis* analysis);
	
	// ====== STATIC ======
	private:
	static map<DataflowNode, vector<NodeState*> > nodeStateMap;
	static bool nodeStateMapInit;
	
	public:
	// returns the NodeState object associated with the given dataflow node.
	// index is used when multiple NodeState objects are associated with a given node
	// (ex: SgFunctionCallExp has 3 NodeStates: entry, function body, exit)
	static NodeState* getNodeState(const DataflowNode& n, int index=0);
	
	// returns a vector of NodeState objects associated with the given dataflow node.
	static const vector<NodeState*> getNodeStates(const DataflowNode& n);
	
	// returns the number of NodeStates associated with the given DataflowNode
	static int numNodeStates(DataflowNode& n);
	
	private:
	// initializes the nodeStateMap
	static void initNodeStateMap();
	
	public:
	/*// copies the facts from that to this
	void copyFacts(NodeState &that);
	
	// copies the dfInfoBelow lattices from that to this
	void copyLatticesBelow(NodeState &that);
	
	// copies the dfInfoAbove lattices from the given map to this
	void copyLatticesAbove(const LatticeMap& thatInfo);
	
	// copies the dfInfoBelow lattices from the given map to this
	void copyLatticesBelow(const LatticeMap& thatInfo);
	
	protected:
	// copies the dfInfoAbove or dfInfoBelow lattices from that to this
	void copyLattices(const LatticeMap& dfInfo, 
	                  const LatticeMap& thatInfo);
	*/
	
	// copies from's above lattices for the given analysis to to's above lattices for the same analysis
	static void copyLattices_aEQa(Analysis* analysis, NodeState& to, const NodeState& from);
	
	// copies from's above lattices for analysisA to to's above lattices for analysisB
	static void copyLattices_aEQa(Analysis* analysisA, NodeState& to, Analysis* analysisB, const NodeState& from);
	
	// copies from's above lattices for the given analysis to to's below lattices for the same analysis
	static void copyLattices_bEQa(Analysis* analysis, NodeState& to, const NodeState& from);
	
	// copies from's above lattices for analysisA to to's below lattices for analysisB
	static void copyLattices_bEQa(Analysis* analysisA, NodeState& to, Analysis* analysisB, const NodeState& from);
	
	// copies from's below lattices for the given analysis to to's below lattices for the same analysis
	static void copyLattices_bEQb(Analysis* analysis, NodeState& to, const NodeState& from);
	
	// copies from's below lattices for the given analysis to to's above lattices for the same analysis
	static void copyLattices_aEQb(Analysis* analysis, NodeState& to, const NodeState& from);
	
	protected:
	// makes dfInfoX a copy of dfInfoY
	static void copyLattices(vector<Lattice*>& dfInfoX, const vector<Lattice*>& dfInfoY);
		
	/*public:
	void operator=(NodeState& that);*/
};

#endif
