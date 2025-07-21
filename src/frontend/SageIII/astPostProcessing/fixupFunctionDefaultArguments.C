#include "sage3basic.h"
#include "fixupFunctionDefaultArguments.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// DQ (7/20/2025): Added preformance tracing using Matt's tool (built on top of the default perfoermance analysis in ROSE).
#define USING_PERFORMANCE_TRACING 0

bool
containsLambdaSupportForFixupFunctionDefaultArguments (SgExpression* node)
   {
  // This function takes the initializer for any default initialization.
  // I need it becasue the SageInterface::deleteAST is not robust enough to support the rather complicated 
  // case of deleting a lambda expression and allof the associated generated classes and member functions.

#if USING_PERFORMANCE_TRACING || 0
     TimingPerformance timer ("Fixup function default arguments: containsLambdaSupportForFixupFunctionDefaultArguments");
#endif

     class LambdaTestTraversal : public AstSimpleProcessing
        {
          public:
               bool foundLambda;

               LambdaTestTraversal() : foundLambda(false) {}
               void visit (SgNode* node)
                  {
                    SgLambdaExp* lambda = isSgLambdaExp(node);
                    if (lambda != nullptr)
                       {
                         foundLambda = true;
                       }
                  }
        };

  // Now build the traveral object and call the traversal (preorder) on the AST subtree.
     LambdaTestTraversal traversal;
     traversal.traverse(node, preorder);

     return traversal.foundLambda;
   }


size_t
numberOfNodeInAST (SgExpression* node)
   {
  // DQ (7/20/2025): This function id for debuggin information, it counts the number
  // of nodes in the subtrees represented by the input expression.

#if USING_PERFORMANCE_TRACING
     TimingPerformance timer ("Fixup function default arguments: numberOfNodeInAST");
#endif

     class NodeCounter : public AstSimpleProcessing
        {
          public:
               size_t nodeCount;

               NodeCounter() : nodeCount(0) {}
          
               void visit (SgNode* node)
                  {
                    nodeCount++;
                  }
        };

  // Now build the traveral object and call the traversal (preorder) on the AST subtree.
     NodeCounter traversal;
     traversal.traverse(node, preorder);

     return traversal.nodeCount;
   }


