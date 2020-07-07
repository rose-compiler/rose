

// Author: Markus Schordan
// $Id: AstDOTGeneration.C,v 1.7 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTDOTGENERATION_C
#define ASTDOTGENERATION_C

#include "sage3basic.h"
#include "AstDOTGeneration.h"
#include "transformationTracking.h"
#include "stringify.h"                                  // automatic enum-to-string functions
#include <boost/foreach.hpp>

// DQ (10/21/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#define TEMPLATE_IMPLEMENTATIONS
#include "AstDOTGenerationImpl.C"
#undef TEMPLATE_IMPLEMENTATIONS
#include "AstConsistencyTests.h"

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   #include "AsmUnparser_compat.h"
#endif

using namespace std;                                    // DQ (12/31/2005): This is OK if not declared in a header file
using namespace Rose;                                   // until this file is all moved into the rose namespace

void
AstDOTGeneration::generate(SgNode* node, string filename, traversalType tt, string filenamePostfix)
   {
     init();
     traversal=tt;
     this->filenamePostfix=filenamePostfix;
     DOTInheritedAttribute ia;
     traverse(node,ia);

  // DQ (9/22/2017): Fixed this to allow for an empty filenamePostfix string (and avoid output of ".." in the filename.
  // string filename2=string("./")+filename+"."+filenamePostfix+".dot";
     string filename2=string("./")+filename;
     if (filenamePostfix != "")
        {
          filename2 += "." + filenamePostfix + ".dot";
        }
     filename2 += ".dot";

     dotrep.writeToFileAsGraph(filename2);
   }

void
AstDOTGeneration::generate(SgProject* node, traversalType tt, string filenamePostfix) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;

  ROSE_ASSERT(node != NULL);
// printf ("Starting AstDOTGeneration::generate() at node = %s \n",node->class_name().c_str());

  traverse(node,ia);
}

void
AstDOTGeneration::generateInputFiles(SgProject* node, traversalType tt, string filenamePostfix, bool excludeTemplateInstantiations) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;

  ROSE_ASSERT(node != NULL);
// printf ("Starting AstDOTGeneration::generateInputFiles() at node = %s \n",node->class_name().c_str());

  // DQ (12/20/2018): Skip the template instantiations that can make the files to large for DOT visualizations.
  if (excludeTemplateInstantiations == true)
     {
       ia.skipTemplateInstantiations = true;
     }

  traverseInputFiles(node,ia);
}

void
AstDOTGeneration::generateWithinFile(SgFile* node, traversalType tt, string filenamePostfix) {
  init();
  traversal=tt;
  this->filenamePostfix=filenamePostfix;
  DOTInheritedAttribute ia;

  ROSE_ASSERT(node != NULL);
// printf ("Starting AstDOTGeneration::generateWithinFile() at node = %s \n",node->class_name().c_str());

  traverseWithinFile(node,ia);
}

DOTInheritedAttribute
AstDOTGeneration::evaluateInheritedAttribute(SgNode* node, DOTInheritedAttribute ia)
   {
  // I think this may no longer be required, but I will leave it in place for now
     visitedNodes.insert(node);

#if 0
     printf ("AstDOTGeneration::evaluateInheritedAttribute(): node = %s \n",node->class_name().c_str());
#endif

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
                  {
#if 0
                 // DQ (8/22/2018): This can be handy to comment out when debuging associating comments
                 // and CPP directives to IR nodes (e.g. unparse header support).
                 // This permits the visualization of the AST to be smaller (skips things in std namespace for example).
                    ia.skipSubTree = true;
#endif
                  }

            // DQ (12/20/2018): Skip template instantiations that can make the DOT files too large to
            // generate or look at easily.
            // DQ (12/15/2018): Use this mechanism to simplify the AST for visualization.
            // DQ (1/6/2015): This allows us to simplify the AST visualization by reducing the
            // number of nodes in the AST specific to template instantiations.
            // DQ (2/14/2015): I think we need to have a mechanism to support this so that we can better
            // handle visualization of selected portions of large files.
               if (ia.skipTemplateInstantiations == true)
                  {
                 // DQ (2/12/2015): Test skipping instantiated templates since this frequently makes
                 // the generated dot file too large to be visualized. Ultimately this should be
                 // some sort of optional behavior.
                    SgTemplateInstantiationDecl*               templateInstantationClassDeclaration          = isSgTemplateInstantiationDecl(node);
                    SgTemplateInstantiationFunctionDecl*       templateInstantationFunctionDeclaration       = isSgTemplateInstantiationFunctionDecl(node);
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
                 // SgTemplateMemberFunctionDeclaration*       templateMemberFunctionDeclaration             = isSgTemplateMemberFunctionDeclaration(node);

                    if (templateInstantationClassDeclaration != NULL ||
                        templateInstantationFunctionDeclaration != NULL ||
                        templateInstantationMemberFunctionDeclaration != NULL ||
                     // templateMemberFunctionDeclaration != NULL ||
                        false)
                       {
                         ia.skipSubTree = true;
                       }
                  }
             }

#define DEBUG_DSL_EXAMPLES 0

#if 0
       // DQ (2/12/2015): Test skipping template member functions that are not from the current file.
          SgTemplateInstantiationMemberFunctionDecl* templateInstantationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
          if (templateInstantationMemberFunctionDeclaration != NULL)
             {
             }
