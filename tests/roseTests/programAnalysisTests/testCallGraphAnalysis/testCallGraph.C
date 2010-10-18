// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (8/15/2009): This is a call graph test program that I have 
// modified from CG.C to generate tests more specific my own 
// requirements for testing.

#include "rose.h"
#include <GraphAccess.h>
#include <GraphUpdate.h>
#include <CallGraph.h>

using namespace std;

#if 0
bool filter ( SgNode *n )
   {
  // false will filter out ALL nodes
     bool filterNode = true;
#if 0
     SgFunctionDeclaration *fct = isSgFunctionDeclaration( n );
     if ( fct != NULL )
        {
          bool ignoreFunction = (fct->get_file_info()->isCompilerGenerated() == true);
          if ( ignoreFunction == true )
               filterNode = false;
        }
#endif
     return filterNode;
   }
#endif


std::string edgeLabel(SgDirectedGraphEdge* edge)
   {
     return "some label";
   };

std::string nodeLabel(SgGraphNode* node)
   {
     return "some label";
   };

struct OnlyCurrentDirectory : public std::unary_function<bool,SgFunctionDeclaration*>
   {
     bool operator() (SgFunctionDeclaration* node) const
        {
          std::string stringToFilter = ROSE_COMPILE_TREE_PATH + std::string("/tests");
          std::string srcDir = ROSE_AUTOMAKE_TOP_SRCDIR;

#if 0
          printf ("stringToFilter = %s \n",stringToFilter.c_str());
          printf ("srcDir         = %s \n",srcDir.c_str());
#endif
          string sourceFilename = node->get_file_info()->get_filename();
          string sourceFilenameSubstring = sourceFilename.substr(0,stringToFilter.size());
          string sourceFilenameSrcdirSubstring = sourceFilename.substr(0,srcDir.size());
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
                    return false;
       }
   };

struct OnlyNonCompilerGenerated : public std::unary_function<bool,SgFunctionDeclaration*>
   {
     bool operator() (SgFunctionDeclaration* node) const
        {
       // false will filter out ALL nodes
          bool filterNode = true;
#if 1
          SgFunctionDeclaration *fct = isSgFunctionDeclaration( node );
          if ( fct != NULL )
             {
               bool ignoreFunction = (fct->get_file_info()->isCompilerGenerated() == true);
               if ( ignoreFunction == true )
                    filterNode = false;
             }
#endif
          return filterNode;
        }
   };


class SecurityVulnerabilityAttribute : public AstAttribute
   {
     public:
          SgDirectedGraphEdge* edge;
          SgGraphNode* node;

          SecurityVulnerabilityAttribute(SgGraphNode*);
          SecurityVulnerabilityAttribute(SgDirectedGraphEdge*);
          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();
   };

SecurityVulnerabilityAttribute::SecurityVulnerabilityAttribute(SgGraphNode* n)
   : edge(NULL),node(n)
   {
   }

SecurityVulnerabilityAttribute::SecurityVulnerabilityAttribute(SgDirectedGraphEdge* e)
   : edge(e),node(NULL)
   {
   }

vector<AstAttribute::AttributeEdgeInfo>
SecurityVulnerabilityAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     if( edge != NULL )
        {
          string vulnerabilityName  = edge->get_name()  ;
          string vulnerabilityColor   = "green";
          string vulnerabilityOptions = " arrowsize=2.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

       // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
          AstAttribute::AttributeEdgeInfo vulnerabilityNode ( edge->get_from(),edge->get_to(), vulnerabilityName, vulnerabilityOptions);
          v.push_back(vulnerabilityNode);
        }

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SecurityVulnerabilityAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;
     if(node != NULL)
        {
          string vulnerabilityName    = node->get_name() ;
          string vulnerabilityColor   = "blue";
          string vulnerabilityOptions = " arrowsize=2.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

       // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
          AstAttribute::AttributeNodeInfo vulnerabilityNode ( node, vulnerabilityName, vulnerabilityOptions);
          v.push_back(vulnerabilityNode);
        }

     return v;
   }


