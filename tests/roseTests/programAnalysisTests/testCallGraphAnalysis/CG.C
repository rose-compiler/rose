// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"
#undef CONFIG_ROSE /* prevent error about including both private and public headers; must be between rose_config.h and rose.h */

#include "rose.h"
#include <CallGraph.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include<map>

#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
#endif

using namespace std;


bool var_SOLVE_FUNCTION_CALLS_IN_DB = false;

std::string stripGlobalModifer(std::string str){
	if( str.substr(0,2) == "::" )
	  str = str.substr(2);

	return str;

};



bool nodeCompareGraph(const SgGraphNode* a,const SgGraphNode* b)
{
    SgFunctionDeclaration* funcDecl1 = isSgFunctionDeclaration(a->get_SgNode());
    ROSE_ASSERT(funcDecl1 != NULL);
    
    SgFunctionDeclaration* funcDecl2 = isSgFunctionDeclaration(b->get_SgNode());
    ROSE_ASSERT(funcDecl2 != NULL);
    	
    return stripGlobalModifer(funcDecl1->get_qualified_name().getString()) 
         != stripGlobalModifer(funcDecl2->get_qualified_name().getString());
}

bool nodeCompareGraphPair(const std::pair<SgGraphNode*,int>& a,const std::pair<SgGraphNode*,int>& b)
{
  return nodeCompareGraph(a.first,b.first);
}



void sortedCallGraphDump(string fileName, SgIncidenceDirectedGraph* cg)
{
    ROSE_ASSERT(var_SOLVE_FUNCTION_CALLS_IN_DB == false);
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

  {
    int i = 0;
    for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin();
        outEdgeIt != outEdges.end(); ++outEdgeIt )
    {
      i++;

    }

    if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
      std::cout << "We have output " << i << std::endl;
  }

  for (list<pair<SgGraphNode *,int> >::iterator it=cgNodes.begin();it!=cgNodes.end();it++)
  {

    //get list over the end-points for which this node points to
    list<SgGraphNode*> calledNodes;

    for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin();
        outEdgeIt != outEdges.end(); ++outEdgeIt )
    {

      if(outEdgeIt->first == it->second)
      {
        SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
        ROSE_ASSERT(graphEdge!=NULL);
        calledNodes.push_back(graphEdge->get_to());
      }
    }

    calledNodes.sort(nodeCompareGraph);
    calledNodes.unique();

    //Output the unique graph
    SgFunctionDeclaration* cur_function = isSgFunctionDeclaration((it->first)->get_SgNode());

    if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
      std::cout << "Node " << cur_function << " has " << calledNodes.size() << " calls to it." << std::endl;
    
    if (calledNodes.size()==0)
    {
      /*
         std::cout << "First node of this type" << std::endl;
         if((*i)->functionDeclaration != NULL && (*i) != NULL )
       */

      {
        ROSE_ASSERT( cur_function != NULL );
        file << stripGlobalModifer(cur_function->get_qualified_name().getString()) <<" ->";
      }

    }
    else
    {
      //  std::cout << "Second First node of this type " << (*i)->properties->nid << " " << (*i)->properties->functionName <<std::endl;
      //std::cout << "Second First node of this type " << (*i)->properties->nid << " " <<(*i)->properties->label << " " << (*i)->properties->type << " " << (*i)->properties->scope << " " << (*i)->properties->functionName << std::endl;

      {
        ROSE_ASSERT( cur_function != NULL );
        file << stripGlobalModifer(cur_function->get_qualified_name().getString()) <<" ->";
      }

      for (list<SgGraphNode *>::iterator j=calledNodes.begin();j!=calledNodes.end();j++)
      {
        SgFunctionDeclaration* j_function = isSgFunctionDeclaration((*j)->get_SgNode());

          file << " " << stripGlobalModifer( j_function->get_qualified_name().getString() );

      }		
    }
    file << endl;


  }

  file.close();

};

#if 0
// DQ (8/15/2009): This version does not handle files outside of the ROSE source tree.
struct OnlyCurrentDirectory : public std::unary_function<bool,SgFunctionDeclaration*>
{
  bool operator() (SgFunctionDeclaration* node) const
  {
    std::string stringToFilter = ROSE_COMPILE_TREE_PATH+std::string("/tests"); 
    std::string srcDir = ROSE_AUTOMAKE_TOP_SRCDIR; 

    if(string(node->get_file_info()->get_filename()).substr(0,stringToFilter.size()) == stringToFilter  )
      return true;
    else if( string(node->get_file_info()->get_filename()).substr(0,srcDir.size()) == srcDir  )
      return true;
    else
      return false;
  };
}; 