#endif
#if DEBUG_DSL_EXAMPLES
       // DQ (2/12/2015): Test skipping template member functions that are not from the current file.
       // I think we need to have a mechanism to support this so that we can better handle visualization
       // of selected portions of large files.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
          if (functionDeclaration != NULL)
             {
#if 0
               printf ("DOT file generation: functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
#endif
#if 0
            // if (functionDeclaration->get_name() != "main" && functionDeclaration->get_name() != "makeStencils")
               if (functionDeclaration->get_name() != "pointRelax" && functionDeclaration->get_name() != "define")
                  {
                    ia.skipSubTree = true;
                  }
#endif
             }
            else
             {
#if 0
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
               if (classDeclaration != NULL && classDeclaration->get_name() != "Multigrid")
                  {
                    ia.skipSubTree = true;
                  }
#endif
             }
#endif
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


void AstDOTGeneration::addAdditionalNodesAndEdges(SgNode* node)
   {
  //*****
  // Nodes and edges can be annotated with additional information. This information is in
  // the form of additional nodes and edges. These is added to the output on a per-node basis.

  // DQ (11/17/2013): Added assertion.
     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In AstDOTGeneration::addAdditionalNodesAndEdges(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // DQ (7/4/2008): Support for edges specified in AST attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
       // Loop over all the attributes at this IR node
          BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
             {
               AstAttribute* attribute = astAttributeContainer->operator[](name);
               ROSE_ASSERT(attribute != NULL);

            // This can return a non-empty list in user-defined attributes (derived from AstAttribute).
#if 0
               printf ("Calling attribute->additionalNodeInfo() \n");
#endif
               std::vector<AstAttribute::AttributeNodeInfo> nodeList = attribute->additionalNodeInfo();
#if 0
               printf ("nodeList.size() = %lu \n",nodeList.size());
#endif
               for (std::vector<AstAttribute::AttributeNodeInfo>::iterator i_node = nodeList.begin(); i_node != nodeList.end(); i_node++)
                  {
                    SgNode* nodePtr   = i_node->nodePtr;
                    string nodelabel  = i_node->label;
                    string nodeoption = i_node->options;
#if 0
                    printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding a node nodelabel = %s nodeoption = %s \n",nodelabel.c_str(),nodeoption.c_str());
#endif
                 // dotrep.addNode(NULL,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
                 // dotrep.addNode( nodePtr, dotrep.traceFormat(ia.tdTracePos) + nodelabel, nodeoption );
                    dotrep.addNode( nodePtr, nodelabel, nodeoption );
                  }
#if 0
               printf ("Calling attribute->additionalEdgeInfo() \n");
#endif
               std::vector<AstAttribute::AttributeEdgeInfo> edgeList = attribute->additionalEdgeInfo();
#if 0
               printf ("edgeList.size() = %lu \n",edgeList.size());
#endif
               for (std::vector<AstAttribute::AttributeEdgeInfo>::iterator i_edge = edgeList.begin(); i_edge != edgeList.end(); i_edge++)
                  {
                    string edgelabel  = i_edge->label;
                    string edgeoption = i_edge->options;
#if 0
                    printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding an edge from i_edge->fromNode = %p to i_edge->toNode = %p edgelabel = %s edgeoption = %s \n",i_edge->fromNode,i_edge->toNode,edgelabel.c_str(),edgeoption.c_str());
#endif
                    dotrep.addEdge(i_edge->fromNode,edgelabel,i_edge->toNode,edgeoption + "dir=forward");
                  }
             }
        }
#if 0
     printf ("Leaving AstDOTGeneration::addAdditionalNodesAndEdges(): node = %p = %s \n",node,node->class_name().c_str());
#endif
   }


void
AstDOTGeneration::writeIncidenceGraphToDOTFile(SgIncidenceDirectedGraph* graph,  const std::string& filename)
   {
  // Output all nodes
     rose_graph_integer_node_hash_map & nodes = graph->get_node_index_to_node_map ();

     for( rose_graph_integer_node_hash_map::iterator it = nodes.begin(); it != nodes.end(); ++it )
        {
          SgGraphNode* node = it->second;

          if( commentOutNodeInGraph(node) == false )
             {
               string nodeoption;
               string nodelabel=string("\\n")+node->get_name();

               nodelabel += additionalNodeInfo(node);

               string additionalOptions = additionalNodeOptions(node);

               string x;
               string y;
               x += additionalOptions;

               nodeoption += additionalOptions;

            // dotrep.addNode(node,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
               dotrep.addNode(node,nodelabel,nodeoption);

               addAdditionalNodesAndEdges(node);
             }
        }

  // Output edges
     rose_graph_integer_edge_hash_multimap & outEdges = graph->get_node_index_to_edge_multimap_edgesOut ();

     for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin(); outEdgeIt != outEdges.end(); ++outEdgeIt )
        {
       // if(debug) std::cerr << " add edge from node ... " << std::endl; // debug
          SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
          ROSE_ASSERT(graphEdge!=NULL);

          if ( commentOutNodeInGraph(graphEdge) == false )
             {
               string edgelabel=string("\\n")+graphEdge->get_name();

               string edgeoption = additionalEdgeOptions(graphEdge->get_from(),graphEdge->get_to(),edgelabel);
               dotrep.addEdge(graphEdge->get_from(),edgelabel,graphEdge->get_to(),edgeoption + "dir=forward");
               addAdditionalNodesAndEdges(graphEdge);
             }

        }

     dotrep.writeToFileAsGraph(filename);
   }


