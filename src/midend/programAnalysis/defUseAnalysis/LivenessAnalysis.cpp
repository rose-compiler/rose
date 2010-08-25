/******************************************
 * Category: DFA
 * Variable Liveness Analysis Definition
 * created by tps in Feb 2007
 *****************************************/

/******************************************
 * Algorithm used:
 * for all n, in[n] = out[n] = 0
 * w = { set of all nodes }
 * repeat until w empty
 *    n = w.pop()
 *    out[n] = OR (n' ELEM SUCC[n]) in[n']
 *    in[n] = use[n] OR (out[n] - def[n])
 *    if change to in[n]
 *       for all predecessors m of n, w.push(m)
 *****************************************/
// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 19,1 MB to 8,8MB

#include "sage3basic.h"
#include "LivenessAnalysis.h"
#include "DefUseAnalysis_perFunction.h"
#include "GlobalVarAnalysis.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "BottomUpTraversalLiveness.h"
using namespace std;

/******************************************
 * Merge two containers
 *****************************************/
template<class T>
T LivenessAnalysis::merge_no_dups(T& v1, T& v2) {
	T ret(v1);
	for (typename T::const_iterator i = v2.begin(); i != v2.end(); ++i) {
		bool notfound = true;
		for (typename T::const_iterator z = ret.begin(); z != ret.end(); ++z) {
			if (*z == *i) {
				notfound = false;
				break;
			}
		}
		if (notfound) {
			ret.push_back(*i);
		}
	}
	return ret;
}

/******************************************
 * Helper Function
 * Get the enclosing function definition of a node
 *****************************************/
SgFunctionDefinition*
LivenessAnalysis::getFunction(SgNode* node) {
	ROSE_ASSERT(node);
	// make sure that this node is within a function
	SgNode* parent = node->get_parent();
	ROSE_ASSERT(parent);
	while (!isSgFunctionDeclaration(parent)) {
		parent = parent->get_parent();
		ROSE_ASSERT(parent);
		if (isSgProject(parent)) {
			cerr
					<< " This node is not within a function. Cant run Variable Liveness Analysis."
					<< endl;
			return NULL;
		}
	}
	SgFunctionDeclaration* func = isSgFunctionDeclaration(parent);
	ROSE_ASSERT(func);
	SgFunctionDefinition* funcDef = func->get_definition();
	ROSE_ASSERT(funcDef);
	return funcDef;
}

/******************************************
 * Helper Function
 * !Print out live-in and live-out variables for a node
 *****************************************/
void LivenessAnalysis::printInAndOut(SgNode* sgNode) {
	if (DEBUG_MODE)
		cout << ">>> in and out for : " << sgNode << "  "
				<< sgNode->class_name() << endl;
	std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it_in =
			in.find(sgNode);
	if (it_in != in.end()) {
		std::vector<SgInitializedName*> currIn = in[sgNode];
		std::vector<SgInitializedName*>::const_iterator it2 = currIn.begin();
		if (DEBUG_MODE)
			cout << "   in : ";
		for (; it2 != currIn.end(); ++it2) {
			SgInitializedName* init = isSgInitializedName(*it2);
			ROSE_ASSERT(init);
			std::string name = ".";
			name = init->get_name().str();
			if (DEBUG_MODE)
				cout << name << ", ";
		}
		if (DEBUG_MODE)
			cout << endl;
	}
	std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it_out =
			out.find(sgNode);
	if (it_out != out.end()) {
		std::vector<SgInitializedName*> currOut = out[sgNode];
		std::vector<SgInitializedName*>::const_iterator it3 = currOut.begin();
		if (DEBUG_MODE)
			cout << "   out : ";
		for (; it3 != currOut.end(); ++it3) {
			SgInitializedName* init = isSgInitializedName(*it3);
			ROSE_ASSERT(init);
			std::string name = ".";
			name = init->get_name().str();
			if (DEBUG_MODE)
				cout << name << ", ";
		}
		if (DEBUG_MODE)
			cout << endl;
	}
}

