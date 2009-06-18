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

bool nodeCompareGraph(const SgGraphNode* a,const SgGraphNode* b)
{
	string aStr;

        Properties* a_property = dynamic_cast<Properties*>(a->getAttribute("Properties"));
        Properties* b_property = dynamic_cast<Properties*>(b->getAttribute("Properties"));

        if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
          ROSE_ASSERT(a_property != NULL);
           aStr=stripGlobalModifer(a_property->functionName);
        }else
        {
          SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(a->get_SgNode());
          ROSE_ASSERT(funcDecl != NULL);

          aStr=stripGlobalModifer(funcDecl->get_qualified_name().getString());
        }

	string bStr;
        if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
          ROSE_ASSERT(b_property != NULL);

          bStr=stripGlobalModifer(b_property->functionName);
        }else
        {
          SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(b->get_SgNode());
          ROSE_ASSERT(funcDecl != NULL);

          bStr=stripGlobalModifer(funcDecl->get_qualified_name().getString());
        }

	// compare length
	if (aStr.length()<bStr.length()) return true;
	else if (aStr.length()>bStr.length()) return false;
	// no lengthdifference, compare text
	else return aStr.compare(bStr)<0;
}

bool nodeCompareGraphPair(const std::pair<SgGraphNode*,int>& a,const std::pair<SgGraphNode*,int>& b)
{
  return nodeCompareGraph(a.first,b.first);
}



void sortedCallGraphDump(string fileName, SgIncidenceDirectedGraph* cg)
{
        
        //Opening output file
        ofstream file;
	file.open(fileName.c_str());


        //Get all nodes of the current CallGraph
	list<std::pair<SgGraphNode*,int> > cgNodes;

        rose_graph_integer_node_hash_map & nodes =
                 cg->get_node_index_to_node_map ();


        for( rose_graph_integer_node_hash_map::iterator it = nodes.begin();
             it != nodes.end(); ++it )
        {
          cgNodes.push_back( pair<SgGraphNode*,int>(it->second,it->first) );
        }


        cgNodes.sort(nodeCompareGraphPair);
        cgNodes.unique();

        //Otuput the call graph in a unique graph-dump

        rose_graph_integer_edge_hash_multimap & outEdges
          = cg->get_node_index_to_edge_multimap_edgesOut ();

	for (list<pair<SgGraphNode *,int> >::iterator it=cgNodes.begin();it!=cgNodes.end();it++)
	{

          //get list over the end-points for which this node points to
          list<SgGraphNode*> calledNodes;


          for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.find(it->second);
              outEdgeIt != outEdges.end(); ++outEdgeIt )
          {

            SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
            ROSE_ASSERT(graphEdge!=NULL);
            calledNodes.push_back(graphEdge->get_to());
          }

          calledNodes.sort(nodeCompareGraph);
          calledNodes.unique();

          //Output the unique graph
          Properties* cur_property = dynamic_cast<Properties*>((it->first)->getAttribute("Properties"));
          SgFunctionDeclaration* cur_function = isSgFunctionDeclaration((it->first)->get_SgNode());

          if (calledNodes.size()==0)
          {
            /*
               std::cout << "First node of this type" << std::endl;
               if((*i)->functionDeclaration != NULL && (*i) != NULL )
             */

            if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
            {
              ROSE_ASSERT( cur_property != NULL );
              file << stripGlobalModifer(cur_property->functionName) <<" ->";
            }else
            {
              ROSE_ASSERT( cur_function != NULL );
              file << stripGlobalModifer(cur_function->get_qualified_name().getString()) <<" ->";
            }

          }
          else
          {
            //  std::cout << "Second First node of this type " << (*i)->properties->nid << " " << (*i)->properties->functionName <<std::endl;
            //std::cout << "Second First node of this type " << (*i)->properties->nid << " " <<(*i)->properties->label << " " << (*i)->properties->type << " " << (*i)->properties->scope << " " << (*i)->properties->functionName << std::endl;


            if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
            {
              ROSE_ASSERT( cur_property != NULL );
              file << stripGlobalModifer(cur_property->functionName) <<" ->";
            }else
            {
              ROSE_ASSERT( cur_function != NULL );
              file << stripGlobalModifer(cur_function->get_qualified_name().getString()) <<" ->";
            }

            for (list<SgGraphNode *>::iterator j=calledNodes.begin();j!=calledNodes.end();j++)
            {
              Properties* j_property = dynamic_cast<Properties*>((*j)->getAttribute("Properties"));
              SgFunctionDeclaration* j_function = isSgFunctionDeclaration((*j)->get_SgNode());

              if (var_SOLVE_FUNCTION_CALLS_IN_DB == true)
                file << " " << stripGlobalModifer( j_property->functionName );
              else
                file << " " << stripGlobalModifer( j_function->get_qualified_name().getString() );

            }		
          }
          file << endl;


        }

        file.close();

};

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
        std::vector<std::string> argvList(argv, argv+argc);
        std::string dbName = "";
        CommandlineProcessing::isOptionWithParameter(argvList,"-db:","(name)", dbName,true);

#ifdef HAVE_SQLITE3
        var_SOLVE_FUNCTION_CALLS_IN_DB = true;


        if(dbName == "")
        {
          std::cerr << "Error: Please specify a database name with the -db:name option" << std::endl;
          exit(1);
        }

       sqlite3x::sqlite3_connection* gDB = open_db(dbName);


#endif

        // Build the AST used by ROSE

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
	cout << "Writing custom compare to: "<<graphCompareOutput<<endl;

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

          SgIncidenceDirectedGraph* incidenceGraph = loadCallGraphFromDB(*gDB);

          sortedCallGraphDump(graphCompareOutput,incidenceGraph);	

#endif
        }else{
          // Not SQL Database case
          printf ("Not using the SQLite Database ... \n");
          newGraph = cgb.getGraph();

          sortedCallGraphDump(graphCompareOutput,newGraph);	

        }



	GenerateDotGraph (newGraph,outFileName.c_str());
	


	return 0;
}