int
main( int argc, char * argv[] )
   {
     RoseTestTranslator test;
  // r = test.translate(argc,argv); // returns an error if any of the ends fails
     std::vector<std::string> argvList(argv, argv+argc);
     std::string dbName = "";
     CommandlineProcessing::isOptionWithParameter(argvList,"-db:","(name)", dbName,true);

     bool var_SOLVE_FUNCTION_CALLS_IN_DB = false;
  // var_SOLVE_FUNCTION_CALLS_IN_DB = true;

#ifdef HAVE_SQLITE3
     var_SOLVE_FUNCTION_CALLS_IN_DB = true;
     std::cout << "Analyzing in DATABASE" << std::endl;

     if (dbName == "")
        {
          std::cerr << "Error: Please specify a database name with the -db:name option" << std::endl;
          exit(1);
        }

     sqlite3x::sqlite3_connection* gDB = open_db(dbName);
#else
     std::cout << "Analyzing outside DATABASE" << std::endl;
#endif

     var_SOLVE_FUNCTION_CALLS_IN_DB = false;

     SgProject* project = new SgProject(argvList);

     CallGraphBuilder CGBuilder( project);

#if 0
     CGBuilder.buildCallGraph();
#else
     CGBuilder.buildCallGraph( OnlyCurrentDirectory() );
  // CGBuilder.buildCallGraph( OnlyNonCompilerGenerated() );

  // CGBuilder.buildCallGraph( );
#endif

  // GenerateDotGraph(CGBuilder.getGraph(),"callgraph.dot");

     ClassHierarchyWrapper hier( project );

  // Use the information in the graph to output a dot file for the call graph

  // CallGraphDotOutput output( *(CGBuilder.getGraph()) );

  // TPS (01Dec2008): Enabled mysql and this fails.
  // seems like it is not supposed to be included
     SgIncidenceDirectedGraph *newGraph;
     if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
        {
#ifdef HAVE_SQLITE3
          writeSubgraphToDB( *gDB,  CGBuilder.getGraph() );

          hier.writeHierarchyToDB(*gDB);

          solveVirtualFunctions(*gDB, "ClassHierarchy" );
          solveFunctionPointers( *gDB );
          cout << "Loading from DB...\n";
          newGraph = loadCallGraphFromDB(*gDB);
          cout << "Loaded\n";
#endif
        }
       else
        {
       // Not SQL Database case
          printf ("Not using the SQLite Database ... \n");
          newGraph = CGBuilder.getGraph();

       // filterGraph(*newGraph,filterNodes());
        }

  // Generate a filename for this whole project (even if it has more than one file)
     string generatedProjectName = SageInterface::generateProjectName( project );

     printf ("generatedProjectName            = %s \n",generatedProjectName.c_str());
     printf ("project->get_outputFileName()   = %s \n",project->get_outputFileName().c_str());
     printf ("project->get_dataBaseFilename() = %s \n",project->get_dataBaseFilename().c_str());
 
  // DQ (7/12/2009): Modified to use string instead of ostringstream.
     string uncoloredFileName = generatedProjectName + "_callgraph.dot";
     string coloredFileName   = generatedProjectName + "_colored_callgraph.dot";

  // Example of how to generate a modified Call Graph
 //  OutputDot::writeToDOTFile(newGraph, coloredFileName.c_str(),"Incidence Graph", nodeLabel,edgeLabel );

  // This generated the dot file for the AST.
     generateDOT( *project );

  // This generates the dot file for the Call Graph
  // GenerateDotGraph(newGraph, uncoloredFileName.c_str());
   {
     AstDOTGeneration dotgen;
     dotgen.writeIncidenceGraphToDOTFile(newGraph, uncoloredFileName.c_str());
   }

  // Generate colored graph
     rose_graph_integer_node_hash_map & nodes =
     newGraph->get_node_index_to_node_map ();

     for( rose_graph_integer_node_hash_map::iterator it = nodes.begin(); it != nodes.end(); ++it )
        {
          SgGraphNode* node = it->second;
          AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(node);
          ROSE_ASSERT(newAttribute != NULL);

          node->addNewAttribute("SecurityVulnerabilityAttribute",newAttribute);
        }

#if 0
     rose_graph_integer_edge_hash_multimap & outEdges = newGraph->get_node_index_to_edge_multimap_edgesOut ();
     for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin(); outEdgeIt != outEdges.end(); ++outEdgeIt )
        {
          SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
          ROSE_ASSERT(graphEdge!=NULL);

          AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(graphEdge);
          ROSE_ASSERT(newAttribute != NULL);

          graphEdge->addNewAttribute("SecurityVulnerabilityAttribute",newAttribute);
        }
#endif

   {
     AstDOTGeneration dotgen;
     dotgen.writeIncidenceGraphToDOTFile(newGraph, coloredFileName.c_str());
   }

     cout << "Done with DOT\n";

     printf ("\nLeaving main program ... \n");

     return 0; // backend(project);
   }