void
fixupFunctionDefaultArguments( SgNode* node )
   {
  // This function determines the best function declaration where to associate default arguments.
     ASSERT_not_null(node);

     TimingPerformance timer ("Fixup function default arguments:");

     SgSourceFile* file = nullptr;

     static size_t totalNumberOfNodedDeleted = 0;
     
     if (node->get_parent() == nullptr)
        {
          SgProject *project = isSgProject(node);
          if (project != nullptr)
             {
#if USING_PERFORMANCE_TRACING
               TimingPerformance timer ("Fixup function default arguments: processing the SgProject");
#endif
            // GB (9/4/2009): Added this case for handling SgProject nodes. We do
            // this simply by iterating over the list of files in the project and
            // calling this function recursively. This is only one level of
            // recursion since files are not nested.
               SgFilePtrList &files = project->get_fileList();
               SgFilePtrList::iterator fIterator;
               for (fIterator = files.begin(); fIterator != files.end(); ++fIterator)
                  {
                    SgFile *file = *fIterator;
                    ASSERT_not_null(file);
                    fixupFunctionDefaultArguments(file);
                  }
             }
        }
       else
        {
       // When processing templates we need to get the SgFile so that we can check the command line options.
          file = TransformationSupport::getSourceFile(node);
          if (file != nullptr)
             {
#if USING_PERFORMANCE_TRACING
               TimingPerformance timer ("Fixup function default arguments: processing the SgSourceFile");
#endif
            // This simplifies how the traversal is called!
               FixupFunctionDefaultArguments declarationFixupTraversal(file);

            // This inherited attribute is used for all traversals (within the iterative approach we define)
               FixupFunctionDefaultArgumentsInheritedAttribute inheritedAttribute;

               {
#if USING_PERFORMANCE_TRACING
               TimingPerformance timer ("Fixup function default arguments: traversal to generate map of sets");
#endif
            // This will be called iteratively so that we can do a fixed point iteration
               declarationFixupTraversal.traverse(node,inheritedAttribute);
               }

               {
#if USING_PERFORMANCE_TRACING
               TimingPerformance timer ("Fixup function default arguments: processing the map of sets");
#endif
            // Now we have assembled the global data structure to represent the function declarations using
            // default arguments and we have to use this data to eliminate the redundant default entries.
               std::map<SgFunctionDeclaration*,FixupFunctionDefaultArguments::SetStructure*>::iterator i = declarationFixupTraversal.mapOfSetsOfFunctionDeclarations.begin();
               while (i != declarationFixupTraversal.mapOfSetsOfFunctionDeclarations.end())
                  {
                    SgFunctionDeclaration* firstNondefiningDeclaration        = i->first;
                    FixupFunctionDefaultArguments::SetStructure* setStructure = i->second;

                    ASSERT_not_null(firstNondefiningDeclaration);
                    ASSERT_not_null(setStructure);
#if 0
                 // DQ (7/20/2025): This is called more often that I wish to worry about being processed.
                    TimingPerformance timer ("Fixup function default arguments: processing the sets");
#endif
                    SgFunctionDeclaration* bestFunctionDeclarationForDefaultArguments = setStructure->associatedFunctionDeclaration;
                    std::set<SgFunctionDeclaration*> & setOfFunctionDeclarations = setStructure->setOfFunctionDeclarations;
                    std::set<SgFunctionDeclaration*>::iterator j = setOfFunctionDeclarations.begin();
                    while (j != setOfFunctionDeclarations.end())
                       {
                         SgFunctionDeclaration* functionDeclarationFromSet = *j;
                         if (functionDeclarationFromSet != bestFunctionDeclarationForDefaultArguments)
                            {
#if USING_PERFORMANCE_TRACING
                              TimingPerformance timer ("Fixup function default arguments: processing a function");
#endif
                              SgFunctionDeclaration* functionDeclarationWithRedundantDefaultArguments = functionDeclarationFromSet;
                              SgInitializedNamePtrList & argList = functionDeclarationWithRedundantDefaultArguments->get_args();
                              SgInitializedNamePtrList::iterator k = argList.begin();
                              while (k != argList.end())
                                 {
                                   SgInitializedName* arg = *k;
                                   SgExpression* defaultArgument = arg->get_initializer();
                                   if (defaultArgument != nullptr)
                                      {
                                     // DQ (1/27/2019): Test this for a Lambda Expression: see Cxx11_tests/test2019_38.C.
                                     // This sort of test would not be suffient, comment out the call to delete instead.
                                        bool foundLambda = containsLambdaSupportForFixupFunctionDefaultArguments(defaultArgument);
                                        if (foundLambda == false)
                                           {
#if USING_PERFORMANCE_TRACING
                                             TimingPerformance timer ("Fixup function default arguments: traversal to process sets: deleteAST (now skipping deleteAST)");
#endif
#if 0
                                             size_t numberOfNodesToDelete = numberOfNodeInAST(defaultArgument);
                                             totalNumberOfNodedDeleted += numberOfNodesToDelete;
                                             printf ("In fixupFunctionDefaultArguments(): defaultArgument = %p = %s numberOfNodesToDelete = %zu totalNumberOfNodedDeleted = %zu \n",
                                                  defaultArgument,defaultArgument->class_name().c_str(),numberOfNodesToDelete,totalNumberOfNodedDeleted);
#endif
                                          // DQ (7/20/2025): After discussion with Tristan, we will skip the call to deleteAST() because
                                          // it is expensive (after the performance bug with symbol table fixup, it now takes 1/3 of the
                                          // time to just call deleteAST() (on a input program including rose.h header file as a benchmark).
                                          // Note that for this input program, there are 20M IR nodes, but only 1700 need to be deleted,
                                          // so this is an insignificant memory issue. A plan is to at a later date use a function that
                                          // Tristan wrote to identify and delete all of the IR nodes that are disconected from the AST
                                          // (as defined with a root at SgProject).
                                          // SageInterface::deleteAST(defaultArgument);
                                             defaultArgument->set_parent(NULL);
                                             arg->set_initializer(NULL);
                                           }
                                          else
                                           {
                                             printf ("NOTE: Skipping call to SageInterface::deleteAST() for default arguments containing lambda expressions \n");
                                           }

                                        defaultArgument = nullptr;
                                        arg->set_initializer(nullptr);
                                      }
                                   
                                   k++;
                                 }
                            }

                         j++;
                       }

                    i++;
                  }
               } // end of timer
             }
        }
   }

