// Example ROSE Preprocessor
// used for testing ROSE infrastructure
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astGraph.h"
using namespace std;



//See header file for comment
void AST_Graph::writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling graphNullPointers){
     defaultFilterBinary* x2 = new defaultFilterBinary();
     defaultFilterUnary*  x1 = new defaultFilterUnary();
     AST_Graph::DataMemberPointersToIR<defaultFilterUnary, defaultFilterBinary> graph(*x1,*x2, memory_pool_traversal, graphNullPointers );
     graph.writeToFileAsGraph(filename);
}

//See header file for comment
void AST_Graph::writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling graphNullPointers, int depth){
     defaultFilterBinary* x2 = new defaultFilterBinary();
     defaultFilterUnary*  x1 = new defaultFilterUnary();

     AST_Graph::DataMemberPointersToIR<defaultFilterUnary, defaultFilterBinary> graph( *x1,*x2,AST_Graph::whole_graph_AST,graphNullPointers);
     graph.generateGraph(node,depth); 
     graph.writeToFileAsGraph(filename);
}
AST_Graph::defaultFilterUnary::result_type
AST_Graph::defaultFilterUnary::operator()(defaultFilterUnary::argument_type nodeFilter){
   result_type defaultReturn;
   defaultReturn.addToGraph = true;
   defaultReturn.DOTOptions = "";
   if (nodeFilter.first != NULL)
      defaultReturn.DOTLabel   = nodeFilter.first->class_name();
   return defaultReturn;
};

AST_Graph::defaultFilterBinary::result_type
AST_Graph::defaultFilterBinary::operator()(defaultFilterBinary::first_argument_type nodeFilter,
                                           defaultFilterBinary::second_argument_type edgeFilter){
   result_type defaultReturn;
   defaultReturn.addToGraph = true;
   defaultReturn.DOTOptions = "";
   defaultReturn.DOTLabel   = "";

   return defaultReturn;
};

AST_Graph::nodePartOfGraph::result_type
AST_Graph::nodePartOfGraph::operator()(pair<SgNode*,std::string>& x)
   {
     result_type functionalReturn;
     functionalReturn.addToGraph = true;
     functionalReturn.DOTOptions = "";
     functionalReturn.DOTLabel      = "";
     
     SgLocatedNode* locatedNode = isSgLocatedNode(x.first);
     if(locatedNode!=NULL)
        {
          Sg_File_Info* fileInfo = locatedNode->get_file_info();
          std::string filename(rose::utility_stripPathFromFileName(fileInfo->get_filename()));

          if (filename.find("rose_edg_required_macros_and_functions.h") != std::string::npos)
             {
               functionalReturn.addToGraph = false;
             }

          if (fileInfo->isCompilerGenerated()==true)
             {
            // std::cout << "Is compiler generated\n";
               functionalReturn.addToGraph = false;
             }
        }
           
     return functionalReturn;
   }

AST_Graph::filterSgFileInfo::result_type
AST_Graph::filterSgFileInfo::operator()(pair<SgNode*,std::string>& x)
   {
     result_type functionalReturn;
     functionalReturn.addToGraph = true;
     functionalReturn.DOTOptions = "";
     functionalReturn.DOTLabel      = "";

     
     if (isSg_File_Info(x.first) != NULL)
          functionalReturn.addToGraph = false;

     return functionalReturn;
   }

AST_Graph::filterSgFileInfo::result_type
AST_Graph::filterSgFileInfoAndGnuCompatabilityNode::operator()(pair<SgNode*,std::string>& x)
   {
     result_type functionalReturn;
     functionalReturn.addToGraph = true;
     functionalReturn.DOTOptions = "";
     functionalReturn.DOTLabel      = "";

     filterSgFileInfo value1;
     nodePartOfGraph  value2;
  // bool returnValue = (filterSgFileInfo)(x) && (nodePartOfGraph)(x);
  // bool returnValue = (filterSgFileInfo).(x) && (nodePartOfGraph).(x);
     functionalReturn.addToGraph = (value1(x).addToGraph && value2(x).addToGraph);
     
     return functionalReturn;
   }

