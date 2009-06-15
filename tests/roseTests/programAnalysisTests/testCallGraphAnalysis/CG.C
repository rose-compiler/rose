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
        
      
	string aStr;
        if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
           aStr=stripGlobalModifer(a->properties->functionName);
        }else
          aStr=stripGlobalModifer(a->functionDeclaration->get_qualified_name().getString());

	string bStr;
        if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
          bStr=stripGlobalModifer(b->properties->functionName);
        }else
          bStr=stripGlobalModifer(b->functionDeclaration->get_qualified_name().getString());

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
                  /*
                  std::cout << "First node of this type" << std::endl;
                  if((*i)->functionDeclaration != NULL && (*i) != NULL )
                  */

                  if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
                    file << stripGlobalModifer((*i)->properties->functionName) <<" ->";
                  else
                    file << stripGlobalModifer((*i)->functionDeclaration->get_qualified_name().getString()) <<" ->";

		}
		else
		{
                //  std::cout << "Second First node of this type " << (*i)->properties->nid << " " << (*i)->properties->functionName <<std::endl;
                  //std::cout << "Second First node of this type " << (*i)->properties->nid << " " <<(*i)->properties->label << " " << (*i)->properties->type << " " << (*i)->properties->scope << " " << (*i)->properties->functionName << std::endl;


                        if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
                   	   file << stripGlobalModifer((*i)->properties->functionName) <<" ->";
                        else
                           file << stripGlobalModifer((*i)->functionDeclaration->get_qualified_name().getString()) <<" ->";			
			prevCalledNode=NULL;
			for (list<CallGraphCreate::Node *>::iterator j=calledNodes.begin();j!=calledNodes.end();j++)
			{
				// if the previouse node is the same like this, skip it
				if ((*j)==prevCalledNode) continue;
				prevCalledNode=(*j);
				// dup to file

                                if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
                                  file << " " << stripGlobalModifer((*j)->properties->functionName);
                                else
                                  file << " " << stripGlobalModifer((*j)->functionDeclaration->get_qualified_name().getString());

			}		
		}
		file << endl;
	}

	file.close();
};

int main (int argc, char **argv){
	string	outFileName;

#ifdef HAVE_SQLITE3
        var_SOLVE_FUNCTION_CALLS_IN_DB = true;
       sqlite3x::sqlite3_connection* gDB = open_db("DATABASE");
#endif

        // Build the AST used by ROSE

        std::vector<std::string> argvList(argv, argv+argc);
	// generate compareisonFile

        std::string graphCompareOutput = "";
       CommandlineProcessing::isOptionWithParameter(argvList,"-compare:","(graph)",graphCompareOutput,true);

       CommandlineProcessing::removeArgsWithParameters(argvList,"-compare:");

	SgProject * project = frontend (argvList);
	ROSE_ASSERT (project != NULL);
	
        if(graphCompareOutput == "" )
          graphCompareOutput=((project->get_outputFileName())+".cg.dmp");

	// Build the callgraph according to Anreases example
	CallGraphBuilder		cgb (project);
	cgb.buildCallGraph();
	cgb.classifyCallGraph();
  // write a dotfile vor visualisation
  outFileName=((project->get_outputFileName())+".dot");




	cout << "Writing Callgraph to: "<<outFileName<<endl;

        CallGraphDotOutput output( *(cgb.getGraph()) );

        CallGraphCreate *newGraph;
        if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
#ifdef HAVE_SQLITE3
          output.writeSubgraphToDB(*gDB );
          /*
          output.filterNodesByDirectory( *gDB, "/export" );
          output.filterNodesByDB( *gDB, "__filter.db" );*/

          output.solveVirtualFunctions( *gDB, "ClassHierarchy" );
          output.solveFunctionPointers( *gDB );
          std::vector<std::string> keepDirs;
          keepDirs.push_back( ROSE_COMPILE_TREE_PATH+std::string("/tests%") );

          filterNodesKeepPaths(*gDB, keepDirs);

          /*
          std::vector<std::string> removeFunctions;
          removeFunctions.push_back("::main%" );
          filterNodesByFunctionName(*gDB,removeFunctions);
          */
          cout << "Loading from DB...\n";
          newGraph = output.loadGraphFromDB( *gDB );
          cout << "Loaded\n";

#endif
        }else{
          // Not SQL Database case
          printf ("Not using the SQLite Database ... \n");
          newGraph = cgb.getGraph();
        }



	GenerateDotGraph (newGraph,outFileName.c_str());
	


	sortedCallGraphDump(graphCompareOutput,newGraph);	

	return 0;
}