/******************************************
 * Helper Function
 * used for sorting
 *****************************************/
static bool sort_using_greater_than(SgNode* u, SgNode* v) {
	return u > v;
}

/**********************************************************
 * Traverse the CFG backwards from a given node to
 * determine whether a change has occurred at that path
 * until the first split (2 in-edges) or root.
 *********************************************************/
template<typename T>
bool LivenessAnalysis::hasANodeAboveCurrentChanged(T source) {
	bool changed = false;
	// go cfg back until split or root
	// check nodeChangeMap if a all nodes have changed.
	// if any has changed return true.
	vector<FilteredCFGEdge<IsDFAFilter> > in_edges = source.outEdges();
	if (in_edges.size() == 1) {
		FilteredCFGEdge<IsDFAFilter> filterEdge = in_edges[0];
		FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
		SgNode* sgNode = filterNode.getNode();
		ROSE_ASSERT(sgNode);
		bool changedInTable = nodeChangedMap[sgNode];
		if (DEBUG_MODE)
			cout << " >>> backward CFG : visiting node : " << sgNode << " "
					<< sgNode->class_name() << "  changed : " << changedInTable
					<< endl;
		if (changedInTable) {
			return true;
		} else {
			changed = hasANodeAboveCurrentChanged(filterNode);
		}
	}
	return changed;
}

// get all nodes above
template<class T>
void LivenessAnalysis::getAllNodesAbove(T source, std::vector<T>& visited) {
	vector<FilteredCFGNode<IsDFAFilter> > worklist;
	worklist.push_back(source);
	while (!worklist.empty()) {
		source = worklist.front();
		worklist.erase(worklist.begin());
		vector<FilteredCFGEdge<IsDFAFilter> > in_edges = source.inEdges();
		vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator it =
				in_edges.begin();
		for (; it != in_edges.end(); ++it) {
			FilteredCFGEdge<IsDFAFilter> filterEdge = *it;
			FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.source();
			SgNode* sgNode = filterNode.getNode();
			ROSE_ASSERT(sgNode);
			if (find(visited.begin(), visited.end(), filterNode)
					== visited.end()) {
				worklist.push_back(filterNode);
				visited.push_back(filterNode);
			}
		}
	}
}

// get all nodes below
template<class T>
void LivenessAnalysis::getAllNodesBelow(T source, std::vector<T>& visited) {
	vector<FilteredCFGNode<IsDFAFilter> > worklist;
	worklist.push_back(source);
	while (!worklist.empty()) {
		source = worklist.front();
		worklist.erase(worklist.begin());
		vector<FilteredCFGEdge<IsDFAFilter> > in_edges = source.outEdges();
		vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator it =
				in_edges.begin();
		for (; it != in_edges.end(); ++it) {
			FilteredCFGEdge<IsDFAFilter> filterEdge = *it;
			FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
			SgNode* sgNode = filterNode.getNode();
			ROSE_ASSERT(sgNode);
			if (find(visited.begin(), visited.end(), filterNode)
					== visited.end()) {
				worklist.push_back(filterNode);
				visited.push_back(filterNode);
			}
		}
	}
}

/**********************************************************
 * Def-use algorithm used for variable live analysis
 * This algorithm is almost a match to def-use algorithm
 *********************************************************/
