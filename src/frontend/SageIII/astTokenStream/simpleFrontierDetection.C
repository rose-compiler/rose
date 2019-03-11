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
// Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
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

#if 1
  // DQ (11/13/2018): I want to use the other constructor that will always at least set the SgSourceFile pointer.
     printf ("Exitng as a test! \n");
     ROSE_ASSERT(false);
#endif

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile)
   {
     sourceFile        = input_sourceFile;
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

  // DQ (11/13/2018): We need to make sure that the SgSourceFile pointer is set.
  // SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute;
     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);
     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile);

  // DQ (11/13/2018): Enforce that this is set because will need it for the unparing of header files with the token unparsing.
     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);

#if 0
  // static int random_counter = 0;
  // printf ("*** In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d n = %p = %s \n",random_counter,n,n->class_name().c_str());
     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {
          Sg_File_Info* fileInfo = statement->get_file_info();
          ROSE_ASSERT(fileInfo != NULL);
          printf ("*** In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s filename = %s \n",n,n->class_name().c_str(),fileInfo->get_filenameString().c_str());
        }
#endif

     if (isSgGlobal(n) != NULL)
        {
          SgGlobal* globalScope = isSgGlobal(n);
#if 0
          printf ("In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): globalScope = %p globalScope->get_parent() = %p \n",globalScope,globalScope->get_parent());
#endif

       // DQ (8/13/2018): Bot of these should be true.
          ROSE_ASSERT(globalScope->get_parent() != NULL);
       // ROSE_ASSERT(globalScope->get_parent() == sourceFile);
        }

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

       // DQ (4/14/2015): We need to detect modified IR nodes and then set there coresponding parent statement as being transformed.
          if (locatedNode->get_isModified() == true)
             {
#if 0
               printf ("Found locatedNode = %p = %s as get_isModified = %s \n",locatedNode,locatedNode->class_name().c_str(),locatedNode->get_isModified() ? "true" : "false");
            // ROSE_ASSERT(false);
#endif

            // DQ (4/14/2015): We need to detect modified IR nodes and then set there coresponding parent statement as being transformed.
               SgStatement* statement = TransformationSupport::getStatement(locatedNode);

            // DQ (4/16/2015): I want to verify that we have not returned a statement at
            // a higher position in the AST than the locatedNode if it was a SgStatement.
               if (isSgStatement(locatedNode) != NULL)
                  {
                    ROSE_ASSERT(statement == locatedNode);
                  }
               ROSE_ASSERT(isSgStatement(locatedNode) == NULL || statement == locatedNode);

               if (statement != NULL)
                  {
#if 0
                    printf ("Marking statement = %p = %s to be a transformation and output in code generation \n",statement,statement->class_name().c_str());
#endif
#if 0
                    printf ("BEFORE: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement->get_file_info()->getFileName() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
#if 1
                 // Note that both of these must be set.
                    statement->setTransformation();
                    statement->setOutputInCodeGeneration();
#endif
#if 0
                    printf ("AFTER: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement->get_file_info()->getFileName() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
#if 0
                    string physicalFile = statement->get_startOfConstruct()->get_physical_filename();
                    printf ("AFTER: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): physicalFile = %s \n",physicalFile.c_str());
                    string raw_filename = statement->get_startOfConstruct()->get_raw_filename();
                    printf ("AFTER: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): raw_filename = %s \n",raw_filename.c_str());
                    statement->get_startOfConstruct()->display("startOfConstruct: AFTER: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): debug");
                    statement->get_endOfConstruct()  ->display("endOfConstruct: AFTER: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): debug");
                    printf ("statement->get_startOfConstruct()->get_raw_filename() = %s \n",statement->get_startOfConstruct()->get_raw_filename().c_str());
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
#if 0
                    printf ("WARNING: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): parent statement == NULL for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
                  }
             }
        }

     return returnAttribute;
   }





SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
SimpleFrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // DQ (4/14/2015): This function does not appear to do anything, because the pointers to the attributes 
  // in the synthesizedAttributeList are always NULL.  


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

       // DQ (4/14/2015): I think this is always true, implying that this function does nothing.
       // ROSE_ASSERT(synthesizedAttributeList[i].node == NULL);

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

                 // DQ (11/13/2018): When header file unparsing is used, then we need to check if this currentStatement that might be a transformation 
                 // (or the child attribute that is a statement might be or contain a transformation) is from the same file as the current scope. In 
                 // this case the #include statement would be unparsed and we would not set the current statement as containing a transformation.
                 // n->set_containsTransformation(true);

                    ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);
                    ROSE_ASSERT(inheritedAttribute.sourceFile->get_file_info() != NULL);

                    if (inheritedAttribute.sourceFile->get_unparseHeaderFiles() == true)
                       {
                      // int sourceFile_file_id = inheritedAttribute.sourceFile->get_file_info()->get_file_id();
                      // int child_file_id      = statement->get_file_info()->get_file_id();
                         int sourceFile_file_id = inheritedAttribute.sourceFile->get_file_info()->get_physical_file_id();
                         int child_file_id      = statement->get_file_info()->get_physical_file_id();
#if 0
                         printf ("   --- sourceFile_file_id = %d child_file_id = %d \n",sourceFile_file_id,child_file_id);
#endif
                         if (sourceFile_file_id == child_file_id)
                            {
                              n->set_containsTransformation(true);
                            }
                       }
                      else
                       {
                         n->set_containsTransformation(true);
                       }
