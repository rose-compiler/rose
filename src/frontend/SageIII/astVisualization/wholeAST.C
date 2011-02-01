// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

#include "wholeAST.h"

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   #include "AsmUnparser_compat.h"
#endif

#include "merge.h"

// **********************************************************
// **********************************************************
//       Source code which can be more later
// **********************************************************
// **********************************************************

using namespace std;

#if 1
CustomAstDOTGeneration::~CustomAstDOTGeneration()
   {
   }
#endif

void
CustomAstDOTGenerationData::addNode(NodeType n)
   {
     ROSE_ASSERT(this != NULL);
  // printf ("In CustomAstDOTGenerationData::addNode(): n->target = %p = %s options = %s size_ld = %ld size_d = %d \n",
  //      n.target,n.target->class_name().c_str(),n.optionString.c_str(),n.optionString.size(),n.optionString.size());
     ROSE_ASSERT(n.optionString.size() < 4000);
     nodeList.push_back(n);
   }

void
CustomAstDOTGenerationData::addEdge(EdgeType e)
   {
     ROSE_ASSERT(this != NULL);
     edgeList.push_back(e);
   }

void 
CustomAstDOTGenerationData::addEdges()
   {
  // Add the local list of edges to the dot graph

     for (std::list<EdgeType>::iterator i = edgeList.begin(); i != edgeList.end(); i++)
        {
          ROSE_ASSERT ((*i).start != NULL);
          ROSE_ASSERT ((*i).end   != NULL);
       // printf ("Adding an edge from a %s to a %s with options = %s \n",
       //      (*i).start->sage_class_name(),(*i).end->sage_class_name(),(*i).optionString.c_str());
          dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
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
       // printf ("Adding node label info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("Called for node = %s \n",node->class_name().c_str());

     std::string returnString;
  // printf ("default: returnString = %s size() = %ld nodeList.size() = %ld \n",returnString.c_str(),returnString.size(),nodeList.size());

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.begin();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
       // printf ("Before: Adding node option info (%s) to %s (*i).optionString.size() = %ld \n",returnString.c_str(),node->sage_class_name(),(*i).optionString.size());
          ROSE_ASSERT(i->target == node);
          returnString = (*i).optionString;
       // printf ("Adding node option info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

  // printf ("returnString = %s size() = %ld \n",returnString.c_str(),returnString.size());

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
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalEdgeOptions(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("CustomAstDOTGenerationData::additionalEdgeOptions from = %p = %s -> label = %s -> to = %p = %s \n",
  //      from,from->class_name().c_str(),label.c_str(),to,to->class_name().c_str());

     std::string returnString;
#if 0
  // This will find the edge in the edge list, but the edgList is always empty, unless we 
  // add edges explicitly.  So this code is not the way to identify edges. Instead we
  // should just have the base class generate an empty string.

     const EdgeType tmp (from,to,label,"");
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).optionString;
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }
#endif
     return returnString;
   }

std::string
CustomAstDOTGenerationData::unparseToCompleteStringForDOT( SgNode* astNode )
   {
  // Generate the std::string (pass a SgUnparse_Info object)
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code
     std::string outputString = globalUnparseToString(astNode,inputUnparseInfoPointer);
     std::string returnString = StringUtility::escapeNewLineCharaters(outputString);

     delete inputUnparseInfoPointer;

     return returnString;
   }

void
CustomAstDOTGenerationData::internalGenerateGraph( std::string filename, SgProject* project )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "CustomAstDOTGenerationData::internalGenerateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Required setup on base class
     init();

  // Specify the traversal that will be used (I forget why this is required)
  // DOTgraph.traversal = DOTGeneration::TOPDOWNBOTTOMUP;
     traversal = AstDOTGeneration::TOPDOWNBOTTOMUP;

  // Build the DOT specific inherited attribute
     DOTInheritedAttribute ia;

     traverse(project,ia);
  // traverseInputFiles(project,ia);

  // Put extra code here
     addEdges();

  // Output the source code to a new node (we need a function to escape all the """ and "\n")
  // std::string sourceCodeString = unparseToCompleteStringForDOT(project);
  // dotrep.addNode(project,sourceCodeString,"shape=box");

     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomAstDOTGeneration::addNode( NodeType n )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(n.optionString.size() < 4000);
     DOTgraph.addNode(n);
   }

void
CustomAstDOTGeneration::addEdge( EdgeType e )
   {
     ROSE_ASSERT(this != NULL);
     DOTgraph.addEdge(e);
   }

void
CustomAstDOTGeneration::internalGenerateGraph( std::string filename, SgProject* project )
   {
     ROSE_ASSERT(this != NULL);
     DOTgraph.internalGenerateGraph(filename,project);
   }











#if 1
CustomMemoryPoolDOTGenerationData::~CustomMemoryPoolDOTGenerationData() 
   {
   }
#endif

void
CustomMemoryPoolDOTGenerationData::internalGenerateGraph(std::string dotfilename)
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "CustomMemoryPoolDOTGenerationData::internalGenerateGraph(" + dotfilename + "):";
     TimingPerformance timer (label);

     filename = dotfilename;

  // printf ("skipNodeList.size() = %ld \n",(long)skipNodeList.size());

#if 0
     std::set<SgNode*>::iterator i = skipNodeList.begin();
     while(i != skipNodeList.end())
        {
          printf ("     *i = %p = %s \n",*i,(*i)->class_name().c_str());
          Sg_File_Info* fileInfo = isSg_File_Info(*i);
          if (fileInfo != NULL)
             {
                if (fileInfo->get_parent() != NULL)
                   {
                     printf ("parent = %p = %s \n",fileInfo->get_parent(),fileInfo->get_parent()->class_name().c_str());
                     fileInfo->display("debugging CustomMemoryPoolDOTGenerationData");
                   }
             }
          i++;
        }
#endif

     traverseMemoryPool();
     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomMemoryPoolDOTGenerationData::addNode(NodeType n)
   {
  // printf ("Adding an node in CustomMemoryPoolDOTGenerationData::addNode \n");
     nodeList.push_back(n);
   }

void
CustomMemoryPoolDOTGenerationData::addEdge(EdgeType e)
   {
  // printf ("Adding an edge in CustomMemoryPoolDOTGenerationData::addEdge \n");
     edgeList.push_back(e);
   }

