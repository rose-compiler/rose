// Virtual CFG tester: checks whether the CFG can be created for a function,
// and whether the forward and backward edge sets are consistent

#include "rose.h"
#include <algorithm>
#include <time.h>

using namespace std;
using namespace VirtualCFG;

//! start from a CFG node 'n', collect all other CFG nodes which can be reached from 'n'
void getReachableNodes(CFGNode n, set<CFGNode>& s) {
	if (s.find(n) != s.end()) return; // n is already in s
	s.insert(n);
	vector<CFGEdge> oe = n.outEdges();
	for (vector<CFGEdge>::const_iterator i = oe.begin(); i != oe.end(); ++i) {
		getReachableNodes(i->target(), s);
	}
}

void testCFG(SgFunctionDefinition* stmt) {
	// First, get the reachable CFG nodes from the start of the function def
	set<CFGNode> nodes;
	getReachableNodes(stmt->cfgForBeginning(), nodes);

	// Dump graph of CFG
	{
		//std::ofstream graph("graph.dot");
		//cfgToDotForDebugging(graph, "dotGraph", stmt->cfgForBeginning());
	}

	// Ensure that the procedure can return (this might need to be removed later
	// if the CFG accounts for non-returning subroutine calls)
	// ROSE_ASSERT (nodes.find(stmt->cfgForEnd()) != nodes.end());
	if (nodes.find(stmt->cfgForEnd()) == nodes.end())
	{
		printf ("Warning: function does not return \n");
	}

	// Get the forward edges from each node
	map<CFGNode, vector<CFGEdge> > forwardEdges; // Source to set of edges
	for (set<CFGNode>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		vector<CFGEdge> oe = i->outEdges();
		forwardEdges[*i] = oe;
	}

	// Get the backward edges to each node from other nodes in the set
	// There may be spurious edges from unreachable nodes to nodes that are
	// reachable, but those don't matter for checking the CFG correctness
	map<CFGNode, vector<CFGEdge> > backwardEdges; // Source to set of edges
	for (set<CFGNode>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		vector<CFGEdge> ie = i->inEdges();
		for (vector<CFGEdge>::const_iterator j = ie.begin(); j != ie.end(); ++j) {
			if (nodes.find(j->source()) != nodes.end()) {
				backwardEdges[j->source()].push_back(*j);
			}
		}
	}

	// Ideally, the forward and backward edge maps are the same now; start to
	// check this
	// We could just check whether forwardEdges == backwardEdges, but this
	// produces better diagnostics and might allow exceptions to be added in
	// later
	bool anyMismatches = false;
	for (set<CFGNode>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		CFGNode n = *i;
		const vector<CFGEdge>& forwardEdgesFromN = forwardEdges[n];
		const vector<CFGEdge>& backwardEdgesFromN = backwardEdges[n];
		for (vector<CFGEdge>::const_iterator j = forwardEdgesFromN.begin();
				j != forwardEdgesFromN.end(); ++j) {
			if (std::find(backwardEdgesFromN.begin(), backwardEdgesFromN.end(), *j) == backwardEdgesFromN.end()) {
				cerr << "Found edge in forward set which is not in backward set: " << j->source().toStringForDebugging() << " -> " << j->target().toStringForDebugging() << " " << j->toStringForDebugging() << endl;
				anyMismatches = true;
			}
		}
		for (vector<CFGEdge>::const_iterator j = backwardEdgesFromN.begin();
				j != backwardEdgesFromN.end(); ++j) {
			if (std::find(forwardEdgesFromN.begin(), forwardEdgesFromN.end(), *j) == forwardEdgesFromN.end()) {
				cerr << "Found edge in backward set which is not in forward set: " << j->source().toStringForDebugging() << " -> " << j->target().toStringForDebugging() << " " << j->toStringForDebugging() << endl;
				anyMismatches = true;
			}
		}
	}
	if (anyMismatches) {
		ROSE_ASSERT (!"Stopping because of mismatches in CFG edges");
	}
}

int main(int argc, char *argv[]) {

	struct timespec ts_now;
	long mtime_0, mtime_1, mtime_2, mtime_3;

	SgProject* sageProject = frontend(argc,argv);

	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_0 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	AstTests::runAllTests(sageProject);

	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_1 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	NodeQuerySynthesizedAttributeType functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);

	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_2 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;

	for (NodeQuerySynthesizedAttributeType::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
		ROSE_ASSERT (proc);
		testCFG(proc);
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_now)    ;
	mtime_3 = ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;


	cout << "--------------------------------------------------" << endl;
	cout << "runAllTests: " << mtime_1 - mtime_0 << endl;
	cout << "pre-process: " << mtime_2 - mtime_1  << endl;
	cout << "cfg: " << mtime_3 - mtime_2  << endl;
	cout << "--------------------------------------------------" << endl;
	return 0;
}

