#ifndef VIRTUAL_CFG_ITERATOR
#define VIRTUAL_CFG_ITERATOR

#include "virtualCFG.h"
#include "DataflowCFG.h"
//#include "baseCFGIterator.h"

#include <list>
#include <set>

namespace VirtualCFG{

// Iterates over DataflowNodes in a VirtualCFG, respecting dependences in the graph.
// Supports both forward and backward iteration.
class iterator/* : public virtual BaseCFG::iterator*/
{
	//protected:
	public:
		
        std::list<DataflowNode> remainingNodes;
	//map<DataflowNode, bool> visited;
        std::set<DataflowNode> visited;
	bool initialized;

	public:
	iterator();
	
	iterator(const DataflowNode &start);
        virtual ~iterator() { }
	
	void init(const DataflowNode &start);

	protected:
	// returns true if the given DataflowNode is in the remainingNodes list and false otherwise
	bool isRemaining(DataflowNode n);
		
	// advances this iterator in the given direction. Forwards if fwDir=true and backwards if fwDir=false.
	// if pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
	//    depending on fwDir) are pushed onto remainingNodes
	void advance(bool fwDir, bool pushAllChildren);
	
	public:
	virtual void operator ++ (int);
	
	bool eq(const iterator& other_it) const;
	
	bool operator==(const iterator& other_it) const;
	
	bool operator!=(const iterator& it) const;
		
	DataflowNode& operator * ();
	
	// Returns a fresh iterator that starts at node n
	static iterator begin(DataflowNode n);
	
	// Returns an empty iterator that can be compared to any other iterator to 
	// check if it has completed passing over its iteration space
	static iterator end();
	
	// Contains the state of an iterator, allowing iterators to be 
	// checkpointed and restarted.
	class checkpoint/* : public virtual BaseCFG::iterator::checkpoint*/
	{
          std::list<DataflowNode> remainingNodes;
          std::set<DataflowNode> visited;
	
		public:
          checkpoint(const std::list<DataflowNode>& remainingNodes, const std::set<DataflowNode>& visited);
		
		checkpoint(const checkpoint& that);
		
		std::string str(std::string indent="");
		
		friend class iterator;
	};
	
	// Returns a checkpoint of this iterator's progress.
	checkpoint getChkpt();
	
	// Loads this iterator's state from the given checkpoint.
	void restartFromChkpt(checkpoint& chkpt);
	
	std::string str(std::string indent="");
};

class back_iterator : /*public virtual BaseCFG::backiterator, */public virtual iterator
{
	public:
	back_iterator(): iterator() {}
	
	back_iterator(const DataflowNode &end): iterator(end) { }
	
	void operator ++ (int);
};

class dataflow : /*public virtual BaseCFG::dataflow, */public virtual iterator
{
	DataflowNode terminator;
	public:
	//dataflow(): iterator() {}
	
	dataflow(const DataflowNode &terminator_arg);
	
	//dataflow(const DataflowNode &start) : iterator(start) {}
	dataflow(const DataflowNode &start, const DataflowNode &terminator_arg);
	
	void init(const DataflowNode &start_arg, const DataflowNode &terminator_arg);
	
	// Initializes this iterator's terminator node
	/*void setTerminator(const DataflowNode &terminator) {
		initialized = true;
		this->terminator = terminator;
	}*/
	
	void add(const DataflowNode &next);
	
	//void operator ++ (int);
	
	// Contains the state of an dataflow iterator, allowing dataflow 
	// iterators to be checkpointed and restarted.
	class checkpoint/* : public virtual BaseCFG::dataflow::checkpoint*/
	{
		iterator::checkpoint iChkpt;
		DataflowNode terminator;
		
		public:
		checkpoint(const iterator::checkpoint& iChkpt, const DataflowNode& terminator);
		
		checkpoint(const checkpoint &that);
			
		std::string str(std::string indent="");
		
		friend class dataflow;
	};
	
	// Returns a checkpoint of this dataflow iterator's progress.
	checkpoint getChkpt();
	
	// Loads this dataflow iterator's state from the given checkpoint.
	void restartFromChkpt(checkpoint& chkpt);
	
	std::string str(std::string indent="");
};

class back_dataflow: /*public virtual BaseCFG::back_dataflow,*/ public virtual dataflow
{
	public:	
	//back_dataflow(): back_iterator() {}
	
	back_dataflow(const DataflowNode &terminator_arg) : dataflow(terminator_arg) {}
	
	//back_dataflow(const DataflowNode &end) : iterator(end) {}
	
	back_dataflow(const DataflowNode &end, const DataflowNode &terminator_arg);
		
	void operator ++ (int);
};
}
#endif
