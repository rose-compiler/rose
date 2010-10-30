#include "ranknpDepIfMeet.h"

/*******************************************************************************************
 *******************************************************************************************
 *******************************************************************************************/

/******************
 * IfMeetLat *
 ******************/
IfMeetLat::IfMeetLat(DataflowNode parentNode): ifLeft(parentNode), ifRight(parentNode), parentNode(parentNode)
{
	initialized = false;
	isIfMeet = false;
}

IfMeetLat::IfMeetLat(const IfMeetLat &that): ifLeft(that.parentNode), ifRight(that.parentNode), parentNode(that.parentNode)
{ 
	this->initialized = that.initialized;
	this->ifHist      = that.ifHist;
	this->isIfMeet    = that.isIfMeet;
}

// initializes this Lattice to its default state
void IfMeetLat::initialize()
{
	initialized = true;
	ifHist.clear();
	isIfMeet = false;
	ifLeft   = parentNode;
	ifRight  = parentNode;
}

// returns a copy of this lattice
Lattice* IfMeetLat::copy() const
{ return new IfMeetLat(*this); }

// overwrites the state of this Lattice with that of that Lattice
void IfMeetLat::copy(Lattice* that)
{
	initialized = dynamic_cast<IfMeetLat*>(that)->initialized;
	ifHist      = dynamic_cast<IfMeetLat*>(that)->ifHist;
	isIfMeet    = dynamic_cast<IfMeetLat*>(that)->isIfMeet;
	ifLeft      = dynamic_cast<IfMeetLat*>(that)->ifLeft;
	ifRight     = dynamic_cast<IfMeetLat*>(that)->ifRight;
	parentNode  = dynamic_cast<IfMeetLat*>(that)->parentNode;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool IfMeetLat::meetUpdate(Lattice* that)
{
	if(!initialized)
	{
		copy(that);
		isIfMeet = false;
		initialized=true;
		//printf("meetUpdate Case 1, result: %s\n", str("").c_str());
		return true;
	}
	else if(!dynamic_cast<IfMeetLat*>(that)->initialized)
	{
		initialized=true;
		//printf("meetUpdate Case 2, result: %s\n", str("").c_str());
		return false;
	}
	else 
	{
		initialized=true;
		//printf("ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<IfMeetLat*>(that)->ifHist.size());
		//ROSE_ASSERT(ifHist.size() == dynamic_cast<IfMeetLat*>(that)->ifHist.size());

		// Only bother once we have information from both incoming branches 
		if(ifHist.size() > 0 && dynamic_cast<IfMeetLat*>(that)->ifHist.size()>0)
		{
			//printf("meetUpdate Case 3, this: %s\n", str("").c_str());
			//printf("                   that: %s\n", dynamic_cast<IfMeetLat*>(that)->str("").c_str());
			
			// If the two lattices came from different sides of an if statement
			if(ifHist[ifHist.size()-1] != dynamic_cast<IfMeetLat*>(that)->ifHist[ifHist.size()-1])
			{
				// Record that this is a meet node for an if statement
				isIfMeet = true;
				// erase the marker for the just-finished if statement from this lattice
				ifHist.pop_back();
				//printf("meetUpdate Case 3a\n");
				return true;
			}
			//printf("meetUpdate Case 3b\n");
			return false;
		}
		// Else, if we have new information with a shorter ifHist list, cut the ifHist
		// list of this lattice down to match it since this information is fresher
		else if(dynamic_cast<IfMeetLat*>(that)->ifHist.size() < ifHist.size())
		{
			while(dynamic_cast<IfMeetLat*>(that)->ifHist.size() < ifHist.size())
				ifHist.erase(ifHist.end());
			//printf("after length cut ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<IfMeetLat*>(that)->ifHist.size());
		}	
		else
		{
			//printf("meetUpdate Case 4, result: %s\n", str("").c_str());
			return false;
		}
	}
}

bool IfMeetLat::operator==(Lattice* that)
{
	return initialized == dynamic_cast<IfMeetLat*>(that)->initialized && 
	       (ifHist == dynamic_cast<IfMeetLat*>(that)->ifHist) &&
	       (isIfMeet == dynamic_cast<IfMeetLat*>(that)->isIfMeet);
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string IfMeetLat::str(string indent)
{
	ostringstream ss;

	ss << indent << "IfMeetLat[initialized="<<(initialized?"true":"false")<<", isIfMeet="<<(isIfMeet?"true":"false")<<"\n";
	ss << indent << "          ifHist = <";
	for(vector<bool>::iterator it = ifHist.begin(); it!=ifHist.end(); )
	{
		ss << (*it?"true":"false");
		it++;
		if(it!=ifHist.end()) ss << ", ";
	}
	ss << ">]";
	return ss.str();
}

bool IfMeetLat::getIsIfMeet()
{
	return isIfMeet;
}

/******************
 * IfMeetDetector *
 ******************/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> IfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void IfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
//		printf("IfMeetDetector::genInitState() A\n");
	//vector<Lattice*> initLattices;
//		printf("IfMeetDetector::genInitState() B\n");
	IfMeetLat* ifml = new IfMeetLat(n);
//		printf("IfMeetDetector::genInitState() C\n");
	initLattices.push_back(ifml);
//		printf("IfMeetDetector::genInitState() D\n");
	
	//return initLattices;
}
		
// the transfer function that is applied to every node
// n - the dataflow node that is being processed
// state - the NodeState object that describes the state of the node, as established by earlier 
//         analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//          as input and overwrites them with the result of the transfer.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.
bool IfMeetDetector::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	/*printf("-----------------------------------\n");
	printf("IfMeetDetector::transfer() function %s() node=<%s | %s>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());*/
	
	bool modified = false;
	IfMeetLat* ifml = dynamic_cast<IfMeetLat*>(dfInfo.front());
	
	// if this node is a descendant of an if statement
	if(n == ifml->ifLeft)
	{
		ifml->ifHist.push_back(0);
		// change ifLeft so that we won't push 0 again in case this transfer function ever gets called again
		ifml->ifLeft = ifml->parentNode;
	}
	if(n == ifml->ifRight)
	{
		ifml->ifHist.push_back(1);
		// change ifRight so that we won't push 1 again in case this transfer function ever gets called again
		ifml->ifRight = ifml->parentNode;
	}
		
	if(isSgIfStmt(n.getNode()) && n.getIndex()==1)
	{
		// iterate over both the descendants
		vector<DataflowEdge> edges = n.outEdges();
		for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
		{
			// Set ifLeft and ifRight to tell each descendant that they're on the left/right branch of this if statement
			if((*ei).condition() == eckTrue)
			{
				ifml->ifLeft = ((*ei).target());
			}
			else if((*ei).condition() == eckFalse)
			{
				ifml->ifRight = ((*ei).target());
			}
		}
	}
	
	//printf("ifml->ifHist.size()=%d\n", ifml->ifHist.size());
	ifml->initialized = true;
	return true;
}

