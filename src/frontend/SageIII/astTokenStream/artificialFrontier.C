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

ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute()
   {
     sourceFile        = NULL;

#if 0
     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
#endif

#if 1
  // DQ (11/13/2018): I want to use the other constructor that will always at least set the SgSourceFile pointer.
     printf ("Exitng as a test! \n");
     ROSE_ABORT();
#endif

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }


ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute(SgSourceFile* input_sourceFile)
   {
     sourceFile        = input_sourceFile;

#if 0
     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
#endif

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }


ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
   {
     sourceFile        = input_sourceFile;

#if 0
     processChildNodes = processed;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
#endif

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }


ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute ( const ArtificialFrontier_InheritedAttribute & X )
   {
     sourceFile        = X.sourceFile;

#if 0
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;
#endif

  // isPartOfTypedefDeclaration   = X.isPartOfTypedefDeclaration;
  // isPartOfConditionalStatement = X.isPartOfConditionalStatement;
   }


ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute()
   {
     node       = NULL;

#if 0
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
#endif
   }

ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute(SgNode* n)
   {
     node       = isSgStatement(n);

#if 0
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
#endif
   }

ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute(const ArtificialFrontier_SynthesizedAttribute & X)
   {
     node       = X.node;

#if 0
     isFrontier = X.isFrontier;

  // frontierNodes    = X.frontierNodes;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
#endif
   }


ArtificialFrontierTraversal::
ArtificialFrontierTraversal( SgSourceFile* sourceFile)
   {
  // This is the number of IR nodes in the current file and helpful in randomly 
  // marking IR nodes for testing to be either from the AST or from the token stream.
     numberOfNodes = numberOfNodesInSubtree(sourceFile);
   }


ArtificialFrontier_InheritedAttribute
ArtificialFrontierTraversal::evaluateInheritedAttribute(SgNode* n, ArtificialFrontier_InheritedAttribute inheritedAttribute)
   {

     static int random_counter = 0;

#define DEBUG_INHERIT 0

     int lowerbound = -1;
     int upperbound = -1;

  // Unparse the middle of the AST from the AST and the outer 25% from the token stream.
     bool forceUnparseFromTokenStream = false;
     bool forceUnparseFromAST         = false;

  // DQ (11/13/2018): We need to make sure that the SgSourceFile pointer is set.
  // ArtificialFrontier_InheritedAttribute returnAttribute;
     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);
     ArtificialFrontier_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile);

  // DQ (11/13/2018): Enforce that this is set because will need it for the unparing of header files with the token unparsing.
     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);

     SgStatement* statement = isSgStatement(n);

#if DEBUG_INHERIT
  // static int random_counter = 0;
  // printf ("*** In ArtificialFrontierTraversal::evaluateInheritedAttribute(): random_counter = %d n = %p = %s \n",random_counter,n,n->class_name().c_str());
     if (statement != NULL)
        {
          Sg_File_Info* fileInfo = statement->get_file_info();
          ROSE_ASSERT(fileInfo != NULL);
          printf ("\n\nIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("*** In ArtificialFrontierTraversal::evaluateInheritedAttribute(): n = %p = %s filename = %s \n",n,n->class_name().c_str(),fileInfo->get_filenameString().c_str());
          printf (" --- file_id = %d physical_file_id = %d \n",fileInfo->get_file_id(),fileInfo->get_physical_file_id());
          printf (" --- lowerbound     = %d upperbound = %d \n",lowerbound,upperbound);
          printf (" --- random_counter = %d numberOfNodes = %d \n",random_counter,numberOfNodes);
        }
#endif

  // DQ (5/20/2021): Also skip the SgSourceFile and the SgGlobalScope.
  // Ignore IR nodes that are front-end specific (declarations of builtin functions, etc.).
  // if (n->get_file_info()->isFrontendSpecific() == false)
  // if (n->get_file_info()->isFrontendSpecific() == false && statement != NULL)
     if (n->get_file_info()->isFrontendSpecific() == false && statement != NULL && isSgSourceFile(n) == NULL && isSgGlobal(n) == NULL)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }

     if (isSgGlobal(n) != NULL)
        {
          SgGlobal* globalScope = isSgGlobal(n);
#if 0
          printf ("In ArtificialFrontierTraversal::evaluateInheritedAttribute(): globalScope = %p globalScope->get_parent() = %p \n",globalScope,globalScope->get_parent());
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
       // if (locatedNode->get_isModified() == true)
       //    {
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

       // DQ (5/3/2021): In order to be marked as a transformation, the node must be a statement.
       // The evaluateSynthesizedAttribute() function will set the parent scopes as containing a transformation.

          if (statement != NULL)
             {
#if 0
               printf ("Marking statement = %p = %s to be a transformation and output in code generation \n",statement,statement->class_name().c_str());
#endif
#if 0
               printf ("BEFORE: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): statement->get_file_info()->getFileName() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
            // if (ROSE_tokenUnparsingTestingMode == true)
            //    {
#if DEBUG_INHERIT && 0
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("ROSE_tokenUnparsingTestingMode = %s \n",ROSE_tokenUnparsingTestingMode ? "true" : "false");
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
#if DEBUG_INHERIT && 0
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): forceUnparseFromTokenStream = %s \n",forceUnparseFromTokenStream ? "true" : "false");
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): forceUnparseFromAST         = %s \n",forceUnparseFromAST ? "true" : "false");
#endif
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
#if DEBUG_INHERIT || 0
                         printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): max_numberOfNodes = %d \n",max_numberOfNodes);