#else

// DQ (8/15/2009): This version handles an explicitly set (hard coded for now) secondary directory location.
// DQ (8/16/2009): This is part of an include mechanism (everything else is deleted).
struct OnlyCurrentDirectory : public std::unary_function<bool,SgFunctionDeclaration*>
   {
     bool operator() (SgFunctionDeclaration* node) const
        {
          std::string stringToFilter = ROSE_COMPILE_TREE_PATH + std::string("/tests");
          std::string srcDir = ROSE_AUTOMAKE_TOP_SRCDIR  + std::string("/tests") ;
#if 1
       // Hard code this for initial testing on target exercise.
          std::string secondaryTestSrcDir =  ROSE_AUTOMAKE_TOP_SRCDIR+std::string("/developersScratchSpace");
#endif

#if 0
          printf ("stringToFilter = %s \n",stringToFilter.c_str());
          printf ("srcDir         = %s \n",srcDir.c_str());
#endif
          string sourceFilename = node->get_file_info()->get_filename();
          string sourceFilenameSubstring = sourceFilename.substr(0,stringToFilter.size());
          string sourceFilenameSrcdirSubstring = sourceFilename.substr(0,srcDir.size());
          string sourceFilenameSecondaryTestSrcdirSubstring = sourceFilename.substr(0,secondaryTestSrcDir.size());
#if 0
          printf ("sourceFilename                = %s \n",sourceFilename.c_str());
          printf ("sourceFilenameSubstring       = %s \n",sourceFilenameSubstring.c_str());
          printf ("sourceFilenameSrcdirSubstring = %s \n",sourceFilenameSrcdirSubstring.c_str());
#endif
       // if (string(node->get_file_info()->get_filename()).substr(0,stringToFilter.size()) == stringToFilter  )
          if (sourceFilenameSubstring == stringToFilter)
               return true;
            else 
            // if ( string(node->get_file_info()->get_filename()).substr(0,srcDir.size()) == srcDir )
               if (sourceFilenameSrcdirSubstring == srcDir)
                    return true;
                 else
                    if (sourceFilenameSecondaryTestSrcdirSubstring == secondaryTestSrcDir)
                         return true;
                      else
                         return false;
       }
   };
#endif


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

  std::cout << "Outputing to DB:" << dbName << std::endl;

#endif

  // Build the AST used by ROSE

  // generate compareisonFile

  std::string graphCompareOutput = "";
  CommandlineProcessing::isOptionWithParameter(argvList,"-compare:","(graph)",graphCompareOutput,true);

  CommandlineProcessing::removeArgsWithParameters(argvList,"-compare:");

  SgProject * project = frontend (argvList);
  ROSE_ASSERT (project != NULL);

  printf ("graphCompareOutput = %s \n",graphCompareOutput.c_str());

  if(graphCompareOutput == "" )
    graphCompareOutput=((project->get_outputFileName())+".cg.dmp");

  // Build the callgraph according to Anreases example
  CallGraphBuilder		cgb (project);

#if 1
// Filtered call graph
  cgb.buildCallGraph( OnlyCurrentDirectory() );
#else
// Unfiltered call graph
  cgb.buildCallGraph();
#endif
  // cgb.classifyCallGraph();
  // write a dotfile vor visualisation

  outFileName=((project->get_outputFileName())+".dot");

  cout << "Writing Callgraph to: "<<outFileName<<endl;
  cout << "Writing custom compare to: "<<graphCompareOutput<<endl;

  //CallGraphDotOutput output( *(cgb.getGraph()) );

  SgIncidenceDirectedGraph *newGraph;

  if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
  {
#ifdef HAVE_SQLITE3
    writeSubgraphToDB(*gDB, cgb.getGraph() );

    solveVirtualFunctions( *gDB, "ClassHierarchy" );
    solveFunctionPointers( *gDB );

    cout << "Loading from DB...\n";
    newGraph = loadCallGraphFromDB(*gDB);
    cout << "Loaded\n";


#endif
  }else{
    // Not SQL Database case
    newGraph = cgb.getGraph();
    printf ("Not using the SQLite Database ... \n");
  }


  sortedCallGraphDump(graphCompareOutput,newGraph);	

  AstDOTGeneration dotgen;

  dotgen.writeIncidenceGraphToDOTFile(newGraph, outFileName.c_str());

//  OutputDot::writeToDOTFile(newGraph, outFileName.c_str(), "Call Graph");
  //GenerateDotGraph (newGraph,outFileName.c_str());



  return 0;
}
