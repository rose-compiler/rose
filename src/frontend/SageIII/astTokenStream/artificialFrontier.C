#include "sage3basic.h"
#include "tokenStreamMapping.h"
#include "previousAndNextNode.h"

using namespace std;

ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute()
   {
     sourceFile = nullptr;

  // DQ (11/13/2018): I want to use the other constructor that will always at least set the SgSourceFile pointer.
     printf ("Exitng as a test! \n");
     ROSE_ABORT();
   }

ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute(SgSourceFile* input_sourceFile)
   {
     sourceFile = input_sourceFile;
   }

ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute(SgSourceFile* input_sourceFile, int /*start*/, int /*end*/, bool /*processed*/)
   {
     sourceFile = input_sourceFile;
   }

ArtificialFrontier_InheritedAttribute::
ArtificialFrontier_InheritedAttribute(const ArtificialFrontier_InheritedAttribute &X)
   {
     sourceFile = X.sourceFile;
   }

ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute()
   {
     node = nullptr;
   }

ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute(SgNode* n)
   {
     node = isSgStatement(n);
   }

ArtificialFrontier_SynthesizedAttribute::
ArtificialFrontier_SynthesizedAttribute(const ArtificialFrontier_SynthesizedAttribute &X)
   {
     node = X.node;
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

     ASSERT_not_null(inheritedAttribute.sourceFile);
     ArtificialFrontier_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile);
     ASSERT_not_null(inheritedAttribute.sourceFile);

     SgStatement* statement = isSgStatement(n);