#endif
                       }
                  }
                 else
                  {
                 // DQ (12/1/2013): This is the testing mode for all regression tests.
                    ROSE_ASSERT(forceUnparseFromAST == false);
                    ROSE_ASSERT(forceUnparseFromTokenStream == false);
#if DEBUG_INHERIT || 0
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): numberOfNodes = %d \n",numberOfNodes);
#endif
                    lowerbound = numberOfNodes / 4;
                    upperbound = 3 * (numberOfNodes / 4);

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }

#if DEBUG_INHERIT || 0
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d lowerbound = %d upperbound = %d \n",random_counter,lowerbound,upperbound);
#endif
            // bool skipMarkingAsToBeUnparsedFromAST  = false;
            // if ( (random_counter > lowerbound && random_counter < upperbound) || forceFromTokenStream == true)
               if ( random_counter >= lowerbound && random_counter <= upperbound )
                  {
#if DEBUG_INHERIT || 0
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",
                    random_counter,statement,statement->class_name().c_str());
#endif
                 // options = "color=\"red\"";
                 // options = "penwidth=5, color=\"red\"";
#if 0
                 // DQ (5/31/2021): set this as a transformation.
                    statement->set_containsTransformation(true);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
#if 0
                    if (returnAttribute.unparseFromTheAST == false)
                       {
                      // Mark this as the frontier only if this is the first time (while decending into the AST) 
                      // that we will unparse this subtree from the AST.
                         returnAttribute.isFrontier = true;
                       }

                    returnAttribute.unparseFromTheAST       = true;
                    returnAttribute.unparseUsingTokenStream = false;
#endif
                  }
                 else
                  {
#if 0
                    returnAttribute.unparseUsingTokenStream = true;
#endif
                  }

#if DEBUG_INHERIT && 0
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): returnAttribute.unparseFromTheAST       = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): returnAttribute.unparseUsingTokenStream = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
#endif
          // }
          //else
          // {
          // }

#if 0
            // Note that both of these must be set.
               statement->setTransformation();
               statement->setOutputInCodeGeneration();

            // DQ (5/3/2021): There may be some additional details of setting the physical_file_id as well to make 
            // sure that it is output into the correct file when header file unparsing is being supported.
#endif
#if 0
               printf ("AFTER: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): statement->get_file_info()->getFileName() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
#if 0
               string physicalFile = statement->get_startOfConstruct()->get_physical_filename();
               printf ("AFTER: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): physicalFile = %s \n",physicalFile.c_str());
               string raw_filename = statement->get_startOfConstruct()->get_raw_filename();
               printf ("AFTER: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): raw_filename = %s \n",raw_filename.c_str());
               statement->get_startOfConstruct()->display("startOfConstruct: AFTER: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): debug");
               statement->get_endOfConstruct()  ->display("endOfConstruct: AFTER: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): debug");
               printf ("statement->get_startOfConstruct()->get_raw_filename() = %s \n",statement->get_startOfConstruct()->get_raw_filename().c_str());
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ABORT();
#endif
             }
            else
             {
#if 0
               printf ("WARNING: In ArtificialFrontierTraversal::evaluateInheritedAttribute(): parent statement == NULL for locatedNode = %p = %s \n",
                    locatedNode,locatedNode->class_name().c_str());
#endif
             }
          // }
        }

     return returnAttribute;
   }


