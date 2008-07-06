// Author: Markus Schordan
// $Id: AstDOTGeneration.C,v 1.7 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTDOTGENERATION_C
#define ASTDOTGENERATION_C

#include "sage3.h"
#include "AstDOTGeneration.h"
#include "AstConsistencyTests.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
AstDOTGeneration::generate(SgNode* node, string filename, traversalType tt, string filenamePostfix)
   {
     init();
     traversal=tt;
     this->filenamePostfix=filenamePostfix;
     DOTInheritedAttribute ia;
     traverse(node,ia);
     string filename2=string("./")+filename+"."+filenamePostfix+"dot";
     dotrep.writeToFileAsGraph(filename2);
   }

void
AstDOTGeneration::generate(SgProject* node, traversalType tt, string filenamePostfix) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;
  traverse(node,ia);
}

void
AstDOTGeneration::generateInputFiles(SgProject* node, traversalType tt, string filenamePostfix) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;
  traverseInputFiles(node,ia);
}

void
AstDOTGeneration::generateWithinFile(SgFile* node, traversalType tt, string filenamePostfix) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;
  traverseWithinFile(node,ia);
}

DOTInheritedAttribute 
AstDOTGeneration::evaluateInheritedAttribute(SgNode* node, DOTInheritedAttribute ia)
   {
  // I think this may no longer be required, but I will leave it in place for now
     visitedNodes.insert(node);

  // We might not want to increment the trace position information for
  // the IR nodes from rose_edg_required_macros_and_functions.h
  // ia.tdbuTracePos = tdbuTrace++;
  // ia.tdTracePos   = tdTrace++;

  // DQ (5/3/2006)
  // We put macros and functions required for GNU compatability in the file: 
  //    rose_edg_required_macros_and_functions.h
  // and we want to avoid generating nodes for these within visualizations of the AST.
  // Once EDG supports these functions (we have collected the onese missed by EDG here)
  // this file will not be required.  We could filter on declaration first to avoid
  // lots of string comparision.  Or use a static pointer to save the first fileInfo
  // from "rose_edg_required_macros_and_functions.h" and then use the 
  // Sg_File_Info::sameFile() function (this reduces to an integer comparision internally).
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
       // Build factored versions of the rather numerous tests (we are now not 
       // able to rely on short-circuit evaluation which makes the code easier 
       // to debug using valgrind).
          bool isCompilerGeneratedOrPartOfTransformation = fileInfo->isCompilerGenerated();

       // DQ (5/3/2006): All constructs from the rose_edg_required_macros_and_functions.h 
       // file are marked as compiler generated.  These are declarations required for GNU 
       // compatability and we would like to have them be ignored because they should be
       // considered builtin and not explicitly represented.  In a sense this file is special.
       // Not that if we traverse the AST without constraint then we traverse these IR nodes.
          if (isCompilerGeneratedOrPartOfTransformation == true)
             {
               std::string targetFileName      = "rose_edg_required_macros_and_functions.h";
               std::string rawFileName         = node->get_file_info()->get_raw_filename();
               std::string filenameWithoutPath = StringUtility::stripPathFromFileName(rawFileName);
               if (filenameWithoutPath == targetFileName)
                    ia.skipSubTree = true;
             }
        }

  // We might not want to increment the trace position information for
  // the IR nodes from rose_edg_required_macros_and_functions.h
     if (ia.skipSubTree == false)
        {
          ia.tdbuTracePos = tdbuTrace++;
          ia.tdTracePos   = tdTrace++;
        }

     return ia;
   }

