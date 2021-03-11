// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

// #include "general_token_defs.h"

// DQ (11/29/2013): Added to support marking of redundant mappings of statements to token streams.
#include "tokenStreamMapping.h"

// DQ (12/4/2014): This is redundant with being included in "tokenStreamMapping.h".
// #include "frontierDetection.h"

#include "previousAndNextNode.h"

// DQ (11/29/2013): Added to support marking of redundant mappings of statements to token streams.
// #include "tokenStreamMapping.h"


using namespace std;
using namespace Rose;

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
ROSE_DLL_API bool ROSE_tokenUnparsingTestingMode = false;

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
     sourceFile        = NULL;
     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
   {
     sourceFile        = input_sourceFile;
     processChildNodes = processed;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

  // isPartOfTypedefDeclaration   = X.isPartOfTypedefDeclaration;
  // isPartOfConditionalStatement = X.isPartOfConditionalStatement;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = NULL;
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
   {
     node       = isSgStatement(n);
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
     node       = X.node;
     isFrontier = X.isFrontier;

     frontierNodes    = X.frontierNodes;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
   }


FrontierDetectionForTokenStreamMapping::
FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
  // This is the number of IR nodes in the current file and helpful in randomly 
  // marking IR nodes for testing to be either from the AST or from the token stream.
     numberOfNodes = numberOfNodesInSubtree(sourceFile);
   }


#if 0
std::vector<AstAttribute::AttributeNodeInfo>
FrontierDetectionForTokenStreamMapping::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string vulnerabilityName    = " " + vulnerabilityPointer->get_name();
     string vulnerabilityColor   = vulnerabilityPointer->get_color();
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

  // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, vulnerabilityName, vulnerabilityOptions);
     v.push_back(vulnerabilityNode);

     return v;
   }
#endif

FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute)
   {
     static int random_counter = 0;

#if 0
     printf ("*** In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d n = %p = %s \n",random_counter,n,n->class_name().c_str());
#endif

     SgStatement* statement = isSgStatement(n);

  // Ignore IR nodes that are front-end specific (declarations of builtin functions, etc.).
  // if (n->get_file_info()->isFrontendSpecific() == false)
     if (n->get_file_info()->isFrontendSpecific() == false && statement != NULL)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }

     FrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute;

  // if (statement != NULL && random_counter > 30 && random_counter < 40)
     if (statement != NULL)
        {
#if 0
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());
#endif
          string name    = "token_frontier";
          string options = "color=\"blue\"";

       // This will later just check if this is a statement marked as a transformation or that it has been modified (checking the SgNode->isModified member flag).
       // if (random_counter > 30 && random_counter < 40)
       // int numberOfnode = numberOfNodes;

       // Mark the middle 50% of IR nodes to come from the AST, instead of the token stream.
       // int lowerbound = numberOfNodes / 4;
       // int upperbound = 3 * (numberOfNodes / 4);
          int lowerbound = -1;
          int upperbound = -1;
#if 0
#if 0
       // Unparse wholely from the token stream.
          bool forceUnparseFromTokenStream = true;
          bool forceUnparseFromAST         = false; // (forceUnparseFromTokenStream == false);
#else
       // Unparse wholely from the AST.
          bool forceUnparseFromTokenStream = false;
          bool forceUnparseFromAST         = true;
#endif
#else
       // Unparse the middle of the AST from the AST and the outer 25% from the token stream.
          bool forceUnparseFromTokenStream = false;
          bool forceUnparseFromAST         = false;
#endif
#if 0
          printf ("ROSE_tokenUnparsingTestingMode = %s \n",ROSE_tokenUnparsingTestingMode ? "true" : "false");
