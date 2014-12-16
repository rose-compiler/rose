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

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
// ROSE_DLL_API bool ROSE_tokenUnparsingTestingMode = false;

SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute()
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


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
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


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute ( const SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
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


SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = NULL;
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
   {
     node       = isSgStatement(n);
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
     node       = X.node;
     isFrontier = X.isFrontier;

  // frontierNodes    = X.frontierNodes;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
   }


SimpleFrontierDetectionForTokenStreamMapping::
SimpleFrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
  // This is the number of IR nodes in the current file and helpful in randomly 
  // marking IR nodes for testing to be either from the AST or from the token stream.
     numberOfNodes = numberOfNodesInSubtree(sourceFile);
   }


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute
SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(SgNode* n, SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute)
   {
     static int random_counter = 0;

     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute;

#if 0
     printf ("*** In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d n = %p = %s \n",random_counter,n,n->class_name().c_str());
#endif

     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != NULL)
        {
          if (locatedNode->isTransformation() == true)
             {
#if 0
               printf ("Found locatedNode = %p = %s as transformation \n",locatedNode,locatedNode->class_name().c_str());
            // ROSE_ASSERT(false);
#endif
             }
        }

     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {

        }

     return returnAttribute;
   }





SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
SimpleFrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
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
     printf ("### In SimpleFrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n);

#if 0
     printf ("   --- synthesizedAttributeList.size() = %zu \n",synthesizedAttributeList.size());
#endif

     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
       // ROSE_ASSERT(synthesizedAttributeList[i].node != NULL);
#if 0
          printf ("   --- synthesizedAttributeList[i=%zu].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
               i,synthesizedAttributeList[i].node,
               synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
               synthesizedAttributeList[i].isFrontier                                  ? "true" : "false",
               synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true" : "false",
               synthesizedAttributeList[i].unparseFromTheAST                           ? "true" : "false",
               synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true" : "false",
               synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
#if 1
          SgStatement* statement = isSgStatement(synthesizedAttributeList[i].node);
          if (statement != NULL)
             {
               SgStatement* currentStatement = isSgStatement(n);
#if 0
               if (currentStatement != NULL)
                  {
                    printf ("   --- currentStatement->isTransformation()    = %s \n",currentStatement->isTransformation() ? "true" : "false");
                  }
               printf ("   --- statement->isTransformation()           = %s \n",statement->isTransformation() ? "true" : "false");
               printf ("   --- statement->get_containsTransformation() = %s \n",statement->get_containsTransformation() ? "true" : "false");
#endif
            // if (statement->isTransformation() == true || statement->get_containsTransformation() == true)
               if ( currentStatement != NULL && currentStatement->isTransformation() == false && (statement->isTransformation() == true || statement->get_containsTransformation() == true) )
                  {
                    n->set_containsTransformation(true);
                  }
             }
#endif
        }

     return returnAttribute;
   }



int
SimpleFrontierDetectionForTokenStreamMapping::numberOfNodesInSubtree(SgSourceFile* sourceFile)
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
simpleFrontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
   {
  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).

     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     SimpleFrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 0
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p \n",sourceFile);
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
     SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);

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

   }