void
CustomMemoryPoolDOTGenerationData::skipNode(SgNode* n)
   {
#if 1
  // This is the normal (non-debugging) mode.
     skipNodeList.insert(n);
#else
  // DQ (7/26/2010): For debugging we want to include the frontend IR nodes so that we can debug the type table.
     printf ("Avoid skipping Frontend specific IR nodes to support debugging: avoid skipping %p = %s \n",n,n->class_name().c_str());
     if (isSg_File_Info(n) != NULL)
          skipNodeList.insert(n);
#endif
  // visitedNodes.insert(n);
   }

void
CustomMemoryPoolDOTGenerationData::skipEdge(EdgeType e)
   {
     skipEdgeSet.insert(e);
   }

void 
CustomMemoryPoolDOTGenerationData::addEdges()
   {
  // Add the local list of edges to the dot graph
  // printf ("CustomAstDOTGenerationData::addEdges(): skipEdgeSet.size() = %ld \n",skipEdgeSet.size());

     for (std::list<EdgeType>::iterator i = edgeList.begin(); i != edgeList.end(); i++)
        {
          ROSE_ASSERT ((*i).start != NULL);
          ROSE_ASSERT ((*i).end   != NULL);
       // printf ("Adding an edge from a %s to a %s with options = %s \n",
       //      (*i).start->sage_class_name(),(*i).end->sage_class_name(),(*i).optionString.c_str());
#if 1
//          dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
#else
          EdgeType e((*i).start,(*i).end);
          if (skipEdgeSet.find(e) != skipEdgeSet.end())
             {
               dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
             }
#endif
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
       // printf ("Adding node label info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("Called for node = %s \n",node->class_name().c_str());

     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end(); 
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).optionString;
       // printf ("Adding node option info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

#if 0
// These are inferior defaults (I think).
// To improve the default output add additional information here
// Note you need to add "\\n" for newline
string
CustomMemoryPoolDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
     ostringstream ss;
     ss << "\\n";

  // print number of max successors (= container size)
     AstSuccessorsSelectors::SuccessorsContainer c;
     AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
     ss << c.size() << "\\n";

  // add class name
     if (SgClassDeclaration* n = dynamic_cast<SgClassDeclaration*>(node))
        {
          ss << n->get_qualified_name().str() << "\\n";
        }

  // add memory location of node to dot output
     ss << node << "\\n";

     return ss.str();
   }

string
CustomMemoryPoolDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // return an empty string for default implementation
     return string("");
   }
#endif

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
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(from = %s to %s) label = %s edgeList.size() = %ld \n",from->class_name().c_str(),to->class_name().c_str(),label.c_str(),edgeList.size());

     std::string returnString;
#if 0
  // This will find the edge in the edge list, but the edgList is always empty, unless we 
  // add edges explicitly.  So this code is not the way to identify edges. Instead we
  // should just have the base class generate an empty string.

  // Build an example edge so that we can test againt it in the edge list
     const EdgeType tmp (from,to,label);
     std::list<EdgeType>::iterator i = edgeList.end();

  // Find the unique edge between the associated IR nodes that also matches the given label
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = i->optionString;
          printf ("i->optionString = %s \n",i->optionString.c_str());

       // DQ (3/5/2007): color the parent edges blue
          if (from->get_parent() == to)
             {
            // Note that labelfontcolor does not appear to work
            // returnString = "color=\"blue\" decorate labelfontcolor=blue4";
            // returnString = "color=\"blue\" decorate";
               returnString = "color=\"blue\" decorate labelfontcolor=\"blue4\"";
             }

       // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
       // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
          SgStatement* statement = isSgStatement(from);
          if (statement != NULL)
             {
               if (statement->hasExplicitScope() == true && statement->get_scope() == to)
                  {
                 // returnString = "color=\"green\" decorate labelfontcolor=green4";
                    returnString = "color=\"green\" decorate";
                  }
             }
     
          SgInitializedName* initializedName = isSgInitializedName(from);
          if (initializedName != NULL)
             {
               if (initializedName->get_scope() == to)
                  {
                 // returnString = "color=\"green\" decorate labelfontcolor=green4";
                    returnString = "color=\"green\" decorate";
                  }
             }

          SgType* type = isSgType(to);
          if (type != NULL)
             {
            // returnString = "color=\"gold1\" decorate labelfontcolor=gold4";
               returnString = "color=\"gold1\" decorate";
             }

          printf ("Adding edge option info (%p) to (%p) returnString = %s \n",from,to,returnString.c_str());
        }
#else
  // DQ (3/6/2007): This builds a default set of mappings of edge colors and edge options to edges.

  // Color all edges that lead to a SgType (this is overwritten for the parent edges)
     SgType* type = isSgType(to);
     if (type != NULL)
        {
       // returnString = "color=\"gold1\" decorate labelfontcolor=gold4";
          returnString = "color=\"gold1\" decorate ";
        }

  // DQ (3/5/2007): color the parent edges blue
     if (from->get_parent() == to && label == "parent")
        {
       // Note that labelfontcolor does not appear to work
       // returnString = "color=\"blue\" decorate labelfontcolor=blue4";
       // returnString = "color=\"blue\" decorate labelfontcolor=\"blue4\"";
          returnString = "color=\"blue\" decorate ";
        }

  // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
  // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
     SgStatement* statement = isSgStatement(from);
     if (statement != NULL)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to && label == "scope")
             {
            // returnString = "color=\"green\" decorate labelfontcolor=green4";
               returnString = "color=\"green\" decorate ";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != NULL)
        {
          if (initializedName->get_scope() == to && label == "scope")
             {
            // returnString = "color=\"green\" decorate labelfontcolor=green4";
               returnString = "color=\"green\" decorate ";
             }
        }

  // returnString += "decorate labelfontcolor=red";
#endif
     return returnString;
   }


void
CustomMemoryPoolDOTGenerationData::visit(SgNode* node)
   {
     ROSE_ASSERT(node!= NULL);

  // printf ("CustomMemoryPoolDOTGenerationData::visit(): node = %p = %s \n",node,node->class_name().c_str());

     bool ignoreThisIRnode = false;
  // bool ignoreThisIRnode = true;
#if 0
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
            // visitedNodes.insert(node);  I don't think we have to do this, it will be added at the end of the function
               ignoreThisIRnode = true;
             }
        }
#endif

#if 1
  // If the node is to be skipped then set: ignoreThisIRnode = true
     if ( skipNodeList.find(node) != skipNodeList.end() )
          ignoreThisIRnode = true;