#endif
       // DQ (12/1/2013): Added switch to control testing mode for token unparsing.
       // Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
       // variable so that all regression tests can be processed to mix the unparsing of 
       // the token stream with unparsing from the AST.
          if (ROSE_tokenUnparsingTestingMode == true)
             {

               if (forceUnparseFromTokenStream == true || forceUnparseFromAST == true)
                  {
                 // We need to set this to a sufficently high number so that the conditional below will always be false.
                    static int max_numberOfNodes = 10000000; // (int)SgNode::numberOfNodes();

                    if (forceUnparseFromAST == true)
                       {
                         ROSE_ASSERT(forceUnparseFromTokenStream == false);
                         lowerbound = -1;
                         upperbound = max_numberOfNodes;
                       }
                      else
                       {
                         ROSE_ASSERT(forceUnparseFromTokenStream == true);

                         lowerbound = max_numberOfNodes;
                         upperbound = -1;
#if 0
                         printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): max_numberOfNodes = %d \n",max_numberOfNodes);
#endif
                       }
                  }
                 else
                  {
                 // DQ (12/1/2013): This is the testing mode for all regression tests.
                    ROSE_ASSERT(forceUnparseFromAST == false);
                    ROSE_ASSERT(forceUnparseFromTokenStream == false);

                    lowerbound = numberOfNodes / 4;
                    upperbound = 3 * (numberOfNodes / 4);
                  }

            // bool skipMarkingAsToBeUnparsedFromAST  = false;
            // if ( (random_counter > lowerbound && random_counter < upperbound) || forceFromTokenStream == true)
               if ( random_counter >= lowerbound && random_counter <= upperbound )
                  {
#if 0
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());
#endif
                    options = "color=\"red\"";

                    if (returnAttribute.unparseFromTheAST == false)
                       {
                      // Mark this as the frontier only if this is the first time (while decending into the AST) 
                      // that we will unparse this subtree from the AST.
                         returnAttribute.isFrontier = true;
                       }

                    returnAttribute.unparseFromTheAST       = true;
                    returnAttribute.unparseUsingTokenStream = false;
                  }
                 else
                  {
                    returnAttribute.unparseUsingTokenStream = true;
                  }
             }
            else
             {
            // DQ (12/1/2013): The default is to unparse using the token stream, unless transformations are detected.
            // We may at some point introduce a mechanism to disable transformations where macros have 
            // been expanded so that we can define a SAFE system.  This has not been done yet.

            // returnAttribute.unparseUsingTokenStream = true;
               if (statement->get_file_info()->isTransformation() == true)
                  {
#if 0
                    printf ("Found an AST transformation: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                 // returnAttribute.isFrontier = true;
                    if (returnAttribute.unparseFromTheAST == false)
                       {
                      // Mark this as the frontier only if this is the first time (while decending into the AST) 
                      // that we will unparse this subtree from the AST.
                         returnAttribute.isFrontier = true;
                       }

                    returnAttribute.unparseFromTheAST       = true;
                    returnAttribute.unparseUsingTokenStream = false;
                  }
                 else
                  {
                    returnAttribute.unparseUsingTokenStream = true;
                  }
             }

       // DQ (11/30/2013): Allow us to ignore class defintions in typedefs.
       // Mark the whole subtree as being unparsed from the AST so that synthizized attributes can be more esily computed.
       // if (inheritedAttribute.unparseFromTheAST == true)
       // if (inheritedAttribute.unparseFromTheAST == true && skipMarkingAsToBeUnparsedFromAST == false)
          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if 0
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
#endif
               returnAttribute.unparseFromTheAST = true;
               returnAttribute.unparseUsingTokenStream = false;
             }

       // AstAttribute::AttributeNodeInfo* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
       // AstAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
          FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
#if 0
          printf ("Adding attribute: test 1: blue \n");
#endif
          statement->setAttribute(name,attribute);

#if 0
          if (returnAttribute.isFrontier == true)
             {
            // attribute->set_unparseUsingTokenStream(true);
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }
            else
             {
            // attribute->set_containsNodesToBeUnparsedFromTheAST(true);
             }
#endif
#if 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif
        }
       else
        {
       // Default setting for all non-SgStatements.
       // returnAttribute.unparseUsingTokenStream = true;

          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if 0
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
#endif
               returnAttribute.unparseFromTheAST = true;
               returnAttribute.unparseUsingTokenStream = false;
             }
            else
             {
            // Default setting for all non-SgStatements.
               returnAttribute.unparseUsingTokenStream = true;
             }

#if 0
          printf ("Non-SgStatement node: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif
        }