DOTSynthesizedAttribute
AstDOTGeneration::evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l)
   {
     SubTreeSynthesizedAttributes::iterator iter;
     ROSE_ASSERT(node);

#if 0
     printf ("AstDOTGeneration::evaluateSynthesizedAttribute(): node = %s \n",node->class_name().c_str());
#endif

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
     string nodelabel=string("\\n")+node->class_name();

  // DQ (1/24/2009): Added support for output of isForward flag in the dot graph.
     SgDeclarationStatement* genericDeclaration = isSgDeclarationStatement(node);
     if (genericDeclaration != NULL)
        {
       // At the moment the mnemonic name is stored, but it could be computed in the
       // future from the kind and the tostring() function.
          string name = (genericDeclaration->isForward() == true) ? "isForward" : "!isForward";
          ROSE_ASSERT(name.empty() == false);

       // DQ (3/20/2011): Added class names to the generated dot file graphs of the AST.
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(genericDeclaration);
          if (classDeclaration != NULL)
             {
               nodelabel += string("\\n") + classDeclaration->get_name();
             }

       // DQ (3/20/2011): Added function names to the generated dot file graphs of the AST.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(genericDeclaration);
          if (functionDeclaration != NULL)
             {
               nodelabel += string("\\n") + functionDeclaration->get_name();
             }

       // DQ (2/29/2012): Added typedef names to the generated dot file graphs of the AST.
          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(genericDeclaration);
          if (typedefDeclaration != NULL)
             {
               nodelabel += string("\\n") + typedefDeclaration->get_name();

            // DQ (11/21/2015): Adding output of typedefBaseTypeContainsDefiningDeclaration field.
               nodelabel += string("\\n typedefBaseTypeContainsDefiningDeclaration = ") + (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() ? "true " : "false ");
             }

       // DQ (3/20/2011): Added function names to the generated dot file graphs of the AST.
          SgNamespaceDeclarationStatement* namespaceDeclarationStatement = isSgNamespaceDeclarationStatement(genericDeclaration);
          if (namespaceDeclarationStatement != NULL)
             {
               nodelabel += string("\\n") + namespaceDeclarationStatement->get_name();
             }

       // DQ (4/5/2015): Added variable names and associated info to the generated dot file graphs of the AST.
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(genericDeclaration);
          if (variableDeclaration != NULL)
             {
            // This is added to support debugging of multiple variables in the same declaration.
               nodelabel += string("\\n isAssociatedWithDeclarationList = ") + (variableDeclaration->get_isAssociatedWithDeclarationList() ? "true " : "false ");

            // DQ (4/5/2015): I think this is not used and should be removed.
            // nodelabel += string("\\n isFirstDeclarationOfDeclarationList = ") + (variableDeclaration->get_isFirstDeclarationOfDeclarationList() ? "true " : "false ");

            // DQ (11/21/2015): Adding output of typedefBaseTypeContainsDefiningDeclaration field.
               nodelabel += string("\\n variableDeclarationContainsBaseTypeDefiningDeclaration = ") + (variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() ? "true " : "false ");
             }

       // DQ (11/26/2015): Adding friend specification to support debugging test2012_59.C
       // (multiple function definitions for the same function due to EDG template function normalizations).
          nodelabel += string("\\n isFriend = ") + (genericDeclaration->get_declarationModifier().isFriend() ? "true " : "false ");

       // DQ (4/2/2020): Need to add more detail to the graph output so that we can debug Cxx_tests/test2020_02.C.
          nodelabel += string("\\n isExtern = ") + (genericDeclaration->get_declarationModifier().get_storageModifier().isExtern() ? "true " : "false ");
          
          nodelabel += string("\\n") + name;
        }

#if 0
  // DQ (12/4/2014): Added support for debugging the unparsing using the token stream.
  // SgStatement* genericStatement = isSgStatement(node);
     if (genericStatement != NULL)
        {
          nodelabel += string("\\n") + string("isModified = ") + string(genericStatement->get_isModified() ? "true" : "false");
          nodelabel += string("\\n") + string("containsTransformation = ") + string(genericStatement->get_containsTransformation() ? "true" : "false");
          nodelabel += string("\\n") + string("isTransformation = ") + string(genericStatement->isTransformation() ? "true" : "false");
        }
#endif

  // DQ (4/6/2011): Added support for output of the name for SgInitializedName IR nodes.
     SgInitializedName* initializedName = isSgInitializedName(node);
     if (initializedName != NULL)
        {
          nodelabel += string("\\n") + initializedName->get_name();
#if 0
       // DQ (4/14/2015): We need to have these additional data members output (similar to SgStatement).
          nodelabel += string("\\n") + string("isModified = ") + string(initializedName->get_isModified() ? "true" : "false");
          nodelabel += string("\\n") + string("containsTransformation = ") + string(initializedName->get_containsTransformation() ? "true" : "false");
#endif
        }

  // DQ (9/24/2018): Output this information for all located nodes (so that we can include expressions).
     SgLocatedNode* genericLocatedNode = isSgLocatedNode(node);
     if (genericLocatedNode != NULL)
        {
          nodelabel += string("\\n") + string("isModified = ") + string(genericLocatedNode->get_isModified() ? "true" : "false");
          nodelabel += string("\\n") + string("containsTransformation = ") + string(genericLocatedNode->get_containsTransformation() ? "true" : "false");
          if (genericLocatedNode->get_file_info())
          {
            nodelabel += string("\\n") + string("isTransformation = ") + string(genericLocatedNode->isTransformation() ? "true" : "false");
          }
          else
          {
            nodelabel += ("\\n *no file info*");
          }

        }

  // DQ (4/6/2011): Added support for output of the value within SgIntVal IR nodes.
     SgIntVal* intValue = isSgIntVal(node);
     if (intValue != NULL)
        {
          nodelabel += string("\\n value = ") + StringUtility::numberToString(intValue->get_value());
        }

  // DQ (4/6/2011): Added support for output of the name associated with SgVarRefExp IR nodes.
     SgVarRefExp* varRefExp = isSgVarRefExp(node);
     if (varRefExp != NULL)
        {
          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();

       // DQ (1/1/2014): test2014_01.c demonstrates where there is no associated SgVariableSymbol.
          if (variableSymbol == NULL)
             {
               printf ("WARNING: variableSymbol == NULL: varRefExp = %p \n",varRefExp);
             }
          ROSE_ASSERT(variableSymbol != NULL);

          string name = "unknown";
          if (variableSymbol != NULL)
             {
            // printf ("Commented out call to variableSymbol->get_name() \n");
               name = variableSymbol->get_name();
             }
          nodelabel += string("\\n name = ") + name;
        }

  // DQ (8/28/2014): Added support for output of the name for function IR nodes.
     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(node);
     if (functionRefExp != NULL)
        {
          SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();

       // DQ (1/1/2014): test2014_01.c demonstrates where there is no associated SgVariableSymbol.
          if (functionSymbol == NULL)
             {
               printf ("WARNING: functionSymbol == NULL: functionRefExp = %p \n",functionRefExp);
             }
          ROSE_ASSERT(functionSymbol != NULL);

          string name = "unknown";
          if (functionSymbol != NULL)
             {
               name = functionSymbol->get_name();
             }
          nodelabel += string("\\n name = ") + name;
        }

  // DQ (8/28/2014): Added support for output of the name for member function IR nodes.
     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(node);
     if (memberFunctionRefExp != NULL)
        {
          SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();

       // DQ (1/1/2014): test2014_01.c demonstrates where there is no associated SgVariableSymbol.
          if (memberFunctionSymbol == NULL)
             {
               printf ("WARNING: memberFunctionSymbol == NULL: memberFunctionRefExp = %p \n",memberFunctionRefExp);
             }
          ROSE_ASSERT(memberFunctionSymbol != NULL);

          string name = "unknown";
          if (memberFunctionSymbol != NULL)
             {
               name = memberFunctionSymbol->get_name();
             }
          nodelabel += string("\\n name = ") + name;
        }

  // DQ (10/19/2014): Added support for output of the name for SgConstructorInitializer IR nodes.
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(node);
     if (constructorInitializer != NULL)
        {
          SgClassDeclaration* classDeclaration = constructorInitializer->get_class_decl();

       // DQ (10/20/2014): This can be NULL in rare occasions.
       // ROSE_ASSERT(classDeclaration != NULL);

          if (classDeclaration != NULL)
             {
               string name = classDeclaration->get_name();
               nodelabel += string("\\n name = ") + name;
             }
        }

#if 0
  // DQ (4/27/2014): This causes the snippet test code: testJava5a.passed, to fail.
  // DQ (4/24/2014): Added support for output of the type name for expression IR nodes.
  // I need this for debugging, but I think we might not want this to be used all of the time.
     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
          SgType* type = expression->get_type();
          ROSE_ASSERT(type != NULL);
          string unparsedTypeString = type->unparseToString();
          nodelabel += string("\\n type = ") + unparsedTypeString;
        }
