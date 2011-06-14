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

    bool operator() (SgFunctionDeclaration * node) const
    {
        std::string stringToFilter = ROSE_COMPILE_TREE_PATH + std::string("/tests");
        std::string srcDir = ROSE_AUTOMAKE_TOP_SRCDIR + std::string("/tests");

        // Hard code this for initial testing on target exercise.
        std::string secondaryTestSrcDir = ROSE_AUTOMAKE_TOP_SRCDIR + std::string("/developersScratchSpace");

        string sourceFilename = node->get_file_info()->get_filename();
        string sourceFilenameSubstring = sourceFilename.substr(0, stringToFilter.size());
        string sourceFilenameSrcdirSubstring = sourceFilename.substr(0, srcDir.size());
        string sourceFilenameSecondaryTestSrcdirSubstring = sourceFilename.substr(0, secondaryTestSrcDir.size());

        if (sourceFilenameSubstring == stringToFilter)
            return true;
        else if (sourceFilenameSrcdirSubstring == srcDir)
            return true;
        else
            if (sourceFilenameSecondaryTestSrcdirSubstring == secondaryTestSrcDir)
            return true;
        else
            return false;
    }
};


int main(int argc, char **argv)
{
    std::vector<std::string> argvList(argv, argv + argc);

    //Read the comparison file
    std::string graphCompareOutput = "";
    CommandlineProcessing::isOptionWithParameter(argvList, "-compare:", "(graph)", graphCompareOutput, true);
    CommandlineProcessing::removeArgsWithParameters(argvList, "-compare:");
    
    //Run frontend
    SgProject* project = frontend(argvList);
    ROSE_ASSERT(project != NULL);

    // Build the callgraph 
    CallGraphBuilder cgb(project);
    cgb.buildCallGraph(OnlyCurrentDirectory());


     if (graphCompareOutput == "")
        graphCompareOutput = ((project->get_outputFileName()) + ".cg.dmp");

    cout << "Writing custom compare to: " << graphCompareOutput << endl;

    SgIncidenceDirectedGraph *newGraph = cgb.getGraph();
    sortedCallGraphDump(graphCompareOutput, newGraph);

    return 0;
}
