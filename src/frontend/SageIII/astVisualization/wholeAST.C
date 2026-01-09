#include "sage3basic.h"
#include "rose_config.h"

#include "transformationTracking.h"
#include "wholeAST.h"

// **********************************************************
// **********************************************************
//       Source code which can be more later
// **********************************************************
// **********************************************************

using namespace std;
using namespace Rose;

CustomAstDOTGenerationEdgeType::CustomAstDOTGenerationEdgeType (SgNode* n1, SgNode* n2, std::string label, std::string options)
   : start(n1), end(n2), labelString(label), optionString(options)
   {
  // Nothing to do!
   }

bool
CustomAstDOTGenerationEdgeType::operator!=(const CustomAstDOTGenerationEdgeType & edge) const
   {
     bool returnValue = (edge.start != start) || (edge.end != end) || (edge.labelString != labelString);

     printf ("In CustomAstDOTGenerationEdgeType::operator!=(): edge.start = %p start = %p edge.end = %p end = %p edge.labelString = %s labelString = %s returnValue = %s \n",
             edge.start,start,edge.end,end,edge.labelString.c_str(),labelString.c_str(),returnValue ? "true" : "false");

     return returnValue;
   }

bool
CustomAstDOTGenerationEdgeType::operator==(const CustomAstDOTGenerationEdgeType & edge) const
   {
     bool returnValue = (edge.start == start) && (edge.end == end) && (edge.labelString == labelString);

     printf ("In CustomAstDOTGenerationEdgeType::operator==(): edge.start = %p start = %p edge.end = %p end = %p edge.labelString = %s labelString = %s returnValue = %s \n",
             edge.start,start,edge.end,end,edge.labelString.c_str(),labelString.c_str(),returnValue ? "true" : "false");

     return returnValue;
   }

bool
CustomAstDOTGenerationEdgeType::operator< (const CustomAstDOTGenerationEdgeType & edge) const
   {
  // This function must only be consistant in how it implements the "<" operator.
  // Cast pointer to size_t variables and compare the size_t type variables directly.
     size_t edge_start_size_t = (size_t) edge.start;
     size_t start_size_t      = (size_t) start;
     size_t edge_end_size_t   = (size_t) edge.start;
     size_t end_size_t        = (size_t) start;

     bool returnValue = (edge_start_size_t < start_size_t) || 
                        ((edge_start_size_t == start_size_t) && (edge_end_size_t < end_size_t)) || 
                        ((edge_start_size_t == start_size_t) && (edge_end_size_t == end_size_t) && (edge.labelString < labelString) ); 

     return returnValue;
   }

CustomAstDOTGeneration::~CustomAstDOTGeneration()
   {
   }

void
CustomAstDOTGenerationData::addNode(NodeType n)
   {
     // Here and in following, void* variable used in comparison to avoid warning messages.
     // Apparently this code is not well-defined C++ as this can be NULL [Rasmussen, 2024.02.17].
     void* autological_undefined_compare{this};
     ASSERT_not_null(autological_undefined_compare);
     ASSERT_require(n.optionString.size() < 4000);
     nodeList.push_back(n);
   }

void
CustomAstDOTGenerationData::addEdge(EdgeType e)
   {
     void* autological_undefined_compare{this};
     ASSERT_not_null(autological_undefined_compare);
     edgeList.push_back(e);
   }

void 
CustomAstDOTGenerationData::addEdges()
   {
  // Add the local list of edges to the dot graph
     for (EdgeType edge : edgeList)
        {
          ASSERT_not_null(edge.start);
          ASSERT_not_null(edge.end);
          dotrep.addEdge(edge.start,edge.end,edge.optionString);
        }
   }

std::string
CustomAstDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).labelString;
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.begin();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          ASSERT_require(i->target == node);
          returnString = (*i).optionString;
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalEdgeInfo(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const EdgeType tmp (from,to,label,"");
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).labelString;
          printf ("Adding edge label info (%p) to (%p): returnString = %s \n",from,to,returnString.c_str());
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalEdgeOptions(SgNode*/*from*/, SgNode*/*to*/, std::string /*label*/)
   {
     std::string returnString;
     return returnString;
   }

string
CustomAstDOTGenerationData::unparseToCompleteStringForDOT(SgNode* astNode)
   {
  // Generate the std::string (pass a SgUnparse_Info object)
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code

     string outputString = globalUnparseToString(astNode,inputUnparseInfoPointer);
     string returnString = StringUtility::escapeNewlineAndDoubleQuoteCharacters(outputString);

     delete inputUnparseInfoPointer;
     return returnString;
   }

void
CustomAstDOTGenerationData::internalGenerateGraph( std::string filename, SgProject* project )
   {
     std::string label =  "CustomAstDOTGenerationData::internalGenerateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Required setup on base class
     init();

     traversal = AstDOTGeneration::TOPDOWNBOTTOMUP;

  // Build the DOT specific inherited attribute
     DOTInheritedAttribute ia;
     traverse(project,ia);

  // Put extra code here
     addEdges();

     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomAstDOTGeneration::addNode( NodeType n )
   {
     void* autological_undefined_compare{this};
     ASSERT_not_null(autological_undefined_compare);
     ASSERT_require(n.optionString.size() < 4000);
     DOTgraph.addNode(n);
   }

void
CustomAstDOTGeneration::addEdge( EdgeType e )
   {
     void* autological_undefined_compare{this};
     ASSERT_not_null(autological_undefined_compare);
     DOTgraph.addEdge(e);
   }

void
CustomAstDOTGeneration::internalGenerateGraph( std::string filename, SgProject* project )
   {
     void* autological_undefined_compare{this};
     ASSERT_not_null(autological_undefined_compare);
     DOTgraph.internalGenerateGraph(filename,project);
   }

CustomMemoryPoolDOTGenerationData::~CustomMemoryPoolDOTGenerationData() 
   {
   }

void
CustomMemoryPoolDOTGenerationData::internalGenerateGraph(std::string dotfilename)
   {
     std::string label =  "CustomMemoryPoolDOTGenerationData::internalGenerateGraph(" + dotfilename + "):";
     TimingPerformance timer (label);

     filename = dotfilename;

     traverseMemoryPool();
     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomMemoryPoolDOTGenerationData::addNode(NodeType n)
   {
     nodeList.push_back(n);
   }

void
CustomMemoryPoolDOTGenerationData::addEdge(EdgeType e)
   {
     edgeList.push_back(e);
   }

void
CustomMemoryPoolDOTGenerationData::skipNode(SgNode* n)
   {
  // This is the normal (non-debugging) mode.
     skipNodeList.insert(n);
   }

void
CustomMemoryPoolDOTGenerationData::skipEdge(EdgeType e)
   {
     if (skipEdgeSet.find(e) == skipEdgeSet.end())
        {
          skipEdgeSet.insert(e);
        }
       else
        {
          printf ("ERROR: In CustomMemoryPoolDOTGenerationData::skipEdge(): edge is already present in set \n");
          ASSERT_require(false);
        }

  // find() operator on edge set.
     EdgeType edge(e.start,e.end,e.labelString);
     ASSERT_require(skipEdgeSet.find(edge) != skipEdgeSet.end());
     ASSERT_require(! (skipEdgeSet.find(edge) == skipEdgeSet.end()) );

     std::set<EdgeType>::iterator element = skipEdgeSet.find(edge);

     ASSERT_require(e.start == edge.start);
     ASSERT_require(e.end == edge.end);
     ASSERT_require(e.labelString == edge.labelString);

     ASSERT_require(e.start == element->start);
     ASSERT_require(e.end == element->end);
     ASSERT_require(e.labelString == element->labelString);
   }

void 
CustomMemoryPoolDOTGenerationData::addEdges()
   {
     for (EdgeType edge : edgeList)
        {
          ASSERT_not_null(edge.start);
          ASSERT_not_null(edge.end);
        }
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end();

     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).labelString;
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // This virtual function adds options (after the node label) in the output DOT file
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end(); 
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).optionString;
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalEdgeInfo(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const EdgeType tmp (from,to,label);
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).labelString;
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function adds options (after the node label) in the output DOT file
     std::string returnString;

  // This builds a default set of mappings of edge colors and edge options to edges.
  // Color all edges that lead to a SgType (this is overwritten for the parent edges)
     SgType* type = isSgType(to);
     if (type != nullptr)
        {
          returnString = "color=\"gold1\" ";
        }

     if (from->get_parent() == to && label == "parent")
        {
          returnString = "color=\"blue\" ";
        }

     SgStatement* statement = isSgStatement(from);
     if (statement != nullptr)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to && label == "scope")
             {
               returnString = "color=\"green\" ";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != nullptr)
        {
          if (initializedName->get_scope() == to && label == "scope")
             {
               returnString = "color=\"green\" ";
             }
        }
     return returnString;
   }