#if 0
                 // DQ (12/31/2014): This will not work since we would not have an associated token stream.
                    SgBasicBlock* basicBlock = isSgBasicBlock(n);
                    if (basicBlock != NULL && basicBlock->isCompilerGenerated() == true)
                       {
                      // Mark this as a transformation so that we can output it.
                       }
#endif
                  }
             }

       // DQ (4/14/2015): Adding support for modified SgInitializedName in a statement to trigger containsTransformation flag.
          SgInitializedName* initializedName = isSgInitializedName(synthesizedAttributeList[i].node);
          if (initializedName != NULL)
             {
               SgStatement* currentStatement = isSgStatement(n);
#if 0
               if (currentStatement != NULL)
                  {
                    printf ("   --- currentStatement->isTransformation()    = %s \n",currentStatement->isTransformation() ? "true" : "false");
                  }
               printf ("   --- initializedName->isTransformation()           = %s \n",initializedName->isTransformation() ? "true" : "false");
               printf ("   --- initializedName->get_containsTransformation() = %s \n",initializedName->get_containsTransformation() ? "true" : "false");
#endif
               if ( currentStatement != NULL && currentStatement->isTransformation() == false && (initializedName->isTransformation() == true || initializedName->get_containsTransformation() == true) )
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
  // DQ (11/8/2015): This function sets the nodes as containing transforamtions (which is essential).
  // DQ (4/14/2015): After an more detailed evaluation of this function it does not acomplish it's objectives.

  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).

  // DQ (11/13/2018): We need to make sure that the SgSourceFile pointer is set.
  // SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     ROSE_ASSERT(sourceFile != NULL);
     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute (sourceFile);
     SimpleFrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 0
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
     printf ("   --- sourceFile->get_globalScope()                = %p \n",sourceFile->get_globalScope());
     printf ("   --- sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif
#if 0
  // printf ("   --- global scope NOT present in tokenSubsequenceMap \n");
  // ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) == sourceFile->get_tokenSubsequenceMap().end());

     printf ("   --- global scope IS present in tokenSubsequenceMap \n");
     ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) != sourceFile->get_tokenSubsequenceMap().end());

  // ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap()[sourceFile->get_globalScope()] != NULL);
  // TokenStreamSequenceToNodeMapping* tmp_tokenSequence = sourceFile->get_tokenSubsequenceMap()[sourceFile->get_globalScope()];
  // ROSE_ASSERT(tmp_tokenSequence != NULL);
  // tmp_tokenSequence->display("token sequence for global scope");
#endif

  // DQ (12/2/2018): This can be empty for an empty file (see test in: roseTests/astTokenStreamTests).
  // ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) != sourceFile->get_tokenSubsequenceMap().end());

#if 0
  // Debugging (this set in the constructor).
     printf ("numberOfNodes = %d \n",fdTraversal.numberOfNodes);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_1 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): part 1: transformedStatementSet_1.size() = %zu \n",transformedStatementSet_1.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_1 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): part 1: modifiedLocatedNodesSet_1.size() = %zu \n",modifiedLocatedNodesSet_1.size());
#endif

  // fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseInputFiles(sourceFile,inheritedAttribute);

  // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
  // Note return value not used.
  // SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
     fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_2 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): part 2: transformedStatementSet_2.size() = %zu \n",transformedStatementSet_2.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_2 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In simpleFrontierDetectionForTokenStreamMapping(): part 2: modifiedLocatedNodesSet_2.size() = %zu \n",modifiedLocatedNodesSet_2.size());
#endif

#if 0
     printf ("   --- topAttribute.isFrontier                                  = %s \n",topAttribute.isFrontier ? "true" : "false");
     printf ("   --- topAttribute.unparseUsingTokenStream                     = %s \n",topAttribute.unparseUsingTokenStream ? "true" : "false");
     printf ("   --- topAttribute.unparseFromTheAST                           = %s \n",topAttribute.unparseFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",topAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",topAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
  // DQ (11/20/2013): Test using support for multiple files for Java testing.
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
  // SgProject* project = isSgProject(sourceFile->get_project());
     SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

     generateDOTforMultipleFile(*project,"in_simpleFrontierDetectionForTokenStreamMapping");
#endif
#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Leaving simpleFrontierDetectionForTokenStreamMapping(): sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif
   }
