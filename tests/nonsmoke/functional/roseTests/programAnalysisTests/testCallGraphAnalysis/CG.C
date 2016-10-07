// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.

// DQ (8/26/2016): Added support for the call graph generation for template functions in classes 
// that are moved by EDG outside of the class and for which we previously only captured the 
// function prototype and for which the new fix adds the function definitions to the AST.
// Note that this cased the results to change and for many of the save correct answer to be
// updated to reflect the improved AST.  As a result of studing the results, the current
// call graph computation has a number of observed limitations:
//    1) SgConstructorInitializers are not included as function calls
//    2) implicit base class constructor calls are not handled.
// Note that these were acceptable limitations for the time when the original call graph support 
// was written.  However, we will be replacing this call graph with a future on based on a subset 
// of the interprocedural control flow graph.


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

using namespace std;

std::string stripGlobalModifer(std::string str)
{
    if (str.substr(0, 2) == "::")
        str = str.substr(2);

    return str;

};

bool nodeCompareGraph(const SgGraphNode* a, const SgGraphNode* b)
{
    SgFunctionDeclaration* funcDecl1 = isSgFunctionDeclaration(a->get_SgNode());
    ROSE_ASSERT(funcDecl1 != NULL);

    SgFunctionDeclaration* funcDecl2 = isSgFunctionDeclaration(b->get_SgNode());
    ROSE_ASSERT(funcDecl2 != NULL);

    return stripGlobalModifer(funcDecl1->get_qualified_name().getString())
            < stripGlobalModifer(funcDecl2->get_qualified_name().getString());
}

bool nodeCompareGraphPair(const std::pair<SgGraphNode*, int>& a, const std::pair<SgGraphNode*, int>& b)
{
    ROSE_ASSERT( (a.first != b.first) || (a.second == b.second));
    return nodeCompareGraph(a.first, b.first);
}

void sortedCallGraphDump(string fileName, SgIncidenceDirectedGraph* cg)
{
    //Opening output file
    ofstream file;
    file.open(fileName.c_str());

    //Get all nodes of the current CallGraph
    list<pair<SgGraphNode*, int> > cgNodes;

    rose_graph_integer_node_hash_map & nodes = cg->get_node_index_to_node_map();

    for (rose_graph_integer_node_hash_map::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        cgNodes.push_back(pair<SgGraphNode*, int>(it->second, it->first));
    }

    cgNodes.sort(nodeCompareGraphPair);
    cgNodes.unique();

    for (list<pair<SgGraphNode *, int> >::iterator it = cgNodes.begin(); it != cgNodes.end(); it++)
    {
        //get list over the end-points for which this node points to
        list<SgGraphNode*> calledNodes;
        rose_graph_integer_edge_hash_multimap & outEdges = cg->get_node_index_to_edge_multimap_edgesOut ();
        for (rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin();
                outEdgeIt != outEdges.end(); ++outEdgeIt)
        {

            if (outEdgeIt->first == it->second)
            {
                SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
                ROSE_ASSERT(graphEdge != NULL);
                calledNodes.push_back(graphEdge->get_to());
            }
        }

        calledNodes.sort(nodeCompareGraph);
        calledNodes.unique();

        //Output the unique graph
        SgFunctionDeclaration* cur_function = isSgFunctionDeclaration((it->first)->get_SgNode());

        if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
            std::cout << "Node " << cur_function << " has " << calledNodes.size() << " calls to it." << std::endl;

        if (calledNodes.size() == 0)
        {
            {
                ROSE_ASSERT(cur_function != NULL);
                file << stripGlobalModifer(cur_function->get_qualified_name().getString()) << " ->";
            }

        }
        else
        {
            {
                ROSE_ASSERT(cur_function != NULL);
                file << stripGlobalModifer(cur_function->get_qualified_name().getString()) << " ->";
            }

            for (list<SgGraphNode *>::iterator j = calledNodes.begin(); j != calledNodes.end(); j++)
            {
                SgFunctionDeclaration* j_function = isSgFunctionDeclaration((*j)->get_SgNode());

                file << " " << stripGlobalModifer(j_function->get_qualified_name().getString());

            }
        }
        file << endl;


    }

    file.close();

};


struct OnlyCurrentDirectory : public std::unary_function<bool, SgFunctionDeclaration*>
{
    static size_t nselected;

#define DEBUG_SELECTOR 0