void
CustomMemoryPoolDOTGenerationData::visit(SgNode* node)
   {
     ASSERT_not_null(node);
     bool ignoreThisIRnode = false;

  // If the node is to be skipped then set: ignoreThisIRnode = true
     if ( skipNodeList.find(node) != skipNodeList.end() )
          ignoreThisIRnode = true;

     if (visitedNodes.find(node) == visitedNodes.end() && ignoreThisIRnode == false )
        {
       // This node has not been visited previously (and should not be ignored)
          std::string nodeoption;
          std::string nodelabel = std::string("\\n") + typeid(*node).name();

       // These is both virtual function calls
          nodelabel  += additionalNodeInfo(node);
          nodeoption += additionalNodeOptions(node);

       // Permit filtering of IR nodes
          if ( skipNodeList.find(node) == skipNodeList.end() )
             {
               dotrep.addNode(node,nodelabel,nodeoption);
             }

       // Now call Andres's mechanism to get the list of ALL pointers to IR nodes
          std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
          std::vector<std::pair<SgNode*,std::string> >::iterator i = listOfIRnodes.begin();
          while (i != listOfIRnodes.end())
             {
            // Might we want to test if this is node previously ignored (a gnu compatability IR node)
            // and do something like include that IR node in the AST?
               SgNode* n             = (*i).first;
               std::string edgelabel = (*i).second;

               if ( n == nullptr)
                  {
                    if ( skipNodeList.find(node) == skipNodeList.end() )
                       {
                         EdgeType edge(node,n,edgelabel);

                      // DQ (11/26/2016): Could the logic is flipped here, should it be NOT EQUALS instead?
                         bool ignoreEdge = skipEdgeSet.find(edge) != skipEdgeSet.end();
                         if (ignoreEdge == false)
                            {
                              dotrep.addNullValue(node,"",edgelabel,"");
                            }
                       }
                  }
                 else
                  {
                 // Only build the edge if the node is not on the list of IR nodes to skip
                 // We can't use the visitedNodes list since just be cause we have visited
                 // the IR node does not imply that we should skip the edge (quite the opposent!)
                    if ( skipNodeList.find(n) == skipNodeList.end() )
                       {
                         bool ignoreEdge = false;
                         EdgeType edge(node,n,edgelabel);

                      // DQ (11/26/2016): Could the logic is flipped here, should it be NOT EQUALS instead?
                      // This appears to always be false.
                         ignoreEdge = skipEdgeSet.find(edge) != skipEdgeSet.end();
                         if (ignoreEdge == true)
                            {
                              EdgeSetType::iterator example_iterator = skipEdgeSet.find(edge);
                              ASSERT_require(edgelabel == example_iterator->labelString);
                            }

                         if (ignoreEdge == false)
                            {
                           // These are both virtual function calls
                              std::string additionalEdgeOption = additionalEdgeOptions(node,n,edgelabel);
                              std::string additionalEdgeLabel  = additionalEdgeInfo(node,n,edgelabel);

                              dotrep.addEdge ( node, edgelabel + additionalEdgeLabel, n, additionalEdgeOption + "dir=forward" );
                            }
                       }
                  }
               i++;
             }
        }
       else
        {
       // Ignoring this IR node which has previously been visited (or added by the user as a colored IR node)
        }


  // Add this node to the list of visited nodes so that we will not traverse it again 
  // This allows for it to have been placed into this list previously as a colored IR node
  // and thus prevents it from showing up twice.
     visitedNodes.insert(node);
   }

CustomMemoryPoolDOTGeneration::CustomMemoryPoolDOTGeneration(s_Filter_Flags* f /*= nullptr*/)
{
  internal_init (f);
}

void CustomMemoryPoolDOTGeneration::internal_init(s_Filter_Flags* f /*= nullptr*/)
{
   if (f == nullptr)
     filterFlags = new s_Filter_Flags();
   else
     filterFlags = f;
}

void
CustomMemoryPoolDOTGeneration::addNode( NodeType n )
   {
     DOTgraph.addNode(n);
   }

void
CustomMemoryPoolDOTGeneration::addEdge( EdgeType e )
   {
     DOTgraph.addEdge(e);
   }

void
CustomMemoryPoolDOTGeneration::skipNode(SgNode* n)
   {
     DOTgraph.skipNode(n);
   }

void
CustomMemoryPoolDOTGeneration::skipEdge(EdgeType e)
   {
     DOTgraph.skipEdge(e);
   }

void
CustomMemoryPoolDOTGeneration::internalGenerateGraph( std::string filename )
   {
     DOTgraph.internalGenerateGraph(filename);
   }

