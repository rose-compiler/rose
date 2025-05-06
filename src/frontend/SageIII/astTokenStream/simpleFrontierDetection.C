#include "sage3basic.h"
#include "tokenStreamMapping.h"
#include "previousAndNextNode.h"

using namespace std;

SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
     sourceFile        = nullptr;
     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

  // DQ (11/13/2018): I want to use the other constructor that will always at least set the SgSourceFile pointer.
     printf ("Exitng as a test! \n");
     ROSE_ABORT();
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
   }


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int /*start*/, int /*end*/, bool processed)
   {
     sourceFile        = input_sourceFile;
     processChildNodes = processed;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
   }


SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(const SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute &X)
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;
   }


SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = nullptr;
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

#define DEBUG_INHERIT 0

     ASSERT_not_null(inheritedAttribute.sourceFile);
     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile);
     ASSERT_not_null(inheritedAttribute.sourceFile);

#if DEBUG_INHERIT
     SgStatement* statement = isSgStatement(n);
     if (statement != nullptr)
        {
          Sg_File_Info* fileInfo = statement->get_file_info();
          ASSERT_not_null(fileInfo);
          printf ("\n\nIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII \n");
          printf ("*** In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s filename = %s \n",n,n->class_name().c_str(),fileInfo->get_filenameString().c_str());
          printf (" --- file_id = %d physical_file_id = %d \n",fileInfo->get_file_id(),fileInfo->get_physical_file_id());
        }
#endif

     if (isSgGlobal(n) != nullptr)
        {
          SgGlobal* globalScope = isSgGlobal(n);
          ASSERT_not_null(globalScope->get_parent());
        }

     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != nullptr)
        {
          if (locatedNode->isTransformation() == true)
             {
#if DEBUG_INHERIT
               printf ("Found locatedNode = %p = %s as transformation \n",locatedNode,locatedNode->class_name().c_str());
#endif
             }

       // DQ (6/2/2021): An IR node that is a container should not be marked as is modified, but this should likely be fied elsewhere.
          SgClassDefinition* classDefinition = isSgClassDefinition(n);
          if (classDefinition != nullptr)
             {
               if (classDefinition->get_isModified() == true)
                  {
                    classDefinition->set_isModified(false);
                  }
             }

       // DQ (4/14/2015): We need to detect modified IR nodes and then set there coresponding parent statement as being transformed.
          if (locatedNode->get_isModified() == true)
             {
#if DEBUG_INHERIT
               printf ("Found locatedNode = %p = %s as get_isModified = %s \n",locatedNode,locatedNode->class_name().c_str(),locatedNode->get_isModified() ? "true" : "false");
#endif

            // DQ (4/14/2015): We need to detect modified IR nodes and then set there coresponding parent statement as being transformed.
               SgStatement* statement = TransformationSupport::getStatement(locatedNode);

            // DQ (4/16/2015): I want to verify that we have not returned a statement at
            // a higher position in the AST than the locatedNode if it was a SgStatement.
               if (isSgStatement(locatedNode) != nullptr)
                  {
                    ROSE_ASSERT(statement == locatedNode);
                  }
               ROSE_ASSERT(isSgStatement(locatedNode) == nullptr || statement == locatedNode);

            // DQ (5/3/2021): In order to be marked as a transformation, the node must be a statement.
            // The evaluateSynthesizedAttribute() function will set the parent scopes as containing a transformation.

               if (statement != nullptr)
                  {
#if DEBUG_INHERIT
                    printf ("Marking statement = %p = %s to be a transformation and output in code generation \n",statement,statement->class_name().c_str());
#endif
#if DEBUG_INHERIT
                    printf ("BEFORE: In SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement->get_file_info()->getFileName() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
                 // Note that both of these must be set.
                    statement->setTransformation();
                    statement->setOutputInCodeGeneration();
                  }
             }
        }

#if DEBUG_INHERIT
     printf ("Leaving SimpleFrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     printf (" --- returnAttribute.sourceFile                          = %p \n",returnAttribute.sourceFile);
     printf (" --- returnAttribute.processChildNodes                   = %s \n",returnAttribute.processChildNodes ? "true" : "false");
     printf (" --- returnAttribute.isFrontier                          = %s \n",returnAttribute.isFrontier ? "true" : "false");
     printf (" --- returnAttribute.unparseUsingTokenStream             = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
     printf (" --- returnAttribute.unparseFromTheAST                   = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
     printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif

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

     ASSERT_not_null(n);

#define DEBUG_SYNTH 0

#if DEBUG_SYNTH
     printf ("\n\nSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### In SimpleFrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n);

     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
#if DEBUG_SYNTH
          printf ("   --- synthesizedAttributeList[i=%zu].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
               i,synthesizedAttributeList[i].node,
               synthesizedAttributeList[i].node != nullptr                             ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
               synthesizedAttributeList[i].isFrontier                                  ? "true" : "false",
               synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true" : "false",
               synthesizedAttributeList[i].unparseFromTheAST                           ? "true" : "false",
               synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true" : "false",
               synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
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
               if (currentStatement != nullptr && currentStatement->isTransformation() == false && (statement->isTransformation() == true || statement->get_containsTransformation() == true) )
                  {

                 // DQ (11/13/2018): When header file unparsing is used, then we need to check if this currentStatement that might be a transformation 
                 // (or the child attribute that is a statement might be or contain a transformation) is from the same file as the current scope. In 
                 // this case the #include statement would be unparsed and we would not set the current statement as containing a transformation.
                 // n->set_containsTransformation(true);

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

       // DQ (4/14/2015): Adding support for modified SgInitializedName in a statement to trigger containsTransformation flag.
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

#if DEBUG_SYNTH
     SgStatement* currentStatement = isSgStatement(n);
     if (currentStatement != nullptr)
        {
          printf ("Leaving evaluateSynthesizedAttribute(): currentStatement = %p = %s \n",n,n->class_name().c_str());
          printf (" --- currentStatement->isTransformation()           = %s \n",currentStatement->isTransformation() ? "true" : "false");
          printf (" --- currentStatement->get_containsTransformation() = %s \n",currentStatement->get_containsTransformation() ? "true" : "false");
          printf (" --- returnAttribute.node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
               returnAttribute.node,
               returnAttribute.node != nullptr                             ? returnAttribute.node->class_name().c_str() : "null",
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
SimpleFrontierDetectionForTokenStreamMapping::numberOfNodesInSubtree(SgSourceFile* sourceFile)
   {
     int value = 0;
     TimingPerformance timer ("AST SimpleFrontierDetectionForTokenStreamMapping::numberOfNodesInSubtree():");

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}

           // We only want to count statements since we only test the token/AST unparsing at the statement level.
              void visit ( SgNode* n ) { if (isSgStatement(n) != nullptr) count++; }
        };

     CountTraversal counter;
     counter.traverseWithinFile(sourceFile,preorder);
     value = counter.count;

     return value;
   }

void
simpleFrontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile, bool traverseHeaderFiles )
   {
  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).
     ASSERT_not_null(sourceFile);
     TimingPerformance timer ("AST Simple Frontier Detection For Token Stream Mapping:");

     SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute (sourceFile);
     SimpleFrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

     if (traverseHeaderFiles == true)
        {
          fdTraversal.traverse(sourceFile,inheritedAttribute);
        }
       else
        {
          fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
        }
   }
