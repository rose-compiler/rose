#include "sage3basic.h"
#include "tokenStreamMapping.h"
#include "previousAndNextNode.h"

using namespace std;

// Inherited attribute member functions
DetectMacroOrIncludeFileExpansionsInheritedAttribute::DetectMacroOrIncludeFileExpansionsInheritedAttribute()
   {
   }

DetectMacroOrIncludeFileExpansionsInheritedAttribute::
DetectMacroOrIncludeFileExpansionsInheritedAttribute( const DetectMacroOrIncludeFileExpansionsInheritedAttribute & X )
   {
   }


// Synthesized attribute member functions
DetectMacroOrIncludeFileExpansionsSynthesizedAttribute::DetectMacroOrIncludeFileExpansionsSynthesizedAttribute()
   {
     node = NULL;
   }

DetectMacroOrIncludeFileExpansionsSynthesizedAttribute::DetectMacroOrIncludeFileExpansionsSynthesizedAttribute( SgNode* n )
   {
     node = n;
   }

DetectMacroOrIncludeFileExpansionsSynthesizedAttribute::
DetectMacroOrIncludeFileExpansionsSynthesizedAttribute( const DetectMacroOrIncludeFileExpansionsSynthesizedAttribute & X )
   {
     node = X.node;
   }



// AST traversal class member functions
DetectMacroOrIncludeFileExpansions::DetectMacroOrIncludeFileExpansions( std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap )
   : tokenStreamSequenceMap(input_tokenStreamSequenceMap)
   {
   }


DetectMacroOrIncludeFileExpansionsInheritedAttribute 
DetectMacroOrIncludeFileExpansions::evaluateInheritedAttribute(
   SgNode* n, 
   DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute )
   {
     return inheritedAttribute;
   }