#if DEBUG_INHERIT || 0
     if (statement != nullptr)
        {
          Sg_File_Info* fileInfo = statement->get_file_info();
          ASSERT_not_null(fileInfo);
          printf ("\n\nIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("*** In ArtificialFrontierTraversal::evaluateInheritedAttribute(): n = %p = %s filename = %s \n",n,n->class_name().c_str(),fileInfo->get_filenameString().c_str());
          printf (" --- file_id = %d physical_file_id = %d \n",fileInfo->get_file_id(),fileInfo->get_physical_file_id());
          printf (" --- lowerbound     = %d upperbound = %d \n",lowerbound,upperbound);
          printf (" --- random_counter = %d numberOfNodes = %d \n",random_counter,numberOfNodes);
        }
#endif

     if (n->get_file_info()->isFrontendSpecific() == false && statement != nullptr && isSgSourceFile(n) == nullptr && isSgGlobal(n) == nullptr)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }

     if (isSgGlobal(n) != nullptr)
        {
          SgGlobal* globalScope = isSgGlobal(n);
          ASSERT_not_null(globalScope->get_parent());
        }

     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != nullptr)
        {
       // DQ (4/14/2015): We need to detect modified IR nodes and then set there coresponding parent statement as being transformed.
          SgStatement* statement = TransformationSupport::getStatement(locatedNode);

       // DQ (4/16/2015): I want to verify that we have not returned a statement at
       // a higher position in the AST than the locatedNode if it was a SgStatement.
          if (isSgStatement(locatedNode) != nullptr)
             {
               ASSERT_require(statement == locatedNode);
             }
          ASSERT_require(isSgStatement(locatedNode) == nullptr || statement == locatedNode);

       // DQ (5/3/2021): In order to be marked as a transformation, the node must be a statement.
       // The evaluateSynthesizedAttribute() function will set the parent scopes as containing a transformation.
          if (statement != nullptr)
             {
               if (forceUnparseFromTokenStream == true || forceUnparseFromAST == true)
                  {
                 // We need to set this to a sufficently high number so that the conditional below will always be false.
                    static int max_numberOfNodes = 10000000;

                    if (forceUnparseFromAST == true)
                       {
                         ASSERT_require(forceUnparseFromTokenStream == false);
                         lowerbound = -1;
                         upperbound = max_numberOfNodes;
                       }
                      else
                       {
                         ASSERT_require(forceUnparseFromTokenStream == true);

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
                    ASSERT_require(forceUnparseFromAST == false);
                    ASSERT_require(forceUnparseFromTokenStream == false);
#if DEBUG_INHERIT || 0
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): numberOfNodes = %d \n",numberOfNodes);
#endif
                    lowerbound = numberOfNodes / 4;
                    upperbound = 3 * (numberOfNodes / 4);
                  }

#if DEBUG_INHERIT || 0
               printf ("In artificial FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d lowerbound = %d upperbound = %d \n",
                    random_counter,lowerbound,upperbound);
#endif
               if ( random_counter >= lowerbound && random_counter <= upperbound )
                  {
#if DEBUG_INHERIT || 0
                    printf ("In artificial FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",
                         random_counter,statement,statement->class_name().c_str());
#endif
#if DEBUG_INHERIT || 1
                    if (random_counter == lowerbound || random_counter == upperbound)
                       {
                         printf ("In artificial FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",
                              random_counter,statement,statement->class_name().c_str());
                       }
#endif
                  }
            // DQ (9/3/2021): test if we ever mark a statement to be transformed.
            // If we never mark a statement to be a transforamtion then why do we 
            // have any errors in the token-based unparsing tests?
               ASSERT_require(statement->get_containsTransformation() == false);
               ASSERT_require(statement->isTransformation()           == false);
               ASSERT_require(statement->isOutputInCodeGeneration()   == false);
             }
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

     ASSERT_not_null(n);

#define DEBUG_SYNTH 0

#if DEBUG_SYNTH
     printf ("\n\nSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### In ArtificialFrontierTraversal::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     ArtificialFrontier_SynthesizedAttribute returnAttribute(n);

  // DQ (5/31/2021): set this as a transformation.
     SgStatement* statement = isSgStatement(n);
     if (statement != nullptr)
        {
          SgBasicBlock* basicBlockParent = isSgBasicBlock(statement->get_parent());
          SgExprStatement* exprStatement = isSgExprStatement(statement);
          if (exprStatement != nullptr && basicBlockParent != nullptr)
             {
               statement->setTransformation();
             }
        }

     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
          SgStatement* statement = isSgStatement(synthesizedAttributeList[i].node);
          if (statement != nullptr)
             {
               SgStatement* currentStatement = isSgStatement(n);
#if DEBUG_SYNTH
               if (currentStatement != nullptr)
                  {
                    printf ("   --- currentStatement->isTransformation()    = %s \n",currentStatement->isTransformation() ? "true" : "false");
                  }
               printf ("   --- statement->isTransformation()           = %s \n",statement->isTransformation() ? "true" : "false");
               printf ("   --- statement->get_containsTransformation() = %s \n",statement->get_containsTransformation() ? "true" : "false");
#endif

               if ( currentStatement != nullptr && currentStatement->isTransformation() == false && (statement->isTransformation() == true || statement->get_containsTransformation() == true) )
                  {
                    ASSERT_not_null(inheritedAttribute.sourceFile);
                    ASSERT_not_null(inheritedAttribute.sourceFile->get_file_info());
#if DEBUG_SYNTH
                    printf ("   --- inheritedAttribute.sourceFile->get_unparseHeaderFiles() = %s \n",inheritedAttribute.sourceFile->get_unparseHeaderFiles() ? "true" : "false");
#endif
                    if (inheritedAttribute.sourceFile->get_unparseHeaderFiles() == true)
                       {
                         int currentStatement_file_id = currentStatement->get_file_info()->get_physical_file_id();
                         int child_file_id            = statement->get_file_info()->get_physical_file_id();
#if DEBUG_SYNTH
                         printf ("   --- currentStatement_file_id = %d child_file_id = %d \n",currentStatement_file_id,child_file_id);
#endif
                      // DQ (5/3/2021): Since this is source file and header file dependent we need to do more than just 
                      // set the value in the IR nodes that are shared across multiple files.  We require a file-based 
                      // (indexed) map of lists of scopes that are to be marked as containing transformations.
                      // Something like: std::map< SgSourceFile* , std::set<SgScopeStatement*> > to make it simple for 
                      // ROSETTA, we need to store this outside of the map.

                         if (currentStatement_file_id == child_file_id)
                            {
                              n->set_containsTransformation(true);
                            }
                       }
                      else
                       {
                         n->set_containsTransformation(true);
                       }
                  }
             }

          SgInitializedName* initializedName = isSgInitializedName(synthesizedAttributeList[i].node);
          if (initializedName != nullptr)
             {
               SgStatement* currentStatement = isSgStatement(n);
               if (currentStatement != nullptr && currentStatement->isTransformation() == false && (initializedName->isTransformation() == true || initializedName->get_containsTransformation() == true) )
                  {
                    n->set_containsTransformation(true);
                  }
             }
        }

     return returnAttribute;
   }



int
ArtificialFrontierTraversal::numberOfNodesInSubtree(SgSourceFile* sourceFile)
   {
     int value = 0;
     TimingPerformance timer ("AST ArtificialFrontierTraversal::numberOfNodesInSubtree():");

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}
              void visit ( SgNode* n ) { if (isSgStatement(n) != nullptr) count++; }
        };

     CountTraversal counter;
     counter.traverseWithinFile(sourceFile,preorder);
     value = counter.count;

     return value;
   }


void
buildArtificialFrontier ( SgSourceFile* sourceFile, bool traverseHeaderFiles )
   {
     TimingPerformance timer ("AST Build Artificial Frontier For Testing Token Stream Mapping:");

  // DQ (11/8/2015): This function sets the nodes as containing transforamtions (which is essential).
  // DQ (4/14/2015): After an more detailed evaluation of this function it does not acomplish it's objectives.

  // This frontier detection happens before we associate token subsequences to the AST (in a separate map).
     ASSERT_not_null(sourceFile);
     ArtificialFrontier_InheritedAttribute inheritedAttribute (sourceFile);
     ArtificialFrontierTraversal fdTraversal(sourceFile);

     if (traverseHeaderFiles == true)
        {
          fdTraversal.traverse(sourceFile,inheritedAttribute);
        }
       else
        {
          fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
        }
   }
