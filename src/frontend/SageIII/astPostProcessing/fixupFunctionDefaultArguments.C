// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "fixupFunctionDefaultArguments.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif




bool
containsLambdaSupportForFixupFunctionDefaultArguments (SgExpression* node)
   {
  // This function takes the initializer for any default initialization.
  // I need it becasue the SageInterface::deleteAST is not robust enough to support the rather complicated 
  // case of deleting a lambda expression and allof the associated generated classes and member functions.

     class LambdaTestTraversal : public AstSimpleProcessing
        {
          public:
               bool foundLambda;

               LambdaTestTraversal() : foundLambda(false) {}
               void visit (SgNode* node)
                  {
                    SgLambdaExp* lambda = isSgLambdaExp(node);
                    if (lambda != NULL)
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


void
fixupFunctionDefaultArguments( SgNode* node )
   {
  // This function determins the best function declaration where to associate default arguments.

     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In fixupFunctionDefaultArguments(): This function detects the correct function declaration to associated default arguments node = %p = %s \n",node,node->class_name().c_str());
#endif

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup function default arguments:");

     SgSourceFile* file = NULL;

     if (node->get_parent() == NULL)
        {
#if 0
          printf ("In fixupFunctionDefaultArguments(): Detected AST fragement not associated with primary AST, ignore template handling ... \n");
#endif
          SgProject *project = isSgProject(node);
          if (project != NULL)
             {
            // GB (9/4/2009): Added this case for handling SgProject nodes. We do
            // this simply by iterating over the list of files in the project and
            // calling this function recursively. This is only one level of
            // recursion since files are not nested.
               SgFilePtrList &files = project->get_fileList();
               SgFilePtrList::iterator fIterator;
               for (fIterator = files.begin(); fIterator != files.end(); ++fIterator)
                  {
                    SgFile *file = *fIterator;
                    ROSE_ASSERT(file != NULL);
#if 0
                    printf ("Calling fixupFunctionDefaultArguments() for file = %p = %s \n",file,file->class_name().c_str());
#endif
                    fixupFunctionDefaultArguments(file);
#if 0
                    printf ("DONE: Calling fixupFunctionDefaultArguments() for file = %p = %s \n",file,file->class_name().c_str());
#endif
                  }
             }
            else
             {
            // DQ (4/25/2013): Output at least a warning message.
#if 0
               printf ("WARNING: In fixupFunctionDefaultArguments(): This is not a proper AST, default arguments not processed... \n");
#endif
             }
        }
       else
        {
#if 0
          printf ("In fixupFunctionDefaultArguments(): Getting the SgFile from the AST node \n");
#endif
          file = TransformationSupport::getSourceFile(node);
       // When processing templates we need to get the SgFile so that we can check the command line options.
       // ROSE_ASSERT(file != NULL);
#if 0
          printf ("current file is: %s \n",(file != NULL) ? file->getFileName().c_str() : "NULL SgFile pointer");
#endif
          if (file != NULL)
             {
            // This simplifies how the traversal is called!
               FixupFunctionDefaultArguments declarationFixupTraversal(file);

            // This inherited attribute is used for all traversals (within the iterative approach we define)
               FixupFunctionDefaultArgumentsInheritedAttribute inheritedAttribute;

            // This will be called iteratively so that we can do a fixed point iteration
               declarationFixupTraversal.traverse(node,inheritedAttribute);

            // Now we have assembled the global data structure to represent the function declarations using
            // default arguments and we have to use this data to eliminate the redundant default entries.
#if 0
               printf ("NOTE: In fixupFunctionDefaultArguments(): Use assembled data structure of functions containing defaut arguments to eliminate redundant default arguments \n");
#endif
               std::map<SgFunctionDeclaration*,FixupFunctionDefaultArguments::SetStructure*>::iterator i = declarationFixupTraversal.mapOfSetsOfFunctionDeclarations.begin();
               while (i != declarationFixupTraversal.mapOfSetsOfFunctionDeclarations.end())
                  {
                    SgFunctionDeclaration* firstNondefiningDeclaration        = i->first;
                    FixupFunctionDefaultArguments::SetStructure* setStructure = i->second;

                    ROSE_ASSERT(firstNondefiningDeclaration != NULL);
                    ROSE_ASSERT(setStructure != NULL);

                    SgFunctionDeclaration* bestFunctionDeclarationForDefaultArguments = setStructure->associatedFunctionDeclaration;
#if 0
                    printf ("In fixupFunctionDefaultArguments(): best function = %p associated distance = %d \n",bestFunctionDeclarationForDefaultArguments,setStructure->distanceInSourceSequence);
#endif
                    std::set<SgFunctionDeclaration*> & setOfFunctionDeclarations = setStructure->setOfFunctionDeclarations;
                    std::set<SgFunctionDeclaration*>::iterator j = setOfFunctionDeclarations.begin();
                    while (j != setOfFunctionDeclarations.end())
                       {
                         SgFunctionDeclaration* functionDeclarationFromSet = *j;
                         if (functionDeclarationFromSet != bestFunctionDeclarationForDefaultArguments)
                            {
                              SgFunctionDeclaration* functionDeclarationWithRedundantDefaultArguments = functionDeclarationFromSet;
#if 0
                              printf ("In fixupFunctionDefaultArguments(): Remove this function declaration's default arguments: functionDeclarationWithRedundantDefaultArguments = %p \n",functionDeclarationWithRedundantDefaultArguments);
#endif
                              SgInitializedNamePtrList & argList = functionDeclarationWithRedundantDefaultArguments->get_args();
                              SgInitializedNamePtrList::iterator k = argList.begin();
                              while (k != argList.end())
                                 {
                                   SgInitializedName* arg = *k;
#if 0
                                   printf ("arg = %p name = %s \n",arg,arg->get_name().str());
#endif
                                   SgExpression* defaultArgument = arg->get_initializer();
                                   if (defaultArgument != NULL)
                                      {
#if 0
                                        printf ("In fixupFunctionDefaultArguments(): Found a redundant default argument = %p = %s \n",defaultArgument,defaultArgument->class_name().c_str());
#endif
                                     // DQ (1/27/2019): Test this for a Lambda Expression: see Cxx11_tests/test2019_38.C.
                                     // This sort of test would not be suffient, comment out the call to delete instead.
                                     // printf ("Commenting out call to SageInterface::deleteAST(): for default argument \n");
                                     // SageInterface::deleteAST(defaultArgument);

                                        bool foundLambda = containsLambdaSupportForFixupFunctionDefaultArguments(defaultArgument);
                                        if (foundLambda == false)
                                           {
                                             SageInterface::deleteAST(defaultArgument);
                                           }
                                          else
                                           {
                                             printf ("NOTE: Skipping call to SageInterface::deleteAST() for default arguments containing lambda expressions \n");
                                           }

                                        defaultArgument = NULL;

                                        arg->set_initializer(NULL);
                                      }
                                   
                                   k++;
                                 }
                            }
                         j++;
                       }
                    
                    i++;
                  }
             }
            else
             {
            // DQ (4/25/2013): Output at least a warning message.
#if 0
            // DQ (5/25/2013): Comment out this warning message since I think it is causing the binaryAnalysis tests to fail.
               printf ("WARNING: In fixupFunctionDefaultArguments(): This function handles only SgProject and SgFile IR nodes as input, default arguments not processed... \n");
#endif
             }
        }
   }

FixupFunctionDefaultArgumentsInheritedAttribute::FixupFunctionDefaultArgumentsInheritedAttribute()
   {
   }


FixupFunctionDefaultArguments::FixupFunctionDefaultArguments(SgSourceFile* file)
   {
   }


FixupFunctionDefaultArgumentsInheritedAttribute
FixupFunctionDefaultArguments::evaluateInheritedAttribute ( SgNode* node, FixupFunctionDefaultArgumentsInheritedAttribute inheritedAttribute )
   {
  // This function generates a data structure of function declarations using default arguments so that we can determine the function declaration 
  // that is closest to the default argument and thus the function that should be the ONLY function declaration to specify the default arguments.
  // This is a post-processing phase because this information must be reconstructed, because EDG loses this information in its AST.

  // I think that our design does not need inherited attributes, but this might change.
     FixupFunctionDefaultArgumentsInheritedAttribute returnValue;

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
#if 0
          printf ("In FixupFunctionDefaultArguments::evaluateInheritedAttribute(): functionDeclaration = %p = %s name = %s \n",
               functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
          SgFunctionParameterList* parameterList = functionDeclaration->get_parameterList();
          ROSE_ASSERT(parameterList != NULL);
          SgInitializedNamePtrList & initializedNameList = parameterList->get_args();
          SgInitializedNamePtrList::iterator i = initializedNameList.begin();

          bool functionNotProcessed = false;
          while (functionNotProcessed == false && i != initializedNameList.end())
             {
               if ( (*i)->get_initializer() != NULL)
                  {
                    SgInitializedName* initializedName = *i;
                    SgExpression* defaultExpression = initializedName->get_initializer();
                    ROSE_ASSERT(defaultExpression != NULL);
#if 0
                    printf ("Found a default argument: initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                    SgFunctionDeclaration* firstNondefiningFunctionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
                    ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
                    std::map<SgFunctionDeclaration*,SetStructure*>::iterator existingSetIterator = mapOfSetsOfFunctionDeclarations.find(firstNondefiningFunctionDeclaration);
                    int d = defaultExpression->get_file_info()->get_source_sequence_number() - functionDeclaration->get_file_info()->get_source_sequence_number();
#if 0
                    printf ("   --- defaultExpression->get_file_info()->get_source_sequence_number()   = %d \n",defaultExpression->get_file_info()->get_source_sequence_number());
                    printf ("   --- functionDeclaration->get_file_info()->get_source_sequence_number() = %d \n",functionDeclaration->get_file_info()->get_source_sequence_number());
#endif
                 // DQ (6/24/2013): This test fails for ROSE compiling ROSE (largest input codes yet tested).
                 // Issue a warning and commented out the assertion for now.  Need to investigate this further.
                 // This might be OK to have be negative in the case of some template or whare one of the other 
                 // of the source sequence points are unknown (compiler generated).
                    if (d < 0)
                       {
                         printf ("WARNING: negative distance generated between source sequence numbers: d = %d (commented out assertion) \n",d);
                       }
                 // Because of the way we construct the AST initally, we should only have positive values or zero as distance.
                 // ROSE_ASSERT(d >= 0);
                    
                    if (existingSetIterator != mapOfSetsOfFunctionDeclarations.end())
                       {
                      // This is the previously built setStructure.
                         SetStructure* existingSet = existingSetIterator->second;
#if 0
                         printf ("For existing SetStructure: distance d = %d \n",d);
#endif
                         if (d <= existingSet->distanceInSourceSequence)
                            {
                           // This is the better function to use.
#if 0
                              printf ("Resetting the current minimal distance and storing the associated function declaration: d = %d functionDeclaration = %p \n",d,functionDeclaration);
#endif
                              existingSet->associatedFunctionDeclaration = functionDeclaration;
                              existingSet->distanceInSourceSequence = d;
                            }
                         existingSet->setOfFunctionDeclarations.insert(functionDeclaration);
                       }
                      else
                       {
                         SetStructure* newSet = new SetStructure();
                         mapOfSetsOfFunctionDeclarations[firstNondefiningFunctionDeclaration] = newSet;
#if 0
                         printf ("For new SetStructure: distance d = %d \n",d);
#endif
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

#if 0
  // DQ (4/26/2013): Debugging code ot chase down function call argument errors in the default expressions 
  // constructed and the default arguments that we see in the final AST.
     SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(node);
     if (functionCallExpression != NULL)
        {
          SgExpressionPtrList & expList = functionCallExpression->get_args()->get_expressions();
          SgExpressionPtrList::iterator i = expList.begin();
          while (i != expList.end())
             {
               printf ("function call argument expression = %p = %s \n",*i,(*i)->class_name().c_str());
               (*i)->get_file_info()->display("function call argument expression");
               i++;
             }
        }
#endif

     return returnValue;
   }