void
CustomMemoryPoolDOTGeneration::edgeFilter(SgNode* nodeSource, SgNode* nodeSink, std::string edgeName )
   {
  // This function skips the representation of edges
     if (edgeName == "parent")
        {
          ASSERT_not_null(nodeSource);
        }
       else
        {
          ASSERT_not_null(nodeSource);
          if (nodeSink == nullptr)
             {
            // Filter the null pointer edges.
               EdgeType edge(nodeSource,nodeSink,edgeName);
               skipEdge(edge);
             }
        }

  // DQ (1/24/2007): Added skipNodeList to permit filtering of IR nodes, if the source node is not present 
  // then we don't have an edge to skip, if the source node is present and the sink node is to be skipped, 
  // then skip the edge.
     if ( DOTgraph.skipNodeList.find(nodeSource) != DOTgraph.skipNodeList.end() )
        {
          if ( nodeSink != nullptr && DOTgraph.skipNodeList.find(nodeSink) != DOTgraph.skipNodeList.end() )
             {
               EdgeType edge(nodeSource,nodeSink,edgeName);
               skipEdge(edge);
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::typeFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgType(node) != nullptr)
        {
          skipNode(node);
        }

     if (isSgFunctionParameterTypeList(node) != nullptr)
        {
          skipNode(node);
        }

     if (isSgTypedefSeq(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::symbolFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgSymbol(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::asmFileFormatFilter(SgNode* node)
   {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
  // Support to skip output of binary file format in generation of AST visualization.
     if (isSgAsmExecutableFileFormat(node) != nullptr)
        {
          skipNode(node);
        }
#endif
   }

void
CustomMemoryPoolDOTGeneration::asmTypeFilter(SgNode* node)
   {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
  // Support to skip output of binary expression type information in generation of AST visualization.
     if (isSgAsmType(node) != nullptr)
        {
          skipNode(node);
        }
#endif
   }

void
CustomMemoryPoolDOTGeneration::emptySymbolTableFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgSymbolTable(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::emptyBasicBlockFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     SgBasicBlock* block = isSgBasicBlock(node);
     if (block != nullptr)
        {
          if (block->get_statements().empty() == true)
             {
               skipNode(node);
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::emptyFunctionParameterListFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     SgFunctionParameterList* parameterList = isSgFunctionParameterList(node);
     if (parameterList != nullptr)
        {
          if (parameterList->get_args().empty() == true)
             {
               skipNode(node);
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::expressionFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgExpression(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::variableDefinitionFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgVariableDefinition(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::variableDeclarationFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgVariableDeclaration(node) != nullptr)
        {
          skipNode(node);
        }

     if (isSgInitializedName(node) != nullptr)
        {
          skipNode(node);
        }

     if (isSgStorageModifier(node) != nullptr)
        {
          skipNode(node);
        }

     if (isSgFunctionParameterList(node) != nullptr)
        {
          skipNode(node);
        }

     variableDefinitionFilter(node);
   }

void
CustomMemoryPoolDOTGeneration::ctorInitializerListFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgCtorInitializerList(node) != nullptr)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::binaryExecutableFormatFilter(SgNode* /*node*/)
   {
  // This function skips the representation of specific kinds of IR nodes 
   }

void
CustomMemoryPoolDOTGeneration::commentAndDirectiveFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != nullptr)
        {
          AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();
          if (comments != nullptr)
             {
               for (auto comment : *comments)
                  {
                    ASSERT_not_null(comment);
                    ASSERT_not_null(comment->get_file_info());
                    skipNode(comment->get_file_info());
                  }
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::fileInfoFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     Sg_File_Info* fileInfoNode = isSg_File_Info(node);
     if (fileInfoNode != nullptr)
        {
          skipNode(fileInfoNode);
        }
   }

void
CustomMemoryPoolDOTGeneration::frontendCompatibilityFilter(SgNode* node)
   {
  // This filters out the gnu compatability IR nodes (which tend to confuse everyone!)
     ASSERT_not_null(node);

  // Mark all SgFunctionSymbol IR nodes that are associated with the a frontend specific function
     SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(node);
     if (functionSymbol != nullptr)
        {
          SgDeclarationStatement* declaration = functionSymbol->get_declaration();
          ASSERT_not_null(declaration);
          if (declaration->get_file_info() == nullptr)
             {
               printf ("Error: declaration->get_file_info() == NULL declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }
          ASSERT_not_null(declaration->get_file_info());
          if (declaration != nullptr && declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(functionSymbol);
             }
        }

     SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(node);
     if (typedefSymbol != nullptr)
        {
          SgDeclarationStatement* declaration = typedefSymbol->get_declaration();
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(typedefSymbol);
             }
        }

     SgVariableSymbol* variableSymbol = isSgVariableSymbol(node);
     if (variableSymbol != nullptr)
        {
          SgInitializedName* declaration = variableSymbol->get_declaration();
          ASSERT_not_null(declaration);
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(variableSymbol);
             }

          SgStorageModifier & storageModifier = declaration->get_storageModifier();
          skipNode(&storageModifier);
        }

     SgFunctionParameterList* functionParateterList = isSgFunctionParameterList(node);
     if (functionParateterList != nullptr)
        {
          SgInitializedNamePtrList::iterator i =        functionParateterList->get_args().begin();
          while (i != functionParateterList->get_args().end())
             {
               SgInitializedName* declaration = *i;
               ASSERT_not_null(declaration);
               if (declaration->get_file_info()->isFrontendSpecific() == true)
                  {
                    SgStorageModifier & storageModifier = declaration->get_storageModifier();
                    skipNode(&storageModifier);
                  }

               i++;
             }
        }

  // Skip the Sg_File_Info objects that are associated with frontend specific IR nodes.
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != nullptr)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
               printf ("skip fileInfo = %p parent is %p = %s \n",fileInfo,node,node->class_name().c_str());
               skipNode(node);
             }
        }

  // Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != nullptr)
        {
       // skipNode(currentNodeFileInfo);
          if (currentNodeFileInfo->isFrontendSpecific() == true)
             {
               skipNode(currentNodeFileInfo);
             }
        }

     if (isSgModifier(node) != nullptr)
        {
       // skipNode(node);
        }

     if (isSgTypedefSeq(node) != nullptr)
        {
          SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
          if (typedefSeq->get_typedefs().empty() == true)
             {
               skipNode(node);
             }
        }

     if (isSgFunctionParameterTypeList(node) != nullptr)
        {
       // skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::defaultFilter(SgNode* node)
   {
  // Default filter to simplify the whole AST graph
     if (filterFlags->m_commentAndDirective == 1)
          commentAndDirectiveFilter(node);

     if (filterFlags->m_binaryExecutableFormat == 1)
          binaryExecutableFormatFilter(node);

     if (filterFlags->m_edge == 1)
        {
          std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
          std::vector<std::pair<SgNode*,std::string> >::iterator i = listOfIRnodes.begin();
          while (i != listOfIRnodes.end())
             {
            // Might we want to test if this is node previously ignored (a gnu compatibility IR node)
            // and do something like include that IR node in the AST?
               SgNode* n             = (*i).first;
               std::string edgelabel = (*i).second;

            // Run all the edges through the filter (the filter will single out the ones to be skipped)
               edgeFilter(node,n,edgelabel);
  
               i++;
             }
        }

  // DQ (1/9/2017): Add this here to test elimination of empty typedef sequences.
     if (isSgTypedefSeq(node) != nullptr)
        {
          SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
          if (typedefSeq->get_typedefs().empty() == true)
             {
               skipNode(node);
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::defaultColorFilter(SgNode* node)
   {
     ASSERT_not_null(node);

     SgStatement* statement = isSgStatement(node);
     if (statement != nullptr)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(statement->variantT())
             {
               case V_SgTemplateFunctionDeclaration:
               case V_SgTemplateMemberFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=blueviolet,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
                    break;
                  }

               case V_SgFunctionDeclaration:
               case V_SgProgramHeaderStatement:
               case V_SgProcedureHeaderStatement:
               case V_SgMemberFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=royalblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateInstantiationFunctionDecl: // functions
               case V_SgTemplateInstantiationMemberFunctionDecl: // functions
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=lightseagreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateDeclaration:
                  {
                 // This type is not supposed to be used in the new EDG 4.x work.
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + templateDeclaration->get_name().getString() + 
                                          "\\n  " + StringUtility::numberToString(templateDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateClassDeclaration:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=blueviolet,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (classDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                    break;
                  }

               case V_SgClassDeclaration:
               case V_SgModuleStatement:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (classDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (classDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == true) ? "typedefBaseTypeContainsDefiningDeclaration" : "!typedefBaseTypeContainsDefiningDeclaration";
                 // string flagString2 = (typedefDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + typedefDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + StringUtility::numberToString(typedefDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateTypedefDeclaration:
                  {
                    SgTemplateTypedefDeclaration* templateTypedefDeclaration = isSgTemplateTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=blueviolet,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (templateTypedefDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (templateTypedefDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + templateTypedefDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(templateTypedefDeclaration) + "  ";
                    break;
                  }

               case V_SgTemplateInstantiationTypedefDeclaration:
                  {
                    SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (templateInstantiationTypedefDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (templateInstantiationTypedefDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + templateInstantiationTypedefDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(templateInstantiationTypedefDeclaration) + "  ";
                    break;
                  }

               case V_SgVariableDeclaration:
                  {
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true) ? "variableDeclarationContainsBaseTypeDefiningDeclaration" : "!variableDeclarationContainsBaseTypeDefiningDeclaration";
                    labelWithSourceCode = "\\n  " + flagString +
                                          "\\n  " + StringUtility::numberToString(variableDeclaration) + "  ";

                    string isPrivate   = variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false";
                    string isProtected = variableDeclaration->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false";
                    string isPublic    = variableDeclaration->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false";

                    labelWithSourceCode += "\\n isPrivate   = " + isPrivate   + "  ";
                    labelWithSourceCode += "\\n isProtected = " + isProtected + "  ";
                    labelWithSourceCode += "\\n isPublic    = " + isPublic    + "  ";
                    break;
                  }

               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString   = (enumDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + enumDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + StringUtility::numberToString(enumDeclaration) + "  ";
                    break;
                  }

               case V_SgNamespaceDeclarationStatement:
                  {
                    SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + namespaceDeclaration->get_name().getString() + 
                                          "\\n  " + StringUtility::numberToString(namespaceDeclaration) + "  ";
                    break;
                  }

               case V_SgNamespaceDefinitionStatement:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";

                    SgNamespaceDefinitionStatement* ns = isSgNamespaceDefinitionStatement(node);
                    ASSERT_not_null(ns);

                    size_t index = ns->namespaceIndex() + 1;
                    size_t count = ns->numberOfNamespaceDefinitions();

                    if (ns->get_isUnionOfReentrantNamespaceDefinitions() == false)
                       {
                         labelWithSourceCode = "\\n  " +  StringUtility::numberToString(index) + " of " +  StringUtility::numberToString(count) + "  ";
                       }
                      else
                       {
                         labelWithSourceCode = "\\n  union of " +  StringUtility::numberToString(count) + " reentrant namespace definitions ";
                       }

                    labelWithSourceCode += "\\n  " +  StringUtility::numberToString(node) + "  ";
                    break;
                  }

               case V_SgNamespaceAliasDeclarationStatement:
                  {
                    SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + namespaceAliasDeclaration->get_name().getString() + 
                                          "\\n  " + StringUtility::numberToString(namespaceAliasDeclaration) + "  ";
                    break;
                  }

               default:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
                    break;
                  }
             }

          // Liao, 5/8/2014, display unique ID if set (non-zero)
          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgExpression(node) != nullptr)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";

          string labelWithSourceCode;
          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          SgValueExp* valueExp = isSgValueExp(node);
          if (valueExp != nullptr)
             {
               if (valueExp->get_parent() == nullptr)
                  {
                    printf ("Error: valueExp = %p = %s valueExp->get_parent() == NULL \n",valueExp,valueExp->class_name().c_str());
                    valueExp->get_file_info()->display("Error: valueExp->get_parent() == nullptr");

                 // In the case of a SgIntVal, we can try to output a little more information
                    SgIntVal* intVal = isSgIntVal(valueExp);
                    if (intVal != nullptr)
                       {
                         printf ("Error: intVal = %d \n",intVal->get_value());
                       }
                    SgUnsignedLongVal* unsignedLongVal = isSgUnsignedLongVal(valueExp);
                    if (unsignedLongVal != nullptr)
                       {
                         printf ("Error: unsignedLongVal = %lu \n",unsignedLongVal->get_value());
                       }
                    SgCharVal* charVal = isSgCharVal(valueExp);
                    if (charVal != nullptr)
                       {
                         printf ("Error: charVal = %d \n",charVal->get_value());
                       }
                  }

               if (valueExp->get_parent() == nullptr)
                  {
                    printf ("ERROR: valueExp->get_parent() == NULL in AST visualization: valueExp = %p = %s \n",valueExp,valueExp->class_name().c_str());
                  }

               SgCharVal* charVal = isSgCharVal(valueExp);
               if (charVal != nullptr)
                  {
                    char value = charVal->get_value();
                    labelWithSourceCode += string("\\n alpha/numeric value = ") + (isalnum(value) ? "true" : "false") + "  ";
                  }

               SgStringVal* stringVal = isSgStringVal(valueExp);
               if (stringVal != nullptr)
                  {
                    bool is_wchar     = stringVal->get_wcharString();
                    bool is_16Bitchar = stringVal->get_is16bitString();
                    bool is_32Bitchar = stringVal->get_is32bitString();
                    labelWithSourceCode += string("\\n is_wchar = ") + (is_wchar ? "true" : "false") + "  ";
                    labelWithSourceCode += string("\\n is_16Bitchar = ") + (is_16Bitchar ? "true" : "false") + "  ";
                    labelWithSourceCode += string("\\n is_32Bitchar = ") + (is_32Bitchar ? "true" : "false") + "  ";
                  }

            // Output the value so that we can provide more information.
               labelWithSourceCode += string("\\n value = ") + valueExp->get_constant_folded_value_as_string() + "  ";
             }

          SgCastExp* castExp = isSgCastExp(node);
          if (castExp != nullptr)
             {
               ASSERT_not_null(castExp->get_startOfConstruct());
               labelWithSourceCode += string("\\n castContainsBaseTypeDefiningDeclaration: ") + ((castExp->get_castContainsBaseTypeDefiningDeclaration() == true) ? "true" : "false") + "  ";
               labelWithSourceCode += string("\\n cast is: ") + ((castExp->get_startOfConstruct()->isCompilerGenerated() == true) ? "implicit" : "explicit") + "  ";
             }

       // DQ (2/2/2011): Added support for fortran...
          SgActualArgumentExpression* actualArgumentExpression = isSgActualArgumentExpression(node);
          if (actualArgumentExpression != nullptr)
             {
               labelWithSourceCode += string("\\n name = ") + actualArgumentExpression->get_argument_name() + "  ";
             }

       // DQ (4/8/2013): Added support to output if this function is using operator syntax.
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);
          if (functionCallExp != nullptr)
             {
               labelWithSourceCode += string("\\n call uses operator syntax: ") + ((functionCallExp->get_uses_operator_syntax() == true) ? "true" : "false") + "  ";
             }

       // DQ (4/26/2013): Added support for marking as compiler generated.
          SgExpression* expression = isSgExpression(node);
          ASSERT_not_null(expression);
          if (expression->get_file_info()->isCompilerGenerated() == true)
             {
               labelWithSourceCode += string("\\n compiler generated ");
             }

          if (expression->get_file_info()->isDefaultArgument() == true)
             {
               labelWithSourceCode += string("\\n default argument ");
             }

          if (expression->get_file_info()->isImplicitCast() == true)
             {
               labelWithSourceCode += string("\\n implicit cast ");

            // Make sure this is a cast sine otherwise this setting makes no sense.
               ASSERT_not_null(isSgCastExp(node));
             }

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }


     if (isSgType(node) != nullptr)
        {
          SgType* type = isSgType(node);
          ASSERT_not_null(type);

          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";

          string labelWithSourceCode;
          SgTypeDefault* defaultType = isSgTypeDefault(node);
          if (defaultType != nullptr)
             {
               labelWithSourceCode += string("\\n  name = ") + defaultType->get_name().str() + "  ";
             }

          SgTemplateType* templateType = isSgTemplateType(node);
          if (templateType != nullptr)
             {
               labelWithSourceCode += string("\\n  name = ") + templateType->get_name().str() + "  " + "position = " + StringUtility::numberToString(templateType->get_template_parameter_position()) + " ";
             }

          SgNamedType* namedType = isSgNamedType(node);
          if (namedType != nullptr)
             {
               labelWithSourceCode += string("\\n") + namedType->get_name().str();
             }

          SgModifierType* modifierType = isSgModifierType(node);
          if (modifierType != nullptr)
             {
               SgTypeModifier tm = modifierType->get_typeModifier(); 
               if (tm.isRestrict()) {
                 labelWithSourceCode += string("\\n restrict  ") ;
               }
               labelWithSourceCode += string("\\n    frontend_type_reference() = ") + (modifierType->get_frontend_type_reference() != nullptr ? "true" : "false") + "    ";
             }

          labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode += string("\\n   ");

          SgModifierType* mod_type = isSgModifierType(node);
          if (mod_type != nullptr)
             {
               if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
                    long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

                    labelWithSourceCode += string("UPC: ");

                    if (block_size == 0) // block size empty
                       {
                         labelWithSourceCode += string("shared[] ");
                       }
                    else if (block_size == -1) // block size omitted
                       {
                         labelWithSourceCode += string("shared ");
                       }
                    else if (block_size == -2) // block size is *
                       {
                         labelWithSourceCode += string("shared[*] ");
                       }
                    else
                       {
                         ASSERT_require(block_size > 0);
                         stringstream ss;
                         ss << block_size;
                         labelWithSourceCode += string("shared["+ss.str()+"] ");
                       }
                  }

               if (mod_type->get_typeModifier().get_constVolatileModifier().isConst() == true)
                  {
                    labelWithSourceCode += string("\\n const ");
                  }

               if (mod_type->get_typeModifier().get_elaboratedTypeModifier().get_modifier() != SgElaboratedTypeModifier::e_default)
                  {
                    stringstream ss;
                    ss << mod_type->get_typeModifier().get_elaboratedTypeModifier().get_modifier();
                    labelWithSourceCode += string("\\n type modifier enum value = "+ss.str()+" ");
                  }
             }

          labelWithSourceCode += string("\\n   ");

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     SgSupport* supportNode = isSgSupport(node);
     if (supportNode != nullptr)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(supportNode->variantT())
             {
               case V_SgProject:
                  {
                    SgProject* project = isSgProject(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=3,color=\"blue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + project->get_outputFileName() + "  ";
                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(project) + "  ";
                    break;
                  }

               case V_SgSourceFile:
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
               case V_SgBinaryComposite:
#endif
                  {
                    SgFile* file = isSgFile(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  getFileName = ") + file->getFileName() + "  ";
                    labelWithSourceCode += string("\\n  sourceFileNameWithPath = ") + file->get_sourceFileNameWithPath() + "  ";

                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(file) + "  ";
                    ASSERT_not_null(SgNode::get_globalFunctionTypeTable());
                    break;
                  }

               case V_SgIncludeFile:
                  {
                    SgIncludeFile* include_file = isSgIncludeFile(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";

                 // Trigger the root for the include tree associated with the input source file to be collored differently.
                    if (include_file->get_isRootSourceFile() == true)
                       {
                         additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                         labelWithSourceCode = string("INCLUDE TREE ROOT\\n");
                       }

                    labelWithSourceCode += string("\\n  ") + include_file->get_filename() + "  ";
                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(include_file) + "  ";
                    break;
                  }

               case V_Sg_File_Info:
                  {
                 // Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
                    Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
                    if (currentNodeFileInfo != nullptr)
                       {
                         if (currentNodeFileInfo->isFrontendSpecific() == false)
                            {
                              additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=brown,fontname=\"7x13bold\",fontcolor=black,style=filled";
                           // Make this statement different in the generated dot graph
                              labelWithSourceCode = 
                                 "\\n" + currentNodeFileInfo->get_filenameString() + 
                                 "\\n line: "          +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                    " column: "        +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n raw line: "      +  StringUtility::numberToString(currentNodeFileInfo->get_raw_line()) + 
                                    " raw column: "    +  StringUtility::numberToString(currentNodeFileInfo->get_raw_col()) + "  " +
                              // Information output regarding the source position information.
                                 "\\n hasPositionInSource() = "                    + (currentNodeFileInfo->hasPositionInSource() == true ? "T" : "F") + 
                                    " isTransformation() = "                       + (currentNodeFileInfo->isTransformation() == true ? "T" : "F") + "  " +
                                    " isCompilerGenerated() = "                    + (currentNodeFileInfo->isCompilerGenerated() == true ? "T" : "F") + "  " +
                                 "\\n isOutputInCodeGeneration() = "               + (currentNodeFileInfo->isOutputInCodeGeneration() == true ? "T" : "F") + "  " +
                                    " isShared() = "                               + (currentNodeFileInfo->isShared() == true ? "T" : "F") + "  " +
                                    " isFrontendSpecific() = "                     + (currentNodeFileInfo->isFrontendSpecific() == true ? "T" : "F") + "  " +
                                  "\\n isSourcePositionUnavailableInFrontend() = " + (currentNodeFileInfo->isSourcePositionUnavailableInFrontend() == true ? "T" : "F") + "  " +
                                    " isCommentOrDirective() = "                   + (currentNodeFileInfo->isCommentOrDirective() == true ? "T" : "F") + "  " +
                                    " isToken() = "                                + (currentNodeFileInfo->isToken() == true ? "T" : "F") + "  " +
                                 "\\n pointer value: " +  StringUtility::numberToString(currentNodeFileInfo) + "  ";
                            }
                       }
                    break;
                  }

               case V_SgSymbolTable:
                  {
                    SgSymbolTable* symbolTable = isSgSymbolTable(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string symbolTableName = "Unnamed Symbol Table";
                    if (symbolTable->get_no_name() == true)
                         symbolTableName = symbolTable->get_name().getString();
                    labelWithSourceCode = string("\\n  ") + symbolTableName +
                                          string("\\n  ") + StringUtility::numberToString(symbolTable) + "  ";
                    break;
                  }

               case V_SgTypedefSeq:
                  {
                    SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(typedefSeq) + "  ";
                    break;
                  }

               case V_SgTemplateArgument:
                  {
                    SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
                    additionalNodeOptions = "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string typeString;
                    switch (templateArgument->get_argumentType())
                       {
                         case SgTemplateArgument::argument_undefined:
                              typeString = "argument_undefined";
                              break;

                         case SgTemplateArgument::type_argument:
                              typeString = "type_argument";
                              typeString += string("\\n type = ") + templateArgument->get_type()->unparseToString();
                              typeString += string("\\n type = ") + StringUtility::numberToString(templateArgument->get_type()) + "  ";
                              break;

                         case SgTemplateArgument::nontype_argument:
                              typeString = "nontype_argument";
                              typeString += string("\\n expression = ") + StringUtility::numberToString(templateArgument->get_expression()) + "  ";
                              break;

                         case SgTemplateArgument::template_template_argument:
                              typeString = "template_template_argument";
                              break;

                         case SgTemplateArgument::start_of_pack_expansion_argument:
                              typeString = "start_of_pack_expansion_argument";
                              break;

                         default:
                            {
                              printf ("Error: default reached in case V_SgTemplateArgument: templateArgument->get_argumentType() \n");
                              ASSERT_require(false);
                            }
                       }

                    typeString += string("\\n explicitlySpecified = ") +  ((templateArgument->get_explicitlySpecified() == true) ? "true" : "false") + "  ";

                    labelWithSourceCode = string("\\n  ") + typeString + 
                                          string("\\n  ") + StringUtility::numberToString(templateArgument) + "  ";
                    break;
                  }

               case V_SgTemplateParameter:
                  {
                    SgTemplateParameter* templateParameter = isSgTemplateParameter(node);
                    additionalNodeOptions = "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";

                    string typeString;
                    switch (templateParameter->get_parameterType())
                       {
                         case SgTemplateParameter::parameter_undefined:
                              typeString = "parameter_undefined";
                              break;

                         case SgTemplateParameter::type_parameter:
                              typeString = "type_parameter";
                              typeString += string("\\n type = ") + templateParameter->get_type()->unparseToString();
                              typeString += string("\\n type = ") + StringUtility::numberToString(templateParameter->get_type()) + "  ";
                              break;

                         case SgTemplateParameter::nontype_parameter:
                              typeString = "nontype_parameter";
                              break;

                         case SgTemplateParameter::template_parameter:
                              typeString = "template_parameter";
                              break;

                         default:
                            {
                              printf ("Error: default reached in case V_SgTemplateParameter: templateParameter->get_argumentType() = %d \n",templateParameter->get_parameterType());
                              ASSERT_require(false);
                            }
                       }

                    labelWithSourceCode = string("\\n  ") + typeString + 
                                          string("\\n  ") + StringUtility::numberToString(templateParameter) + "  ";
                    break;
                  }

               case V_SgBaseClass:
                  {
                    SgBaseClass* baseClass = isSgBaseClass(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"blue\",fillcolor=greenyellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(baseClass) + "  ";
                    break;
                  }

               case V_SgFunctionParameterTypeList:
                  {
                    SgFunctionParameterTypeList* functionParameterTypeList = isSgFunctionParameterTypeList(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode  = string("\\n  ") + StringUtility::numberToString(functionParameterTypeList->get_arguments().size()) + "  ";
                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(functionParameterTypeList) + "  ";
                    break;
                  }

               default:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    break;
                  }
             }

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
             {
               labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

       if (isSgInitializedName(node) != nullptr)
       {
         SgInitializedName* initializedName = isSgInitializedName(node);
         string additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=darkturquoise,fontname=\"7x13bold\",fontcolor=black,style=filled";
         string labelWithSourceCode = string("\\n  ") + initializedName->get_name().getString() +
           string("\\n  ") + StringUtility::numberToString(initializedName) + "  ";

         // Support for __device__ keyword to be used for CUDA in function calls.
         // This implements an idea suggested by Jeff Keasler.
         labelWithSourceCode += string("\\n  ") + "using_device_keyword = " + (initializedName->get_using_device_keyword() ? "true" : "false") + "  ";

         AST_NODE_ID id = TransformationTracking::getId(node) ;
         if (id != 0)
           labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
         NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
         addNode(graphNode);
       }

     if (isSgSymbol(node) != nullptr)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"blue\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
          string labelWithSourceCode;

          SgSymbol* symbol = isSgSymbol(node);
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(node);
          if (aliasSymbol != nullptr)
             {
               labelWithSourceCode += string("\\n alias to: ") + aliasSymbol->get_base()->class_name();
             }

          labelWithSourceCode += string("\\n name: ") + symbol->get_name();
          labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(node) + "  ";

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgToken(node) != nullptr)
        {
          SgToken* token = isSgToken(node);
          ASSERT_not_null(token);
        }

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
     if (isSgAsmType(node) != nullptr)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
          string labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
     if (isSgAsmNode(node) != nullptr)
        {
       // Color selection for the binary file format and binary instruction IR nodes.
          string additionalNodeOptions;
          string labelWithSourceCode;

       // Make this statement different in the generated dot graph
          switch(node->variantT())
             {
               case V_SgAsmElfSection:
               case V_SgAsmPESection:
               case V_SgAsmNESection:
               case V_SgAsmLESection:
                  {
                    SgAsmGenericSection* genericSection = isSgAsmGenericSection(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=royalblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + genericSection->get_name()->get_string() +
                                          "\\n  " +  StringUtility::numberToString(genericSection) + "  ";
                    break;
                  }

               case V_SgAsmElfFileHeader:
               case V_SgAsmPEFileHeader:
               case V_SgAsmNEFileHeader:
               case V_SgAsmLEFileHeader:
               case V_SgAsmDOSFileHeader:
                  {
                    SgAsmGenericHeader* genericHeader = isSgAsmGenericHeader(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + genericHeader->get_name()->get_string() + 
                                          "\\n  " +  StringUtility::numberToString(genericHeader) + "  ";
                    break;
                  }

               case V_SgAsmElfSectionTableEntry:
               case V_SgAsmPESectionTableEntry:
               case V_SgAsmNESectionTableEntry:
               case V_SgAsmLESectionTableEntry:
                  {
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=darkturquoise,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " +  StringUtility::numberToString(node) + "  ";
                    break;
                  }

               case V_SgAsmElfSegmentTableEntry:
                  {
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " +  StringUtility::numberToString(node) + "  ";
                    break;
                  }

               case V_SgAsmElfSymbol:
               case V_SgAsmCoffSymbol:
                  {
                    SgAsmGenericSymbol* genericSymbol = isSgAsmGenericSymbol(node);
                    additionalNodeOptions = "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + genericSymbol->get_name()->get_string() + 
                                          "\\n  " +  StringUtility::numberToString(genericSymbol) + "  ";
                    break;
                  }

               default:
                  {
                 // It appears that we can't unparse one of these (not implemented)
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
                    break;
                  }
             }

          SgAsmInstruction* asmInstruction = isSgAsmInstruction(node);
          if (asmInstruction != nullptr)
             {
               string unparsedInstruction = asmInstruction->toString();
               string addressString       = StringUtility::numberToString( (void*) asmInstruction->get_address() );

               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
               labelWithSourceCode = "\\n  " + unparsedInstruction + 
                                     "\\n  address: " + addressString +
                                     "\\n  (generated label: " +  StringUtility::numberToString(asmInstruction) + ")  ";
             }

          if (SgAsmExpression* asmExpression = isSgAsmExpression(node)) {
              const std::string unparsedExpression = asmExpression->toString();
              additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
              labelWithSourceCode = "\\n" + unparsedExpression +
                                    "\\n  (generated label: " +  StringUtility::numberToString(asmExpression) + ")  ";
          }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
#endif
   }

/* Initialize the filters for the default case */
CustomMemoryPoolDOTGeneration::s_Filter_Flags::s_Filter_Flags()
{
  setDefault();
}

CustomMemoryPoolDOTGeneration::s_Filter_Flags::~s_Filter_Flags()
   {
  // Nothing to do here!
   }

void
CustomMemoryPoolDOTGeneration::s_Filter_Flags::setDefault()
   {
  // Initial values for the filters
     m_asmFileFormat = 0;         /*asmFileFormatFilter()*/
     m_asmType = 0;               /* asmTypeFilter()*/
     m_binaryExecutableFormat = 0;/*binaryExecutableFormatFilter()*/
     m_commentAndDirective = 1;   /* commentAndDirectiveFilter()*/
     m_ctorInitializer = 0;       /*ctorInitializerListFilter()*/

     m_default = 1;              /* defaultFilter ()*/
     m_defaultColor = 1;         /*defaultColorFilter()*/
     m_edge    = 1;              /* edgeFilter ()*/
     m_emptySymbolTable = 0;    /*emptySymbolTableFilter()*/

  // DQ (7/22/2012): Ignore some empty IR nodes.
     m_emptyBasicBlock = 0;             /*emptyBasicBlockFilter()*/
     m_emptyFunctionParameterList = 0;  /*emptyFunctionParameterListFilter()*/

     m_expression = 0 ;          /* expressionFilter ()*/

     m_fileInfo =  1;             /* fileInfoFilter ()*/

  // DQ (7/25/2010): Temporary testing (debugging).
  // m_frontendCompatibility = 1;/* frontendCompatibilityFilter()*/
     m_frontendCompatibility = 0;/* frontendCompatibilityFilter()*/

     m_symbol     = 0;           /*symbolFilter ()*/
     m_type    = 0;              /* typeFilter ()*/
     m_variableDeclaration = 0;  /*variableDeclarationFilter()*/

     m_variableDefinition = 0 ;  /*variableDefinitionFilter()*/

  // This is the normal (non-debugging) mode.
     m_noFilter = 0;               /* no filtering */
   }

void CustomMemoryPoolDOTGeneration::s_Filter_Flags::print_filter_flags ()
{
  printf ("Current filter flags' values are: \n");

  printf ("\t m_asmFileFormat = %d \n", m_asmFileFormat);
  printf ("\t m_asmType = %d \n", m_asmType);
  printf ("\t m_binaryExecutableFormat = %d \n", m_binaryExecutableFormat);
  printf ("\t m_commentAndDirective = %d \n", m_commentAndDirective);
  printf ("\t m_ctorInitializer = %d \n", m_ctorInitializer);

  printf ("\t m_default = %d \n", m_default);
  printf ("\t m_defaultColor = %d \n", m_defaultColor);
  printf ("\t m_edge = %d \n", m_edge);
  printf ("\t m_emptySymbolTable = %d \n", m_emptySymbolTable);
  printf ("\t m_expression = %d \n", m_expression);

  printf ("\t m_fileInfo = %d \n", m_fileInfo);
  printf ("\t m_frontendCompatibility = %d \n", m_frontendCompatibility);
  printf ("\t m_symbol = %d \n", m_symbol);
  printf ("\t m_type = %d \n", m_type);
  printf ("\t m_variableDeclaration = %d \n", m_variableDeclaration);

  printf ("\t m_variableDefinition = %d \n", m_variableDefinition);

  printf ("\t m_noFilter = %d \n", m_noFilter);
}

/* Construct an instance from */
CustomMemoryPoolDOTGeneration::s_Filter_Flags::s_Filter_Flags(std::vector <std::string>& argvList)
   {
     setDefault(); // set default options first

  // stop here if no arguments are specified at all
     if ( argvList.size() == 0)
          return;

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","asmFileFormatFilter", m_asmFileFormat, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","asmTypeFilter", m_asmType, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","binaryExecutableFormatFilter", m_binaryExecutableFormat, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","commentAndDirectiveFilter", m_commentAndDirective, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","ctorInitializerListFilter", m_ctorInitializer, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","defaultColorFilter", m_defaultColor, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","defaultFilter", m_default, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","edgeFilter", m_edge, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","emptySymbolTableFilter", m_emptySymbolTable, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","emptyBasicBlockFilter", m_emptyBasicBlock, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","emptyFunctionParameterListFilter", m_emptyFunctionParameterList, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","expressionFilter", m_expression, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","fileInfoFilter", m_fileInfo, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","frontendCompatibilityFilter", m_frontendCompatibility, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","symbolFilter", m_symbol, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","typeFilter", m_type, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","variableDeclarationFilter", m_variableDeclaration, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","variableDefinitionFilter", m_variableDefinition, true);

     CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","noFilter", m_noFilter, true);

     if (CommandlineProcessing::isOption(argvList, "-rose:","help", false)
      || CommandlineProcessing::isOption(argvList, "-help","", false)
      || CommandlineProcessing::isOption(argvList, "--help","", false))
        {
          print_commandline_help();
          print_filter_flags();
        }
   }

void
CustomMemoryPoolDOTGeneration::s_Filter_Flags::print_commandline_help()
{
  cout<<"   -rose:help                     show this help message"<<endl;

  cout<<"   -rose:dotgraph:asmFileFormatFilter              [0|1]  Disable or enable asmFileFormat filter"<<endl;
  cout<<"   -rose:dotgraph:asmTypeFilter                    [0|1]  Disable or enable asmType filter"<<endl;
  cout<<"   -rose:dotgraph:binaryExecutableFormatFilter     [0|1]  Disable or enable binaryExecutableFormat filter"<<endl;
  cout<<"   -rose:dotgraph:commentAndDirectiveFilter        [0|1]  Disable or enable commentAndDirective filter"<<endl;
  cout<<"   -rose:dotgraph:ctorInitializerListFilter        [0|1]  Disable or enable ctorInitializerList filter"<<endl;

  cout<<"   -rose:dotgraph:defaultFilter                    [0|1]  Disable or enable default filter"<<endl;
  cout<<"   -rose:dotgraph:defaultColorFilter               [0|1]  Disable or enable defaultColor filter"<<endl;
  cout<<"   -rose:dotgraph:edgeFilter                       [0|1]  Disable or enable edge filter"<<endl;
  cout<<"   -rose:dotgraph:expressionFilter                 [0|1]  Disable or enable expression filter"<<endl;
  cout<<"   -rose:dotgraph:fileInfoFilter                   [0|1]  Disable or enable fileInfo filter"<<endl;

  cout<<"   -rose:dotgraph:frontendCompatibilityFilter      [0|1]  Disable or enable frontendCompatibility filter"<<endl;
  cout<<"   -rose:dotgraph:symbolFilter                     [0|1]  Disable or enable symbol filter"<<endl;

  cout<<"   -rose:dotgraph:emptySymbolTableFilter           [0|1]  Disable or enable emptySymbolTable filter"<<endl;
  cout<<"   -rose:dotgraph:emptyFunctionParameterListFilter [0|1]  Disable or enable emptyFunctionParameterList filter"<<endl;
  cout<<"   -rose:dotgraph:emptyBasicBlockFilter            [0|1]  Disable or enable emptyBasicBlock filter"<<endl;

  cout<<"   -rose:dotgraph:typeFilter                       [0|1]  Disable or enable type filter"<<endl;
  cout<<"   -rose:dotgraph:variableDeclarationFilter        [0|1]  Disable or enable variableDeclaration filter"<<endl;

  cout<<"   -rose:dotgraph:variableDefinitionFilter         [0|1]  Disable or enable variableDefinitionFilter filter"<<endl;

  cout<<"   -rose:dotgraph:noFilter                         [0|1]  Disable or enable no filtering"<<endl;
  cout<<"   -DSKIP_ROSE_BUILTIN_DECLARATIONS                Enable builtin function filtering, defaut is to show all of them"<<endl;
}


class SimpleColorMemoryPoolTraversal
   : public CustomMemoryPoolDOTGeneration,
     public ROSE_VisitTraversal
   {
     public:
          static const set<SgNode*> defaultSetOfIRnodes;
          const set<SgNode*> & setOfIRnodes;

      //! Required traversal function
          void visit (SgNode* node);

          static void generateGraph( string filename, const set<SgNode*> & firstAST = defaultSetOfIRnodes , CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags = nullptr);

          void markFirstAST();
          void buildExcludeList();

          SimpleColorMemoryPoolTraversal(const set<SgNode*> & s = defaultSetOfIRnodes, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags = nullptr) : CustomMemoryPoolDOTGeneration(flags) , setOfIRnodes(s) {};

       // Destructor defined because base class has virtual members
          virtual ~SimpleColorMemoryPoolTraversal();
   };

const set<SgNode*> SimpleColorMemoryPoolTraversal::defaultSetOfIRnodes;

SimpleColorMemoryPoolTraversal::~SimpleColorMemoryPoolTraversal()
   {
   }

void
SimpleColorMemoryPoolTraversal::generateGraph(string filename, const set<SgNode*> & firstAST, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags /*= nullptr*/)
   {
     std::string label =  "SimpleColorMemoryPoolTraversal::generateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Custom control over the coloring of the "whole" AST for a memory pool traversal
     SimpleColorMemoryPoolTraversal traversal(firstAST, flags);
     traversal.traverseMemoryPool();
     traversal.buildExcludeList();
     traversal.internalGenerateGraph(filename);
   }

void
SimpleColorMemoryPoolTraversal::markFirstAST ()
   {
  // Mark the IR node that are in the first AST before the deep AST copy
#ifdef _MSC_VER
   // DQ (11/27/2009): I think that MSVC is correct and this should be a const_iterator (GNU is lacks because it is a const reference).
     set<SgNode*>::const_iterator i = setOfIRnodes.begin();
#else
     set<SgNode*>::iterator i = setOfIRnodes.begin();
#endif
     while (i != setOfIRnodes.end())
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
          string labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

          NodeType graphNode(*i,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
          i++;
        }
   }

void
SimpleColorMemoryPoolTraversal::buildExcludeList ()
   {
  // Mark the IR node that are in the first AST before the deep AST copy
#ifdef _MSC_VER
   // DQ (11/27/2009): I think that MSVC is correct and this should be a const_iterator (GNU is lacks because it is a const reference).
     set<SgNode*>::const_iterator i = setOfIRnodes.begin();
#else
     set<SgNode*>::iterator i = setOfIRnodes.begin();
#endif
     while (i != setOfIRnodes.end())
        {
          skipNode(*i);
          i++;
        }
   }

void
SimpleColorMemoryPoolTraversal::visit(SgNode* node)
   {
     ASSERT_not_null(filterFlags);

  // DQ (3/2/2010): Test if we have turned off all filtering of the AST.
     if ( filterFlags->m_noFilter == 0) 
        {
       // We allow filtering of the AST.
          if ( filterFlags->m_default== 1)
             {
               defaultFilter(node);
             }
          if ( filterFlags->m_type == 1) 
             {
               typeFilter(node);
             }
          if ( filterFlags->m_expression == 1) 
             {
               expressionFilter(node);
             }
          if ( filterFlags->m_emptySymbolTable == 1) 
             {
               emptySymbolTableFilter(node);
             }
          if ( filterFlags->m_emptyBasicBlock == 1) 
             {
               emptyBasicBlockFilter(node);
             }

          if ( filterFlags->m_emptyFunctionParameterList == 1) 
             {
               emptyFunctionParameterListFilter(node);
             }
          if ( filterFlags->m_variableDefinition == 1) 
             {
               variableDefinitionFilter(node);
             }

          if ( filterFlags->m_variableDeclaration == 1) 
             {
               variableDeclarationFilter(node);
             }

          if ( filterFlags->m_ctorInitializer == 1) 
             {
               ctorInitializerListFilter(node);
             }
          if ( filterFlags->m_symbol == 1) 
             {
               symbolFilter(node);
             }
          if ( filterFlags->m_asmFileFormat == 1) 
             {
               asmFileFormatFilter(node);
             }

          if ( filterFlags->m_asmType == 1) 
             {
               asmTypeFilter(node);
             }

       // Control output of Sg_File_Info object in graph of whole AST.
          if ( filterFlags->m_fileInfo == 1) 
             {
               fileInfoFilter(node);
             }
        }

     if ( filterFlags->m_defaultColor == 1) 
        {
          defaultColorFilter(node);
        }

  // Color this IR node differently if it in NOT in the original AST
     if (setOfIRnodes.find(node) == setOfIRnodes.end())
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=greenyellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
          string labelWithSourceCode = "\\n" +  StringUtility::numberToString(node) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
   }

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class SimpleColorFilesInheritedAttribute
   {
     public:
      //! Max number of different types of nodes that we define
          static const int maxSize;
          static const std::string additionalNodeOptionsArray[5];

       // Current options to use with DOT visualizations
          string currentNodeOptions;

       // True if subtree is shared
          bool sharedSubTree;

          void setNodeOptions(SgFile* file);

       // Default constructor
          SimpleColorFilesInheritedAttribute () : sharedSubTree(false)
             {
#ifndef CXX_IS_ROSE_CODE_GENERATION
               ASSERT_require(currentNodeOptions.size() < 4000);
#endif
             }
   };

const int SimpleColorFilesInheritedAttribute::maxSize = 5;

// colors that will be represented as "black": azure1
const string SimpleColorFilesInheritedAttribute::additionalNodeOptionsArray[SimpleColorFilesInheritedAttribute::maxSize] = 
   { "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=blue1,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=greenyellow,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=gold1,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled"
   };

void
SimpleColorFilesInheritedAttribute::setNodeOptions(SgFile* file)
   {
     ASSERT_not_null(file->get_file_info());

  // file_id's can be negative for defalut generated files (NULL_FILE, Transforamtions, etc.)
     int file_id = file->get_file_info()->get_file_id();
     int index = 0;
     if (file_id > 0)
        {
       // Select from a small subset of optional setting strings.
          index = (file->get_file_info()->get_file_id() + 0) % maxSize;
        }
     currentNodeOptions = additionalNodeOptionsArray[index];
     ASSERT_require(currentNodeOptions.size() < 4000);
   }

class SimpleColorFilesTraversal
   : public CustomAstDOTGeneration,
     public SgTopDownProcessing<SimpleColorFilesInheritedAttribute>
   {
     public:
       // Use this as an empty list to support the default argument option to generateGraph
          static set<SgNode*> emptyNodeList;

       // Reference to list that will be colored differently
          set<SgNode*> & specialNodeList;

      //! Required traversal function
          SimpleColorFilesInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             SimpleColorFilesInheritedAttribute inheritedAttribute );

          static void generateGraph( SgProject* project, string filename, set<SgNode*> & specialNodeList = SimpleColorFilesTraversal::emptyNodeList );

       // Constructor
          SimpleColorFilesTraversal(set<SgNode*> & s) : specialNodeList(s) {}

       // Destructor defined because base class has virtual members
          virtual ~SimpleColorFilesTraversal() {}
   };

// Use this as an empty list to support the default argument option to generateGraph
set<SgNode*> SimpleColorFilesTraversal::emptyNodeList;

// Functions required by the tree traversal mechanism
SimpleColorFilesInheritedAttribute
SimpleColorFilesTraversal::evaluateInheritedAttribute (
     SgNode* node,
     SimpleColorFilesInheritedAttribute inheritedAttribute )
   {
     ASSERT_not_null(node);
     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);

     SgProject* project = isSgProject(node);
     if (project != nullptr)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"blue\",fillcolor=cyan4,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }

     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);
     SgFile* file = isSgFile(node);
     if (file != nullptr)
        {
          inheritedAttribute.setNodeOptions(file);
        }

     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != nullptr)
        {
          if (fileInfo->isShared() == true)
               inheritedAttribute.sharedSubTree = true;
        }

     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);
     if (inheritedAttribute.sharedSubTree == true)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=diamond,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=magenta1,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }
     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);

     string additionalNodeOptions = inheritedAttribute.currentNodeOptions;
     string labelWithSourceCode;

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
     if (classDeclaration != nullptr)
        {
          labelWithSourceCode   = "\\n" +  classDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
        }
       else
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
          if (functionDeclaration != nullptr)
             {
               labelWithSourceCode   = "\\n" +  functionDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
             }
            else
             {
               labelWithSourceCode   = "\\n" +  StringUtility::numberToString(node) + "  ";
             }
        }
     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);

  // Check if this is an IR node that is to be handled specially
     if (specialNodeList.find(node) != specialNodeList.end())
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"blue\",fillcolor=black,fontname=\"12x24bold\",fontcolor=white,style=filled";
          ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);
        }
     ASSERT_require(inheritedAttribute.currentNodeOptions.size() < 4000);

     additionalNodeOptions = inheritedAttribute.currentNodeOptions;
     ASSERT_require(additionalNodeOptions.size() < 4000);

     NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
     ASSERT_require(graphNode.optionString.size() < 4000);
     addNode(graphNode);

     return inheritedAttribute;
   }

void
SimpleColorFilesTraversal::generateGraph(SgProject* project, string filename, set<SgNode*> & specialNodeList )
   {
     std::string label =  "SimpleColorFilesTraversal::generateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Custom control over the coloring of the "whole" AST for a memory pool traversal
     SimpleColorFilesTraversal traversal(specialNodeList);
     SimpleColorFilesInheritedAttribute ih;
     traversal.traverse(project,ih);

     traversal.internalGenerateGraph(filename,project);
   }


#include "wholeAST_API.h"

void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags)
{
  SimpleColorMemoryPoolTraversal::generateGraph(filename, skippedNodeSet, flags);
}

// DQ (6/3/2007): Interface functions defined in wholeAST_API.h
void
generateWholeGraphOfAST( string filename, set<SgNode*> & skippedNodeSet )
   {
     SimpleColorMemoryPoolTraversal::generateGraph(filename,skippedNodeSet);
   }

// Facilitate gdb to call this function
void   
generateWholeGraphOfAST( const char* filename)
{
  generateWholeGraphOfAST (std::string(filename), nullptr);
}

void   
generateWholeGraphOfAST( string filename)
{
  generateWholeGraphOfAST (filename, nullptr);
}

void
generateWholeGraphOfAST( string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags/*= nullptr*/)
   {
  // Make this the default type of graph that we produce (filtering frontend specific IR nodes)
     if (flags == nullptr)
        {
          flags = new CustomMemoryPoolDOTGeneration::s_Filter_Flags();
        }
     generateWholeGraphOfAST_filteredFrontendSpecificNodes(filename, flags);
   }

void
generateWholeGraphOfAST_filteredFrontendSpecificNodes( string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags)
   {
     std::set<SgNode*> skippedNodeSet = SageInterface::getFrontendSpecificNodes();
     SimpleColorMemoryPoolTraversal::generateGraph(filename,skippedNodeSet, flags);
   }

void
generateGraphOfAST( SgProject* project, string filename, set<SgNode*> skippedNodeSet )
   {
     SimpleColorFilesTraversal::generateGraph(project,filename,skippedNodeSet);
   }

void
generateGraphOfAST( SgProject* project, string filename )
   {
     set<SgNode*> emptyNodeSet;
     SimpleColorFilesTraversal::generateGraph(project,filename,emptyNodeSet);
   }