DOTSynthesizedAttribute
AstDOTGeneration::evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l)
   {
     SubTreeSynthesizedAttributes::iterator iter;
     ROSE_ASSERT(node);

  // DQ (5/3/2006): Skip this IR node if it is specified as such in the inherited attribute
     if (ia.skipSubTree == true)
        {
       // I am unclear if I should return NULL or node as a parameter to DOTSynthesizedAttribute
       // Figured this out: if we return a valid pointer then we get a node in the DOT graph 
       // (with just the pointer value as a label), where as if we return a DOTSynthesizedAttribute 
       // with a NUL pointer then the node will NOT appear in the DOT graph.
       // return DOTSynthesizedAttribute(node);
          return DOTSynthesizedAttribute(NULL);
        }

     string nodeoption;
     if(AstTests::isProblematic(node))
        {
       // cout << "problematic node found." << endl;
          nodeoption="color=\"orange\" ";
        }
     string nodelabel=string("\\n")+node->sage_class_name();
     nodelabel += additionalNodeInfo(node);

  // DQ (11/1/2003) added mechanism to add additional options (to add color, etc.)
  // nodeoption += additionalNodeOptions(node);
     string additionalOptions = additionalNodeOptions(node);
  // printf ("nodeoption = %s size() = %ld \n",nodeoption.c_str(),nodeoption.size());
  // printf ("additionalOptions = %s size() = %ld \n",additionalOptions.c_str(),additionalOptions.size());

     string x;
     string y;
     x += additionalOptions;

     nodeoption += additionalOptions;

     switch(traversal)
        {
          case TOPDOWNBOTTOMUP:
               dotrep.addNode(node,dotrep.traceFormat(ia.tdbuTracePos,tdbuTrace)+nodelabel,nodeoption);
               break;
          case PREORDER:
          case TOPDOWN:
               dotrep.addNode(node,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
               break;
          case POSTORDER:
          case BOTTOMUP:
               dotrep.addNode(node,dotrep.traceFormat(buTrace)+nodelabel,nodeoption);
               break;
          default:
               assert(false);
        }
  
     ++tdbuTrace;
     ++buTrace;
     DOTSynthesizedAttribute d(0);

  // add edges or null values
     int testnum=0;
     for (iter = l.begin(); iter != l.end(); iter++)
        {
          string edgelabel = string(node->get_traversalSuccessorNamesContainer()[testnum]);
          string toErasePrefix = "p_";

          if (AstTests::isPrefix(toErasePrefix,edgelabel))
             {
               edgelabel.erase(0, toErasePrefix.size());
             }

          if ( iter->node == NULL)
             {
            // SgNode* snode=node->get_traversalSuccessorContainer()[testnum];
               AstSuccessorsSelectors::SuccessorsContainer c;
               AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
               SgNode* snode=c[testnum];

            // isDefault shows that the default constructor for synth attribute was used
               if (l[testnum].isDefault() && snode && (visitedNodes.find(snode) != visitedNodes.end()) )
                  {
                 // handle bugs in SAGE
                    dotrep.addEdge(node,edgelabel,snode,"dir=forward arrowhead=\"odot\" color=red ");
                  }
                 else
                  {
                    if (snode == NULL)
                       {
                         dotrep.addNullValue(node,"",edgelabel,"");
                       }
                  }
             }
            else
             {
            // DQ (3/5/2007) added mechanism to add additional options (to add color, etc.)
               string edgeoption = additionalEdgeOptions(node,iter->node,edgelabel);

               switch(traversal)
                  {
                    case TOPDOWNBOTTOMUP:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=both");
                         break;
                    case PREORDER:
                    case TOPDOWN:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=forward");
                         break;
                    case POSTORDER:
                    case BOTTOMUP:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=back");
                         break;
                    default:
                         assert(false);
                  }
             }

          testnum++;
        }

  // DQ (7/4/2008): Support for edges specified in AST attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
            // std::string name = i->first;
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);

               printf ("Calling attribute->additionalNodeInfo() \n");
               std::vector<AstAttribute::AttributeNodeInfo> nodeList = attribute->additionalNodeInfo();
               printf ("nodeList.size() = %lu \n",nodeList.size());
               for (std::vector<AstAttribute::AttributeNodeInfo>::iterator i_node = nodeList.begin(); i_node != nodeList.end(); i_node++)
                  {
                    SgNode* nodePtr   = i_node->nodePtr;
                    string nodelabel  = i_node->label;
                    string nodeoption = i_node->options;
                    printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding a node nodelabel = %s nodeoption = %s \n",nodelabel.c_str(),nodeoption.c_str());
                 // dotrep.addNode(NULL,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
                    dotrep.addNode( nodePtr, dotrep.traceFormat(ia.tdTracePos) + nodelabel, nodeoption );
                  }

            // printf ("Calling attribute->additionalEdgeInfo() \n");
               std::vector<AstAttribute::AttributeEdgeInfo> edgeList = attribute->additionalEdgeInfo();
            // printf ("edgeList.size() = %lu \n",edgeList.size());
               for (std::vector<AstAttribute::AttributeEdgeInfo>::iterator i_edge = edgeList.begin(); i_edge != edgeList.end(); i_edge++)
                  {
                    string edgelabel  = i_edge->label;
                    string edgeoption = i_edge->options;
                 // printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding an edge from i_edge->fromNode = %p to i_edge->toNode = %p edgelabel = %s edgeoption = %s \n",i_edge->fromNode,i_edge->toNode,edgelabel.c_str(),edgeoption.c_str());
                    dotrep.addEdge(i_edge->fromNode,edgelabel,i_edge->toNode,edgeoption + "dir=forward");
                  }
             }
        }



     switch(node->variantT())
        {
          case V_SgFile: 
             {
               string filename = (static_cast<SgFile*>(node))->getFileName();

            // DQ (7/4/2008): Fix filenamePostfix to go before the "."
            // string filename2=string("./")+ROSE::stripPathFromFileName(filename)+"."+filenamePostfix+"dot";
               string filename2=string("./")+ROSE::stripPathFromFileName(filename)+filenamePostfix+".dot";

            // cout << "generating DOT file: " << filename2 << " ... ";
               dotrep.writeToFileAsGraph(filename2);
            // cout << "done." << endl;
               break;
             }

       // DQ (7/23/2005): Implemented default case to avoid g++ warnings 
       // about enum values not handled by this switch
          default: 
             {
            // nothing to do here
               break;
             }
        }

     d.node = node;
     return d;
   }