#if 0
     printf ("*** Leaving FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );

     if (returnAttribute.isFrontier == true)
        {
          ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
        }

     return returnAttribute;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // The goal of this function is to identify the node ranges in the frontier that are associated with 
  // tokens stream unparsing, and AST node unparsing.  There ranges are saved and concatinated as we
  // proceed in the evaluation of the synthesized attributes up the AST.

  // We want to generate a IR node range in each node which contains children so that we can concatinate the lists
  // across the whole AST and define the frontier in terms of IR nodes which will then be converted into 
  // token ranges to be unparsed and specific IR nodes to be unparsed from the AST directly.

     ROSE_ASSERT(n != NULL);

#if 0
     printf ("\n\n### In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n);

  // We need to handle SgStatement, plus the SgSourceFile because we need to copy synthesized results from the SgGlobal to the SgSourceFile.
     SgStatement* statement = isSgStatement(n);
     SgSourceFile* sourceFile = isSgSourceFile(n);
     if (statement != NULL || sourceFile != NULL)
        {
#if 0
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): SgStatement or SgSourceFile = %p = %s \n",n,n->class_name().c_str());
#endif

       // Mark these directly.
          returnAttribute.isFrontier              = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST       = inheritedAttribute.unparseFromTheAST;

          if (inheritedAttribute.isFrontier == true)
             {
            // The frontier is by definition, to be unparsed from the AST.  The frontier is not 
            // defined in the inherited attribute when it can be unparsed from the token stream.
#if 0
               printf ("For inheritedAttribute.isFrontier == true: set returnAttribute.unparseFromTheAST = true \n");
#endif
               returnAttribute.unparseFromTheAST = true;
             }

          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if 0
               printf ("For inheritedAttribute.unparseFromTheAST == true: set returnAttribute.containsNodesToBeUnparsedFromTheAST = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }

          if (inheritedAttribute.unparseUsingTokenStream == true)
             {
#if 0
               printf ("For inheritedAttribute.unparseUsingTokenStream == true: set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
             }

       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false);
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );

#if 0
          printf ("After using info from the inheritedAttribute: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
          if (synthesizedAttributeList.size() == 0)
             {
               returnAttribute.unparseUsingTokenStream = true;
             }
#endif

#if 0
       // Check if there is any frontier amongst the child IR nodes.
          bool isFrontier = false;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               if (synthesizedAttributeList[i].isFrontier == true)
                  {
                    isFrontier = true;
                  }
             }

          if (isFrontier == true)
             {
            // If there is a frontier amongst the children, then ALL of them are a part of the frontier.
               for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                  {
                    synthesizedAttributeList[i].isFrontier = true;
                  }
             }
#endif

       // bool nodeTouchesFronter = false;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
            // ROSE_ASSERT(synthesizedAttributeList[i].node != NULL);
#if 0
               printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                    i,synthesizedAttributeList[i].node,
                    synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                    synthesizedAttributeList[i].isFrontier                                  ? "true" : "false",
                    synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true" : "false",
                    synthesizedAttributeList[i].unparseFromTheAST                           ? "true" : "false",
                    synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true" : "false",
                    synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
               if (synthesizedAttributeList[i].node == NULL)
                  {
#if 0
                    printf ("Case of synthesizedAttributeList[i].node == NULL: skipping using info from this child synthesized attribute to set the returnAttribute \n");
#endif
                    continue;
                  }

            // if (synthesizedAttributeList[i].isFrontier == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if ( (synthesizedAttributeList[i].isFrontier == true && synthesizedAttributeList[i].unparseFromTheAST == true) || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if (synthesizedAttributeList[i].isFrontier == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
               if (synthesizedAttributeList[i].isFrontier == true)
                  {
#if 0
                    printf ("Marking containsNodesToBeUnparsedFromTheAST = true (based on synthesizedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
                 // returnAttribute.isFrontier = true;
                    returnAttribute.containsNodesToBeUnparsedFromTheAST = true;

                    if (returnAttribute.unparseUsingTokenStream == true)
                       {
#if 0
                         printf ("Marking returnAttribute.unparseUsingTokenStream = false (based on synthesizedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
                         returnAttribute.unparseUsingTokenStream = false;
                       }
#if 0
                    if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
                       {
                         returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                       }
#endif
#if 0
                    if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
                         returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                       }
#endif
                  }
                 else
                  {
                    if (synthesizedAttributeList[i].unparseUsingTokenStream == false && synthesizedAttributeList[i].unparseFromTheAST == false)
                       {
#if 0
                         printf ("Case of: synthesizedAttributeList[i].unparseUsingTokenStream == false && synthesizedAttributeList[i].unparseFromTheAST == false \n");
#endif
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseFromTheAST == false);
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseUsingTokenStream == false);

                      // returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;

                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == true)
                            {
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }

                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
                            {
                              returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                            }

                      // Lack of AST usage in the subtree implies that there are nodes unparsed from the token stream.
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                            {
#if 0
                              printf ("Set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
                           // returnAttribute.unparseUsingTokenStream = true;
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }
#if 0
                      // Lack of token stream usage in the subtree implies that there are nodes unparsed from the AST.
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == false)
                            {
                           // returnAttribute.unparseUsingTokenStream = true;
                              returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                            }
#endif
                       }
                      else
                       {
#if 0
                         printf ("Case of: synthesizedAttributeList[i].unparseUsingTokenStream == true OR synthesizedAttributeList[i].unparseFromTheAST == true \n");
#endif
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseUsingTokenStream == true || synthesizedAttributeList[i].unparseFromTheAST == true);

                         if (synthesizedAttributeList[i].unparseUsingTokenStream == true)
                            {
#if 0
                              printf ("   --- Set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }
                           else
                            {
                              if (synthesizedAttributeList[i].unparseFromTheAST == true)
                                 {
#if 0
                                   printf ("   --- Set returnAttribute.containsNodesToBeUnparsedFromTheAST = true \n");
#endif
                                   returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                                 }
                                else
                                 {
                                   printf ("ERROR: can't reach this case! \n");
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                  }

            // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
            //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
               ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                            (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                            (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );
#if 0
               printf ("At base of loop over synthesized attribute list elements i = %" PRIuPTR " \n",i);
               printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
               printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
               printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
               printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
             }

       // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
       //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

          if (inheritedAttribute.unparseFromTheAST == true)
             {
            // This does not imply that this is a frontier node, since it could be that all of the children were frontier nodes and this is to be unparsed from the AST, but not a frontier???
            // ROSE_ASSERT(returnAttribute.isFrontier == true);
               ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false);
             }

          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
             {
               string name    = "token_frontier";
               string options = "color=\"green\"";
               FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);

               if (statement != NULL)
                  {
#if 0
                    printf ("Adding attribute: test 2: green \n");
#endif
                    statement->setAttribute(name,attribute);
                  }
             }
            else
             {
            // DQ (11/30/2013): We need to build a frontier even if the whole AST is to be unparsed from the token stream.
               SgGlobal* globalScope = isSgGlobal(n);
               if (globalScope != NULL)
                  {
                 // If this is the global scope and no attributes have triggered generation from the AST, then unparse from the token stream and build an appropriate frontier node.
#if 0
                    printf ("If this is the global scope and no attributes have triggered generation from the AST, then unparse from the token stream and build an appropriate frontier node. \n");
#endif
                    returnAttribute.isFrontier = true;

                    string name    = "token_frontier";
                    string options = "color=\"blue\"";
                    FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
#if 0
                    printf ("Adding attribute: test 3: blue \n");
#endif
                    ROSE_ASSERT(statement != NULL);
                    statement->setAttribute(name,attribute);
                  }
             }
#if 0
          printf ("After loop over all child attributes in synthesizedAttributeList: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
       // If this contains IR nodes to be unparsed from the AST and from the token stream then we have 
       // to report the list explicitly, else the action of unparsing from either the AST or the token 
       // stream can be reported using the single node (and not entered into the frontier list but 
       // progated usign the synthesized attribute.
       // if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
             {
               returnAttribute.unparseFromTheAST       = false;
               returnAttribute.unparseUsingTokenStream = false;
#if 0
               printf ("   --- Saving the list of relevant frontier nodes! \n");
#endif
            // DQ (12/1/2013): This implements a restriction no conditional statements so they will not be unparsed using a mixture of tokens and AST unparsing.
               SgIfStmt* ifStatement = isSgIfStmt(statement);

            // bool specialCaseNode = (ifStatement != NULL);
               bool specialCaseNode = false;
          
            // if (ifStatement != NULL)
            // if (specialCaseNode == true)
               if (ifStatement != NULL)
                  {
                 // There are special cases where we don't (at least presently) want to mix the two types on unparsing.
                 // This is how those special cases are handled.
#if 0
                    printf ("Synthesized attribute evaluation is part of a conditional statment (containing a conditional expression/statement): statment = %p = %s \n",statement,statement->class_name().c_str());
#endif
#if 0
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST != synthesizedAttributeList[1].containsNodesToBeUnparsedFromTheAST)
                       {
#if 0
                         printf ("This node has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(statement);
               if (typedefDeclaration != NULL)
                  {
#if 0
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST == true && synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
#if 0
                         printf ("This SgTypedefDeclaration node has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

            // DQ (12/1/2013): This handles the details of the SgForStatement (which has 3 relevant children (excluding the body).
               SgForStatement* forStatement = isSgForStatement(statement);
               if (forStatement != NULL)
                  {
                 // There are special cases where we don't (at least presently) want to mix the two types on unparsing.
                 // This is how those special cases are handled.
#if 0
                    printf ("Synthesized attribute evaluation is part of a SgForStatement (containing a conditional expression/statement): statment = %p = %s \n",statement,statement->class_name().c_str());
#endif
#if 0
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                 // I don't know why unparseUsingTokenStream are different, but the containsNodesToBeUnparsedFromTheAST are the same.
                 // if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST != synthesizedAttributeList[1].containsNodesToBeUnparsedFromTheAST)
                    if ( (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[1].unparseUsingTokenStream) ||
                         (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[2].unparseUsingTokenStream) )
                       {
#if 0
                         printf ("This node (SgForStatement) has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               if (specialCaseNode == true)
                  {
                 // Mark the current node to be a frontier, instead of the child nodes.  Mark as to be unparse from the AST (no choice since subtrees must be unparsed from the AST).
#if 0
                    printf ("Handling synthesized attribute as a special case: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                    returnAttribute.isFrontier   = true;

                    bool unparseUsingTokenStream = false;
                    bool unparseFromTheAST       = true;

                    FrontierNode* frontierNode = new FrontierNode(statement,unparseUsingTokenStream,unparseFromTheAST);
                    ROSE_ASSERT(frontierNode != NULL);
                    returnAttribute.frontierNodes.push_back(frontierNode);

                    string name    = "token_frontier";
                    string options = "color=\"red\"";
                    FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
#if 0
                    printf ("Adding attribute: test 4: red \n");
#endif
                    statement->setAttribute(name,attribute);
                  }
                 else
                  {
                 // This is the non-special case.
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         SgStatement* synthesized_attribute_statement = synthesizedAttributeList[i].node;
#if 0
                         printf ("   --- synthesized_attribute_statement = %p = %s \n",synthesized_attribute_statement,synthesized_attribute_statement != NULL ? synthesized_attribute_statement->class_name().c_str() : "null");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].isFrontier                                  = %s \n",i,synthesizedAttributeList[i].isFrontier ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].unparseUsingTokenStream                     = %s \n",i,synthesizedAttributeList[i].unparseUsingTokenStream ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].unparseFromTheAST                           = %s \n",i,synthesizedAttributeList[i].unparseFromTheAST ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].containsNodesToBeUnparsedFromTheAST         = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].containsNodesToBeUnparsedFromTheTokenStream = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
                         if (synthesized_attribute_statement != NULL)
                            {
                              FrontierNode* frontierNode = new FrontierNode(synthesized_attribute_statement,synthesizedAttributeList[i].unparseUsingTokenStream,synthesizedAttributeList[i].unparseFromTheAST);
                              ROSE_ASSERT(frontierNode != NULL);

                              if (synthesizedAttributeList[i].unparseUsingTokenStream == true)
                                 {
                                   returnAttribute.frontierNodes.push_back(frontierNode);
                                 }
                                else
                                 {
                                   if (synthesizedAttributeList[i].unparseFromTheAST == true)
                                      {
                                        returnAttribute.frontierNodes.push_back(frontierNode);
                                      }
                                     else
                                      {
#if 0
                                        printf ("Current node = %p = %s not added to frontier node list (add any lists from synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR ") \n",n,n->class_name().c_str(),i,synthesizedAttributeList[i].frontierNodes.size());
#endif
                                     // We don't record nodes that are just containing subtrees to be unparsed from the AST.
                                     // if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                           {
                                             for (size_t j = 0; j < synthesizedAttributeList[i].frontierNodes.size(); j++)
                                                {
                                                  FrontierNode* frontierNode = synthesizedAttributeList[i].frontierNodes[j];
                                                  ROSE_ASSERT(frontierNode != NULL);

                                                  returnAttribute.frontierNodes.push_back(frontierNode);
#if 0
                                                  printf ("Include synthesized_attribute frontierNodes into frontierNodes (%p = %s) \n",frontierNode,frontierNode->display().c_str());
#endif
                                                }
                                           }
                                      }
                                 }
                            }
                           else
                            {
#if 0
                              printf ("WARNING: synthesized_attribute_statement == NULL \n");
#endif
                            }
#if 0
                      // ROSE_ASSERT(synthesized_attribute_statement != NULL);
                         if (synthesized_attribute_statement != NULL)
                            {
                              printf ("   ---   --- Include synthesized_attribute_statement into frontierNodes (%p = %s) \n",synthesized_attribute_statement,synthesized_attribute_statement->class_name().c_str());
                              returnAttribute.frontierNodes.push_back(synthesized_attribute_statement);

                              printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].containsNodesToBeUnparsedFromTheAST = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST ? "true" : "false");

                           // We don't record nodes that are just containing subtrees to be unparsed from the AST.
                              if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                 {
                                   for (size_t j = 0; j < synthesizedAttributeList[i].frontierNodes.size(); j++)
                                      {
                                        SgStatement* statement = synthesizedAttributeList[i].frontierNodes[j];
                                        ROSE_ASSERT(statement != NULL);

                                        returnAttribute.frontierNodes.push_back(statement);

                                        printf ("Include synthesized_attribute frontierNodes into frontierNodes (%p = %s) \n",statement,statement->class_name().c_str());
                                      }
                                 }
                            }
#endif
                       }
                  }
             }
            else
             {
#if 0
               printf ("   --- List of frontier nodes not saved, but the current node is saved \n");
#endif
            // SgStatement* statement = synthesizedAttributeList[i].node;
            // ROSE_ASSERT(statement != NULL);
               if (statement != NULL)
                  {
                    FrontierNode* frontierNode = new FrontierNode(statement,returnAttribute.unparseUsingTokenStream,returnAttribute.unparseFromTheAST);
                    returnAttribute.frontierNodes.push_back(frontierNode);

                 // If this is marked as containing nodes to be unparsed from either the token stream or the AST then reset the unparsing for this specific node.
                    if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true || returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
                         returnAttribute.unparseFromTheAST       = false;
                         returnAttribute.unparseUsingTokenStream = false;
                       }
                  }
                 else
                  {
                 // This case happens for test_CplusplusMacro_C.c, which has nothing but CPP directives.
                  }

#if 0
               if (returnAttribute.unparseUsingTokenStream == true)
                  {
                    SgStatement* statement = isSgStatement(n);
                    returnAttribute.frontierNodes.push_back(statement);
                    printf ("Include current AST node into frontierNodes (%p = %s) ",statement,statement->class_name().c_str());
                  }
                 else
                  {
                 // This does not yet identify this how as to be unparsed from the AST.
                 // ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheAST == true);
                 // returnAttribute.frontierNodes.push_back(synthesizedAttributeList[i].node);              
                  }
#endif
             }

#if 0
          printf ("Frontier (n = %p = %s): ",n,n->class_name().c_str());
          for (size_t j = 0; j < returnAttribute.frontierNodes.size(); j++)
             {
            // SgStatement* statement = returnAttribute.frontierNodes[j];
               FrontierNode* frontierNode = returnAttribute.frontierNodes[j];
               ROSE_ASSERT(frontierNode != NULL);
               SgStatement* statement = frontierNode->node;
               ROSE_ASSERT(statement != NULL);

            // printf (" (%p = %s) ",statement,statement->class_name().c_str());
               printf (" (%s) ",frontierNode->display().c_str());
             }
          printf (" Frontier End \n");
#endif
#if 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Default settings.
       // returnAttribute.unparseUsingTokenStream = true;
          returnAttribute.isFrontier                                  = false;
          returnAttribute.unparseUsingTokenStream                     = false;
          returnAttribute.unparseFromTheAST                           = false;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;
          returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = false;

          returnAttribute.isFrontier                                  = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream                     = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST                           = inheritedAttribute.unparseFromTheAST;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = inheritedAttribute.containsNodesToBeUnparsedFromTheAST;
#if 0
          printf ("Case of non-SgStatement and non-SgSourceFile: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
        }

#if 0
     printf ("### Leaving FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): BOTTOM n = %p = %s \n",n,n->class_name().c_str());
#endif

#if 1
  // This can't be applied to non-statements.
  // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
  //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );
#endif

     return returnAttribute;
   }


int
FrontierDetectionForTokenStreamMapping::numberOfNodesInSubtree(SgSourceFile* sourceFile)
   {
     int value = 0;

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}

           // We only want to count statements since we only test the token/AST unparsing at the statement level.
           // void visit ( SgNode* n ) { count++; }
              void visit ( SgNode* n ) { if (isSgStatement(n) != NULL) count++; }
        };

     CountTraversal counter;
  // SgNode* thisNode = const_cast<SgNode*>(this);
     counter.traverseWithinFile(sourceFile,preorder);
     value = counter.count;

     return value;
   }

void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
   {
  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).

     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p \n",sourceFile);
#endif

#if 0
     std::set<SgStatement*> transformedStatementSet = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In frontierDetectionForTokenStreamMapping(): transformedStatementSet.size() = %zu \n",transformedStatementSet.size());
#endif
#if 0
  // Debugging (this set in the constructor).
     printf ("numberOfNodes = %d \n",fdTraversal.numberOfNodes);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseInputFiles(sourceFile,inheritedAttribute);
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);