ArtificialFrontier_SynthesizedAttribute 
ArtificialFrontierTraversal::evaluateSynthesizedAttribute (SgNode* n, 
     ArtificialFrontier_InheritedAttribute inheritedAttribute, 
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

#define DEBUG_SYNTH 0

#if DEBUG_SYNTH
     printf ("\n\nSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### In ArtificialFrontierTraversal::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     ArtificialFrontier_SynthesizedAttribute returnAttribute(n);

#if 0
     printf ("   --- synthesizedAttributeList.size() = %zu \n",synthesizedAttributeList.size());
#endif

  // DQ (5/31/2021): set this as a transformation.
     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {
#if 0
          if (synthesizedAttributeList.size() == 0)
             {
               if (statement->get_containsTransformation() == true)
                  {
#if 0
                    statement->set_containsTransformation(false);
#endif
#if 0
                    statement->setTransformation();
#endif
                    printf ("##### Setting statement as being a transformation! \n");
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }
                 else
                  {
#if 1
                    bool processStatement = isSgFunctionParameterList(statement) == NULL;
                    if (processStatement == true)
                       {
                         statement->setTransformation();
                       }
#endif
                  }
             }
            else
             {
               printf ("This IR node has non-zero statements \n");
            // if (synthesizedAttributeList.size() == 1)
                  {
                    bool processExprStatement = isSgExprStatement(statement) != NULL;
                    SgStatement* exprStatementParent = isSgStatement(statement->get_parent());
                    if (isSgIfStmt(exprStatementParent) != NULL)
                       {
                         processExprStatement = false;
                       }
                    if (processExprStatement == true)
                       {
                         statement->setTransformation();
                       }
                  }
             }
#else
       // DQ (5/31/2021): I think this is a reasonable test.
#if 0
          bool processExprStatement = (isSgExprStatement(statement) != NULL) || (isSgVariableDeclaration(statement) != NULL);
          SgStatement* exprStatementParent = isSgStatement(statement->get_parent());
          if (isSgIfStmt(exprStatementParent) != NULL)
             {
               processExprStatement = false;
             }
#endif
       // if (processExprStatement == true)
          SgBasicBlock* basicBlockParent = isSgBasicBlock(statement->get_parent());
          SgExprStatement* exprStatement = isSgExprStatement(statement);
          if (exprStatement != NULL && basicBlockParent != NULL)
             {
               statement->setTransformation();
             }
#endif
        }
       else
        {
#if 0
          printf ("This is not a statement! \n");
#endif
        }

     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
       // ROSE_ASSERT(synthesizedAttributeList[i].node != NULL);

       // DQ (4/14/2015): I think this is always true, implying that this function does nothing.
       // ROSE_ASSERT(synthesizedAttributeList[i].node == NULL);

#if DEBUG_SYNTH && 0
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
#if DEBUG_SYNTH
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
#if DEBUG_SYNTH
                    printf ("   --- inheritedAttribute.sourceFile->get_unparseHeaderFiles() = %s \n",inheritedAttribute.sourceFile->get_unparseHeaderFiles() ? "true" : "false");
#endif
                    if (inheritedAttribute.sourceFile->get_unparseHeaderFiles() == true)
                       {
                      // int sourceFile_file_id = inheritedAttribute.sourceFile->get_file_info()->get_file_id();
                      // int child_file_id      = statement->get_file_info()->get_file_id();

                      // DQ (5/25/2021): We want the physicial file id from the current statement, not the original input file.
                      // Else we only correctly compute the containsTransformation flag for the first level of header files in the AST.
                      // int sourceFile_file_id = inheritedAttribute.sourceFile->get_file_info()->get_physical_file_id();
                         int currentStatement_file_id = currentStatement->get_file_info()->get_physical_file_id();
                         int child_file_id            = statement->get_file_info()->get_physical_file_id();
#if DEBUG_SYNTH
                      // printf ("   --- sourceFile_file_id = %d child_file_id = %d \n",sourceFile_file_id,child_file_id);
                         printf ("   --- currentStatement_file_id = %d child_file_id = %d \n",currentStatement_file_id,child_file_id);
#endif
                      // DQ (5/3/2021): Since this is source file and header file dependent we need to do more than just 
                      // set the value in the IR nodes that are shared across multiple files.  We require a file-based 
                      // (indexed) map of lists of scopes that are to be marked as containing transformations.
                      // Something like: std::map< SgSourceFile* , std::set<SgScopeStatement*> > to make it simple for 
                      // ROSETTA, we need to store this outside of the map.

                      // if (sourceFile_file_id == child_file_id)
                         if (currentStatement_file_id == child_file_id)
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

#if DEBUG_SYNTH && 0
     SgStatement* currentStatement = isSgStatement(n);
     if (currentStatement != NULL)
        {
          printf ("Leaving evaluateSynthesizedAttribute(): currentStatement = %p = %s \n",n,n->class_name().c_str());
          printf (" --- currentStatement->isTransformation()           = %s \n",currentStatement->isTransformation() ? "true" : "false");
          printf (" --- currentStatement->get_containsTransformation() = %s \n",currentStatement->get_containsTransformation() ? "true" : "false");
          printf (" --- returnAttribute.node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
               returnAttribute.node,
               returnAttribute.node != NULL                                ? returnAttribute.node->class_name().c_str() : "null",
               returnAttribute.isFrontier                                  ? "true" : "false",
               returnAttribute.unparseUsingTokenStream                     ? "true" : "false",
               returnAttribute.unparseFromTheAST                           ? "true" : "false",
               returnAttribute.containsNodesToBeUnparsedFromTheAST         ? "true" : "false",
               returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
        }
#endif

     return returnAttribute;
   }



int
ArtificialFrontierTraversal::numberOfNodesInSubtree(SgSourceFile* sourceFile)
   {
     int value = 0;

  // DQ (02/20/2021): Using the performance tracking within ROSE.
     TimingPerformance timer ("AST ArtificialFrontierTraversal::numberOfNodesInSubtree():");

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


#if 0
void
buildArtificialFrontier ( SgSourceFile* sourceFile )
#else
// DQ (5/9/2021): Activate this code.
void
buildArtificialFrontier ( SgSourceFile* sourceFile, bool traverseHeaderFiles )
#endif
   {
  // DQ (11/8/2015): This function sets the nodes as containing transforamtions (which is essential).
  // DQ (4/14/2015): After an more detailed evaluation of this function it does not acomplish it's objectives.

  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).

  // DQ (02/20/2021): Using the performance tracking within ROSE.
     TimingPerformance timer ("AST Build Artificial Frontier For Testing Token Stream Mapping:");

  // DQ (11/13/2018): We need to make sure that the SgSourceFile pointer is set.
  // ArtificialFrontier_InheritedAttribute inheritedAttribute;
     ROSE_ASSERT(sourceFile != NULL);
     ArtificialFrontier_InheritedAttribute inheritedAttribute (sourceFile);
     ArtificialFrontierTraversal fdTraversal(sourceFile);

#if 1
     printf ("AFAFAFAFAFAFAFAFAFAFAFAAFAFAFAFAFAAFAFAFAFAFAFAFAFAFAFAFAAFAFAFAF \n");
     printf ("AFAFAFAFAFAFAFAFAFAFAFAAFAFAFAFAFAAFAFAFAFAFAFAFAFAFAFAFAAFAFAFAF \n");
     printf ("In buildArtificialFrontier(): calling traverse() sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
     printf ("   --- traverseHeaderFiles                          = %s \n",traverseHeaderFiles ? "true" : "false");
     printf ("   --- sourceFile->get_globalScope()                = %p \n",sourceFile->get_globalScope());
     printf ("   --- sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
     printf ("AFAFAFAFAFAFAFAFAFAFAFAAFAFAFAFAFAAFAFAFAFAFAFAFAFAFAFAFAAFAFAFAF \n");
     printf ("AFAFAFAFAFAFAFAFAFAFAFAAFAFAFAFAFAAFAFAFAFAFAFAFAFAFAFAFAAFAFAFAF \n");
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
     ROSE_ABORT();
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_1 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In buildArtificialFrontier(): part 1: transformedStatementSet_1.size() = %zu \n",transformedStatementSet_1.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_1 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In buildArtificialFrontier(): part 1: modifiedLocatedNodesSet_1.size() = %zu \n",modifiedLocatedNodesSet_1.size());
#endif

  // fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseInputFiles(sourceFile,inheritedAttribute);

#if 0
  // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
  // Note return value not used.
  // ArtificialFrontier_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
     fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
#else
  // DQ (5/9/2021): Activate this code.
  // DQ (5/2/2021): Where we need to unparse the header files the traversal needs to be over the whole AST.
  // fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
     if (traverseHeaderFiles == true)
        {
          fdTraversal.traverse(sourceFile,inheritedAttribute);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
          fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_2 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In buildArtificialFrontier(): part 2: transformedStatementSet_2.size() = %zu \n",transformedStatementSet_2.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_2 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In buildArtificialFrontier(): part 2: modifiedLocatedNodesSet_2.size() = %zu \n",modifiedLocatedNodesSet_2.size());
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

     generateDOTforMultipleFile(*project,"in_buildArtificialFrontier");
#endif
#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif

#if 0
     printf ("Leaving buildArtificialFrontier(): sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif
   }