#endif

#if 1
  // DQ (4/6/2018): Adding support to output lvvalue information.
     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
          bool is_lvalue = expression->get_lvalue();
          nodelabel += string("\\n lvalue = ") + (is_lvalue ? "true" : "false");
        }
#endif

  // DQ (1/19/2009): Added support for output of what specific instrcution this is in the dot graph.
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     SgAsmInstruction* genericInstruction = isSgAsmInstruction(node);
     if (genericInstruction != NULL)
        {
       // At the moment the mnemonic name is stored, but it could be computed in the
       // future from the kind and the tostring() function.
#if 1
          string unparsedInstruction = unparseInstruction(genericInstruction);
          string addressString       = StringUtility::numberToString( (void*) genericInstruction->get_address() );
       // string name = genericInstruction->get_mnemonic();
          string name = unparsedInstruction + "\\n address: " + addressString;
#else
          string name = unparsedInstruction + "\\n" + addressString;
#endif
          ROSE_ASSERT(name.empty() == false);

          nodelabel += string("\\n") + name;
        }

     SgAsmExpression* genericExpression = isSgAsmExpression(node);
     if (genericExpression != NULL)
        {
          string name = unparseExpression(genericExpression, NULL, NULL);
          if (!name.empty())
              nodelabel += string("\\n") + name;
        }

     if (SgAsmRiscOperation *riscOp = isSgAsmRiscOperation(node)) {
         string name = stringifySgAsmRiscOperationRiscOperator(riscOp->get_riscOperator(), "OP_");
         if (!name.empty())
             nodelabel += "\\n" + name;
     }

  // DQ (10/29/2008): Added some support for additional output of internal names for specific IR nodes.
  // In generall there are long list of these IR nodes in the binary and this helps make some sense of
  // the lists (sections, symbols, etc.).
     SgAsmExecutableFileFormat* binaryFileFormatNode = isSgAsmExecutableFileFormat(node);
     if (binaryFileFormatNode != NULL)
        {
       // The case of binary file format IR nodes can be especially confusing so we want the
       // default to output some more specific information for some IR nodes (e.g. sections).
          string name;

          SgAsmGenericSection* genericSection = isSgAsmGenericSection(node);
          if (genericSection != NULL)
             {
               SgAsmGenericString* genericString = genericSection->get_name();
               ROSE_ASSERT(genericString != NULL);

               name = genericString->get_string();
             }

          SgAsmGenericSymbol* genericSymbol = isSgAsmGenericSymbol(node);
          if (genericSymbol != NULL)
             {
               SgAsmGenericString* genericString = genericSymbol->get_name();
               ROSE_ASSERT(genericString != NULL);

               name = genericString->get_string();

               if (name.empty() == true)
                    name = "no_name_for_symbol";
             }

          SgAsmGenericDLL* genericDLL = isSgAsmGenericDLL(node);
          if (genericDLL != NULL)
             {
               SgAsmGenericString* genericString = genericDLL->get_name();
               ROSE_ASSERT(genericString != NULL);

               name = genericString->get_string();
             }

          SgAsmPEImportItem* peImportItem = isSgAsmPEImportItem(node);
          if (peImportItem != NULL)
             {
               SgAsmGenericString* genericString = peImportItem->get_name();
               ROSE_ASSERT(genericString != NULL);

               name = genericString->get_string();
             }

          SgAsmDwarfLine* asmDwarfLine = isSgAsmDwarfLine(node);
          if (asmDwarfLine != NULL)
             {
               char buffer[100];

            // It does not work to embed the "\n" into the single sprintf parameter.
            // sprintf(buffer," Addr: 0x%08"PRIx64" \n line: %d col: %d ",asmDwarfLine->get_address(),asmDwarfLine->get_line(),asmDwarfLine->get_column());

               sprintf(buffer,"Addr: 0x%08" PRIx64,asmDwarfLine->get_address());
               name = buffer;
               sprintf(buffer,"line: %d col: %d",asmDwarfLine->get_line(),asmDwarfLine->get_column());
               name += string("\\n") + buffer;
             }

          SgAsmDwarfConstruct* asmDwarfConstruct = isSgAsmDwarfConstruct(node);
          if (asmDwarfConstruct != NULL)
             {
               name = asmDwarfConstruct->get_name();
             }

#if 0
       // This might not be the best way to implement this, since we want to detect common base classes of IR nodes.
          switch (node->variantT())
             {
               case V_SgAsmElfSection:
                  {
                    SgAsmElfSection* n = isSgAsmElfSection(node);
                    name = n->get_name();
                    break;
                  }

               default:
                  {
                 // No additional information is suggested for the default case!
                  }
             }
#endif

          if (name.empty() == false)
               nodelabel += string("\\n") + name;
        }
