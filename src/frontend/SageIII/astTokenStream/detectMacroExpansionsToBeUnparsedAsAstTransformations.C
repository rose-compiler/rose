// detectMacroExpansionsToBeUnparsedAsAstTransformations.C

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


#if 0
void 
DetectMacroExpansionsToBeUnparsedAsAstTransformations::visit( SgNode* n )
   {
     ROSE_ASSERT(n != NULL);

     

   }
#endif


// Inherited attribute member functions
DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute::DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute()
   {
   }

DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute::
DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute( const DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute & X )
   {
   }


// Synthesized attribute member functions
DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute::DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute()
   {
     node = NULL;
   }

DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute::DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute( SgNode* n )
   {
     node = n;
   }

DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute::
DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute( const DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute & X )
   {
     node = X.node;
   }



// AST traversal class member functions
// DetectMacroExpansionsToBeUnparsedAsAstTransformations::DetectMacroExpansionsToBeUnparsedAsAstTransformations(SgSourceFile* sourceFile)
//   {
//   }

DetectMacroExpansionsToBeUnparsedAsAstTransformations::DetectMacroExpansionsToBeUnparsedAsAstTransformations( std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap )
   : tokenStreamSequenceMap(input_tokenStreamSequenceMap)
   {
   }


DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute 
DetectMacroExpansionsToBeUnparsedAsAstTransformations::evaluateInheritedAttribute(
   SgNode* n, 
   DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute )
   {
     return inheritedAttribute;
   }


DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute 
DetectMacroExpansionsToBeUnparsedAsAstTransformations::evaluateSynthesizedAttribute ( 
   SgNode* n, 
   DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute, 
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute returnAttribute(n);

#if 0
     printf ("In (Detect Macro Expansions) evaluateSynthesizedAttribute(): n = %p = %s n->get_containsTransformation() = %s \n",n,n->class_name().c_str(),n->get_containsTransformation() ? "true" : "false");
#endif

  // DQ (11/8/2015): This has to be moved to after the tokenStreamSequenceMap has been setup since we need that to determine if 
  // IR nodes have a token mapping or not (subparts of macros expansions will not and we need this infor to recognize parts of 
  // the AST that are associated with macro expansions.
  // DQ (11/8/2015): If this has been marked as containing a transformation then check if there is token info for each of the children.
  // If there is not token info for each of the children then this currentStatement (e.g. n) must be marked as a transformation.
  // This case happens when a transformation is done to a child of a statement that is part of a macro.  In this case the parent will
  // have token information which is the macro call, but since there is a transformation, we have to unparse the fully expanded form 
  // of the macro (from the AST), so the whole subtree must be unparsed.  NOTE: this case might be more complex if multiple statements
  // are associated with a macro (so that there is not a single root of the subtree.  I need to build an example of this to better 
  // understand if there is a problem and if so just what would be the best solution.  It will b at least an iterative refinement of
  // this specific problem.  See tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_135.C for an example
  // of this problem.
     if (n->get_containsTransformation() == true)
        {
#if 0
          printf ("Found case of statement marked as containing a transforamtion \n");
#endif
          SgStatement* currentStatement = isSgStatement(n);
#if 0
          if (currentStatement != NULL)
             {
               printf ("currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
               printf ("   --- currentStatement->isTransformation()    = %s \n",currentStatement->isTransformation() ? "true" : "false");
             }
#endif
       // We have to test for a macro exapansion (will only work on statement level grainularity where parent statement has child statements).
          bool all_children_have_token_info = true;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               SgStatement* statement = isSgStatement(synthesizedAttributeList[i].node);
               if (statement != NULL)
                  {
#if 0
                    printf ("(child) statement = %p = %s \n",statement,statement->class_name().c_str());
                    printf ("   --- statement->isTransformation()           = %s \n",statement->isTransformation() ? "true" : "false");
                    printf ("   --- statement->get_containsTransformation() = %s \n",statement->get_containsTransformation() ? "true" : "false");
#endif
#if 1
                 // DQ (11/8/2015): We might need to also check the surrounding white space as well (except that I think this is set later).
                    if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                       {
                      // If we have a token mapping then we don't have to do anything.
                         TokenStreamSequenceToNodeMapping* mapping = tokenStreamSequenceMap[statement];
                         ROSE_ASSERT(mapping != NULL);
                       }
                      else
                       {
                         all_children_have_token_info = false;
                       }
#endif
                  }
             }

          if (currentStatement != NULL && all_children_have_token_info == false)
             {
#if 0
               printf ("*** Found case of statement marked as containing a transforamtion, but all children without token info (detected a macro expansion): currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif

            // DQ (11/8/2015): I think that this should not apply to a SgBasicBlock (for example see 
            // tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_94.C).
            // The reason is that a block is not the same sort for compound statement as a SgForStatement.
               if (isSgBasicBlock(currentStatement) == NULL)
                  {
                 // Mark as a transformation instead of containing a transformation.
                    currentStatement->setTransformation();

                 // We also need to mark this too!
                    currentStatement->setOutputInCodeGeneration();

                 // And reset this to NOT contain a transformation.
                    currentStatement->set_containsTransformation(false);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
#if 0
       // Debugging code.
          if (isSgForStatement(n) != NULL)
             {
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
#endif
        }

     return returnAttribute;
   }

void
detectMacroExpansionsToBeUnparsedAsAstTransformations ( SgSourceFile* sourceFile )
   {

  // sourceFile->set_tokenSubsequenceMap(tokenMappingTraversal.tokenStreamSequenceMap);
  // tokenMappingTraversal = sourceFile->get_tokenSubsequenceMap();
     map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

  // DQ (11/8/2015): Note that this call to the constructor fails to compile (when called with "()") (GNU g++ version 8.4.3 compiler).
  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute();
     DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute;

     DetectMacroExpansionsToBeUnparsedAsAstTransformations traversal(tokenStreamSequenceMap);

  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // traversal.traverse(sourceFile,inheritedAttribute);
     DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute topAttribute = traversal.traverseWithinFile(sourceFile,inheritedAttribute);

     ROSE_ASSERT(topAttribute.node != NULL);

#if 0
     printf ("Completed detection of macro expansions requiring unparsing from the AST (instead of the token stream if they are not transformed) \n");
     ROSE_ASSERT(false);
#endif
   }
