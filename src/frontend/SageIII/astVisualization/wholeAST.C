// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

#include "transformationTracking.h"
#include "wholeAST.h"

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   #include "AsmUnparser_compat.h"
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "merge.h"
#endif

// **********************************************************
// **********************************************************
//       Source code which can be more later
// **********************************************************
// **********************************************************

using namespace std;
using namespace Rose;

// DQ (4/8/2011): Moved this from the header file...
CustomAstDOTGenerationEdgeType::CustomAstDOTGenerationEdgeType (SgNode* n1, SgNode* n2, std::string label, std::string options)
   : start(n1), end(n2), labelString(label), optionString(options)
   {
  // Nothing to do!
   }

// DQ (4/8/2011): Moved this from the header file...
bool
CustomAstDOTGenerationEdgeType::operator!=(const CustomAstDOTGenerationEdgeType & edge) const
   {
  // return (edge.start != start) || (edge.end != end) || (edge.labelString != labelString);
     bool returnValue = (edge.start != start) || (edge.end != end) || (edge.labelString != labelString);

#if 1
     printf ("In CustomAstDOTGenerationEdgeType::operator!=(): edge.start = %p start = %p edge.end = %p end = %p edge.labelString = %s labelString = %s returnValue = %s \n",
             edge.start,start,edge.end,end,edge.labelString.c_str(),labelString.c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }

// DQ (4/8/2011): Moved this from the header file...
bool
CustomAstDOTGenerationEdgeType::operator==(const CustomAstDOTGenerationEdgeType & edge) const
   {
  // return (edge.start == start) && (edge.end == end) && (edge.labelString == labelString);
     bool returnValue = (edge.start == start) && (edge.end == end) && (edge.labelString == labelString);

#if 1
     printf ("In CustomAstDOTGenerationEdgeType::operator==(): edge.start = %p start = %p edge.end = %p end = %p edge.labelString = %s labelString = %s returnValue = %s \n",
             edge.start,start,edge.end,end,edge.labelString.c_str(),labelString.c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }

// DQ (4/8/2011): Moved this from the header file... This is an issue reported by Insure++.
bool
CustomAstDOTGenerationEdgeType::operator< (const CustomAstDOTGenerationEdgeType & edge) const
   {
  // This function must only be consistant in how it implements the "<" operator.
  // Cast pointer to size_t variables and compare the size_t type variables directly.
     size_t edge_start_size_t = (size_t) edge.start;
     size_t start_size_t      = (size_t) start;
     size_t edge_end_size_t   = (size_t) edge.start;
     size_t end_size_t        = (size_t) start;

  // DQ (11/26/2016): This is a required bug fix to permit the std::set::find() function to work properly.
  // return (edge.start < start) || (((edge.start == start) && (edge.labelString == labelString)) && (edge.end < end)); 
  // return (edge_start_size_t < start_size_t) || (((edge_start_size_t == start_size_t) && (edge.labelString == labelString)) && (edge_end_size_t < end_size_t)); 
  // bool returnValue = (edge_start_size_t < start_size_t) || (((edge_start_size_t == start_size_t) && (edge.labelString == labelString)) && (edge_end_size_t < end_size_t)); 
     bool returnValue = (edge_start_size_t < start_size_t) || 
                        ((edge_start_size_t == start_size_t) && (edge_end_size_t < end_size_t)) || 
                        ((edge_start_size_t == start_size_t) && (edge_end_size_t == end_size_t) && (edge.labelString < labelString) ); 

#if 0
     printf ("In CustomAstDOTGenerationEdgeType::operator<(): edge.start = %p start = %p edge.end = %p end = %p edge.labelString = %s labelString = %s returnValue = %s \n",
             edge.start,start,edge.end,end,edge.labelString.c_str(),labelString.c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }



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
#if 1
          printf ("Adding edge label info (%p) to (%p): returnString = %s \n",from,to,returnString.c_str());
#endif
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

string
CustomAstDOTGenerationData::unparseToCompleteStringForDOT( SgNode* astNode )
   {
  // Generate the std::string (pass a SgUnparse_Info object)
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code
     string outputString = globalUnparseToString(astNode,inputUnparseInfoPointer);

  // DQ (12/8/2016): Implemented new function to address most of Robb's points about this depreicated function.
  // This is part of eliminating a warning that we want to be an error: -Werror=deprecated-declarations.
  // std::string returnString = StringUtility::escapeNewLineCharaters(outputString);
     string returnString = StringUtility::escapeNewlineAndDoubleQuoteCharacters(outputString);

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
#if 0
     printf ("In CustomMemoryPoolDOTGenerationData::skipEdge(): START: e.labelString = %s skipEdgeSet.size() = %zu \n",e.labelString.c_str(),skipEdgeSet.size());
#endif

  // skipEdgeSet.insert(e);
     if (skipEdgeSet.find(e) == skipEdgeSet.end())
        {
          skipEdgeSet.insert(e);
        }
       else
        {
          printf ("ERROR: In CustomMemoryPoolDOTGenerationData::skipEdge(): edge is already present in set \n");
          ROSE_ASSERT(false);
        }

#if 0
     printf ("In CustomMemoryPoolDOTGenerationData::skipEdge(): END: e.labelString = %s skipEdgeSet.size() = %zu \n",e.labelString.c_str(),skipEdgeSet.size());
#endif

  // DQ (11/26/2016): Adding test for find() operator on edge set.
     EdgeType edge(e.start,e.end,e.labelString);
     ROSE_ASSERT(skipEdgeSet.find(edge) != skipEdgeSet.end());
     ROSE_ASSERT(! (skipEdgeSet.find(edge) == skipEdgeSet.end()) );

  // DQ (11/26/2016): Adding test for find() operator on edge set.
     std::set<EdgeType>::iterator element = skipEdgeSet.find(edge);

     ROSE_ASSERT(e.start == edge.start);
     ROSE_ASSERT(e.end == edge.end);
     ROSE_ASSERT(e.labelString == edge.labelString);

     ROSE_ASSERT(e.start == element->start);
     ROSE_ASSERT(e.end == element->end);
     ROSE_ASSERT(e.labelString == element->labelString);
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

#error "DEAD CODE!"

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

#if 0
     ROSE_ASSERT(from != NULL);
     printf ("In CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(): from = %p = %s \n",from,from->class_name().c_str());
     ROSE_ASSERT(to != NULL);
  // printf ("In CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(): to = %p = %s \n",to,to->class_name().c_str());
     printf ("In CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(): SgType::static_variant = %d to = %p \n",(int)SgType::static_variant,to);
     printf ("In CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(): to->variantT() = %d \n",(int)(to->variantT()));
#endif

  // Color all edges that lead to a SgType (this is overwritten for the parent edges)
     SgType* type = isSgType(to);
     if (type != NULL)
        {
       // DQ (2/27/2014): removed "decorate" keyword since it is an error for latest version of zgrviewer (or maybe dot).
       // returnString = "color=\"gold1\" decorate labelfontcolor=gold4";
       // returnString = "color=\"gold1\" decorate ";
          returnString = "color=\"gold1\" ";
        }

  // DQ (3/5/2007): color the parent edges blue
     if (from->get_parent() == to && label == "parent")
        {
       // DQ (2/27/2014): removed "decorate" keyword since it is an error for latest version of zgrviewer (or maybe dot).
       // Note that labelfontcolor does not appear to work
       // returnString = "color=\"blue\" decorate labelfontcolor=blue4";
       // returnString = "color=\"blue\" decorate labelfontcolor=\"blue4\"";
       // returnString = "color=\"blue\" decorate ";
          returnString = "color=\"blue\" ";
        }

  // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
  // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
     SgStatement* statement = isSgStatement(from);
     if (statement != NULL)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to && label == "scope")
             {
            // DQ (2/27/2014): removed "decorate" keyword since it is an error for latest version of zgrviewer (or maybe dot).
            // returnString = "color=\"green\" decorate labelfontcolor=green4";
            // returnString = "color=\"green\" decorate ";
               returnString = "color=\"green\" ";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != NULL)
        {
          if (initializedName->get_scope() == to && label == "scope")
             {
            // DQ (2/27/2014): removed "decorate" keyword since it is an error for latest version of zgrviewer (or maybe dot).
            // returnString = "color=\"green\" decorate labelfontcolor=green4";
            // returnString = "color=\"green\" decorate ";
               returnString = "color=\"green\" ";
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

#if 0
     printf ("CustomMemoryPoolDOTGenerationData::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

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
               printf ("In CustomMemoryPoolDOTGenerationData::visit(): top of while loop: node = %p = %s node->variantT() = %d \n",node,node->class_name().c_str(),(int)(node->variantT()));
               printf ("In CustomMemoryPoolDOTGenerationData::visit(): top of while loop: n = %p \n",n);
               printf ("In CustomMemoryPoolDOTGenerationData::visit(): top of while loop: edgelabel = %s \n",edgelabel.c_str());
               if (n != NULL)
                  {
                    printf ("In CustomMemoryPoolDOTGenerationData::visit(): top of while loop: n->variantT() = %d \n",(int)(n->variantT()));
                  }
#endif
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
#if 1
                 // DQ (11/26/2016): Added to support debugging of how we ignore edges.
                 // This is the only way I have been able to eliminate these edges.
                    if ( skipNodeList.find(node) == skipNodeList.end() )
                       {
                      // dotrep.addNullValue(node,"",edgelabel,"");
                      // DQ (11/26/2016): Adding support to ignore specific edges.
                         EdgeType edge(node,n,edgelabel);

                      // DQ (11/26/2016): Could the logic is flipped here, should it be NOT EQUALS instead?
                         bool ignoreEdge = skipEdgeSet.find(edge) != skipEdgeSet.end();
#if 0
                         printf ("ignoreEdge = %s \n",ignoreEdge ? "true" : "false");
#endif
                         if (ignoreEdge == false)
                            {
                              dotrep.addNullValue(node,"",edgelabel,"");
                            }
                       }
#endif
                  }
                 else
                  {
                 // Only build the edge if the node is not on the list of IR nodes to skip
                 // We can't use the visitedNodes list since just be cause we have visited
                 // the IR node does not imply that we should skip the edge (quite the opposent!)
                 // if ( skipNodeList.find(n) == skipNodeList.end() )
                    if ( skipNodeList.find(n) == skipNodeList.end() )
                       {
#if 0
                         printf ("Edge is being output \n");
#endif
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

                      // DQ (11/26/2016): Adding support to ignore specific edges.
                         EdgeType edge(node,n,edgelabel);

                      // DQ (11/26/2016): Could the logic is flipped here, should it be NOT EQUALS instead?
                      // This appears to always be false.
                      // ignoreEdge = (skipEdgeSet.find(edge) == skipEdgeSet.end());
                      // ignoreEdge = (skipEdgeSet.find(edge) != skipEdgeSet.end());
                      // ignoreEdge = (std::set<EdgeType>::find(skipEdgeSet,edge) != skipEdgeSet.end());
                         ignoreEdge = skipEdgeSet.find(edge) != skipEdgeSet.end();
#if 0
                         printf ("ignoreEdge = %s \n",ignoreEdge ? "true" : "false");
#endif
                         if (ignoreEdge == true)
                            {
                              EdgeSetType::iterator example_iterator = skipEdgeSet.find(edge);
#if 0
                              printf ("example_iterator->start = %p = %s \n",example_iterator->start,example_iterator->start->class_name().c_str());
                              if (example_iterator->end != NULL)
                                 {
                                   printf ("example_iterator->end   = %p = %s \n",example_iterator->end,example_iterator->end->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("example_iterator->end   = NULL \n");
                                 }
                              printf ("example_iterator->labelString = %s \n",example_iterator->labelString.c_str());
#endif
                           // DQ (11/26/2016): These should be the same.
                              ROSE_ASSERT(edgelabel == example_iterator->labelString);
                            }

#if 0
                         printf ("In CustomMemoryPoolDOTGenerationData::visit(): node = %p = %s n = %p = %s edgelabel = %s ignoreEdge = %s skipEdgeSet.size() = %zu \n",
                                 node,node->class_name().c_str(),n,n->class_name().c_str(),edgelabel.c_str(),ignoreEdge ? "true" : "false",skipEdgeSet.size());
#endif
#if 0
                      // DQ (1/8/2017): Adding back this edge (actually this is not handled here).
                      // DQ (11/26/2016): Added to support debugging of how we ignore edges.
                      // This is the only way I have been able to eliminate these edges.
                         if (edgelabel == "parent")
                            {
                              ignoreEdge = true;
                            }
#endif
#if 0
                      // DQ (1/8/2017): Adding back this edge (actually this is not handled here).
                      // DQ (11/26/2016): Added to support debugging of how we ignore edges.
                      // This is the only way I have been able to eliminate these edges.
                         if (edgelabel == "scope")
                            {
                              ignoreEdge = true;
                            }
#endif
#if 0
                         if (edgelabel == "header")
                            {
                              ignoreEdge = true;
                            }
#endif

                         if (ignoreEdge == false)
                            {
                           // DQ (3/5/2007): Support for edge options
                           // These are both virtual function calls
#if 0
                              printf ("In CustomMemoryPoolDOTGenerationData::visit(): calling additionalEdgeOptions(): node->variantT() = %d \n",(int)(node->variantT()));
                              printf ("In CustomMemoryPoolDOTGenerationData::visit(): calling additionalEdgeOptions(): n = %p \n",n);
                              if (n != NULL)
                                 {
                                   printf ("In CustomMemoryPoolDOTGenerationData::visit(): calling additionalEdgeOptions(): n->variantT() = %d \n",(int)(n->variantT()));
                                 }
#endif
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
#if 0
     printf ("TOP of edgeFilter(): edgeName = %s \n",edgeName.c_str());
#endif

  // This function skips the representation of edges
     if (edgeName == "parent")
        {
          ROSE_ASSERT(nodeSource != NULL);
#if 0
       // ROSE_ASSERT(nodeSink != NULL);
          if (nodeSink != NULL)
             {
               printf ("In edgeFilter(): Skipping parent edge! nodeSource = %p = %s nodeSink = %p = %s DOTgraph.skipEdgeSet.size() = %zu \n",
                    nodeSource,nodeSource->class_name().c_str(),nodeSink,nodeSink->class_name().c_str(),DOTgraph.skipEdgeSet.size());
             }
#endif
#if 0
       // DQ (1/25/2017): Don't skip these.
       // DQ (1/8/2017): Don't skip these edges in general.
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
#endif
#if 0
          printf ("In edgeFilter(): Skipped parent edge! DOTgraph.skipEdgeSet.size() = %zu \n",DOTgraph.skipEdgeSet.size());
#endif
        }
       else
        {
          ROSE_ASSERT(nodeSource != NULL);
       // ROSE_ASSERT(nodeSink != NULL);
          if (nodeSink != NULL)
             {
#if 0
               printf ("Edge not skipped: nodeSource = %p = %s nodeSink = %p = %s DOTgraph.skipEdgeSet.size() = %zu \n",
                    nodeSource,nodeSource->class_name().c_str(),nodeSink,nodeSink->class_name().c_str(),DOTgraph.skipEdgeSet.size());
#endif
             }
            else
             {
            // DQ (11/26/2016): Filter the null pointer edges.
               EdgeType edge(nodeSource,nodeSink,edgeName);
               skipEdge(edge);
             }
        }

#if 0
  // DQ (1/25/2017): Don't skip these.
  // DQ (1/8/2017): Don't skip these edges in general.
  // DQ (11/26/2016): Debugging edge filtering.
     if (edgeName == "scope")
        {
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }
#endif
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
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  // DQ (10/18/2009): Added support to skip output of binary file format in generation of AST visualization.
     if (isSgAsmExecutableFileFormat(node) != NULL)
        {
          skipNode(node);
        }
#endif
   }

void
CustomMemoryPoolDOTGeneration::asmTypeFilter(SgNode* node)
   {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  // DQ (10/18/2009): Added support to skip output of binary expression type information in generation of AST visualization.
     if (isSgAsmType(node) != NULL)
        {
          skipNode(node);
        }
#endif
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
CustomMemoryPoolDOTGeneration::emptyBasicBlockFilter(SgNode* node)
   {
  // This function skips the representation of types and IR nodes associated with types
     SgBasicBlock* block = isSgBasicBlock(node);
     if (block != NULL)
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
     if (parameterList != NULL)
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
       // DQ (1/9/2017): If this is an empty typedef sequence, then skip the node in the AST graph (note that this was not sufficent to eliminate the nodes).
       // skipNode(node);
          SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
          if (typedefSeq->get_typedefs().empty() == true)
             {
               skipNode(node);
             }
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
#if 0
               printf ("In defaultFilter(): calling edgeFilter() \n");
#endif
            // Run all the edges through the filter (the filter will single out the ones to be skipped)
               edgeFilter(node,n,edgelabel);
  
               i++;
             }
        }

  // DQ (1/9/2017): Add this here to test elimination of empty typedef sequences.
     if (isSgTypedefSeq(node) != NULL)
        {
       // DQ (1/9/2017): If this is an empty typedef sequence, then skip the node in the AST graph (note that this was not sufficent to eliminate the nodes).
       // skipNode(node);
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
     ROSE_ASSERT(node != NULL);

     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(statement->variantT())
             {
            // DQ (2/27/2012): Added template support (new template declaration IR nodes).
               case V_SgTemplateFunctionDeclaration:
               case V_SgTemplateMemberFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=blueviolet,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                 // DQ (5/10/2018): Added more debugging information now the we explicitly mark when a function is implicit.
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
#if 0
                    printf ("########## functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
#endif
                    break;
                  }

               case V_SgFunctionDeclaration:
               case V_SgProgramHeaderStatement:
               case V_SgProcedureHeaderStatement:
               case V_SgMemberFunctionDeclaration:
            // case V_SgTemplateInstantiationFunctionDecl:
            // case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=royalblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                 // DQ (5/10/2018): Added more debugging information now the we explicitly mark when a function is implicit.
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
                 // printf ("########## functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
                    break;
                  }

            // DQ (7/22/2012): Distingish between these different types.
               case V_SgTemplateInstantiationFunctionDecl: // functions
               case V_SgTemplateInstantiationMemberFunctionDecl: // functions
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=lightseagreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string forwardFlagString = (functionDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string friendFlagString = (functionDeclaration->get_declarationModifier().isFriend() == true) ? "isFriend" : "!isFriend";
                 // DQ (5/10/2018): Added more debugging information now the we explicitly mark when a function is implicit.
                    string implicitFlagString = (functionDeclaration->get_is_implicit_function() == true) ? "is_implicit_function" : "!is_implicit_function";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " + forwardFlagString +
                                          "\\n  " + friendFlagString +
                                          "\\n  " + implicitFlagString +
                                          "\\n  " + StringUtility::numberToString(functionDeclaration) + "  ";
                 // printf ("########## functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
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

            // DQ (7/22/2012): Distingish between these template class and non template class IR nodes.
            // DQ (2/27/2012): Added template support (new template declaration IR nodes).
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
#if 0
                    printf ("########## classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
#endif
                    break;
                  }

               case V_SgClassDeclaration:
               case V_SgModuleStatement:
            // case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (classDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                 // printf ("########## classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    break;
                  }

               case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (classDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (classDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(classDeclaration) + "  ";
                 // printf ("########## classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
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
                                       // "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(typedefDeclaration) + "  ";
                    break;
                  }

            // DQ (11/4/2014): Added support for C++11 template typedefs.
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
#if 0
                    printf ("########## templateTypedefDeclaration->get_name() = %s \n",templateTypedefDeclaration->get_name().str());
#endif
                    break;
                  }

            // DQ (11/5/2014): Added support for C++11 template typedefs.
               case V_SgTemplateInstantiationTypedefDeclaration:
                  {
                    SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (templateInstantiationTypedefDeclaration->isForward() == true) ? "isForward" : "!isForward";
                    string flagString2 = (templateInstantiationTypedefDeclaration->get_isAutonomousDeclaration() == true) ? "isAutonomousDeclaration" : "!isAutonomousDeclaration";
                    labelWithSourceCode = "\\n  " + templateInstantiationTypedefDeclaration->get_name().getString() + 
                                          "\\n  " + flagString +
                                          "\\n  " + flagString2 +
                                          "\\n  " + StringUtility::numberToString(templateInstantiationTypedefDeclaration) + "  ";
                 // printf ("########## templateInstantiationTypedefDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    break;
                  }

               case V_SgVariableDeclaration:
                  {
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string flagString  = (variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true) ? "variableDeclarationContainsBaseTypeDefiningDeclaration" : "!variableDeclarationContainsBaseTypeDefiningDeclaration";
                    labelWithSourceCode = "\\n  " + flagString +
                                          "\\n  " + StringUtility::numberToString(variableDeclaration) + "  ";
#if 1
                 // DQ (6/15/2019): Debugging the outliner support and detection of protected or private data 
                 // members that trigger declarations to be marked as friend declaration in the class definitions.

                 // printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false");
                 // printf ("decl_stmt->get_declarationModifier().get_accessModifier().isProtected() = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
                 // printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false");

                    string isPrivate   = variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false";
                    string isProtected = variableDeclaration->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false";
                    string isPublic    = variableDeclaration->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false";

                    labelWithSourceCode += "\\n isPrivate   = " + isPrivate   + "  ";
                    labelWithSourceCode += "\\n isProtected = " + isProtected + "  ";
                    labelWithSourceCode += "\\n isPublic    = " + isPublic    + "  ";
#endif
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
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + namespaceDeclaration->get_name().getString() + 
                                          "\\n  " + StringUtility::numberToString(namespaceDeclaration) + "  ";
                    break;
                  }

               case V_SgNamespaceDefinitionStatement:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=2,color=\"blue\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";

                    SgNamespaceDefinitionStatement* ns = isSgNamespaceDefinitionStatement(node);
                    ROSE_ASSERT(ns != NULL);

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

            // DQ (4/8/2018): Added support for the namespace alias.
               case V_SgNamespaceAliasDeclarationStatement:
                  {
                    SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"blue\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + namespaceAliasDeclaration->get_name().getString() + 
                                          "\\n  " + StringUtility::numberToString(namespaceAliasDeclaration) + "  ";
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

          // Liao, 5/8/2014, display unique ID if set (non-zero)
          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
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
                    SgCharVal* charVal = isSgCharVal(valueExp);
                    if (charVal != NULL)
                       {
                         printf ("Error: charVal = %d \n",charVal->get_value());
                       }
                  }

            // DQ (8/29/2015): Make this a comment so that I can better debug this problem.
               if (valueExp->get_parent() == NULL)
                  {
                    printf ("ERROR: valueExp->get_parent() == NULL in AST visualization: valueExp = %p = %s \n",valueExp,valueExp->class_name().c_str());
                  }
            // ROSE_ASSERT(valueExp->get_parent() != NULL);

            // labelWithSourceCode = "\\n value = " + valueExp->unparseToString() + "\\n" + StringUtility::numberToString(node) + "  ";
            // labelWithSourceCode = string("\\n value = nnn") + "\\n" + StringUtility::numberToString(node) + "  ";

            // DQ (9/24/2011): Added support to indicate non-printable characters.
               SgCharVal* charVal = isSgCharVal(valueExp);
               if (charVal != NULL)
                  {
                    char value = charVal->get_value();
                    labelWithSourceCode += string("\\n alpha/numeric value = ") + (isalnum(value) ? "true" : "false") + "  ";
                  }

            // DQ (8/13/2014): Added debugging output to support C++11 unicode specific details.
               SgStringVal* stringVal = isSgStringVal(valueExp);
               if (stringVal != NULL)
                  {
                    bool is_wchar     = stringVal->get_wcharString();
                    bool is_16Bitchar = stringVal->get_is16bitString();
                    bool is_32Bitchar = stringVal->get_is32bitString();
                    labelWithSourceCode += string("\\n is_wchar = ") + (is_wchar ? "true" : "false") + "  ";
                    labelWithSourceCode += string("\\n is_16Bitchar = ") + (is_16Bitchar ? "true" : "false") + "  ";
                    labelWithSourceCode += string("\\n is_32Bitchar = ") + (is_32Bitchar ? "true" : "false") + "  ";
                  }

            // DQ (10/4/2010): Output the value so that we can provide more information.
               labelWithSourceCode += string("\\n value = ") + valueExp->get_constant_folded_value_as_string() + "  ";
             }

       // DQ (6/5/2011): Added support to output if this is an implicit or explicit cast.
          SgCastExp* castExp = isSgCastExp(node);
          if (castExp != NULL)
             {
            // DQ (6/5/2011): Output if this is an implicit (compiler generated) or explicit case (non-compiler generated).
               ROSE_ASSERT(castExp->get_startOfConstruct() != NULL);
            // DQ (7/24/2013): Added support to have more debugging information.
               labelWithSourceCode += string("\\n castContainsBaseTypeDefiningDeclaration: ") + ((castExp->get_castContainsBaseTypeDefiningDeclaration() == true) ? "true" : "false") + "  ";
               labelWithSourceCode += string("\\n cast is: ") + ((castExp->get_startOfConstruct()->isCompilerGenerated() == true) ? "implicit" : "explicit") + "  ";
             }

       // DQ (2/2/2011): Added support for fortran...
          SgActualArgumentExpression* actualArgumentExpression = isSgActualArgumentExpression(node);
          if (actualArgumentExpression != NULL)
             {
               labelWithSourceCode += string("\\n name = ") + actualArgumentExpression->get_argument_name() + "  ";
             }

       // DQ (4/8/2013): Added support to output if this function is using operator syntax.
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);
          if (functionCallExp != NULL)
             {
            // DQ (4/8/2013): Added support to output if this function is using operator syntax.
               labelWithSourceCode += string("\\n call uses operator syntax: ") + ((functionCallExp->get_uses_operator_syntax() == true) ? "true" : "false") + "  ";
             }

       // DQ (4/26/2013): Added support for marking as compiler generated.
          SgExpression* expression = isSgExpression(node);
          ROSE_ASSERT(expression != NULL);
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
               ROSE_ASSERT(isSgCastExp(node) != NULL);
             }

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }


     if (isSgType(node) != NULL)
        {
          SgType* type = isSgType(node);
          ROSE_ASSERT(type != NULL);

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

          SgTemplateType* templateType = isSgTemplateType(node);
          if (templateType != NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
               labelWithSourceCode += string("\\n  name = ") + templateType->get_name().str() + "  " + "position = " + StringUtility::numberToString(templateType->get_template_parameter_position()) + " ";
             }

       // DQ (2/28/2018): Added name to the graph node for all SgNamedType IR nodes.
          SgNamedType* namedType = isSgNamedType(node);
          if (namedType != NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
            // labelWithSourceCode += string("\\n  name = ") + namedType->get_name().str() + "  ";
               labelWithSourceCode += string("\\n") + namedType->get_name().str();
             }

          SgModifierType* modifierType = isSgModifierType(node);
          if (modifierType != NULL)
             {
#if 0
               printf ("In CustomMemoryPoolDOTGeneration::defaultColorFilter(): settings = %s \n",modifierType->get_typeModifier().displayString().c_str());
#endif
#if 0
            // DQ (5/4/2013): This makes the graph nodes too large so use it only as required.
               labelWithSourceCode += string("\\n  settings = ") + modifierType->get_typeModifier().displayString() + "  ";
#endif
               SgTypeModifier tm = modifierType->get_typeModifier(); 
               if (tm.isRestrict())
                 labelWithSourceCode += string("\\n restrict  ") ;
#if 0
               if (modifierType->get_frontend_type_reference() != NULL)
                  {
                    labelWithSourceCode += string("\\n  frontend_type_reference() = ") + (modifierType->get_frontend_type_reference() != NULL ? "true" : "false") + "  ";
                  }
#else
               labelWithSourceCode += string("\\n    frontend_type_reference() = ") + (modifierType->get_frontend_type_reference() != NULL ? "true" : "false") + "    ";
#if 0
            // DQ (1/24/2016): Added tempoarary support to use DOT graph to identify SgModifierType is this specific flag test.
               labelWithSourceCode += string("\\n    isGnuAttributeDevice() = ") + (modifierType->get_typeModifier().isGnuAttributeDevice() ? "true" : "false") + "    ";
               printf ("Output specific GNU attribute flag for debugging CUDA support using new __device__ as part of worj with Jeff \n");
#endif
#endif
             }

       // labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(node) + "  ";

       // DQ (5/4/2013): Added to make the formatting of the type information better in the graph node.
          labelWithSourceCode += string("\\n   ");

#if 1
          SgModifierType* mod_type = isSgModifierType(node);
          if (mod_type != NULL)
             {
               if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
                    long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

                    labelWithSourceCode += string("UPC: ");

                    if (block_size == 0) // block size empty
                       {
                      // curprint ("shared[] ") ;
                         labelWithSourceCode += string("shared[] ");
                       }
                    else if (block_size == -1) // block size omitted
                       {
                      // curprint ("shared ") ;
                         labelWithSourceCode += string("shared ");
                       }
                    else if (block_size == -2) // block size is *
                       {
                      // curprint ("shared[*] ") ;
                         labelWithSourceCode += string("shared[*] ");
                       }
                    else
                       {
                         ROSE_ASSERT(block_size > 0);
                         stringstream ss;
                         ss << block_size;

                      // curprint ("shared["+ss.str()+"] ") ;
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

       // DQ (4/22/2014): Added to make the formatting of the type information better in the graph node.
          labelWithSourceCode += string("\\n   ");
#endif

#if 0
       // DQ (4/24/2014): Added string name to unparsed IR node label  (this might not always be wanted).
          string unparsedType = type->unparseToString();
          labelWithSourceCode += unparsedType + string("\\n   ");
#endif

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
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
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
               case V_SgBinaryComposite:
#endif
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
                                 "\\n line: "          +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                    " column: "        +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n raw line: "      +  StringUtility::numberToString(currentNodeFileInfo->get_raw_line()) + 
                                    " raw column: "    +  StringUtility::numberToString(currentNodeFileInfo->get_raw_col()) + "  " +
                              // DQ (1/16/2012): Added more information to the output of the source poisition information.
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
                 // string typeString  = (templateArgument->get_argumentType() == SgTemplateArgument::argument_undefined) ? "argument_undefined" : "!isForward";
                    string typeString;
                    switch (templateArgument->get_argumentType())
                       {
                         case SgTemplateArgument::argument_undefined:
                              typeString = "argument_undefined";
                              break;

                         case SgTemplateArgument::type_argument:
                              typeString = "type_argument";

                           // DQ (8/25/2012): Modified to output the string representing the type.
                              typeString += string("\\n type = ") + templateArgument->get_type()->unparseToString();
                              typeString += string("\\n type = ") + StringUtility::numberToString(templateArgument->get_type()) + "  ";
                              break;

                         case SgTemplateArgument::nontype_argument:
                              typeString = "nontype_argument";

                           // DQ (8/25/2012): Fixed bug to output the value of the tepression pointer.
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
                              ROSE_ASSERT(false);
                            }
                       }

                 // DQ (1/21/2018): Add debugging information to output in each graph node.
                    typeString += string("\\n explicitlySpecified = ") +  ((templateArgument->get_explicitlySpecified() == true) ? "true" : "false") + "  ";

                    labelWithSourceCode = string("\\n  ") + typeString + 
                                          string("\\n  ") + StringUtility::numberToString(templateArgument) + "  ";
                    break;
                  }

            // DQ (9/21/2012): Added visualization support for template parameters.
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
                              ROSE_ASSERT(false);
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

            // DQ (12/6/2012): Added support for SgFunctionParameterTypeList.
               case V_SgFunctionParameterTypeList:
                  {
                    SgFunctionParameterTypeList* functionParameterTypeList = isSgFunctionParameterTypeList(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode  = string("\\n  ") + StringUtility::numberToString(functionParameterTypeList->get_arguments().size()) + "  ";
                    labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(functionParameterTypeList) + "  ";
                 // printf ("########## typedefSeq->get_name() = %s \n",typedefSeq->get_name().str());
                    break;
                  }

               default:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // labelWithSourceCode;
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

    // Liao 11/5/2010, move out of SgSupport
       if (isSgInitializedName(node) != NULL) 
    // case V_SgInitializedName:
          {
            SgInitializedName* initializedName = isSgInitializedName(node);
            string additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"blue\",fillcolor=darkturquoise,fontname=\"7x13bold\",fontcolor=black,style=filled";
            string labelWithSourceCode = string("\\n  ") + initializedName->get_name().getString() +
                                  string("\\n  ") + StringUtility::numberToString(initializedName) + "  ";
#if 1
         // DQ (1/24/2016): Adding support for __device__ keyword to be used for CUDA in function calls.
         // This implements an idea suggested by Jeff Keasler.
            labelWithSourceCode += string("\\n  ") + "using_device_keyword = " + (initializedName->get_using_device_keyword() ? "true" : "false") + "  ";
#endif
         // printf ("########## initializedName->get_name() = %s \n",initializedName->get_name().str());
 //           break;
 
          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
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

          SgSymbol* symbol = isSgSymbol(node);
#if 0
          printf ("In CustomMemoryPoolDOTGeneration::defaultColorFilter(): symbol = %p = %s name = %s \n",symbol,symbol->class_name().c_str(),symbol->get_name().str());
#endif
#if 1
       // DQ (2/28/2018): Added debugging code to make symbols in the graphs more clear.
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(node);
          if (aliasSymbol != NULL)
             {
               labelWithSourceCode += string("\\n alias to: ") + aliasSymbol->get_base()->class_name();
             }

          labelWithSourceCode += string("\\n name: ") + symbol->get_name();
#endif
       // labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode += string("\\n  ") + StringUtility::numberToString(node) + "  ";

          AST_NODE_ID id = TransformationTracking::getId(node) ;
          if (id != 0)
            labelWithSourceCode = string("\\n  ID: ") +StringUtility::numberToString (id) + "  ";
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

       // DQ (1/11/2015): Added assertion.
          ROSE_ASSERT(token != NULL);
#if 0
#if 1
          printf ("In CustomMemoryPoolDOTGeneration::defaultColorFilter(): token->get_classification_code() = %d \n",token->get_classification_code());
#endif
       // DQ (1/11/2015): We can't just be looking up the token classification assuming Fortran since at this point we don't know what language is being used.
       // std::string token_classification_string = SgToken::ROSE_Fortran_keyword_map [token->get_classification_code()].token_lexeme;
          std::string token_classification_string = "keyword langague classification not known";

          labelWithSourceCode = string("\\n  ") + token->get_lexeme_string() + "  " + 
//                              string("\\n  ") + token_classification_string + "  " + 
                                string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

          addNode(graphNode);
#endif
        }

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
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
#endif

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     if (isSgAsmNode(node) != NULL)
        {
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
               string unparsedExpression = unparseExpression(asmExpression, NULL, NULL);

               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"blue\",fillcolor=skyblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
               labelWithSourceCode = "\\n  " + unparsedExpression + 
                                     "\\n  (generated label: " +  StringUtility::numberToString(asmExpression) + ")  ";
             }


          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
#endif

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

  // DQ (7/22/2012): Added support to ignore some empty IR nodes.
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

#if 0
     printf ("In SimpleColorMemoryPoolTraversal::generateGraph(): exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

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

#if 0
     printf ("In SimpleColorMemoryPoolTraversal::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif

#if 0
  // DQ (2/20/2012): This is actually called.
     printf ("This does not appear to be used! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (11/26/2016): Debugging.
  // printf ("SimpleColorMemoryPoolTraversal::visit(): filterFlags->m_noFilter = %d \n",filterFlags->m_noFilter);

  // DQ (3/2/2010): Test if we have turned off all filtering of the AST.
     if ( filterFlags->m_noFilter == 0) 
        {
       // We allow filitering of the AST.
#if 1
       // DQ (11/26/2016): Debugging.
       // printf ("SimpleColorMemoryPoolTraversal::visit(): filterFlags->m_default = %d \n",filterFlags->m_default);

          if ( filterFlags->m_default== 1)
             {
#if 0
            // DQ (11/26/2016): Debugging.
               printf ("SimpleColorMemoryPoolTraversal::visit(): calling defaultFilter() \n");
#endif
               defaultFilter(node);
             }
#endif

#if 1
       // DQ (3/1/2009): Uncommented to allow filtering of types.
          if ( filterFlags->m_type == 1) 
             {
               typeFilter(node);
             }
#endif

#if 1
       // DQ (3/2/2009): Remove some more nodes to make the graphs more clear.
          if ( filterFlags->m_expression == 1) 
             {
               expressionFilter(node);
             }
#endif

#if 1
          if ( filterFlags->m_emptySymbolTable == 1) 
             {
               emptySymbolTableFilter(node);
             }

       // DQ (7/22/2012): Added support to ignore some empty IR nodes.
          if ( filterFlags->m_emptyBasicBlock == 1) 
             {
               emptyBasicBlockFilter(node);
             }

          if ( filterFlags->m_emptyFunctionParameterList == 1) 
             {
               emptyFunctionParameterListFilter(node);
             }
#endif
#if 1
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
#endif
#if 1
          if ( filterFlags->m_symbol == 1) 
             {
               symbolFilter(node);
             }
#endif

#if 1
  // DQ (10/18/2009): Added support to skip output of binary file format in generation of AST visualization.
          if ( filterFlags->m_asmFileFormat == 1) 
             {
               asmFileFormatFilter(node);
             }

          if ( filterFlags->m_asmType == 1) 
             {
               asmTypeFilter(node);
             }
#endif

#if 0 // this is included inside default filter already
       // Ignore Sg_File_Info objects associated with comments and CPP directives
          if ( filterFlags->m_commentAndDirective == 1) 
             {
               commentAndDirectiveFilter(node);
             }
#endif

#if 1
       // Control output of Sg_File_Info object in graph of whole AST.
          if ( filterFlags->m_fileInfo == 1) 
             {
               fileInfoFilter(node);
             }
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
        {
          defaultColorFilter(node);
        }
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
        {
          flags = new CustomMemoryPoolDOTGeneration::s_Filter_Flags();
        }

#if 0
     printf ("Calling CustomMemoryPoolDOTGeneration::print_filter_flags() \n");
     flags->print_filter_flags();
#endif

#if 0
     printf ("DONE: Calling CustomMemoryPoolDOTGeneration::print_filter_flags(): exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

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
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  // DQ (2/20/2012): This(fails when compiled with ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT.
  // This is because with this option the the getSetOfFrontendSpecificNodes() function is not seen.
  // To eliminate the frontend specific nodes we turn off the processing of them in the header file directly.
     set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
#else
     set<SgNode*> skippedNodeSet;
     printf ("ROSE configured for internal frontend development \n");

  // This is OK to still proceed, just that front-end IR nodes will not be filtered.
  // They can however be skipped in rose_edg_required_macros_and_functions.h directly.
  // ROSE_ASSERT(false);
#endif

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