DetectMacroOrIncludeFileExpansionsSynthesizedAttribute 
DetectMacroOrIncludeFileExpansions::evaluateSynthesizedAttribute ( 
   SgNode* n, 
   DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute, 
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     DetectMacroOrIncludeFileExpansionsSynthesizedAttribute returnAttribute(n);

  // DQ (11/10/2015): This traversal detects statements that contain child statements that don't have a mapping in the 
  // token sequence.  These are represnetative of generated code in the source file from either a macro expansion or an 
  // include directive.  Transformations in code generated this way should either not be transformed or it should be 
  // unparsed from the AST at the level of the macro or included file.

#if 1
  // printf ("In (Detect Macro or include file Expansions) evaluateSynthesizedAttribute(): n = %p = %s n->get_containsTransformation() = %s \n",n,n->class_name().c_str(),n->get_containsTransformation() ? "true" : "false");
     printf ("In (Detect Macro or include file Expansions) evaluateSynthesizedAttribute(): n = %s n->get_containsTransformation() = %s \n",n->class_name().c_str(),n->get_containsTransformation() ? "true" : "false");
#endif

     SgStatement* currentStatement = isSgStatement(n);
     if (currentStatement != NULL)
        {
#if 1
       // printf ("currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
          printf ("currentStatement = %s \n",currentStatement->class_name().c_str());
          printf ("   --- currentStatement->isTransformation()    = %s \n",currentStatement->isTransformation() ? "true" : "false");
#endif
       // At this point after the construction of the AST, nothing shoud be marked as a transformation.
          ROSE_ASSERT(currentStatement->isTransformation() == false);

       // We have to test for a macro exapansion (will only work on statement level grainularity where parent statement has child statements).
          bool all_children_have_token_info = true;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               SgStatement* childStatement = isSgStatement(synthesizedAttributeList[i].node);
               if (childStatement != NULL)
                  {
#if 1
                 // printf ("(child) childStatement = %p = %s \n",childStatement,childStatement->class_name().c_str());
                    printf ("(child) childStatement = %s \n",childStatement->class_name().c_str());
                    printf ("   --- childStatement->isTransformation()           = %s \n",childStatement->isTransformation() ? "true" : "false");
                    printf ("   --- childStatement->get_containsTransformation() = %s \n",childStatement->get_containsTransformation() ? "true" : "false");
#endif
                 // At this point after the construction of the AST, nothing shoud be marked as a transformation.
                    ROSE_ASSERT(childStatement->isTransformation() == false);
                    ROSE_ASSERT(childStatement->get_containsTransformation() == false);

                 // DQ (11/8/2015): We might need to also check the surrounding white space as well (except that I think this is set later).
                    if (tokenStreamSequenceMap.find(childStatement) != tokenStreamSequenceMap.end())
                       {
                      // If we have a token mapping then we don't have to do anything.
                         TokenStreamSequenceToNodeMapping* mapping = tokenStreamSequenceMap[childStatement];
                         ROSE_ASSERT(mapping != NULL);
                       }
                      else
                       {
#if 1
                      // printf ("Parent statement = %p = %s No token stream information found for child statement = %p = %s \n",
                      //      currentStatement,currentStatement->class_name().c_str(),statement,statement->class_name().c_str());
                         printf ("Parent statement = %s No token stream information found for childStatement = %s \n",
                              currentStatement->class_name().c_str(),childStatement->class_name().c_str());
                         printf ("   --- at line: %d \n",childStatement->get_file_info()->get_line());

                      // When this is a function declaration, try to understand more about it.
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(childStatement);
                         if (functionDeclaration != NULL)
                            {
                              printf ("   --- functionDeclaration name = %s \n",functionDeclaration->get_name().str());
                            }
#endif
                         all_children_have_token_info = false;
                       }
                  }
             }

          ROSE_ASSERT(currentStatement != NULL);
          if (all_children_have_token_info == false)
             {
#if 1
               printf ("*** Found case of statement representated by a macro (i.e. all children without token info) (detected a macro expansion): currentStatement = %s \n",currentStatement->class_name().c_str());
#endif

            // DQ (11/9/2015): Added support for specific scopes where we don't want them the be 
            // unparsed from the token stream when children of them are transformed.
            // DQ (11/8/2015): I think that this should not apply to a SgBasicBlock (for example see 
            // tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_94.C).
            // The reason is that a block is not the same sort for compound statement as a SgForStatement.
            // if (isSgBasicBlock(currentStatement) == NULL)
               bool current_statement_is_allowed_to_have_statements_with_unmapped_token_sequences = 
                    ( (isSgGlobal(currentStatement) != NULL)          || 
                      (isSgBasicBlock(currentStatement) != NULL)      ||
                   // (isSgEnumDefinition(currentStatement) != NULL)  ||
                      (isSgClassDefinition(currentStatement) != NULL) );

               if (current_statement_is_allowed_to_have_statements_with_unmapped_token_sequences == false)
                  {
#if 1
                    printf ("Marking currentStatement = %p = %s as a macro or include file expansion \n",currentStatement,currentStatement->class_name().c_str());
#endif
#if 0
                 // Mark as defining a macro or include file expansion.
                    currentStatement->set_isMacroOrIncludeExpansion(true);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
#if 1
                 // printf ("This currentStatement = %p = %s is allowed to have a child without a token sequence mapping \n",currentStatement,currentStatement->class_name().c_str());
                    printf ("This currentStatement = %s is allowed to have a child without a token sequence mapping \n",currentStatement->class_name().c_str());
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
detectMacroOrIncludeFileExpansions ( SgSourceFile* sourceFile )
   {

  // sourceFile->set_tokenSubsequenceMap(tokenMappingTraversal.tokenStreamSequenceMap);
  // tokenMappingTraversal = sourceFile->get_tokenSubsequenceMap();
     map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

  // DQ (11/8/2015): Note that this call to the constructor fails to compile (when called with "()") (GNU g++ version 8.4.3 compiler).
  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute();
     DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute;

     DetectMacroOrIncludeFileExpansions traversal(tokenStreamSequenceMap);

  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
  // DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // traversal.traverse(sourceFile,inheritedAttribute);
     DetectMacroOrIncludeFileExpansionsSynthesizedAttribute topAttribute = traversal.traverseWithinFile(sourceFile,inheritedAttribute);

     ROSE_ASSERT(topAttribute.node != NULL);

#if 0
     printf ("Completed detection of macro expansions requiring unparsing from the AST (instead of the token stream if they are not transformed) \n");
     ROSE_ASSERT(false);
#endif
   }
