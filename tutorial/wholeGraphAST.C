/****************************************************************************************
 * This example explains how to generate a DOT graph using the
 *      - whole AST traversal
 *      - memory pool traversal
 * In order to reduce the size of the graphs it is possible to filter on both
 * nodes and edges. Coloring of nodes and edges is also possible.
 ****************************************************************************************/

#include "rose.h"

using namespace std;

// This functor is derived from the STL functor mechanism

/*******************************************************************************************************
 * The unary functional
 *     struct filterOnNodes
 * returns an AST_Graph::FunctionalReturnType and takes a std::pair<SgNode*,std::string> as a paramater.
 * The pair represents a variable of type std::string and a variablename. 
 * The type AST_Graph::FunctionalReturnType contains the variables
 *      * addToGraph : if false do not graph node or edge, else graph
 *      * DOTOptions : a std::string which sets the color of the node etc.
 *
 * PS!!! The std::string is currently not set to anything useful. DO NOT USE THE STRING.
 *       Maybe it does not make sence in this case and should be removed.
 ******************************************************************************************************/
struct filterOnNodes: public std::unary_function< pair< SgNode*, std::string>,AST_Graph::FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( argument_type x ) const;
   };


//The argument to the function is 
filterOnNodes::result_type
filterOnNodes::operator()(filterOnNodes::argument_type x) const
   {
     AST_Graph::FunctionalReturnType returnValue;
     //Determine if the node is to be added to the graph. true=yes
     returnValue.addToGraph = true;
     //set colors etc. for the graph Node
     returnValue.DOTOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at\\N\",sides=4,peripheries=1,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";

     if( isSgProject(x.first) != NULL )
        returnValue.DOTOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at\\N\",sides=4,peripheries=1,color=\"Blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";



     //Filter out SgSymbols from the graph
     if ( isSgSymbol(x.first) != NULL )
          returnValue.addToGraph = false;

     if ( isSgType(x.first) != NULL )
          returnValue.addToGraph = false;

     //Filter out compiler generated nodes
     SgLocatedNode* locatedNode = isSgLocatedNode(x.first);
     if ( locatedNode != NULL )
        {
          Sg_File_Info* fileInfo = locatedNode->get_file_info();
          std::string filename(rose::utility_stripPathFromFileName(fileInfo->get_filename()));



          if (filename.find("rose_edg_macros_and_functions_required_for_gnu.h") != std::string::npos)
             {
               returnValue.addToGraph = false;
             }

          if (fileInfo->isCompilerGenerated()==true)
             {
            // std::cout << "Is compiler generated\n";
               returnValue.addToGraph = false;
             }
        }

     return returnValue;
   }


/*******************************************************************************************************************
 * The binary functional
 *       struct filterOnEdges
 * returns an AST_Graph::FunctionalReturnType and takes a node and a std::pair<SgNode*,std::string> as a paramater.
 * The paramater represents an edge 'SgNode*'->'std::pair<>' where the pair is a variable declaration with name 
 * 'std::string' within the node in the first paramater. 
 *
 * The type AST_Graph::FunctionalReturnType contains the variables
 *      * addToGraph : if false do not graph node or edge, else graph
 *      * DOTOptions : a std::string which sets the color of the node etc.
 *******************************************************************************************************************/
struct filterOnEdges: public std::binary_function< SgNode*,pair< SgNode*, std::string>,AST_Graph::FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( SgNode*,AST_Graph::NodeType & x ) const;
   };


//The edge goes from 'node'->'x'
filterOnEdges::result_type
filterOnEdges::operator()(SgNode* node, pair<SgNode*,std::string>& x) const
   {
     AST_Graph::FunctionalReturnType returnValue;
     //Determine if the edge is to be added to the graph. false=no
     returnValue.addToGraph = false;
     //set the color etc. for the edge
     returnValue.DOTOptions = "";


     return returnValue;
   }



int
main ( int argc, char* argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     //specify that NULL pointers should be represented in the DOT graph
     AST_Graph::pointerHandling pHandling = AST_Graph::graph_NULL;
     //specify how many times the recursive call to follow a pointer to
     //an object and then follow the pointers within that objects again etc.
     //should be performed
     int depth = 3;
     //graph the whole sub graph of project, but filter on both nodes and edges
     AST_Graph::writeGraphOfAstSubGraphToFile("wholeGraphAST1.dot",project,pHandling,filterOnNodes(),filterOnEdges(),depth);
     //graph the whole sub graph of project, but filter on nodes
     AST_Graph::writeGraphOfAstSubGraphToFile("wholeGraphAST2.dot",project,pHandling,filterOnNodes(),depth);
     //graph the whole sub graph of project (do not filter)
     AST_Graph::writeGraphOfAstSubGraphToFile("wholeGraphAST3.dot",project,pHandling,depth);
 
     //graph the memory pool, but filter on nodes and edges
     AST_Graph::writeGraphOfMemoryPoolToFile("wholeGraphASTPool1.dot",pHandling,filterOnNodes(),filterOnEdges());
     //graph the memory pool, but filter on nodes
     AST_Graph::writeGraphOfMemoryPoolToFile("wholeGraphASTPool2.dot",pHandling,filterOnNodes());
     //graph the memory pool (do not filter)
     AST_Graph::writeGraphOfMemoryPoolToFile("wholeGraphASTPool3.dot",pHandling);

     return 0;
   }