#if 0
     printf ("   --- topAttribute.isFrontier                                  = %s \n",topAttribute.isFrontier ? "true" : "false");
     printf ("   --- topAttribute.unparseUsingTokenStream                     = %s \n",topAttribute.unparseUsingTokenStream ? "true" : "false");
     printf ("   --- topAttribute.unparseFromTheAST                           = %s \n",topAttribute.unparseFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",topAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",topAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     map<SgStatement*,FrontierNode*> token_unparse_frontier_map;

#if 0
     printf ("Final Frontier (topAttribute.frontierNodes.size() = %" PRIuPTR ") (n = %p = %s): ",topAttribute.frontierNodes.size(),sourceFile,sourceFile->class_name().c_str());
#endif

  // DQ (11/29/2013): Used to mark statements that are redundantly mapped to a single token sequence.
  // int lastTokenIndex = 0;

     for (size_t j = 0; j < topAttribute.frontierNodes.size(); j++)
        {
       // SgStatement* statement = topAttribute.frontierNodes[j];
       // ROSE_ASSERT(statement != NULL);
          FrontierNode* frontierNode = topAttribute.frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* statement = frontierNode->node;
          ROSE_ASSERT(statement != NULL);

#if 0
          printf (" (%p = %s) \n",statement,statement->class_name().c_str());
#endif
          string name    = "token_frontier";
       // string options = "fillcolor=\"greenyellow\",style=filled";
          string options;
          if (frontierNode->unparseUsingTokenStream == true)
             {
               options = "fillcolor=\"greenyellow\",style=filled";
             }
            else
             {
               ROSE_ASSERT(frontierNode->unparseFromTheAST == true);
               options = "fillcolor=\"skyblue\",style=filled";
             }
          FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( statement, name, options);

#if 0
          printf ("Adding attribute: test 5: skyblue or greenyellow \n");
#endif
          statement->setAttribute(name,attribute);

#if 0
       // DQ (11/29/2013): Get the token mapping to the AST.
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

          printf ("In frontierDetectionForTokenStreamMapping(): tokenStreamSequenceMap.size() = %" PRIuPTR " lastTokenIndex = %d \n",tokenStreamSequenceMap.size(),lastTokenIndex);

          ROSE_ASSERT(tokenStreamSequenceMap.empty() == false);

          if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
             {
               printf ("In frontierDetectionForTokenStreamMapping(): Found associated tokenStreamSequence for statement = %p = %s \n",statement,statement->class_name().c_str());

               TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[statement];
               ROSE_ASSERT(tokenSubsequence != NULL);

               ROSE_ASSERT(tokenStreamSequenceMap.empty() == false);

               if (lastTokenIndex == tokenSubsequence->token_subsequence_end)
                  {
                 // Mark this as a frontier that is redundantly mapped to a token sequence.
                    printf ("Mark this as a frontier that is redundantly mapped to a token sequence \n");
                    ROSE_ASSERT(false);

                 // tokenSubsequence->redundant_token_subsequence = true;
                  }
                 else
                  {
                 // This is the typical case.
                  }

               lastTokenIndex = tokenSubsequence->token_subsequence_end;
             }
            else
             {
            // DQ (11/29/2013): Not certain this should be an error or a warning.
               printf ("WARNING: There is no token sequence mapping for this statement = %p = %s \n",statement,statement->class_name().c_str());
#if 0
               ROSE_ASSERT(false);
#endif
             }
#endif

       // Setup the map of SgStatement pointers to FrontierNode pointers.
          token_unparse_frontier_map[statement] = frontierNode;
        }
#if 0
     printf (" Frontier End \n");
#endif

     sourceFile->set_token_unparse_frontier(token_unparse_frontier_map);

  // Output an optional graph of the AST (just the tree, so we can identify the frontier)
     SgFileList* fileList = isSgFileList(sourceFile->get_parent());

  // DQ (9/10/2018): Alternatives are either the SgFileList (for the source file) or the SgHeaderFileBody (for header files)
  // ROSE_ASSERT(fileList != NULL);
     SgProject* project = fileList != NULL ? isSgProject(fileList->get_parent()) : NULL;
     if (project != NULL)
        {
       // ROSE_ASSERT(project != NULL);

#if 0
          printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier defined \n");
#endif

#if 1
          if ( SgProject::get_verbose() > 1 )
             {
               generateDOT ( *project, "_token_unparsing_frontier" );
             }
#endif
        }

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier defined \n");
#endif

  // Now traverse the AST and record the linked list of nodes to be unparsed as tokens and from the AST.
  // So that we can query next and last statements and determine if they were unparsed from the token 
  // stream or the AST.  Not clear if the edges of token-stream/AST unparsing should be unparsed from the 
  // token stream leading trailing token information or from the AST using the attached CPP info.

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Calling previousAndNextNodeTraversal() \n");
#endif

     string name_next     = "frontier_next";
     string name_previous = "frontier_previous";
     string options = "color=\"red\"";

  // std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = previousAndNextNodeTraversal(sourceFile);
     std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = computePreviousAndNextNodes(sourceFile->get_globalScope(),topAttribute.frontierNodes);

     sourceFile->set_token_unparse_frontier_adjacency(previousAndNextNodeMap);

  // sourceFile->set_token_unparse_frontier(topAttribute.frontierNodes);

#if 0
     printf ("Output previousAndNextNodeMap for frontier: \n");
#endif
     for (size_t j = 0; j < topAttribute.frontierNodes.size(); j++)
        {
          ROSE_ASSERT(topAttribute.frontierNodes[j]->node != NULL);
          FrontierNode* frontierNode = topAttribute.frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* frontier_statement = frontierNode->node;
          ROSE_ASSERT(frontier_statement != NULL);
#if 0
          printf ("frontier_statement = %p = %s \n",frontier_statement,frontier_statement->class_name().c_str());
#endif
          std::map<SgNode*,PreviousAndNextNodeData*>::iterator i = previousAndNextNodeMap.find(frontier_statement);
       // ROSE_ASSERT(i != previousAndNextNodeMap.end());
          if (i != previousAndNextNodeMap.end())
             {
#if 0
               printf ("   --- it for i->first = %p = %s i->second = %p i->second->previous = %p = %s i->second->next = %p = %s \n",
                    i->first,i->first->class_name().c_str(),i->second,
                    i->second->previous,i->second->previous != NULL ? i->second->previous->class_name().c_str() : "null",
                    i->second->next,i->second->next != NULL ? i->second->next->class_name().c_str() : "null");
#endif
               SgStatement* statement = isSgStatement(i->first);
               if (statement != NULL)
                  {
                    PreviousAndNextAttribute* edgeAttribute_next = new PreviousAndNextAttribute(i->first, i->second->next, name_next, options);
                    statement->setAttribute(name_next,edgeAttribute_next);
                    PreviousAndNextAttribute* edgeAttribute_previous = new PreviousAndNextAttribute(i->first, i->second->previous, name_previous, options);
                    statement->setAttribute(name_previous,edgeAttribute_previous);
                  }
             }
            else
             {
               printf ("WARNING: --- could not find associated IR node in previousAndNextNodeMap \n");
             }
        }

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier AND edges defined \n");
#endif

#if 1
     if (project != NULL)
        {
          if ( SgProject::get_verbose() > 1 )
             {
               generateDOT(*project,"_token_unparsing_frontier_with_next_previous_edges");
             }
        }
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier AND edges defined \n");
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Calling previousAndNextNodeTraversal() \n");
#endif

#if 0
  // DQ (11/20/2013): Test using support for multiple files for Java testing.
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
  // SgProject* project = isSgProject(sourceFile->get_project());
  // SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

     printf ("In frontierDetectionForTokenStreamMapping(): Output dot file for project \n");

     generateDOTforMultipleFile(*project,"in_frontierDetectionForTokenStreamMapping");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }




FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(SgNode* n, std::string name, std::string options)
// DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
// : node(n), 
   : name(name), options(options)
   {
#if 0
     printf ("In FrontierDetectionForTokenStreamMappingAttribute constructor! name = %s options = %s \n",name.c_str(),options.c_str());
#endif
  // ROSE_ASSERT(false);
   }

FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute & X)
   {
     printf ("In FrontierDetectionForTokenStreamMappingAttribute copy constructor! \n");
     ROSE_ASSERT(false);
   }

string
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

  // return "red";
  // return "color=\"red\"";
     return options;
   }


vector<AstAttribute::AttributeEdgeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeEdgeInfo>();
   }


vector<AstAttribute::AttributeNodeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeNodeInfo>();
   }

AstAttribute*
FrontierDetectionForTokenStreamMappingAttribute::copy() const
   {
  // Support for the copying of AST and associated attributes on each IR node (required for attributes 
  // derived from AstAttribute, else just the base class AstAttribute will be copied).

#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     ROSE_ASSERT(false);

     return new FrontierDetectionForTokenStreamMappingAttribute(*this);
#else
     printf ("Warning: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     return NULL;
#endif
   }

// DQ (11/14/2017): This addition is not portable, should not be specified outside of the class definition, 
// and fails for C++11 mode on the GNU 4.8.5 compiler and the llvm (some version that Craig used).
// DQ (6/11/2017): Added virtual function now required to eliminate warning at runtime.
AstAttribute::OwnershipPolicy
FrontierDetectionForTokenStreamMappingAttribute::getOwnershipPolicy() const // ROSE_OVERRIDE 
   {
     return CONTAINER_OWNERSHIP;
   }


#if 0
void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseUsingTokenStream( bool flag )
   {
     unparseUsingTokenStream = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseUsingTokenStream()
   {
     return unparseUsingTokenStream;
   }


void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseFromTheAST( bool flag )
   {
     unparseFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseFromTheAST()
   {
     return unparseFromTheAST;
   }

void
FrontierDetectionForTokenStreamMappingAttribute::set_containsNodesToBeUnparsedFromTheAST( bool flag )
   {
     containsNodesToBeUnparsedFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_containsNodesToBeUnparsedFromTheAST()
   {
     return containsNodesToBeUnparsedFromTheAST;
   }
#endif


FrontierNode::FrontierNode(SgStatement* node, bool unparseUsingTokenStream, bool unparseFromTheAST)
   : node(node), 
     unparseUsingTokenStream(unparseUsingTokenStream), 
     unparseFromTheAST(unparseFromTheAST),
     redundant_token_subsequence(false)
   {
  // Enforce specific constraints.
     ROSE_ASSERT(node != NULL);
  // ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) );
     ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) || (unparseUsingTokenStream == false && unparseFromTheAST == false));
     ROSE_ASSERT(redundant_token_subsequence == false);
   }


std::string 
FrontierNode::display()
   {
     string s;

     s += string("node=") + StringUtility::numberToString(node) + string(",") + node->class_name() + string(":");
     s += string("(TS=")  + (unparseUsingTokenStream == true ? "true" : "false"); 
     s += string(",AST=") + (unparseFromTheAST == true ? "true" : "false") + ")";
     s += string(" ");

     return s;
   }