#endif

  // DQ (11/29/2008): Output the directives in the label of the IR node.
     SgC_PreprocessorDirectiveStatement* preprocessorDirective = isSgC_PreprocessorDirectiveStatement(node);
     if (preprocessorDirective != NULL)
        {
          string s = preprocessorDirective->get_directiveString();

       // Change any double quotes to single quotes so that DOT will not misunderstand the generated lables.
          while (s.find("\"") != string::npos)
             {
               s.replace(s.find("\""),1,"\'");
             }

          if (s.empty() == false)
               nodelabel += string("\\n") + s;
        }

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

     DOTSynthesizedAttribute d(0);

  // DQ (7/27/2008): Added mechanism to support pruning of AST
     bool commentoutNode = commentOutNodeInGraph(node);
     if (commentoutNode == true)
        {
       // DQ (11/10/2008): Fixed to only output message when (verbose_level > 0); command-line option.
       // DQ (7/27/2008): For now just return to test this mechanism, then we want to add comment "//" propoerly to generated DOT file.
          if (SgProject::get_verbose() > 0)
             {
               printf ("Skipping the use of this IR node in the DOT Graph \n");
             }
        }
       else
        {

// **************************

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

// **************************
        }



  // DQ (7/4/2008): Support for edges specified in AST attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
       // Loop over all the attributes at this IR node
          BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
             {
               AstAttribute* attribute = astAttributeContainer->operator[](name);
               ROSE_ASSERT(attribute != NULL);

            // This can return a non-empty list in user-defined attributes (derived from AstAttribute).
            // printf ("Calling attribute->additionalNodeInfo() \n");
               std::vector<AstAttribute::AttributeNodeInfo> nodeList = attribute->additionalNodeInfo();
            // printf ("nodeList.size() = %lu \n",nodeList.size());

               for (std::vector<AstAttribute::AttributeNodeInfo>::iterator i_node = nodeList.begin(); i_node != nodeList.end(); i_node++)
                  {
                    SgNode* nodePtr   = i_node->nodePtr;
                    string nodelabel  = i_node->label;
                    string nodeoption = i_node->options;
                 // printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding a node nodelabel = %s nodeoption = %s \n",nodelabel.c_str(),nodeoption.c_str());
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
       // DQ (9/1/2008): Added case for output of SgProject rooted DOT file.
       // This allows source code and binary files to be combined into the same DOT file.
          case V_SgProject:
             {
               SgProject* project = dynamic_cast<SgProject*>(node);
               ROSE_ASSERT(project != NULL);

               string generatedProjectName = SageInterface::generateProjectName( project );
            // printf ("generatedProjectName (from SgProject) = %s \n",generatedProjectName.c_str());
               if (generatedProjectName.length() > 40)
                  {
                 // printf ("Warning: generatedProjectName (from SgProject) = %s \n",generatedProjectName.c_str());
                    generatedProjectName = "aggregatedFileNameTooLong";
                    printf ("Proposed (generated) filename is too long, shortened to: %s \n",generatedProjectName.c_str());
                  }

               string filename = string("./") + generatedProjectName + "." + filenamePostfix + ".dot";

            // printf ("generated filename for dot file (from SgProject) = %s \n",filename.c_str());
               if ( SgProject::get_verbose() >= 1 )
                    printf ("Output the DOT graph from the SgProject IR node (filename = %s) \n",filename.c_str());

               dotrep.writeToFileAsGraph(filename);
               break;
             }

       // case V_SgFile:
          case V_SgSourceFile:
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          case V_SgBinaryComposite:
#endif
             {
               SgFile* file = dynamic_cast<SgFile*>(node);
               ROSE_ASSERT(file != NULL);

               string original_filename = file->getFileName();

            // DQ (7/4/2008): Fix filenamePostfix to go before the "."
            // string filename = string("./") + Rose::utility_stripPathFromFileName(original_filename) + "."+filenamePostfix+"dot";
               string filename = string("./") + Rose::utility_stripPathFromFileName(original_filename) + filenamePostfix + ".dot";

            // printf ("generated filename for dot file (from SgSourceFile or SgBinaryComposite) = %s file->get_parent() = %p \n",filename.c_str(),file->get_parent());

            // printf ("file->get_parent() = %p \n",file->get_parent());
            // cout << "generating DOT file (from SgSourceFile or SgBinaryComposite): " << filename2 << " ... ";

            // DQ (9/1/2008): this effects the output of DOT files when multiple files are specified
            // on the command line.  A SgProject is still built even when a single file is specificed
            // on the command line, however there are cases where a SgFile can be built without a
            // SgProject and this case allows those SgFile rooted subtrees to be output as DOT files.
            // If there is a SgProject then output the dot file from there, else output as a SgFile.
               if (file->get_parent() == NULL)
                  {
                 // If there is no SgProject then output the file now!
                    if ( SgProject::get_verbose() >= 1 )
                         printf ("Output the DOT graph from the SgFile IR node (no SgProject available) \n");

                    dotrep.writeToFileAsGraph(filename);
                  }
                 else
                  {
                 // There is a SgProject IR node, but if we will be traversing it we want to output the
                 // graph then (so that the graph will include the SgProject IR nodes and connect multiple
                 // files (SgSourceFile or SgBinaryComposite IR nodes).
                    if ( visitedNodes.find(file->get_parent()) == visitedNodes.end() )
                       {
                      // This SgProject node was not input as part of the traversal,
                      // so we will not be traversing the SgProject IR nodes and we
                      // have to output the graph now!

                         if ( SgProject::get_verbose() >= 1 )
                              printf ("Output the DOT graph from the SgFile IR node (SgProject was not traversed) \n");

                         dotrep.writeToFileAsGraph(filename);
                         dotrep.clear();
                       }
                      else
                       {
                         if ( SgProject::get_verbose() >= 1 )
                              printf ("Skip the output of the DOT graph from the SgFile IR node (SgProject will be traversed) \n");
                       }
                  }

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


static std::string
generateFileLineColumnString (Sg_File_Info* fileInfo)
   {
  // DQ (9/1/2013): Adding source position information for DOT output.
     string ss;

     ROSE_ASSERT(fileInfo != NULL);
     string file = fileInfo->get_filename();
     file = Rose::utility_stripPathFromFileName(file);

     int line    = fileInfo->get_line();
     int column  = fileInfo->get_col();

     ss += file;

  // DQ (9/1/2013): When ROSE is optimized, this will be output as "::" the output of the line and column integers
  // is optimized away (GNU 4.2.4).  To fix this call to string functions to convert integers to strings explicitly.
     ss += ":";
     ss += StringUtility::numberToString(line);
     ss += ":";
     ss += StringUtility::numberToString(column);

  // DQ (12/21/2014): Adding the physical line number (used by the token mapping).
     int physical_line    = fileInfo->get_physical_line();
     ss += " (physical line=";
     ss += StringUtility::numberToString(physical_line);
     ss += ")";

#if 1
  // DQ (12/21/2014): Adding the physical line number (used by the token mapping).
     int raw_line    = fileInfo->get_raw_line();
     int raw_column  = fileInfo->get_raw_col();
     ss += " (raw line:col=";
     ss += StringUtility::numberToString(raw_line);
     ss += ":";
     ss += StringUtility::numberToString(raw_column);
     ss += ")";
#endif

     ss += "\\n";

     return ss;
   }

static std::string
sourcePositionInformation (SgNode* node)
   {
  // DQ (8/31/2013): Adding source position information for DOT output.
     string ss;

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          Sg_File_Info* fileInfo = locatedNode->get_file_info();
          if (fileInfo != NULL)
             {
               bool hasSpecialMode = false;
               if (fileInfo->isCompilerGenerated() == true)
                  {
                    ss += "compiler generated\\n";
                    hasSpecialMode = true;

                 // DQ (9/7/2016): Add output of raw source position to check on EDG normalized ctor pre-initialization list in templates.
                    ss += generateFileLineColumnString(locatedNode->get_startOfConstruct());
                    ss += generateFileLineColumnString(locatedNode->get_endOfConstruct());
                  }
                 else
                  {
                    if (fileInfo->isFrontendSpecific() == true)
                       {
                         ss += "front-end specific\\n";
                         hasSpecialMode = true;
                       }
                      else
                       {
                         if (fileInfo->isTransformation() == true)
                            {
                              ss += "is part of transformation\\n";
                              hasSpecialMode = true;
                            }
                           else
                            {
                           // ss += "???\\n";
                            }
                       }
                  }

               if (hasSpecialMode == true)
                  {
                    if (fileInfo->isOutputInCodeGeneration() == true)
                       {
                         ss += "IS output in generated code\\n";
                       }
                      else
                       {
                         ss += "is NOT output in generated code\\n";
                       }
                  }
                 else
                  {
                 // DQ (9/1/2013): Name a few cases were we want to output the end of the IR node construct's source position range.
                 // bool outputEndOfConstruct = (isSgAggregateInitializer(node) != NULL || isSgScopeStatement(node) != NULL);
                    bool outputEndOfConstruct = true; // (isSgAggregateInitializer(node) != NULL || isSgStatement(node) != NULL);

                    if (outputEndOfConstruct == true)
                       {
                      // Output the end of the range represented by the IR node's source position.
                         ss += generateFileLineColumnString(locatedNode->get_startOfConstruct());
                         ss += generateFileLineColumnString(locatedNode->get_endOfConstruct());
                       }
                      else
                       {
                      // For an SgStatement this is the startOfConstruct, but for an SgExpression this is the operator position (or sometimes equal to the startOfConstruct).
                         ss += generateFileLineColumnString(fileInfo);
                       }
                  }
             }
            else
             {
               ss += "no source position available\\n";
             }

       // DQ (1/28/2015): Added more info to support debugging the token-stream unparsing.
          if (locatedNode->get_containsTransformationToSurroundingWhitespace() == true)
             {
               ss += "containsTransformationToSurroundingWhitespace == true\\n";
             }
            else
             {
               ss += "containsTransformationToSurroundingWhitespace == false\\n";
             }
        }
       else
        {
       // DQ (9/1/2013): We could handle the source position of some other IR nodes (e.g. output name of the file for SgFile).
          SgFile* file = isSgFile(node);
          if (file != NULL)
             {
               ROSE_ASSERT(file->get_file_info() != NULL);
               ss += generateFileLineColumnString(file->get_file_info());
             }
        }

     return ss;
   }


// DQ (9/19/2013): generate the number associated with each position relative to the attached IR node.
static size_t
numberByRelativePosition(AttachedPreprocessingInfoType* commentsAndCppDirectives, PreprocessingInfo::RelativePositionType pos)
   {
     size_t returnValue = 0;

     for (vector<PreprocessingInfo*>::iterator i = commentsAndCppDirectives->begin(); i != commentsAndCppDirectives->end(); i++)
        {
          if ( (*i)->getRelativePosition() == pos )
             {
               returnValue++;
             }
        }

     return returnValue;
   }


static std::string
commentAndCppInformation (SgNode* node)
   {
  // DQ (8/31/2013): Adding source position information for DOT output.
     string ss;

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          AttachedPreprocessingInfoType* commentsAndCppDirectives = locatedNode->getAttachedPreprocessingInfo();

       // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
       // size_t numberofCommentsAndCppDirectives = 0;

          if (commentsAndCppDirectives != NULL)
             {
            // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
            // numberofCommentsAndCppDirectives = commentsAndCppDirectives->size();

               // MS 11/12/2015: disabled this test as size_t is unsigned;
               // this test is always true and compilers issue a
               // warning
               //if (numberofCommentsAndCppDirectives >= 0)
                  {
                 // ss = string("comments = ") + StringUtility::numberToString(numberofCommentsAndCppDirectives) + "\\n";
                    ss += string("comments/directives (before) = ") + StringUtility::numberToString(numberByRelativePosition(commentsAndCppDirectives,PreprocessingInfo::before)) + "\\n";
                    ss += string("comments/directives (inside) = ") + StringUtility::numberToString(numberByRelativePosition(commentsAndCppDirectives,PreprocessingInfo::inside)) + "\\n";
                    ss += string("comments/directives (after)  = ") + StringUtility::numberToString(numberByRelativePosition(commentsAndCppDirectives,PreprocessingInfo::after)) + "\\n";
                  }
             }
        }

#if 0
    // DQ (9/21/2018): This is incorrect code since CPP directives and comments are not attached to a SgFile, but are attached to a SgGlobal (global scope IR node).
       else
        {
       // DQ (9/1/2013): We could handle the source position of some other IR nodes (e.g. output name of the file for SgFile).
       // SgFile* file = isSgFile(node);
          SgSourceFile* file = isSgSourceFile(node);
          if (file != NULL)
             {
            // ROSE_ASSERT(file->get_file_info() != NULL);
            // ss += generateFileLineColumnString(file->get_file_info());
               AttachedPreprocessingInfoType* commentsAndCppDirectives = file->getAttachedPreprocessingInfo();
               size_t numberofCommentsAndCppDirectives = 0;
               if (commentsAndCppDirectives != NULL)
                  {
                    numberofCommentsAndCppDirectives = commentsAndCppDirectives->size();
                    if (numberofCommentsAndCppDirectives > 0)
                       {
                         ss = string("comments/directives = ") + StringUtility::numberToString(numberofCommentsAndCppDirectives) + "\\n";
                       }
                  }
             }
        }
#endif

     return ss;
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
     ss <<"child_count:"<< c.size() << "\\n";

  // add memory location of node to dot output
     ss << node << "\\n";

  // DQ (7/4/2014): Adding more debug support to the AST visualization.
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);
     if (functionCallExp != NULL)
        {
          string value = "false";
          if (functionCallExp->get_uses_operator_syntax() == true)
             {
               value = "true";
             }
          ss << string("uses_operator_syntax() = ") << value << "\\n";
        }

  // DQ (8/31/2013): Added more information about the IR node to the dot graph.
     ss << sourcePositionInformation(node);

  // DQ (9/19/2013): Added more information about the IR node to the dot graph (comments and C preprocessor directive information).
     ss << commentAndCppInformation(node);

  // DQ (7/4/2008): Added support for output of information about attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          ss << "Attribute list (size=" << astAttributeContainer->size() << "):" << "\\n";
          BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
             {
            // pair<std::string,AstAttribute*>
               AstAttribute* attribute = astAttributeContainer->operator[](name);
               ROSE_ASSERT(attribute != NULL);

            // Note cast to void*
               std::string label = name + " : " + attribute->toString();
               ss << label << "\\n";
             }
          ss << "\\n";
          ss << "\\n";
        }
     // Liao, 4/3/2014  display Unique ID for some nodes
     if (TransformationTracking::getId(node) != 0)
       ss <<"ID:"<<TransformationTracking::getId(node)<<"\\n";
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
#if 0
          printf ("In AstDOTGeneration::additionalNodeOptions(): astAttributeContainer = %p for node = %p = %s \n",astAttributeContainer,node,node->class_name().c_str());