#endif

     if (visitedNodes.find(node) == visitedNodes.end() && ignoreThisIRnode == false )
        {
       // This node has not been visited previously (and should not be ignored)
          std::string nodeoption;
          std::string nodelabel = std::string("\\n") + typeid(*node).name();

       // These is both virtual function calls
          nodelabel  += additionalNodeInfo(node);
          nodeoption += additionalNodeOptions(node);

       // DQ (5/9/2006): Added skipNodeList to permit filtering of IR nodes
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
#if 0
               if (isSgFunctionDeclaration(node) != NULL)
                  {
                    printf ("From: node = %p = %s  To: node = %p %s \n",node,node->class_name().c_str(),n,(n != NULL) ? n->class_name().c_str() : "NULL value");
                  }
#endif

#if 0
               if ( VALGRIND_CHECK_DEFINED(n) )
                  {
                    fprintf(stderr, "Found uninitialized value of i %u of node %p (%s) \n", (unsigned int)(i-listOfIRnodes.begin()),node,node->class_name().c_str());
                  }
#endif
               if ( n == NULL)
                  {
                    if ( skipNodeList.find(node) == skipNodeList.end() )
                       {
                         dotrep.addNullValue(node,"",edgelabel,"");
                       }
                  }
                 else
                  {
                 // Only build the edge if the node is not on the list of IR nodes to skip
                 // We can't use the visitedNodes list since just be cause we have visited
                 // the IR node does not imply that we should skip the edge (quite the opposent!)
                 // if ( skipNodeList.find(n) == skipNodeList.end() )
                    if ( skipNodeList.find(n) == skipNodeList.end() )
                       {
                      // printf ("Edge is being output \n");
                         bool ignoreEdge = false;
#if 0
                         Sg_File_Info* fileInfo = n->get_file_info();
                         if (fileInfo != NULL)
                            {
                           // Check if this is a GNU compatability IR node generated just 
                           // to support frontend compatabilty but should be ignored.
                           // Since such IR nodes are ignored (not assembled into the DOT 
                           // graph, we want to ignore any edges to them as well.
                              ignoreEdge = fileInfo->isFrontendSpecific();
                            }
#endif

#if 0
                      // if (edgelabel == "header")
                      //    {
                              EdgeType edge(node,n,edgelabel);
                              ignoreEdge = (skipEdgeSet.find(edge) == skipEdgeSet.end());
                              printf ("node = %p n = %p edgelabel = %s ignoreEdge = %s \n",node,n,edgelabel.c_str(),ignoreEdge ? "true" : "false");
                      //    }
#endif
#if 0
                         if (edgelabel == "header")
                              ignoreEdge = true;
#endif

                         if (ignoreEdge == false)
                            {
                           // DQ (3/5/2007): Support for edge options
                           // These are both virtual function calls
                              std::string additionalEdgeOption = additionalEdgeOptions(node,n,edgelabel);
                              std::string additionalEdgeLabel  = additionalEdgeInfo(node,n,edgelabel);

                           // dotrep.addEdge ( node, edgelabel + additionalEdgeLabel, n, additionalEdgeOption + "dir=forward" );
                              dotrep.addEdge ( node, edgelabel + additionalEdgeLabel, n, additionalEdgeOption + "dir=forward" );
                           // printf ("Added edge using %p -> label = %s -> %p option = %s \n",node, (edgelabel + additionalEdgeLabel).c_str(), n, (additionalEdgeOption + "dir=forward").c_str());
                            }
                       }
                      else
                       {
                      // printf ("Edge is being skipped \n");
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

#if 0
CustomMemoryPoolDOTGeneration::~CustomMemoryPoolDOTGeneration() 
   {
   }
#endif

CustomMemoryPoolDOTGeneration::CustomMemoryPoolDOTGeneration()
{
  internal_init (NULL);
}
CustomMemoryPoolDOTGeneration::CustomMemoryPoolDOTGeneration(s_Filter_Flags* f /*= NULL*/)
{
  internal_init (f);
}

void CustomMemoryPoolDOTGeneration::internal_init(s_Filter_Flags* f /*= NULL*/)
{
   if (f == NULL)
     filterFlags = new s_Filter_Flags();
   else
     filterFlags = f;
}

void
CustomMemoryPoolDOTGeneration::addNode( NodeType n )
   {
  // printf ("Adding an node in CustomMemoryPoolDOTGeneration::addNode \n");
     DOTgraph.addNode(n);
   }

void
CustomMemoryPoolDOTGeneration::addEdge( EdgeType e )
   {
  // printf ("Adding an edge in CustomMemoryPoolDOTGeneration::addEdge \n");
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
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }

     if (edgeName == "scope")
        {
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }
#if 0
  // DQ (8/15/2008): This does not appear to be working, I don't know why.

  // Filter out the edges between SgAsmElfSections and their SgAsmElfFileHeader
     if (edgeName == "header")
        {
          printf ("In CustomMemoryPoolDOTGeneration::edgeFilter(): Skipping header edges \n");
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }

  // Filter out the edges between SgAsmElfSections and their SgAsmElfFileHeader
     if (edgeName == "file")
        {
          printf ("In CustomMemoryPoolDOTGeneration::edgeFilter(): Skipping file edges \n");
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }
#endif

#if 1
  // DQ (1/24/2007): Added skipNodeList to permit filtering of IR nodes, if the source node is not present 
  // then we don't have an edge to skip, if the source node is present and the sink node is to be skipped, 
  // then skip the edge.
  // printf ("CustomMemoryPoolDOTGeneration::edgeFilter(): testing edge from %p = %s to %p = %s \n",nodeSource,nodeSource->class_name().c_str(),nodeSink,nodeSink->class_name().c_str());
     if ( DOTgraph.skipNodeList.find(nodeSource) != DOTgraph.skipNodeList.end() )
        {
          if ( nodeSink != NULL && DOTgraph.skipNodeList.find(nodeSink) != DOTgraph.skipNodeList.end() )
             {
            // printf ("CustomMemoryPoolDOTGeneration::edgeFilter(): Skipping this edge since it is to a node that is skipped nodeSink = %p = %s \n",nodeSink,nodeSink->class_name().c_str());

            // ROSE_ASSERT(false);

            // skipEdge(nodeSource,nodeSink,edgeName);
               EdgeType edge(nodeSource,nodeSink,edgeName);
               skipEdge(edge);
             }
        }
#endif
#if 0
     printf ("Exiting as a test in CustomMemoryPoolDOTGeneration::edgeFilter() \n");
     ROSE_ASSERT(false);
#endif
   }

void
CustomMemoryPoolDOTGeneration::typeFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgType(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgFunctionParameterTypeList(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgTypedefSeq(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::symbolFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgSymbol(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::asmFileFormatFilter(SgNode* node)
   {
  // DQ (10/18/2009): Added support to skip output of binary file format in generation of AST visualization.
     if (isSgAsmExecutableFileFormat(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::asmTypeFilter(SgNode* node)
   {
  // DQ (10/18/2009): Added support to skip output of binary expression type information in generation of AST visualization.
     if (isSgAsmType(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::emptySymbolTableFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgSymbolTable(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::expressionFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgExpression(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::variableDefinitionFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgVariableDefinition(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::variableDeclarationFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgVariableDeclaration(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgInitializedName(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgStorageModifier(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgFunctionParameterList(node) != NULL)
        {
          skipNode(node);
        }

     variableDefinitionFilter(node);
   }


void
CustomMemoryPoolDOTGeneration::ctorInitializerListFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     if (isSgCtorInitializerList(node) != NULL)
        {
          skipNode(node);
        }
   }


void
CustomMemoryPoolDOTGeneration::binaryExecutableFormatFilter(SgNode* node)
   {
  // This function skips the representation of specific kinds of IR nodes 
#if 0
     if (isSgAsmElfSectionTableEntry(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmPESectionTableEntry(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmPERVASizePair(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmPEImportHintName(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmElfSymbolList(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmGenericFile(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSgAsmCoffSymbol(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
  // Use this as a way to reduce the number of IR nodes in the generated AST to simplify debugging.
     SgAsmElfSection* elfSection = isSgAsmElfSection(node);
     SgAsmElfDynamicSection* elfDynamicSection = isSgAsmElfDynamicSection(node);
     if (elfSection != NULL && elfSection->get_name()->get_string() != ".text" && elfSection->get_name()->get_string() != ".data" && elfDynamicSection == NULL )
        {
          skipNode(node);
        }
#endif
#if 0
     SgAsmGenericSymbol* symbol = isSgAsmGenericSymbol(node);
  // if (symbol != NULL && symbol->get_name() != ".text" && symbol->get_name() != ".data" && symbol->get_name().find("start") == std::string::npos)
     if (symbol != NULL && symbol->get_name()->get_string() != ".text" && symbol->get_name()->get_string() != ".data" && symbol->get_name()->get_string().find("__s") == std::string::npos)
        {
          skipNode(node);
        }
#endif
   }

void
CustomMemoryPoolDOTGeneration::commentAndDirectiveFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();
          if (comments != NULL)
             {
            // printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
               AttachedPreprocessingInfoType::iterator i;
               for (i = comments->begin(); i != comments->end(); i++)
                  {
                    ROSE_ASSERT ( (*i) != NULL );
#if 0
                    printf ("          Attached Comment (relativePosition=%s): %s",
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         (*i)->getString().c_str());
#endif
                    ROSE_ASSERT((*i)->get_file_info() != NULL);
                    skipNode((*i)->get_file_info());

                  }
             }
            else
             {
#if 0
               printf ("No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::fileInfoFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     Sg_File_Info* fileInfoNode = isSg_File_Info(node);
     if (fileInfoNode != NULL)
        {
          skipNode(fileInfoNode);
        }
   }

void
CustomMemoryPoolDOTGeneration::frontendCompatibilityFilter(SgNode* node)
   {
  // DQ (5/11/2006): This filters out the gnu compatability IR nodes (which tend to confuse everyone!)

     ROSE_ASSERT(node != NULL);

#if 0
  // DQ (5/11/2006): Skip IR nodes that are explicitly marked as frontend specific
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
               skipNode(node);
             }
        }
#endif

  // Mark all SgFunctionSymbol IR nodes that are associated with the a frontend specific function
     SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(node);
     if (functionSymbol != NULL)
        {
          SgDeclarationStatement* declaration = functionSymbol->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          if (declaration->get_file_info() == NULL)
             {
               printf ("Error: declaration->get_file_info() == NULL declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }
          ROSE_ASSERT(declaration->get_file_info() != NULL);
          if (declaration != NULL && declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(functionSymbol);
             }
        }

     SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(node);
     if (typedefSymbol != NULL)
        {
          SgDeclarationStatement* declaration = typedefSymbol->get_declaration();
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(typedefSymbol);
             }
        }

     SgVariableSymbol* variableSymbol = isSgVariableSymbol(node);
     if (variableSymbol != NULL)
        {
          SgInitializedName* declaration = variableSymbol->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(variableSymbol);
            // This is redundant
            // skipNode(declaration);
             }

          SgStorageModifier & storageModifier = declaration->get_storageModifier();
          skipNode(&storageModifier);
        }

     SgFunctionParameterList* functionParateterList = isSgFunctionParameterList(node);
     if (functionParateterList != NULL)
        {
          SgInitializedNamePtrList::iterator i =        functionParateterList->get_args().begin();
          while (i != functionParateterList->get_args().end())
             {
               SgInitializedName* declaration = *i;
               ROSE_ASSERT(declaration != NULL);
               if (declaration->get_file_info()->isFrontendSpecific() == true)
                  {
                 // skipNode(declaration);
                    SgStorageModifier & storageModifier = declaration->get_storageModifier();
                    skipNode(&storageModifier);
                  }

               i++;
             }
        }

  // Skip the Sg_File_Info objects that are associated with frontend specific IR nodes.
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
               printf ("skip fileInfo = %p parent is %p = %s \n",fileInfo,node,node->class_name().c_str());
            // skipNode(fileInfo);
               skipNode(node);
             }
            else
             {
            // printf ("Node %p = %s has normal Sg_File_Info object with parent = %s \n",node,node->class_name().c_str(),(fileInfo->get_parent() != NULL) ? fileInfo->get_parent()->class_name().c_str() : "NULL value");
            // fileInfo->display("extranious Sg_File_Info object");
             }
        }

  // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != NULL)
        {
       // skipNode(currentNodeFileInfo);
          if (currentNodeFileInfo->isFrontendSpecific() == true)
             {
               skipNode(currentNodeFileInfo);
             }
#if 0
            else
             {
               string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
            // Make this statement different in the generated dot graph
               string labelWithSourceCode = "\\n" + currentNodeFileInfo->get_filenameString() + 
                                            "\\n" +  StringUtility::numberToString(currentNodeFileInfo) + "  ";

               NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

               addNode(graphNode);
             }
#endif
        }

#if 0
     if (isSgType(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
          
          string labelWithSourceCode;
          if (isSgClassType(node) == NULL)
               labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
       // addNode(graphNode);

       // skipNode(node);
        }
#endif

     if (isSgModifier(node) != NULL)
        {
       // skipNode(node);
        }

#if 0
     if (isSgSymbol(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSg_File_Info(node) != NULL)
        {
          skipNode(node);
        }
#endif
     if (isSgTypedefSeq(node) != NULL)
        {
       // skipNode(node);
        }

     if (isSgFunctionParameterTypeList(node) != NULL)
        {
       // skipNode(node);
        }
   }


void
CustomMemoryPoolDOTGeneration::defaultFilter(SgNode* node)
   {
  // Default fileter to simplify the whole AST graph

  // printf ("Exiting in CustomMemoryPoolDOTGeneration::defaultFilter() \n");
  // ROSE_ASSERT(false);
#if 0
     frontendCompatibilityFilter(node);
#endif
   if (filterFlags->m_commentAndDirective == 1)
     commentAndDirectiveFilter(node);

   if (filterFlags->m_binaryExecutableFormat == 1)
     binaryExecutableFormatFilter(node);

   if (filterFlags->m_edge== 1)
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
   }

void
CustomMemoryPoolDOTGeneration::defaultColorFilter(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(statement->variantT())
             {
               case V_SgFunctionDeclaration:
               case V_SgProgramHeaderStatement:
               case V_SgProcedureHeaderStatement:
               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationFunctionDecl:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=royalblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " +  StringUtility::numberToString(functionDeclaration) + "  ";
                 // printf ("########## functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
                    break;
                  }

               case V_SgTemplateDeclaration:
                  {
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + templateDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(templateDeclaration) + "  ";
                    break;
                  }

               case V_SgClassDeclaration:
               case V_SgModuleStatement:
               case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                 // printf ("########## classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + typedefDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(typedefDeclaration) + "  ";
                    break;
                  }

               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + enumDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(enumDeclaration) + "  ";
                    break;
                  }

               default:
                  {
                 // It appears that we can't unparse one of these (not implemented)
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() + "  ";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
#if 0
                 // DQ (5/14/2006): this is an error when processing stdio.h
                 // DQ (5/14/2006): This fails for SgClassDeclaration
                 // if (isSgVariableDefinition(statement) == NULL)
                    if ( (isSgVariableDefinition(statement) == NULL) && (isSgClassDeclaration(node) == NULL) )
                         labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
#endif
                    break;
                  }
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgExpression(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;
          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          SgValueExp* valueExp = isSgValueExp(node);
          if (valueExp != NULL)
             {
               if (valueExp->get_parent() == NULL)
                  {
                    printf ("Error: valueExp = %p = %s valueExp->get_parent() == NULL \n",valueExp,valueExp->class_name().c_str());
                    valueExp->get_file_info()->display("Error: valueExp->get_parent() == NULL");

                 // In the case of a SgIntVal, we can try to output a little more information
                    SgIntVal* intVal = isSgIntVal(valueExp);
                    if (intVal != NULL)
                       {
                         printf ("Error: intVal = %d \n",intVal->get_value());
                       }
                    SgUnsignedLongVal* unsignedLongVal = isSgUnsignedLongVal(valueExp);
                    if (unsignedLongVal != NULL)
                       {
                         printf ("Error: unsignedLongVal = %lu \n",unsignedLongVal->get_value());
                       }
                  }
               ROSE_ASSERT(valueExp->get_parent() != NULL);
            // labelWithSourceCode = "\\n value = " + valueExp->unparseToString() + "\\n" + StringUtility::numberToString(node) + "  ";
            // labelWithSourceCode = string("\\n value = nnn") + "\\n" + StringUtility::numberToString(node) + "  ";

            // DQ (10/4/2010): Output the value so that we can provide more information.
               labelWithSourceCode += string("\\n value = ") + valueExp->get_constant_folded_value_as_string() + "  ";
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgType(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";

       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;
       // printf ("Graph this node (%s) \n",node->class_name().c_str());
          if (isSgClassType(node) == NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
            // labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
             }

          SgTypeDefault* defaultType = isSgTypeDefault(node);
          if (defaultType != NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
               labelWithSourceCode += string("\\n  name = ") + defaultType->get_name().str() + "  ";
             }

       // labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     SgSupport* supportNode = isSgSupport(node);
     if (supportNode != NULL)
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
                 // printf ("########## SgProject = %s \n",project->get_outputFileName().c_str());
                    break;
                  }

            // case V_SgFile:
               case V_SgSourceFile:
               case V_SgBinaryComposite:
                  {
                    SgFile* file = isSgFile(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + file->get_sourceFileNameWithPath() + "  ";
                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(file) + "  ";
                 // printf ("########## SgFile = %s \n",file->get_sourceFileNameWithPath().c_str());
                    ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
                 // printf ("     SgNode::SgFunctionTypeTable = %p size = %ld \n",SgNode::get_globalFunctionTypeTable(),SgNode::get_globalFunctionTypeTable()->get_function_type_table()->size());
                    break;
                  }

               case V_Sg_File_Info:
                  {
                 // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
                    Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
                    if (currentNodeFileInfo != NULL)
                       {
                      // skipNode(currentNodeFileInfo);
                         if (currentNodeFileInfo->isFrontendSpecific() == true)
                            {
                           // skipNode(currentNodeFileInfo);
                            }
                           else
                            {
                           // additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
                              additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=brown,fontname=\"7x13bold\",fontcolor=black,style=filled";
                           // Make this statement different in the generated dot graph
                              labelWithSourceCode = 
                                 "\\n" + currentNodeFileInfo->get_filenameString() + 
                                 "\\n line: " +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                 " column: " +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n raw line: " +  StringUtility::numberToString(currentNodeFileInfo->get_raw_line()) + 
                                 " raw column: " +  StringUtility::numberToString(currentNodeFileInfo->get_raw_col()) + "  " +
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
                 // printf ("########## symbolTable->get_name() = %s \n",symbolTable->get_name().str());
                    break;
                  }

               case V_SgTypedefSeq:
                  {
                    SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(typedefSeq) + "  ";
                 // printf ("########## typedefSeq->get_name() = %s \n",typedefSeq->get_name().str());
                    break;
                  }

               case V_SgTemplateArgument:
                  {
                    SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
                    additionalNodeOptions = "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(templateArgument) + "  ";
                    break;
                  }

               default:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // labelWithSourceCode;
                    break;
                  }
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
// Liao 11/5/2010, move out of SgSupport
       if (isSgInitializedName(node) != NULL) 
//       case V_SgInitializedName:
          {
            SgInitializedName* initializedName = isSgInitializedName(node);
            string additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=darkturquoise,fontname=\"7x13bold\",fontcolor=black,style=filled";
            string labelWithSourceCode = string("\\n  ") + initializedName->get_name().getString() +
                                  string("\\n  ") + StringUtility::numberToString(initializedName) + "  ";
         // printf ("########## initializedName->get_name() = %s \n",initializedName->get_name().str());
 //           break;
            NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
            addNode(graphNode);
          }


#if 1
     if (isSgSymbol(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"blue\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;

          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

          addNode(graphNode);
        }
#endif

  // DQ (3/25/2007): Added support for graphs including SgToken IR nodes.
     if (isSgToken(node) != NULL)
        {
          SgToken* token = isSgToken(node);
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"blue\",fillcolor=moccasin,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;

          std::string token_classification_string = SgToken::ROSE_Fortran_keyword_map [token->get_classification_code()].token_lexeme;
          labelWithSourceCode = string("\\n  ") + token->get_lexeme_string() + "  " + 
                                string("\\n  ") + token_classification_string + "  " + 
                                string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

          addNode(graphNode);
        }

     if (isSgAsmType(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";

       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;
       // printf ("Graph this node (%s) \n",node->class_name().c_str());
          if (isSgClassType(node) == NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
            // labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
             }

       // labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgAsmNode(node) != NULL)
        {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
       // Color selection for the binary file format and binary instruction IR nodes.

          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

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
                 // printf ("########## genericSection->get_name() = %s \n",genericSection->get_name().c_str());
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
                 // labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() + "  ";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
#if 0
                 // DQ (5/14/2006): this is an error when processing stdio.h
                 // DQ (5/14/2006): This fails for SgClassDeclaration
                 // if (isSgVariableDefinition(statement) == NULL)
                    if ( (isSgVariableDefinition(statement) == NULL) && (isSgClassDeclaration(node) == NULL) )
                         labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
#endif
                    break;
                  }
             }

       // DQ (10/18/2009): Added support to provide more information in the generated graphs of the AST.
          SgAsmInstruction* asmInstruction = isSgAsmInstruction(node);
          if (asmInstruction != NULL)
             {
            // string mnemonicString      = asmInstruction->get_mnemonic();

            // Note that unparsing of instructions is inconsistant with the rest of ROSE.
            // string unparsedInstruction = asmInstruction->unparseToString();

               string unparsedInstruction = unparseInstruction(asmInstruction);

               string addressString       = StringUtility::numberToString( (void*) asmInstruction->get_address() );

               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
               labelWithSourceCode = "\\n  " + unparsedInstruction + 
                                     "\\n  address: " + addressString +
                                     "\\n  (generated label: " +  StringUtility::numberToString(asmInstruction) + ")  ";
             }

       // DQ (10/18/2009): Added support to provide more information in the generated graphs of the AST.
          SgAsmExpression* asmExpression = isSgAsmExpression(node);
          if (asmExpression != NULL)
             {
               string unparsedExpression = unparseExpression(asmExpression);

               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
               labelWithSourceCode = "\\n  " + unparsedExpression + 
                                     "\\n  (generated label: " +  StringUtility::numberToString(asmExpression) + ")  ";
             }


          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
#else
          printf ("Warning: In wholeAST.C ROSE_BUILD_BINARY_ANALYSIS_SUPPORT is not defined \n");
#endif
        }



#if 0
  // DQ (3/5/2007): Mark the parent edge in a different color, this does not appear to work!!!!
     if (node->get_parent() != NULL)
        {
          printf ("Adding a colored edge to the parent from = %p to = %p \n",node,node->get_parent());
          string additionalEdgeOptions = "color=\"blue\" decorate labelfontcolor=\"red\"";
          EdgeType graphEdge(node,node->get_parent(),"",additionalEdgeOptions);
          addEdge(graphEdge);
        }
#endif
#if 0
  // DQ (3/6/2007): In general it is difficult to specify edges when all we have is the IR node

  // DQ (3/5/2007): color the parent edges blue
     if (node->get_parent() != NULL)
        {
          returnString = "color=\"blue\"";
        }

  // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
  // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to)
             {
               returnString = "color=\"green\"";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != NULL)
        {
          if (initializedName->get_scope() == to)
             {
               returnString = "color=\"green\"";
             }
        }

     SgType* type = isSgType(to);
     if (type != NULL)
        {
          returnString = "color=\"yellow\"";
        }
#endif
   }


/* Initialize the filters for the default case */
CustomMemoryPoolDOTGeneration::s_Filter_Flags::s_Filter_Flags()
{
  setDefault();
}

// DQ (7/4/2010): This destructor does not appear to have been implemented!
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
     m_expression = 0 ;          /* expressionFilter ()*/

     m_fileInfo =  1;             /* fileInfoFilter ()*/

  // DQ (7/25/2010): Temporary testing (debugging).
  // m_frontendCompatibility = 1;/* frontendCompatibilityFilter()*/
     m_frontendCompatibility = 0;/* frontendCompatibilityFilter()*/

     m_symbol     = 0;           /*symbolFilter ()*/
     m_type    = 0;              /* typeFilter ()*/
     m_variableDeclaration = 0;  /*variableDeclarationFilter()*/

     m_variableDefinition = 0 ;  /*variableDefinitionFilter()*/

#if 1
  // This is the normal (non-debugging) mode.
     m_noFilter = 0;               /* no filtering */
#else
  // DQ (7/25/2010): Temporary testing (debugging).
     printf ("Disable all filtering as a test! \n");
     m_noFilter = 1;               /* no filtering */
#endif
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
   setDefault(); // set default first
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
  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","expressionFilter", m_expression, true);

  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","fileInfoFilter", m_fileInfo, true);
  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","frontendCompatibilityFilter", m_frontendCompatibility, true);
  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","symbolFilter", m_symbol, true);
  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","typeFilter", m_type, true);
  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","variableDeclarationFilter", m_variableDeclaration, true);

  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","variableDefinitionFilter", m_variableDefinition, true);

  CommandlineProcessing::isOptionWithParameter(argvList, "-rose:dotgraph:","noFilter", m_noFilter, true);

#if 1
  if (CommandlineProcessing::isOption(argvList, "-rose:","help", false)
     || CommandlineProcessing::isOption(argvList, "-help","", false)
     || CommandlineProcessing::isOption(argvList, "--help","", false))
  {
    print_commandline_help();
    print_filter_flags();
  }
#endif

}

void
CustomMemoryPoolDOTGeneration::s_Filter_Flags::print_commandline_help()
{
  cout<<"   -rose:help                     show this help message"<<endl;

  cout<<"   -rose:dotgraph:asmFileFormatFilter           [0|1]  Disable or enable asmFileFormat filter"<<endl;
  cout<<"   -rose:dotgraph:asmTypeFilter                 [0|1]  Disable or enable asmType filter"<<endl;
  cout<<"   -rose:dotgraph:binaryExecutableFormatFilter  [0|1]  Disable or enable binaryExecutableFormat filter"<<endl;
  cout<<"   -rose:dotgraph:commentAndDirectiveFilter     [0|1]  Disable or enable commentAndDirective filter"<<endl;
  cout<<"   -rose:dotgraph:ctorInitializerListFilter     [0|1]  Disable or enable ctorInitializerList filter"<<endl;

  cout<<"   -rose:dotgraph:defaultFilter                 [0|1]  Disable or enable default filter"<<endl;
  cout<<"   -rose:dotgraph:defaultColorFilter            [0|1]  Disable or enable defaultColor filter"<<endl;
  cout<<"   -rose:dotgraph:edgeFilter                    [0|1]  Disable or enable edge filter"<<endl;
  cout<<"   -rose:dotgraph:expressionFilter              [0|1]  Disable or enable expression filter"<<endl;
  cout<<"   -rose:dotgraph:fileInfoFilter                [0|1]  Disable or enable fileInfo filter"<<endl;

  cout<<"   -rose:dotgraph:frontendCompatibilityFilter   [0|1]  Disable or enable frontendCompatibility filter"<<endl;
  cout<<"   -rose:dotgraph:symbolFilter                  [0|1]  Disable or enable symbol filter"<<endl;
  cout<<"   -rose:dotgraph:emptySymbolTableFilter        [0|1]  Disable or enable emptySymbolTable filter"<<endl;
  cout<<"   -rose:dotgraph:typeFilter                    [0|1]  Disable or enable type filter"<<endl;
  cout<<"   -rose:dotgraph:variableDeclarationFilter     [0|1]  Disable or enable variableDeclaration filter"<<endl;

  cout<<"   -rose:dotgraph:variableDefinitionFilter      [0|1]  Disable or enable variableDefinitionFilter filter"<<endl;

  cout<<"   -rose:dotgraph:noFilter                      [0|1]  Disable or enable no filtering"<<endl;
}


class SimpleColorMemoryPoolTraversal
   : public CustomMemoryPoolDOTGeneration,
     public ROSE_VisitTraversal
   {
     public:
       // MangledNameMapTraversal::SetOfNodesType     & setOfIRnodes;
          static const set<SgNode*> defaultSetOfIRnodes;
          const set<SgNode*> & setOfIRnodes;

      //! Required traversal function
          void visit (SgNode* node);

          static void generateGraph( string filename, const set<SgNode*> & firstAST = defaultSetOfIRnodes , CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags = NULL);

          void markFirstAST();
          void buildExcludeList();

          SimpleColorMemoryPoolTraversal(const set<SgNode*> & s = defaultSetOfIRnodes, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags = NULL) : CustomMemoryPoolDOTGeneration(flags) , setOfIRnodes(s) {};

       // Destructor defined because base class has virtual members
          virtual ~SimpleColorMemoryPoolTraversal();
   };

const set<SgNode*> SimpleColorMemoryPoolTraversal::defaultSetOfIRnodes;

#if 1
SimpleColorMemoryPoolTraversal::~SimpleColorMemoryPoolTraversal()
   {
   }
#endif

void
SimpleColorMemoryPoolTraversal::generateGraph(string filename, const set<SgNode*> & firstAST, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags /*= NULL*/)
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "SimpleColorMemoryPoolTraversal::generateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Custom control over the coloring of the "whole" AST for a memory pool traversal
     SimpleColorMemoryPoolTraversal traversal(firstAST, flags);
     traversal.traverseMemoryPool();

  // DQ (1/20/2007): We want this to be an exclude mechanism
  // traversal.markFirstAST();
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

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

          NodeType graphNode(*i,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
#if 0
          additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=white,style=filled";
       // Make this statement different in the generated dot graph

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

          NodeType graphNode2(*i,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode2);
#endif
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
#if 0
          printf ("In SimpleColorMemoryPoolTraversal::buildExcludeList(): skipping node = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
#endif
          skipNode(*i);
          i++;
        }
   }

void
SimpleColorMemoryPoolTraversal::visit(SgNode* node)
   {
     ROSE_ASSERT (filterFlags != NULL);

  // DQ (3/2/2010): Test if we have turned off all filtering of the AST.
     if ( filterFlags->m_noFilter == 0) 
        {
       // We allow filitering of the AST.
#if 1
          if ( filterFlags->m_default== 1) 
               defaultFilter(node);
#endif

#if 1
       // DQ (3/1/2009): Uncommented to allow filtering of types.
          if ( filterFlags->m_type == 1) 
               typeFilter(node);
#endif

#if 1
       // DQ (3/2/2009): Remove some more nodes to make the graphs more clear.
          if ( filterFlags->m_expression == 1) 
               expressionFilter(node);
#endif

#if 1
          if ( filterFlags->m_emptySymbolTable == 1) 
               emptySymbolTableFilter(node);
#endif
#if 1
          if ( filterFlags->m_variableDefinition == 1) 
               variableDefinitionFilter(node);
          if ( filterFlags->m_variableDeclaration == 1) 
               variableDeclarationFilter(node);
          if ( filterFlags->m_ctorInitializer == 1) 
               ctorInitializerListFilter(node);
#endif
#if 1
          if ( filterFlags->m_symbol == 1) 
               symbolFilter(node);
#endif

#if 1
  // DQ (10/18/2009): Added support to skip output of binary file format in generation of AST visualization.
          if ( filterFlags->m_asmFileFormat == 1) 
               asmFileFormatFilter(node);
          if ( filterFlags->m_asmType == 1) 
               asmTypeFilter(node);
#endif

#if 0 // this is included inside default filter already
       // Ignore Sg_File_Info objects associated with comments and CPP directives
          if ( filterFlags->m_commentAndDirective == 1) 
               commentAndDirectiveFilter(node);
#endif

#if 1
       // Control output of Sg_File_Info object in graph of whole AST.
          if ( filterFlags->m_fileInfo == 1) 
               fileInfoFilter(node);
#endif

#if 0
       // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
          Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
          if (currentNodeFileInfo != NULL)
             {
            // skipNode(currentNodeFileInfo);
               if (currentNodeFileInfo->isFrontendSpecific() == true)
                  {
                // skipNode(currentNodeFileInfo);
                  }
#if 0
                 else
                  {
                    string additionalNodeOptions;
                 // string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // Make this statement different in the generated dot graph
                    string labelWithSourceCode = 
                                 "\\n" + currentNodeFileInfo->get_filenameString() + 
                                 "\\n line: " +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                 " column: " +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n pointer value: " +  StringUtility::numberToString(currentNodeFileInfo) + "  ";

                    NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

                    addNode(graphNode);
                  }
#endif
             }
#endif
        }

#if 1
     if ( filterFlags->m_defaultColor == 1) 
          defaultColorFilter(node);
#endif

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
               ROSE_ASSERT(currentNodeOptions.size() < 4000);
#endif
             }

      //! Specific constructors are required
      //  SimpleColorFilesInheritedAttribute () {};
      //  SimpleColorFilesInheritedAttribute ( const SimpleColorFilesInheritedAttribute & X ) {};
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
     ROSE_ASSERT(file->get_file_info() != NULL);

  // DQ (3/22/2007): file_id's can be negative for defalut generated files (NULL_FILE, Transforamtions, etc.)
     int file_id = file->get_file_info()->get_file_id();
     int index = 0;
     if (file_id > 0)
        {
       // Select from a small subset of optional setting strings.
          index = (file->get_file_info()->get_file_id() + 0) % maxSize;
        }
  // printf ("SimpleColorFilesInheritedAttribute::setNodeOptions(): index = %d \n",index);
     currentNodeOptions = additionalNodeOptionsArray[index];
     ROSE_ASSERT(currentNodeOptions.size() < 4000);

#if 0
     printf ("SimpleColorFilesInheritedAttribute::setNodeOptions(): index = %d \n",index);
     file->get_file_info()->display("SimpleColorFilesInheritedAttribute::setNodeOptions()");
     printf ("currentNodeOptions = %s \n",currentNodeOptions.c_str());
#endif
   }

class SimpleColorFilesTraversal
// : public CustomMemoryPoolDOTGeneration,
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
     ROSE_ASSERT(node != NULL);
  // printf ("SimpleColorFilesTraversal::evaluateInheritedAttribute(): node = %s specialNodeList.size() = %ld \n",node->class_name().c_str(),specialNodeList.size());
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"blue\",fillcolor=cyan4,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     SgFile* file = isSgFile(node);
     if (file != NULL)
        {
          inheritedAttribute.setNodeOptions(file);
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isShared() == true)
               inheritedAttribute.sharedSubTree = true;
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     if (inheritedAttribute.sharedSubTree == true)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=diamond,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"blue\",fillcolor=magenta1,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     string additionalNodeOptions = inheritedAttribute.currentNodeOptions;
  // string labelWithSourceCode   = "\\n" +  StringUtility::numberToString(node) + "  ";
     string labelWithSourceCode;

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
     if (classDeclaration != NULL)
        {
          labelWithSourceCode   = "\\n" +  classDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
        }
       else
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
          if (functionDeclaration != NULL)
             {
               labelWithSourceCode   = "\\n" +  functionDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
             }
            else
             {
               labelWithSourceCode   = "\\n" +  StringUtility::numberToString(node) + "  ";
             }
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

  // Check if this is an IR node that is to be handled specially
     if (specialNodeList.find(node) != specialNodeList.end())
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"blue\",fillcolor=black,fontname=\"12x24bold\",fontcolor=white,style=filled";
          ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     additionalNodeOptions = inheritedAttribute.currentNodeOptions;
     ROSE_ASSERT(additionalNodeOptions.size() < 4000);

     NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
     ROSE_ASSERT(graphNode.optionString.size() < 4000);
     addNode(graphNode);

     return inheritedAttribute;
   }

void
SimpleColorFilesTraversal::generateGraph(SgProject* project, string filename, set<SgNode*> & specialNodeList )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
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
void   
generateWholeGraphOfAST( string filename)
{
  generateWholeGraphOfAST (filename, NULL);
}

void
generateWholeGraphOfAST( string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags/*= NULL*/)
   {
  // set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
  // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST",skippedNodeSet);

  // DQ (10/29/2009): Added code to output the default flag setting...
  // CustomMemoryPoolDOTGeneration::print_filter_flags();
  // CustomMemoryPoolDOTGeneration::init_filters();

  // DQ: Turn these off for my sake...
  // CustomMemoryPoolDOTGeneration::filterFlags.m_binaryExecutableFormat = 1;/*binaryExecutableFormatFilter()*/
  // CustomMemoryPoolDOTGeneration::filterFlags.m_asmFileFormat = 1;         /*asmFileFormatFilter()*/
  // CustomMemoryPoolDOTGeneration::filterFlags.m_asmType = 1;               /* asmTypeFilter()*/

  // CustomMemoryPoolDOTGeneration::print_filter_flags();

  // Make this the default type of graph that we produce (filtering frontend specific IR nodes)
     if (flags == NULL ) 
        flags = new CustomMemoryPoolDOTGeneration::s_Filter_Flags();
     generateWholeGraphOfAST_filteredFrontendSpecificNodes(filename, flags);
   }

void
generateWholeGraphOfAST_filteredFrontendSpecificNodes( string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags)
   {
#ifdef _MSC_VER
  // DQ (11/27/2009): This appears to be required for MSVC (I think it is correct for GNU as well).
         extern set<SgNode*> getSetOfFrontendSpecificNodes();
#endif

#if 1
  // Normally we want to skip the frontend IR nodes so avoid cluttering the graphs for users.
     set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
#else
  // DQ (7/26/2010): We want to include the frontend IR nodes so that we can debug the type table.
     printf ("Generating an empty set of Frontend specific IR nodes to skip \n");
     set<SgNode*> skippedNodeSet;
#endif
     SimpleColorMemoryPoolTraversal::generateGraph(filename,skippedNodeSet, flags);
   }

#if 1
void
generateGraphOfAST( SgProject* project, string filename, set<SgNode*> skippedNodeSet )
   {
     SimpleColorFilesTraversal::generateGraph(project,filename,skippedNodeSet);
   }
#endif

void
generateGraphOfAST( SgProject* project, string filename )
   {
     set<SgNode*> emptyNodeSet;
     SimpleColorFilesTraversal::generateGraph(project,filename,emptyNodeSet);
   }
#if 0
void generateGraphOfAST_initFilters (std::vector <std::string>& argvList)
{
     CustomMemoryPoolDOTGeneration::init_filters(argvList);
}
#endif