// prints the Lattices set by the given DivAnalysis 
void printIfMeetDetectorStates(IfMeetDetector* ifmd, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(ifmd, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}

static IfMeetDetector* ifmd=NULL;
// Runs the IfMeetDetector analysis pass
void runIfMeetDetector(bool printStates)
{
	if(ifmd==NULL)
	{
		ifmd = new IfMeetDetector();	
		UnstructuredPassInterDataflow upid_ifmd(ifmd);
		upid_ifmd.runAnalysis();
		
		if(printStates)
			printIfMeetDetectorStates(ifmd, ":");
	}
}

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isIfMeetNode(const DataflowNode& n)
{
	NodeState* state = NodeState::getNodeState(n, 0);
	Lattice* ifmdLat = state->getLatticeBelow(ifmd, 0);
	return dynamic_cast<IfMeetLat*>(ifmdLat)->getIsIfMeet();
}


/*******************************************************************************************
 *******************************************************************************************
 *******************************************************************************************/

/********************
 * RankDepIfMeetLat *
 ********************/
RankDepIfMeetLat::RankDepIfMeetLat(DataflowNode parentNode): ifLeft(parentNode), ifRight(parentNode), parentNode(parentNode)
{
	initialized = false;
	isIfMeet = false;
}

RankDepIfMeetLat::RankDepIfMeetLat(const RankDepIfMeetLat &that): ifLeft(that.parentNode), ifRight(that.parentNode), parentNode(that.parentNode)
{ 
	this->initialized = that.initialized;
	this->ifHist      = that.ifHist;
	this->isIfMeet    = that.isIfMeet;
}

// initializes this Lattice to its default state
void RankDepIfMeetLat::initialize()
{
	initialized = true;
	ifHist.clear();
	isIfMeet = false;
	ifLeft   = parentNode;
	ifRight  = parentNode;
}

// returns a copy of this lattice
Lattice* RankDepIfMeetLat::copy() const
{ return new RankDepIfMeetLat(*this); }

// overwrites the state of this Lattice with that of that Lattice
void RankDepIfMeetLat::copy(Lattice* that)
{
	initialized = dynamic_cast<RankDepIfMeetLat*>(that)->initialized;
	ifHist      = dynamic_cast<RankDepIfMeetLat*>(that)->ifHist;
	isIfMeet    = dynamic_cast<RankDepIfMeetLat*>(that)->isIfMeet;
	ifLeft      = dynamic_cast<RankDepIfMeetLat*>(that)->ifLeft;
	ifRight     = dynamic_cast<RankDepIfMeetLat*>(that)->ifRight;
	parentNode  = dynamic_cast<RankDepIfMeetLat*>(that)->parentNode;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool RankDepIfMeetLat::meetUpdate(Lattice* that)
{
	if(!initialized)
	{
		copy(that);
		isIfMeet = false;
		initialized=true;
		//printf("meetUpdate Case 1, result: %s\n", str("").c_str());
		return true;
	}
	else if(!dynamic_cast<RankDepIfMeetLat*>(that)->initialized)
	{
		initialized=true;
		//printf("meetUpdate Case 2, result: %s\n", str("").c_str());
		return false;
	}
	else 
	{
		initialized=true;
		//printf("ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size());
		//ROSE_ASSERT(ifHist.size() == dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size());

		// Only bother once we have information from both incoming branches 
		if(ifHist.size() > 0 && dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size()>0)
		{
			//printf("meetUpdate Case 3, this: %s\n", str("").c_str());
			//printf("                   that: %s\n", dynamic_cast<RankDepIfMeetLat*>(that)->str("").c_str());
			
			// If the two lattices came from different sides of an if statement
			if(ifHist[ifHist.size()-1] != dynamic_cast<RankDepIfMeetLat*>(that)->ifHist[ifHist.size()-1])
			{
				// Record that this is a meet node for an if statement
				isIfMeet = true;
				// erase the marker for the just-finished if statement from this lattice
				ifHist.pop_back();
				//printf("meetUpdate Case 3a\n");
				return true;
			}
			//printf("meetUpdate Case 3b\n");
			return false;
		}
		// Else, if we have new information with a shorter ifHist list, cut the ifHist
		// list of this lattice down to match it since this information is fresher
		else if(dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size() < ifHist.size())
		{
			while(dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size() < ifHist.size())
				ifHist.erase(ifHist.end());
			//printf("after length cut ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<RankDepIfMeetLat*>(that)->ifHist.size());
		}	
		else
		{
			//printf("meetUpdate Case 4, result: %s\n", str("").c_str());
			return false;
		}
	}
}

bool RankDepIfMeetLat::operator==(Lattice* that)
{
	return initialized == dynamic_cast<RankDepIfMeetLat*>(that)->initialized && 
	       (ifHist == dynamic_cast<RankDepIfMeetLat*>(that)->ifHist) &&
	       (isIfMeet == dynamic_cast<RankDepIfMeetLat*>(that)->isIfMeet);
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string RankDepIfMeetLat::str(string indent)
{
	ostringstream ss;

	ss << indent << "RankDepIfMeetLat[initialized="<<(initialized?"true":"false")<<", isIfMeet="<<(isIfMeet?"true":"false")<<"\n";
	ss << indent << "          ifHist = <";
	for(vector<bool>::iterator it = ifHist.begin(); it!=ifHist.end(); )
	{
		ss << (*it?"true":"false");
		it++;
		if(it!=ifHist.end()) ss << ", ";
	}
	ss << ">]";
	return ss.str();
}

bool RankDepIfMeetLat::getIsIfMeet()
{
	return isIfMeet;
}

/******************
 * IfMeetDetector *
 ******************/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> RankDepIfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void RankDepIfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                                      vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
//		printf("IfMeetDetector::genInitState() A\n");
	//vector<Lattice*> initLat;
//		printf("IfMeetDetector::genInitState() B\n");
	RankDepIfMeetLat* ifml = new RankDepIfMeetLat(n);
//		printf("IfMeetDetector::genInitState() C\n");
	initLattices.push_back(ifml);
//		printf("IfMeetDetector::genInitState() D\n");
	
	//return initLattices;
}
		
// the transfer function that is applied to every node
// n - the dataflow node that is being processed
// state - the NodeState object that describes the state of the node, as established by earlier 
//         analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//          as input and overwrites them with the result of the transfer.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.
bool RankDepIfMeetDetector::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	/*printf("-----------------------------------\n");
	printf("IfMeetDetector::transfer() function %s() node=<%s | %s>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	cout << "dfInfo.front() = "<<dfInfo.front()->str() << "\n";*/
	//bool modified = false;
	RankDepIfMeetLat* ifml = dynamic_cast<RankDepIfMeetLat*>(dfInfo.front());
	
	// if this node is a descendant of an if statement
	if(n == ifml->ifLeft)
	{
		ifml->ifHist.push_back(0);
		// change ifLeft so that we won't push 0 again in case this transfer function ever gets called again
		ifml->ifLeft = ifml->parentNode;
	}
	if(n == ifml->ifRight)
	{
		ifml->ifHist.push_back(1);
		// change ifRight so that we won't push 1 again in case this transfer function ever gets called again
		ifml->ifRight = ifml->parentNode;
	}
		
	// If this is an if statement that depends on the process rank
	if(isSgIfStmt(n.getNode()) && n.getIndex()==1 && isMPIRankDep(func, n))
	{
		// iterate over both the descendants
		vector<DataflowEdge> edges = n.outEdges();
		for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
		{
			// Set ifLeft and ifRight to tell each descendant that they're on the left/right branch of this if statement
			if((*ei).condition() == eckTrue)
			{
				ifml->ifLeft = ((*ei).target());
			}
			else if((*ei).condition() == eckFalse)
			{
				ifml->ifRight = ((*ei).target());
			}
		}
	}
	
	//printf("ifml->ifHist.size()=%d\n", ifml->ifHist.size());
	ifml->initialized = true;
	return true;
}

// prints the Lattices set by the given DivAnalysis 
void printRankDepIfMeetDetectorStates(RankDepIfMeetDetector* ifmd, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(ifmd, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}

static RankDepIfMeetDetector* rdifmd=NULL;
// Runs the IfMeetDetector analysis pass
void runRankDepIfMeetDetector(bool printStates)
{
	if(ifmd==NULL)
	{
		rdifmd = new RankDepIfMeetDetector();	
		UnstructuredPassInterDataflow upid_rdifmd(rdifmd);
		upid_rdifmd.runAnalysis();
		
		if(printStates)
			printRankDepIfMeetDetectorStates(rdifmd, ":");
	}
}

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isRankDepIfMeetNode(const DataflowNode& n)
{
	NodeState* state = NodeState::getNodeState(n, 0);
	Lattice* ifmdLat = state->getLatticeBelow(ifmd, 0);
	return dynamic_cast<RankDepIfMeetLat*>(ifmdLat)->getIsIfMeet();
}