    bool operator() (SgFunctionDeclaration * node) const
      {
        ROSE_ASSERT(node != NULL);

        // build tree, ROSE_COMPILE_TREE_PATH is the output of  `pwd -P`, which means symbolic links are resolved already.
        std::string stringToFilter = ROSE_COMPILE_TREE_PATH + std::string("/tests"); 
        // Liao 6/20/2011, we have to use the same source path without symbolic links to have the right match
        std::string srcDir = ROSE_SOURCE_TREE_PATH + std::string("/tests");  //source tree
        //std::string srcDir = ROSE_AUTOMAKE_TOP_SRCDIR + std::string("/tests");  //source tree
        //
        // Hard code this for initial testing on target exercise.
        std::string secondaryTestSrcDir = ROSE_AUTOMAKE_TOP_SRCDIR + std::string("/developersScratchSpace");

        // Not all SgFunctionDeclaration's come from a file.  If ROSE only ever encountered a defining declaration in the input
        // then it will create a compiler-generated non-defining declaration that is not associated with any file.  The call
        // graph layer always uses the first non-defining declaration, thus we won't always see a valid file, in which case we
        // need to get the file name from the defining declaration.
        string sourceFilename = node->get_file_info()->get_filename();
        if (sourceFilename.empty() || 0==sourceFilename.compare("NULL_FILE")) 
           {
             SgFunctionDeclaration *defdecl = isSgFunctionDeclaration(node->get_definingDeclaration());
             if (defdecl)
                 sourceFilename = defdecl->get_file_info()->get_filename();
           }

        string sourceFilenameSubstring = sourceFilename.substr(0, stringToFilter.size()); // if the file is from the build tree?
        string sourceFilenameSrcdirSubstring = sourceFilename.substr(0, srcDir.size());  // or from the ROSE source tree?
        // or from the developer scratch space?
        string sourceFilenameSecondaryTestSrcdirSubstring = sourceFilename.substr(0, secondaryTestSrcDir.size());

#if DEBUG_SELECTOR
        printf ("test 1 \n");
        printf ("In select: sourceFilenameSubstring = %s \n",sourceFilenameSubstring.c_str());
        printf ("   --- stringToFilter = %s \n",stringToFilter.c_str());
#endif

        bool retval = false;
        if (sourceFilenameSubstring == stringToFilter)
           {
             retval = true;
           } 
          else
           {
#if DEBUG_SELECTOR
             printf ("test 2 \n");
             printf ("   --- sourceFilenameSrcdirSubstring = %s \n",sourceFilenameSrcdirSubstring.c_str());
             printf ("   --- srcDir = %s \n",srcDir.c_str());
#endif
             if (sourceFilenameSrcdirSubstring == srcDir)
                {
                  retval = true;
                } 
               else
                {
#if DEBUG_SELECTOR
                  printf ("test 3 \n");
                  printf ("   --- sourceFilenameSecondaryTestSrcdirSubstring = %s \n",sourceFilenameSecondaryTestSrcdirSubstring.c_str());
                  printf ("   --- secondaryTestSrcDir = %s \n",secondaryTestSrcDir.c_str());
#endif
                  if (sourceFilenameSecondaryTestSrcdirSubstring == secondaryTestSrcDir)
                     {
                       retval = true;
                     }
                    else
                     {
                    // DQ (9/1/2016): Test if this is a template instantiation from a template that would be true using our filter.
                       retval =  SageInterface::isTemplateInstantiationFromTemplateDeclarationSatisfyingFilter(node,this);
                     }
                }
           }

        if (retval) 
           {
#if DEBUG_SELECTOR
             std::cerr <<"OnlyCurrentDirectory is selecting node " << node
                       <<" \"" <<node->get_qualified_name().getString() <<"\"\n";
#endif
             ++nselected;
           }

        return retval;
    }
};

size_t OnlyCurrentDirectory::nselected = 0;


int main(int argc, char **argv)
{
  /*
    std::cerr <<"*******************************************************************************************************\n"
              <<"*** BIG FAT WARNING:  This test, " <<argv[0] <<",\n"
              <<"*** only works if the input files are in the right place in the ROSE source tree!  If they are not\n"
              <<"*** in the correct location the test will pass without doing anything.  Look for output lines that\n"
              <<"*** begin with \"OnlyCurrentDirectory is selecting node\" (there should be at least one).\n"
              <<"*******************************************************************************************************\n";
  */
    std::vector<std::string> argvList(argv, argv + argc);

    //Read the comparison file
    std::string graphCompareOutput = "";
    CommandlineProcessing::isOptionWithParameter(argvList, "-compare:", "(graph)", graphCompareOutput, true);
    CommandlineProcessing::removeArgsWithParameters(argvList, "-compare:");
    
    //Run frontend
    SgProject* project = frontend(argvList);
    ROSE_ASSERT(project != NULL);

#if 0
     AstDOTGeneration astdotgen;
  // SgProject & nonconstProject = (SgProject &) project;
     std::string filenamePostfix;
  // astdotgen.generateInputFiles(project,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
     SgFile* file = project->get_files()[0];
     ROSE_ASSERT(file != NULL);
     astdotgen.generateWithinFile(file,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
#endif

#if 0
  // DEBUGGING: Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

    // Build the callgraph 
    CallGraphBuilder cgb(project);
    OnlyCurrentDirectory selector;
    cgb.buildCallGraph(selector);
    if (0==selector.nselected) {
      std::cerr <<"Error: Test did not detect any function call. All tests contain at least one function call."<<std::endl;
      exit(1);
    }

    if (graphCompareOutput == "")
       graphCompareOutput = ((project->get_outputFileName()) + ".cg.dmp");

    cout << "Writing custom compare to: " << graphCompareOutput << endl;

    SgIncidenceDirectedGraph *newGraph = cgb.getGraph();
    sortedCallGraphDump(graphCompareOutput, newGraph);

    return 0;
}
