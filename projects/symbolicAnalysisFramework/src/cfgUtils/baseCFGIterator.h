#ifndef BASE_CFG_ITERATOR
#define BASE_CFG_ITERATOR

#include "virtualCFG.h"
#include "DataflowCFG.h"


namespace BaseCFG{
	
class iterator : public printable
{
	//protected:
	public:
		
	list<DataflowNode> remainingNodes;
	//map<DataflowNode, bool> visited;
	set<DataflowNode> visited;

	public:
	virtual void operator ++ (int)=0;
	
	virtual bool eq(const iterator& other_it) const=0;
	
	virtual bool operator==(const iterator& other_it) const=0;
	
	virtual bool operator!=(const iterator& it) const=0;
		
	virtual DataflowNode& operator * ()=0;
	
	//virtual static iterator begin(DataflowNode n)=0;
	
	//virtual static iterator end()=0;
	
	// Contains the state of an iterator, allowing iterators to be 
	// checkpointed and restarted.
	class checkpoint : public printable
	{
		// string str(string indent="");
	};
	
	// Returns a checkpoint of this iterator's progress.
	virtual checkpoint* getChkpt()=0;
	
	// Loads this iterator's state from the given checkpoint.
	virtual void restartFromChkpt(checkpoint& chkpt)=0;
	
	// string str(string indent="");
};

class back_iterator : public virtual iterator
{
};

class dataflow : public virtual iterator
{
	virtual void add(const DataflowNode &next)=0;
};

class back_dataflow : public back_iterator, public dataflow
{
};
}
#endif
