#include "sage3basic.h"
#include "intermediateRepresentationNodeGraphs.h"

using namespace std;
using namespace Rose;

void
intermediateRepresentationNodeGraph(SgProject* project, const std::vector<VariantT> & nodeKindList)
   {
  // Build filename...
     string filename = "nodeGraph";
     string dot_header = filename;
     filename += ".dot";

     printf ("In intermediateRepresentationNodeGraph(): filename = %s \n",filename.c_str());

  // Open file...
     ofstream file;
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

     IntermediateRepresentationNodeGraph t(file,project,nodeKindList);
     t.traverseMemoryPool();

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();
   }

IntermediateRepresentationNodeGraph::IntermediateRepresentationNodeGraph(ofstream& inputFile, SgProject* project, const std::vector<VariantT>& inputNodeKindList)
   : file(inputFile), 
     nodeKindList(inputNodeKindList)
   {
     for (size_t i = 0; i < nodeKindList.size(); i++)
        {
          printf ("Adding nodeKindList[%" PRIuPTR "] = %d = %s to nodeKindSet \n",i,Cxx_GrammarTerminalNames[nodeKindList[i]].variant,Cxx_GrammarTerminalNames[nodeKindList[i]].name.c_str());
          include_nodeKindSet.insert(nodeKindList[i]);
        }

  // Build a list of functions within the AST
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgStatement);

     int maxNumberOfNodes = 5000;
     int numberOfNodes = (int)functionDeclarationList.size();
     printf ("Number of IR nodes in IntermediateRepresentationNodeGraph = %d \n",numberOfNodes);

     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
          SgStatement*            statement           = isSgStatement(*i);
          SgTemplateInstantiationFunctionDecl*  templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(*i);

          if (statement != NULL && statement->get_file_info()->isCompilerGenerated() == false)
             {
               include_nodeSet.insert(statement);
             }
          if (templateInstantiationFunctionDeclaration != NULL)
             {
               include_nodeSet.insert(statement);
             }
        }

     if (numberOfNodes <= maxNumberOfNodes)
        {
       // Ouput nodes.
          for (std::set<SgNode*>::iterator i = include_nodeSet.begin(); i != include_nodeSet.end(); i++)
             {
               SgNode* node = *i;
               file << "\"" << StringUtility::numberToString(node) << "\"[" << "label=\"" << node->class_name() << "\\n" << StringUtility::numberToString(node) << "\"];" << endl;
             }

       // Output edges
          for (std::set<SgNode*>::iterator i = include_nodeSet.begin(); i != include_nodeSet.end(); i++)
             {
               SgNode* node = *i;

               std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
               std::vector<std::pair<SgNode*,std::string> >::iterator j = listOfIRnodes.begin();
               while (j != listOfIRnodes.end())
                  {
                    if (include_nodeSet.find(j->first) != include_nodeSet.end())
                       {
                         file << "\"" << StringUtility::numberToString(node) << "\" -> \"" << StringUtility::numberToString(j->first) << "\"[label=\"" << j->second << "\"];" << endl;
                       }

                    j++;
                  }
             }
        }
       else
        {
          printf ("WARNING: IntermediateRepresentationNodeGraph is too large to generate: numberOfNodes = %d (max size = %d) \n",numberOfNodes,maxNumberOfNodes);
        }
   }

void
IntermediateRepresentationNodeGraph::visit(SgNode* /*node*/) {
}