#endif
          BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
             {
            // std::string name = i->first;
               AstAttribute* attribute = astAttributeContainer->operator[](name);
               ROSE_ASSERT(attribute != NULL);

               ss << attribute->additionalNodeOptions();
             }
        }
       else
        {
#if 0
          printf ("In AstDOTGeneration::additionalNodeOptions(): astAttributeContainer == NULL for node = %p = %s \n",node,node->class_name().c_str());
#endif
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


bool
AstDOTGeneration::commentOutNodeInGraph(SgNode* node)
   {
  // return an empty string for default implementation
     bool resultValue = false;

  // DQ (7/4/2008): Added support for output of information about attributes
     AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
          BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
             {
            // std::string name = i->first;
               AstAttribute* attribute = astAttributeContainer->operator[](name);
               ROSE_ASSERT(attribute != NULL);

            // Turn it ON if there is an attribute to do so, but don't turn it off (for attribute to be changed)
               if (resultValue == false)
                    resultValue = attribute->commentOutNodeInGraph();
             }
        }

     return resultValue;
   }


// DQ (6/25/2011): Put the function definition into the source file (avoid function definitions in header files).
// std::string operator()(SgNode* node)
std::string AstDOTGenerationExtended_Defaults::NamedExtraNodeInfo::operator()(SgNode* node)
   {
                        std::ostringstream ss;

                        // add namespace name
                        if (SgNamespaceDeclarationStatement* n = isSgNamespaceDeclarationStatement(node))
                        {
                                ss << n->get_qualified_name().str() << "\\n";
                        }
                        // add class name
                        if (SgClassDeclaration* n = isSgClassDeclaration(node))
                        {
                                ss << n->get_qualified_name().str() << "\\n";
                        }
                        // add function name
                        if (SgFunctionDeclaration* n = isSgFunctionDeclaration(node))
                        {
                                ss << n->get_qualified_name().str() << "\\n";
                        }
                        if (SgFunctionRefExp* n = isSgFunctionRefExp(node))
                        {
                                SgFunctionDeclaration* decl = n->getAssociatedFunctionDeclaration();
                                if (decl) // it's null if through a function pointer
                                {
                                        ss << decl->get_qualified_name().str() << "\\n";
                                }
                        }
                        // add variable name
                        if (SgInitializedName* n = isSgInitializedName(node))
                        {
                                ss << n->get_qualified_name().str() << "\\n";
                        }
                        if (SgVarRefExp* n = isSgVarRefExp(node))
                        {
                                SgVariableSymbol* sym = n->get_symbol();
                                ss << sym->get_name().getString() << "\\n";
                        }
                        // add variable name
                        if (SgVariableSymbol* n = isSgVariableSymbol(node))
                        {
                                ss << n->get_name().str() << "\\n";
                        }

                        return ss.str();
   }


// DQ (6/25/2011): Put the function definition into the source file (avoid function definitions in header files).
// std::string operator()(SgNode* node)
std::string AstDOTGenerationExtended_Defaults::TypeExtraNodeInfo::operator()(SgNode* node)
   {
                        std::ostringstream ss;

                        if (SgExpression* n = isSgExpression(node))
                        {
                                ss << n->get_type()->unparseToString() << "\\n";
                        }

                        return ss.str();
   }


// DQ (6/25/2011): Put the function definition into the source file (avoid function definitions in header files).
// std::string operator()(SgNode* node)
std::string AstDOTGenerationExtended_Defaults::LValueExtraNodeInfo::operator()(SgNode* node)
   {
                        std::ostringstream ss;

                        // adds whether or not it is an l-value
                        if (SgExpression* n = isSgExpression(node))
                        {
                                ss << (n->isLValue() ? "L-Value" : "!L-Value") << "\\n";
                        }

                        return ss.str();
   }

#endif
