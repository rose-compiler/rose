#include "rose.h"
#include <CallGraph.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include<map>

using namespace std;

std::string stripGlobalModifer(std::string str){
	if( str.substr(0,2) == "::" )
	  str = str.substr(2);

	return str;

};

bool nodeCompare(const CallGraphCreate::Node *a,const CallGraphCreate::Node * b)
{
	string aStr=stripGlobalModifer(a->functionDeclaration->get_qualified_name().getString());

	string bStr=stripGlobalModifer(b->functionDeclaration->get_qualified_name().getString());

	// compare length
	if (aStr.length()<bStr.length()) return true;
	else if (aStr.length()>bStr.length()) return false;
	// no lengthdifference, compare text
	else return aStr.compare(bStr)<0;
}


void sortedCallGraphDump(string fileName, CallGraphCreate * cg)
{
	list<CallGraphCreate::Node *> cgNodes;
	CallGraphCreate::Node * prevNode,*prevCalledNode,*node;
	list<CallGraphCreate::Node *> calledNodes;
	
	// get all nodes form the CG
	CallGraphCreate::NodeIterator nodeItr = cg->GetNodeIterator();
	CallGraphCreate::EdgeIterator edgeItr;
	// iterate over all nodes in the call-graph
	while(!nodeItr.ReachEnd())
	{
		CallGraphCreate::Node * currentNode=*nodeItr;
	  // there is at least one edge going to or coming from the node
		if (!(cg->GetNodeEdgeIterator(*nodeItr,GraphAccess::EdgeIn)).ReachEnd() || !(cg->GetNodeEdgeIterator(*nodeItr,GraphAccess::EdgeOut)).ReachEnd())
		{
			// add that node to the list
			cgNodes.push_back(currentNode);
		}
		nodeItr++;
	}
	// sort the list
	cgNodes.sort(nodeCompare);

	// The next section is goint to output the call-graph in unique graph-dump
	ofstream file;
	file.open(fileName.c_str());
	
	prevNode=NULL;
	for (list<CallGraphCreate::Node *>::iterator i=cgNodes.begin();i!=cgNodes.end();i++)
	{
		// is the previouse node the same as this one, then sikp in this test
		if ((*i)==prevNode) continue;
		prevNode=(*i);
		// cleanup the called list from last-time
		calledNodes.clear();
		edgeItr = cg->GetNodeEdgeIterator((*i),GraphAccess::EdgeOut);
		// get all called nodes
		while(!edgeItr.ReachEnd())
		{
			node=cg->GetEdgeEndPoint(*edgeItr,GraphAccess::EdgeOut);
			node=cg->GetEdgeEndPoint(*edgeItr,GraphAccess::EdgeIn);
			// add this node to the called list
			calledNodes.push_back(node);
			edgeItr++;
		}
		calledNodes.sort(nodeCompare);
		// go over the sorted calledNodes list and output to file
		if (calledNodes.size()==0)
		{
			file << stripGlobalModifer((*i)->functionDeclaration->get_qualified_name().getString()) <<" ->";
		}
		else
		{
			file << stripGlobalModifer((*i)->functionDeclaration->get_qualified_name().getString()) <<" ->";			
			prevCalledNode=NULL;
			for (list<CallGraphCreate::Node *>::iterator j=calledNodes.begin();j!=calledNodes.end();j++)
			{
				// if the previouse node is the same like this, skip it
				if ((*j)==prevCalledNode) continue;
				prevCalledNode=(*j);
				// dup to file
				file << " " << stripGlobalModifer((*j)->functionDeclaration->get_qualified_name().getString());
			}		
		}
		file << endl;
	}

	file.close();
};

int main (int argc, char **argv){
	string	outFileName;
	// Build the AST used by ROSE
	SgProject * project = frontend (argc, argv);
	ROSE_ASSERT (project != NULL);
	
	// Build the callgraph according to Anreases example
	CallGraphBuilder		cgb (project);
	cgb.buildCallGraph();
	cgb.classifyCallGraph();
  // write a dotfile vor visualisation
  outFileName=((project->get_outputFileName())+".dot");
	cout << "Writing Callgraph to: "<<outFileName<<endl;
	GenerateDotGraph (cgb.getGraph (),outFileName.c_str());
	
	// generate compareisonFile
	outFileName=((project->get_outputFileName())+".cg.dmp");
	sortedCallGraphDump(outFileName,cgb.getGraph());	

	return 0;
}