template<typename T>
bool LivenessAnalysis::defuse(T cfgNode, bool *unhandled) {
	// get SgNode from CFG Node
	SgNode* sgNode = cfgNode.getNode();
	ROSE_ASSERT(sgNode);
	// count the amount of times this nodes was visited (used for visualization)
	if (visited.find(sgNode) == visited.end())
		visited[sgNode] = 1;
	else
		visited[sgNode]++;
	// remember the Node the comes before
#if 0
	SgNode* sgNodeBefore = getCFGPredNode(cfgNode);
	if (forwardAlgo) {
		sgNodeBefore = getCFGSuccNode(cfgNode);
	}
#endif

	// Determine if the in_edges have been visited before
	// Have we been here before, if not, unhandled is true
	if (forwardAlgo) {
		vector<FilteredCFGEdge<IsDFAFilter> > in_edgesCFG = cfgNode.outEdges();
		for (vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator i =
				in_edgesCFG.begin(); i != in_edgesCFG.end(); ++i) {
			FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
			FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
			SgNode* sgNode2 = filterNode.getNode();
			if (visited.find(sgNode2) == visited.end())
				*unhandled = true;
		}
	} else {
		vector<FilteredCFGEdge<IsDFAFilter> > in_edgesCFG = cfgNode.inEdges();
		for (vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator i =
				in_edgesCFG.begin(); i != in_edgesCFG.end(); ++i) {
			FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
			FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.source();
			SgNode* sgNode2 = filterNode.getNode();
			if (visited.find(sgNode2) == visited.end())
				*unhandled = true;
		}
	}
	bool has_changed = false;
	if (DEBUG_MODE) {
		cout
				<< "\n\n------------------------------------------------------------------\ncurrent Node: "
				<< sgNode << endl;
		//"  previous Node : " << sgNodeBefore << endl;
		printInAndOut(sgNode);
	}

	// ****************************************************************************
	// USE DEF-USE ANALYSIS (previously created) to get def-use info
	// get def and use for this node :: is this not a DEF OR A USE??
	SgInitializedName* initName = isSgInitializedName(sgNode);
	SgVarRefExp* varRef = isSgVarRefExp(sgNode);
	bool defNode = false;
	bool useNode = false;
	if (initName) {
		std::vector<SgNode*> defs = dfa->getDefFor(sgNode, initName);
		//get the def and use for the current node
		std::vector<SgNode*>::const_iterator it = defs.begin();
		for (; it != defs.end(); ++it) {
			SgNode* itNode = *it;
			if (itNode == sgNode)
				defNode = true;
		}
	}
	if (varRef) {
		initName = varRef->get_symbol()->get_declaration();
		ROSE_ASSERT(initName);
		std::vector<SgNode*> uses = dfa->getUseFor(sgNode, initName);
		std::vector<SgNode*>::const_iterator it = uses.begin();
		for (; it != uses.end(); ++it) {
			SgNode* itNode = *it;
			if (itNode == sgNode)
				useNode = true;
		}
	}
#if 0
	SgPntrArrRefExp* varRefArr = isSgPntrArrRefExp(sgNode);
	if (varRefArr) {
		SgExpression* left = varRefArr->get_lhs_operand();
		ROSE_ASSERT(left);
		SgVarRefExp* varRefA = isSgVarRefExp(left);
		ROSE_ASSERT(varRefA);
		initName = varRefA->get_symbol()->get_declaration();
		ROSE_ASSERT(initName);
		std::vector <SgNode*> uses = dfa->getUseFor(sgNode, initName);
		std::vector<SgNode*>::const_iterator it = uses.begin();
		for (;it!=uses.end();++it) {
			SgNode* itNode = *it;
			if (itNode==sgNode)
			useNode=true;
		}
	}
#endif
	// ****************************************************************************

	if (DEBUG_MODE) {
		cout << "     At this point def : " << defNode << "  use : " << useNode
				<< endl;
		if (initName)
			cout << "  initName : " << initName->get_name().str() << endl;
		cout << " Doing out = " << endl;
	}

	// ****************************************************************************
	// do the algo for variable liveness : OUT[n]
	// OUT (SgNode) = {}
	// creating outSgNode ... so we never have to check for this again
	out[sgNode].clear();
	// look at the outgoing edges ....
	// According to algorithm :    out[n] = OR (n' ELEM SUCC[n]) in[n']
	vector<FilteredCFGEdge<IsDFAFilter> > out_edges = cfgNode.outEdges();
	for (vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator i =
			out_edges.begin(); i != out_edges.end(); ++i) {
		FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
		FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
		SgNode* sgNodeNext = filterNode.getNode();
		ROSE_ASSERT(sgNodeNext);
		std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator
				it_in = in.find(sgNodeNext);
		std::vector<SgInitializedName*> tmpIn;
		if (it_in != in.end()) {
			tmpIn = in[sgNodeNext];
		}
		if (DEBUG_MODE)
			cout << "   out : previous node : " << sgNodeNext << " "
					<< sgNodeNext->class_name() << "   in Size : "
					<< tmpIn.size() << "   out[sgNode].size = "
					<< out[sgNode].size() << endl;

		out[sgNode] = merge_no_dups(out[sgNode], tmpIn);
		std::sort(out[sgNode].begin(), out[sgNode].end(),
				sort_using_greater_than);
	}
	// ****************************************************************************


	if (DEBUG_MODE)
		printInAndOut(sgNode);

	// ****************************************************************************
	// do the algo for variable liveness : IN[n]
	// in[n] = use[n] OR (out[n] - def[n])
	// what if it is an assignment
	switch (sgNode->variant()) {
	case V_SgPlusPlusOp:
	case V_SgMinusMinusOp:
	case V_SgAssignOp:
	case V_SgModAssignOp:
	case V_SgDivAssignOp:
	case V_SgMultAssignOp:
	case V_SgLshiftAssignOp:
	case V_SgRshiftAssignOp:
	case V_SgXorAssignOp:
	case V_SgAndAssignOp:
	case V_SgMinusAssignOp:
	case V_SgPlusAssignOp: {
		// go through all initialized names for out
		// and cancel the InitializedName for in if it is
		// defined for this node
		std::vector<SgInitializedName*> vec = out[sgNode];
		for (std::vector<SgInitializedName*>::iterator inIt = vec.begin(); inIt
				!= vec.end(); ++inIt) {
			SgInitializedName* initN = isSgInitializedName(*inIt);
			std::vector<SgNode*> defs = dfa->getDefFor(sgNode, initN);
			std::vector<SgNode*>::const_iterator it = defs.begin();
			for (; it != defs.end(); ++it) {
				SgNode* itNode = *it;
				if (itNode == sgNode) {
					// We mark the current node (SgNode) as being defined here
					// its defined with the variable initName
					defNode = true;
					initName = initN;
					break;
				}
			}
		}
		if (DEBUG_MODE) {
			if (initName)
				cout << " This is an assignment :  initName = "
						<< initName->get_name().str() << "  def : " << defNode
						<< endl;
			else
				cout
						<< " !!! This is an assignment but no initName matched. defnode = "
						<< defNode << endl;
		}
		break;
	}
	default: {
		// WE ARE HANDLING SPECIAL CASES HERE WHERE INFINITE LOOPS MAY OCCUR
#if 0
		//
		// its none of the above, meaning its not a definition
		// none of the above breakPointForWhiles is hit
		*unhandled=true;
		// take care of the case where we have none of the above within a loop (breakPointForWhile)
		// i.e. no : VarRefExp, InitializedName, FunctionDefinition ...
		// If this unhandled node has been added to the map before (visited)
		// then we do want to mark it as handled.
		if (isSgWhileStmt(sgNode) || isSgForStatement(sgNode) || isSgDoWhileStmt(sgNode)) {
			if (breakPointForWhileNode==NULL) {
				breakPointForWhileNode=sgNode;
				breakPointForWhile++;
				if (DEBUG_MODE)
				cout << ">>> Setting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
			} else if (sgNode==breakPointForWhileNode) {
				// reaching the breakPoint for a second time
				// check if any node above this node up to the branch or root has changed
				breakPointForWhile++;
				bool hasAnyNodeAboveChanged = hasANodeAboveCurrentChanged(cfgNode);
				if (hasAnyNodeAboveChanged==false) {
					// need to break this loop
					// add current node to doNotVisitMap
					doNotVisitMap.insert(sgNode);
				}

				if (DEBUG_MODE)
				cout << ">>> Inc Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
				*unhandled = false;
				breakPointForWhileNode=NULL;
				breakPointForWhile=0;
				if (DEBUG_MODE)
				cout << ">>> Resetting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
			} else {
				if (DEBUG_MODE)
				cout << ">>> Skipping unhandled node ... " << endl;
			}
		}
#endif
		break;
	}
	}
	// ****************************************************************************


	// ****************************************************************************
	// THE FOLLOWING ALGORITHM IS APPLIED HERE
	// in[n] = use[n] OR (out[n] - def[n])
	// remember the old in[sgNode]
	std::vector<SgInitializedName*> inOLD;
	{
		std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator
				it_in = in.find(sgNode);
		if (it_in != in.end()) {
			inOLD = in[sgNode];
		}
	}
	// This is save -- out[sgNode] always exists
	in[sgNode] = out[sgNode];
	bool sgNodeForINexists = false;
	{
		std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator
				it_in = in.find(sgNode);
		if (it_in != in.end()) {
			sgNodeForINexists = true;
		}
	}
	if (defNode) {
		// delete all definitions
		// (out[n] - def[n])
		if (sgNodeForINexists) {
			// make sure this operation is safe
			std::vector<SgInitializedName*> vec = in[sgNode];
			for (std::vector<SgInitializedName*>::iterator inIt = vec.begin(); inIt
					!= vec.end(); ++inIt) {
				if (*inIt == initName) {
					vec.erase(inIt); // = initName
					break;
				}
			}
			in[sgNode] = vec;
		}
	}
	if (useNode) {
		// if this initName is used but it is not in the in[sgNode] map
		// then we need to add it
		bool found = false;
		if (sgNodeForINexists) {
			std::vector<SgInitializedName*> vec = in[sgNode];
			std::vector<SgInitializedName*>::iterator inIt = vec.begin();
			for (; inIt != vec.end(); ++inIt) {
				if (*inIt == initName) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			std::string name = initName->get_name().str();
			in[sgNode].push_back(initName); // = varRef
			std::sort(in[sgNode].begin(), in[sgNode].end(),
					sort_using_greater_than);
			if (DEBUG_MODE)
				cout << " did not find initName : " << name
						<< " in in[sgNode]    size: " << in[sgNode].size()
						<< endl;
			if (DEBUG_MODE)
				cout << " added sgNode :   new size [sgNode] = "
						<< in[sgNode].size() << endl;
		}
	}
	// ****************************************************************************


#if 0
	// ****************************************************************************
	if (defNode || useNode) {
		// determine if the node has changed in respect to in and out maps
		// we return whether in and out are the same,
		// prerequise, they are sorted
		if (DEBUG_MODE)
		cout << " This was a def or use node " << endl;
		// has_changed only applies here
		bool equal = false;
		// make sure this node exists in the in and out maps
		std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it_in = in.find(sgNode);
		std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it_out = out.find(sgNode);
		//    std::vector<SgInitializedName*> vec = out[sgNode];
		//    if (it!=in.end() && vec.size()>0) {
		if (it_in!=in.end() && it_out!=out.end()) {
			equal = std::equal(in[sgNode].begin(),in[sgNode].end(),out[sgNode].begin());
			if (!equal)
			has_changed=true;
			if (DEBUG_MODE) {
				cout << " CHECKME : IN AND OUT ARE equal : " << equal << endl;
				printInAndOut(sgNode);
			}
		}
	} else {
		// if it is a arbitraty node, we assume it has changed, so we can traverse further
		has_changed=true;
	}
	// ****************************************************************************
#else

	// ****************************************************************************
	// Has in[n] changed?
	std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it_in =
			in.find(sgNode);
	if (it_in != in.end() && inOLD.size() > 0) {
		// compare the oldIN and the newIN and see if in[n] has changed
		bool equal = false;
		equal = std::equal(in[sgNode].begin(), in[sgNode].end(), inOLD.begin());
		if (!equal)
			has_changed = true;
	}

	// ****************************************************************************
#endif

	if (DEBUG_MODE) {
		cout << " value has changed ... : " << has_changed << endl;
		printInAndOut(sgNode);
	}

	return has_changed;
}

FilteredCFGNode<IsDFAFilter> LivenessAnalysis::run(
		SgFunctionDefinition* funcDecl, bool& abortme) {
	// filter functions -- to only functions in analyzed file

	// ---------------------------------------------------------------
	// algorithm to interate over all CFG nodes and determine liveness
	// ---------------------------------------------------------------
	abort = false;
	counter = 0;
	nrOfNodesVisitedPF = 0;
	breakPointForWhileNode = NULL;
	breakPointForWhile = 0;
	;
	// clear those maps for each function run
	doNotVisitMap.clear();
	nodeChangedMap.clear();

	string funcName = getFullName(funcDecl);
	//  DEBUG_MODE = false;
	DEBUG_MODE_EXTRA = false;

	if (funcName == "") {
		FilteredCFGNode<IsDFAFilter> empty(CFGNode(NULL, 0));
		return empty;
	}
	ROSE_ASSERT(funcDecl);

	if (DEBUG_MODE)
		cout << " Found function " << funcName << endl;

	// DFA on that function
	vector<FilteredCFGNode<IsDFAFilter> > worklist;

	//waitAtMergeNode.clear();

	forwardAlgo = false;
	// add this node to worklist and work through the outgoing edges
	FilteredCFGNode<IsDFAFilter> source = FilteredCFGNode<IsDFAFilter> (
			funcDecl->cfgForEnd());
	if (forwardAlgo)
		source = FilteredCFGNode<IsDFAFilter> (funcDecl->cfgForBeginning());

	//CFGNode cmpSrc = CFGNode(funcDecl->cfgForEnd());
	FilteredCFGNode<IsDFAFilter> rem_source = source;

	if (DEBUG_MODE) {
		std::ofstream f("cfg.dot");
		cfgToDot(f, string("cfg"), source);
		f.close();
	}

	worklist.push_back(source);
	vector<FilteredCFGNode<IsDFAFilter> > debug_path;
	debug_path.push_back(source);

	// add all nodes to worklist -
	if (forwardAlgo) {
		vector<FilteredCFGNode<IsDFAFilter> > allNodesAfter;
		getAllNodesBelow(source, allNodesAfter);
		for (vector<FilteredCFGNode<IsDFAFilter> >::const_iterator i =
				allNodesAfter.begin(); i != allNodesAfter.end(); ++i) {
			FilteredCFGNode<IsDFAFilter> filterNode = *i;
			if (find(worklist.begin(), worklist.end(), filterNode)
					== worklist.end()) {
				worklist.push_back(filterNode);
				debug_path.push_back(filterNode);
			}
		}
	} else {
		vector<FilteredCFGNode<IsDFAFilter> > allNodesBefore;
		getAllNodesAbove(source, allNodesBefore);
		for (vector<FilteredCFGNode<IsDFAFilter> >::const_iterator i =
				allNodesBefore.begin(); i != allNodesBefore.end(); ++i) {
			FilteredCFGNode<IsDFAFilter> filterNode = *i;
			if (find(worklist.begin(), worklist.end(), filterNode)
					== worklist.end()) {
				worklist.push_back(filterNode);
				debug_path.push_back(filterNode);
			}
		}
	}
	//cerr << "   . Total nodes to check : " << worklist.size() << endl;
	bool valueHasChanged = false;
	bool unhandledNode = false;
	while (!worklist.empty()) {
		source = worklist.front();
		worklist.erase(worklist.begin());
		// do current node
		unhandledNode = false;
		SgNode* next = source.getNode();
		if (doNotVisitMap.find(next) != doNotVisitMap.end())
			continue;
		valueHasChanged = defuse(source, &unhandledNode);
		//    cerr << " First run : " << next->class_name() << "  changed: " <<
		//  valueHasChanged << "  unhandled: " << unhandledNode << endl;
		nodeChangedMap[source.getNode()] = valueHasChanged;
		// do follow-up nodes
		// get nodes of outgoing edges and pushback (if not already contained)
		if (DEBUG_MODE) {
			cout << " Current Node: " << source.getNode() << " "
					<< source.getNode()->class_name() << " changed: "
					<< resBool(valueHasChanged) << endl;
			cout << " Current Node: " << source.getNode() << " unhandled: "
					<< resBool(unhandledNode) << endl;
		}
		if (valueHasChanged || unhandledNode) {
			vector<FilteredCFGEdge<IsDFAFilter> > in_edges;
			if (forwardAlgo)
				in_edges = source.inEdges();
			else
				// default :
				in_edges = source.outEdges();
			for (vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator i =
					in_edges.begin(); i != in_edges.end(); ++i) {
				FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
				FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
				if (forwardAlgo)
					filterNode = filterEdge.source();
				if (find(worklist.begin(), worklist.end(), filterNode)
						== worklist.end()) {
					worklist.push_back(filterNode);
					debug_path.push_back(filterNode);
				}
			}
			// ------------------------
			if (DEBUG_MODE)
				printCFGVector(worklist);
		}

		counter++;
                // Liao 1/27/2010
                // Increase the upper limit to handle larger input
		//if (counter == 4000)
		if (counter == 80000)
			abort = true;

		if (abort) {
			cerr << " \n\n\n Maximum iteration count is reached. ABORTING !! INFINITE EXECUTION ... \n\n"
                             << endl;
                        worklist.clear();
#if 1
			// need to fix this, apperently some cases run indefinetly
			abortme = true;
#endif
		}
	}
	nrOfNodesVisitedPF = debug_path.size();

	if (DEBUG_MODE) {
		cout << " Exiting function " << funcName << endl;
		cout << "\nNr of nodes visited " << debug_path.size()
				<< "  of nodes : " << endl;
		printCFGVector(debug_path);
	}

	// ---------------------------------------------------------------
	// algorithm to interate over all CFG and propagate IN and OUT
	// information from all non-statements to statements (move up in AST)
	// ---------------------------------------------------------------

	// we should traverse the AST in bottom up order and collect information
	// for in and out until we reach a statement
#if 0
	SgNode* root = rem_source.getNode();
	ROSE_ASSERT(root);
	BottomUpTraversalLivenessIN* bottomUpIN = new BottomUpTraversalLivenessIN(
			this);
	bottomUpIN->traverse(root);

	BottomUpTraversalLivenessOUT* bottomUpOUT =
	new BottomUpTraversalLivenessOUT(this);
	bottomUpOUT->traverse(root);
#endif

	return rem_source;

}

void
LivenessAnalysis::fixupStatementsINOUT(SgFunctionDefinition* funcDecl) {
	FilteredCFGNode<IsDFAFilter> source = FilteredCFGNode<IsDFAFilter> (
			funcDecl->cfgForEnd());

	SgNode* root = source.getNode();
	ROSE_ASSERT(root);
	BottomUpTraversalLivenessIN* bottomUpIN = new BottomUpTraversalLivenessIN(
			this);
	bottomUpIN->traverse(root);

	BottomUpTraversalLivenessOUT* bottomUpOUT =
	new BottomUpTraversalLivenessOUT(this);
	bottomUpOUT->traverse(root);

}