// To improve the default output add additional information here
// Note you need to add "\\n" for newline
string
AstDOTGeneration::additionalNodeInfo(SgNode* node)
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

  // DQ (7/4/2008): Added support for output of information about attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          ss << "Attribute list:" << "\\n";
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
            // pair<std::string,AstAttribute*>
               std::string name = i->first;
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);

               ss << name << "\\n";
             }
        }

     return ss.str();
   }

// DQ (11/1/2003) added mechanism to add additional options (to add color, etc.)
string
AstDOTGeneration::additionalEdgeInfo(SgNode* from, SgNode* to, string label)
   {
  // return an empty string for default implementation
#if 0
     ostringstream ss;

  // DQ (7/4/2008): Added support for output of information about attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
            // std::string name = i->first;
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);

               ss << attribute->additionalEdgeInfo();
             }
        }

     return ss.str();
#endif
     return "";
   }

// DQ (11/1/2003) added mechanism to add additional options (to add color, etc.)
string
AstDOTGeneration::additionalNodeOptions(SgNode* node)
   {
  // return an empty string for default implementation
     ostringstream ss;

  // DQ (7/4/2008): Added support for output of information about attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
            // std::string name = i->first;
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);

               ss << attribute->additionalNodeOptions();
             }
        }

     return ss.str();
   }

// DQ (11/1/2003) added mechanism to add additional options (to add color, etc.)
string
AstDOTGeneration::additionalEdgeOptions(SgNode* from, SgNode* to, string label)
   {
  // return an empty string for default implementation, but set the parent edge to blue
     return "";
   }


#endif