FixupFunctionDefaultArgumentsInheritedAttribute::FixupFunctionDefaultArgumentsInheritedAttribute()
   {
   }

FixupFunctionDefaultArguments::FixupFunctionDefaultArguments(SgSourceFile* /*file*/)
   {
   }

FixupFunctionDefaultArgumentsInheritedAttribute
FixupFunctionDefaultArguments::evaluateInheritedAttribute(SgNode* node, FixupFunctionDefaultArgumentsInheritedAttribute /*inheritedAttribute*/)
   {
  // This function generates a data structure of function declarations using default arguments so that we can determine the function declaration 
  // that is closest to the default argument and thus the function that should be the ONLY function declaration to specify the default arguments.
  // This is a post-processing phase because this information must be reconstructed, because EDG loses this information in its AST.

  // I think that our design does not need inherited attributes, but this might change.
     FixupFunctionDefaultArgumentsInheritedAttribute returnValue;
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != nullptr)
        {
          SgFunctionParameterList* parameterList = functionDeclaration->get_parameterList();
          ASSERT_not_null(parameterList);
          SgInitializedNamePtrList & initializedNameList = parameterList->get_args();
          SgInitializedNamePtrList::iterator i = initializedNameList.begin();

          bool functionNotProcessed = false;
          while (functionNotProcessed == false && i != initializedNameList.end())
             {
               if ((*i)->get_initializer() != nullptr)
                  {
                    SgInitializedName* initializedName = *i;
                    SgExpression* defaultExpression = initializedName->get_initializer();
                    ASSERT_not_null(defaultExpression);
                    SgFunctionDeclaration* firstNondefiningFunctionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
                    ASSERT_not_null(firstNondefiningFunctionDeclaration);
                    std::map<SgFunctionDeclaration*,SetStructure*>::iterator existingSetIterator = mapOfSetsOfFunctionDeclarations.find(firstNondefiningFunctionDeclaration);
                    int d = defaultExpression->get_file_info()->get_source_sequence_number() - functionDeclaration->get_file_info()->get_source_sequence_number();

                 // DQ (6/24/2013): This test fails for ROSE compiling ROSE (largest input codes yet tested).
                 // Issue a warning and commented out the assertion for now.  Need to investigate this further.
                 // This might be OK to have be negative in the case of some template or whare one of the other 
                 // of the source sequence points are unknown (compiler generated).
                    if (d < 0)
                       {
                         printf ("WARNING: negative distance generated between source sequence numbers: d = %d (commented out assertion) \n",d);
                       }
                    
                    if (existingSetIterator != mapOfSetsOfFunctionDeclarations.end())
                       {
                      // This is the previously built setStructure.
                         SetStructure* existingSet = existingSetIterator->second;
                         if (d <= existingSet->distanceInSourceSequence)
                            {
                           // This is the better function to use.
                              existingSet->associatedFunctionDeclaration = functionDeclaration;
                              existingSet->distanceInSourceSequence = d;
                            }
                         existingSet->setOfFunctionDeclarations.insert(functionDeclaration);
                       }
                      else
                       {
                         SetStructure* newSet = new SetStructure();
                         mapOfSetsOfFunctionDeclarations[firstNondefiningFunctionDeclaration] = newSet;
                         newSet->associatedFunctionDeclaration = functionDeclaration;
                         newSet->distanceInSourceSequence = d;
                         newSet->setOfFunctionDeclarations.insert(functionDeclaration);
                       }

                 // As soon as we have processed a single default argument for the function we have processed 
                 // the function and need not worry about other default arguments.  When we process the AST
                 // (later) to remove the redundant default arguments we will process all of the default 
                 // arguments for each function uniformly.
                    functionNotProcessed = true;
                  }

               i++;
             }
        }

     return returnValue;
   }
