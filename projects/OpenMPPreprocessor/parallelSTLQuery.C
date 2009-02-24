// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure
#include "rose.h"
#include <string>
#include <iomanip>

#include "AstConsistencyTests.h"

#include <algorithm>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class UserDefinedContainerInfo
   {
  // Specification of container that can be optimized by parallel transformation

     public:
       // names of containers (type names) appearing in the for loop header being optimized
          string containerTypeName;

       // names of iterators (type names) appearing in the for loop header being optimized
          string iteratorTypeName;

       // names of acceptable functions that can appear in loop bodies being optimized
       // these names should perhaps be mangled to permit resolution of overloaded functions
          string functionName;

          UserDefinedContainerInfo ( string containerName, string iteratorName, string function )
             : containerTypeName(containerName), iteratorTypeName(iteratorName), functionName(function)
             {}
   };


class OpenMP_Translation_Utility
   {
  // This class contains the list of abstractions that will be optimized and the function to
  // determine if a point in the AST qualifies for the transformation (parallel optimization)

     public:
          typedef list<UserDefinedContainerInfo> AbstractionListType;
          AbstractionListType* abstractionList;

          OpenMP_Translation_Utility( const UserDefinedContainerInfo & X );
          bool isParallelIterationOverUserDefinedContainer( SgNode* astNode , bool nestedForLoop );

          OpenMP_Translation_Utility()
            {
              abstractionList = new list<UserDefinedContainerInfo>;
              ROSE_ASSERT (abstractionList != NULL);
            }

          void addAbstractionTarget ( const UserDefinedContainerInfo & X )
             {
               ROSE_ASSERT (abstractionList != NULL);
               abstractionList->push_back(X);
             }
          int size() const
             {
               ROSE_ASSERT (abstractionList != NULL);
               return abstractionList->size();
             }

#if 0
      // Supporting code for a later implementation using function pointers
         AbstractionListType matchingNameList (
             AbstractionListType l,
             string name,
             UserDefinedContainerInfo::name* memberFunction )
             {
               AbstractionListType newList;
               for (AbstractionListType::iterator i=l.begin(); i != l.end(); i++)
                  {
                    if ( i->memberFunction() == name )
                       newList.push_back(*i);
                  }

               ROSE_ASSERT (newList.size() > 0);
               return newList;
             }
#endif

          AbstractionListType matchingContainerNameList (
             AbstractionListType l, string containerName )
             {
#if 0
             // Code for a later implementation using function pointers
                return matchingNameList(l,containerName,UserDefinedContainerInfo::getContainerName);
#else
               AbstractionListType newList;
//             if (containerName != "iterator")
                  {
                    for (AbstractionListType::iterator i=l.begin(); i != l.end(); i++)
                       {
                         if ( (*i).containerTypeName == containerName )
                            newList.push_back(*i);
                       }
                    ROSE_ASSERT (newList.size() > 0);
                  }

               return newList;
#endif
             }

          AbstractionListType matchingFunctionNameList ( 
             AbstractionListType l, string functionName )
             {
#if 0
             // Code for a later implementation using function pointers
               return matchingNameList(l,containerName,UserDefinedContainerInfo::getFunctionName);
#else
               AbstractionListType newList;
               for (AbstractionListType::iterator i=l.begin(); i != l.end(); i++)
                  {
                    if ( (*i).functionName == functionName )
                       newList.push_back(*i);
                  }

               ROSE_ASSERT (newList.size() > 0);
               return newList;
#endif
             }

          AbstractionListType matchingContainerNameList ( string containerName )
             {
               ROSE_ASSERT (abstractionList != NULL);
               return matchingContainerNameList(*abstractionList,containerName);
             }
          AbstractionListType matchingFunctionNameList ( string functionName )
             {
               ROSE_ASSERT (abstractionList != NULL);
               return matchingFunctionNameList(*abstractionList,functionName);
             }
   };

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class ParallelContainerQueryInheritedAttribute
   {
     public:
       // state information (context)
          bool inForStatement;
          bool inInitializer;
          bool inTestExpression;
          bool inIncrementExpression;
          bool inLoopBody;

      // Specific constructors are required
          ParallelContainerQueryInheritedAttribute ()
             {
               inForStatement        = false;
               inInitializer         = false;
               inTestExpression      = false;
               inIncrementExpression = false;
               inLoopBody            = false;
             }

          ParallelContainerQueryInheritedAttribute ( const ParallelContainerQueryInheritedAttribute & X )
             {
               inForStatement        = X.inForStatement;
               inInitializer         = X.inInitializer;
               inTestExpression      = X.inTestExpression;
               inIncrementExpression = X.inIncrementExpression;
               inLoopBody            = X.inLoopBody;
             }
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class ParallelContainerQuerySynthesizedAttribute
   {
     public:
       // record intermediate status of evaluation
          bool validInitializer;
          bool validTestExpression;
          bool validIncrementExpression;
          bool validLoopBody;

       // record final result
          bool validParallelTransformation;

          ParallelContainerQuerySynthesizedAttribute()
             {
               validInitializer            = false;
               validTestExpression         = false;
               validIncrementExpression    = false;
               validLoopBody               = false;
               validParallelTransformation = false;
             };
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class ParallelContainerQueryTraversal
   : public SgTopDownBottomUpProcessing<ParallelContainerQueryInheritedAttribute,
                                        ParallelContainerQuerySynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

       // names of containers, iterator, and functions (type names) to optimize
       // list<UserDefinedContainerInfo>* containerInfoList;
          OpenMP_Translation_Utility* containerInfoList;

          ParallelContainerQueryTraversal ( OpenMP_Translation_Utility* abstractionList )
             : traversalNodeCounter(0), containerInfoList(abstractionList)
             {
               ROSE_ASSERT (containerInfoList != NULL);
               ROSE_ASSERT (containerInfoList->size() > 0);
             }

       // Functions required by the rewrite mechanism
          ParallelContainerQueryInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             ParallelContainerQueryInheritedAttribute inheritedAttribute );

          ParallelContainerQuerySynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             ParallelContainerQueryInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Functions required by the tree traversal mechanism
ParallelContainerQueryInheritedAttribute
ParallelContainerQueryTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     ParallelContainerQueryInheritedAttribute inheritedAttribute )
   {
     traversalNodeCounter++;

  // printf ("In evaluateInheritedAttribute: (astNode->sage_class_name() = %s) \n",astNode->sage_class_name());

     ROSE_ASSERT (containerInfoList != NULL);
     ROSE_ASSERT (containerInfoList->size() > 0);

     switch(astNode->variantT())
        {
          case V_SgForStatement:
             {
            // printf ("Found the SgForStatement \n");
               inheritedAttribute.inForStatement = true;
               break;
             }

          case V_SgForInitStatement:
             {
            // printf ("Found the SgForInitStatement \n");
               if (inheritedAttribute.inForStatement == true)
                    inheritedAttribute.inInitializer = true;
               break;
             }

          case V_SgExpressionRoot:
             {
            // printf ("Found an expression node (inheritedAttribute.inForStatement = %s) \n",
            //      (inheritedAttribute.inForStatement) ? "true" : "false");
               if (inheritedAttribute.inForStatement == true)
                  {
                 // printf ("The parent is a SgForStatement \n");
                    SgNode* parentForStatement = NULL;
                    SgNode* parent = astNode;
                    while (parentForStatement == NULL)
                       {
                      // printf ("Searching for the parent SgForStatement (node = %s) \n",parent->sage_class_name());
                         ROSE_ASSERT (parent != NULL);
                         parentForStatement = isSgForStatement(parent->get_parent());
                         parent = parent->get_parent();
                       }
                    ROSE_ASSERT (parentForStatement != NULL);

                    SgForStatement* forStatement = isSgForStatement(parentForStatement);

                    ROSE_ASSERT (forStatement != NULL);

                 // Of course we are in a for loop (but check just to make sure)
                 // ROSE_ASSERT (inheritedAttribute.inForStatement == true);

                 // Using the parent node see which expression node we 
                 // are in this invocation of evaluateInheritedAttribute
                    if (astNode == forStatement->get_test_expr_root())
                       {
                      // printf ("Found the test expression \n");
                         inheritedAttribute.inTestExpression = true;
                       }
                      else
                       {
                         if (astNode == forStatement->get_increment_expr_root())
                            {
                           // printf ("Found the increment expression \n");
                              inheritedAttribute.inIncrementExpression = true;
                            }
                       }
                  }
               break;
             }

          case V_SgBasicBlock:
             {
               if (inheritedAttribute.inForStatement == true)
                  {
                 // printf ("Found the Basic Block in the for loop \n");
                    inheritedAttribute.inLoopBody = true;
                  }
               break;
             }

       // DQ (11/28/2005): Added default case to avoid compiler warnings
          default:
             {
             }
        }

     return inheritedAttribute;
   }

ParallelContainerQuerySynthesizedAttribute
ParallelContainerQueryTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     ParallelContainerQueryInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ParallelContainerQuerySynthesizedAttribute returnAttribute;

  // printf ("In evaluateSynthesizedAttribute: (astNode->sage_class_name() = %s) \n",
  //      astNode->sage_class_name());

     switch(astNode->variantT())
        {
          case V_SgForStatement:
             {
               bool isValidForLoopInitializer = synthesizedAttributeList[SgForStatement_for_init_stmt].validInitializer;
            // DQ (11/28/2005): The interface to SgForStatement has been changed to allow for tests to be a SgStatement (declaration)
            // bool isValidForLoopTest        = synthesizedAttributeList[SgForStatement_test_expr_root].validTestExpression;
               bool isValidForLoopTest        = synthesizedAttributeList[SgForStatement_test].validTestExpression;
               bool isValidForLoopIncrement   = synthesizedAttributeList[SgForStatement_increment_expr_root].validIncrementExpression;
               bool isValidForLoopHeader      = isValidForLoopInitializer && isValidForLoopTest && isValidForLoopIncrement;

               bool isValidForLoopBody        = synthesizedAttributeList[SgForStatement_loop_body].validLoopBody;

            // printf ("isValidForLoopInitializer = %s \n",isValidForLoopInitializer ? "true" : "false");
            // printf ("isValidForLoopTest        = %s \n",isValidForLoopTest ? "true" : "false");
            // printf ("isValidForLoopIncrement   = %s \n",isValidForLoopIncrement ? "true" : "false");
            // printf ("isValidForLoopHeader      = %s \n",isValidForLoopHeader ? "true" : "false");
            // printf ("isValidForLoopBody        = %s \n",isValidForLoopBody ? "true" : "false");

               returnAttribute.validParallelTransformation = isValidForLoopHeader && isValidForLoopBody;
               break;
             }

          case V_SgFunctionCallExp:
             {
            // printf ("Found the SgFunctionCallExp \n");

               SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (functionCall != NULL);
               string functionName = TransformationSupport::getFunctionName(functionCall);
               string className = TransformationSupport::getFunctionTypeName(functionCall);

            // printf ("className = %s function name = %s \n",className.c_str(),functionName.c_str());

               if (inheritedAttribute.inInitializer == true)
                  {
                 // initalizer should be either "list::iterator i = list.begin()" or "i = list.begin()"
                 // printf ("Found the SgFunctionCallExp in the for loop initializer \n");
                    OpenMP_Translation_Utility::AbstractionListType l = 
                         containerInfoList->matchingContainerNameList (className);
                    ROSE_ASSERT (l.size() > 0);
                    ROSE_ASSERT (functionName == "begin");
                    returnAttribute.validInitializer = true;
                  }
               if (inheritedAttribute.inTestExpression == true)
                  {
                 // test expression should be "iter != mylist.end()"
                 // printf ("Found the SgFunctionCallExp in the for loop test expression \n");

                    if (className != "iterator")
                       {
                      // Don't test iterator member functions, typically "operator!=()"
                         OpenMP_Translation_Utility::AbstractionListType l = 
                              containerInfoList->matchingContainerNameList (className);
                         ROSE_ASSERT (l.size() > 0);
                         ROSE_ASSERT (functionName == "end");
                         returnAttribute.validTestExpression = true;
                       }
                      else
                       {
                      // need to pass along the synthesized attribute information
                         for (unsigned int i=0; i < synthesizedAttributeList.size(); i++)
                            {
                              returnAttribute.validTestExpression = 
                                   returnAttribute.validTestExpression || 
                                   synthesizedAttributeList[i].validTestExpression;
                            }
                       }
                  }
               if (inheritedAttribute.inIncrementExpression == true)
                  {
                 // This expression should just be "i++", so likely there is something wrong here
                 // (unless it is the operator++() member function)
                 // printf ("Found the SgFunctionCallExp in the for loop increment expression \n");
                    ROSE_ASSERT (functionName == "operator++");
                    returnAttribute.validIncrementExpression = true;
                  }
               if (inheritedAttribute.inLoopBody == true)
                  {
                 // Make sure it is a function from the target library
                 // printf ("Found the SgFunctionCallExp in the for loop body \n");
                    if (className != "iterator")
                       {
                         OpenMP_Translation_Utility::AbstractionListType l = 
                              containerInfoList->matchingFunctionNameList (functionName);
                         ROSE_ASSERT (l.size() > 0);
                         returnAttribute.validLoopBody = true;
                       }
                  }
               
               break;
             }

          default:
             {
            // Merge allthe child attributes to a single synthesized attribute
               for (unsigned int i=0; i < synthesizedAttributeList.size(); i++)
                  {
                    returnAttribute.validInitializer = 
                         returnAttribute.validInitializer || 
                         synthesizedAttributeList[i].validInitializer;
                    returnAttribute.validTestExpression = 
                         returnAttribute.validTestExpression || 
                         synthesizedAttributeList[i].validTestExpression;
                    returnAttribute.validIncrementExpression = 
                         returnAttribute.validIncrementExpression || 
                         synthesizedAttributeList[i].validIncrementExpression;
                    returnAttribute.validLoopBody = 
                         returnAttribute.validLoopBody || 
                         synthesizedAttributeList[i].validLoopBody;
                    returnAttribute.validParallelTransformation = 
                         returnAttribute.validParallelTransformation || 
                         synthesizedAttributeList[i].validParallelTransformation;
                  }
             }
        }

     return returnAttribute;
   }

bool
OpenMP_Translation_Utility::isParallelIterationOverUserDefinedContainer( SgNode* astNode, bool nestedForLoop )
   {
  // Main interface function for analysis

     bool returnValue = false;

     ROSE_ASSERT (abstractionList != NULL);
     ROSE_ASSERT (abstractionList->size() > 0);

     ROSE_ASSERT (nestedForLoop == false);
     if (astNode->variantT() == V_SgForStatement)
        {
          ParallelContainerQueryTraversal treeTraversal(this);
          ParallelContainerQueryInheritedAttribute inheritedAttribute;
          ParallelContainerQuerySynthesizedAttribute returnAttribute;
          returnAttribute = treeTraversal.traverse(astNode,inheritedAttribute);

       // ROSE_ASSERT (returnAttribute.validParallelTransformation == true);

          returnValue = returnAttribute.validParallelTransformation;
        }

     return returnValue;
   }

class TreeVisitorTraversal
   : public SgSimpleProcessing
   {
  // Simple traversal class to imimic the use of the analysis in other traversals of the AST

     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

          OpenMP_Translation_Utility translationUtility;

          TreeVisitorTraversal (const OpenMP_Translation_Utility & intputTranslationUtility)
             : traversalNodeCounter(0)
             {
               translationUtility = intputTranslationUtility;
               ROSE_ASSERT (translationUtility.size() > 0);
             };

       // Functions required by the traversal mechanism
          void visit ( SgNode* astNode )
             {
               bool nestedForLoop = false;
               ROSE_ASSERT (translationUtility.size() > 0);
               bool doTransformation =
                    translationUtility.isParallelIterationOverUserDefinedContainer(astNode,nestedForLoop);

               if (doTransformation == true)
                  {
                 // Print out a celibratory message that we found a place to do the transformation!
                    printf ("\n**************************************************************************** \n");
                    printf ("Found a location to parallelize the iteration over a container using OpenMP! \n");
                    printf ("**************************************************************************** \n\n");
                  }
             }
   };

int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     AstTests::runAllTests(const_cast<SgProject*>(project));

     OpenMP_Translation_Utility translationUtility;
     UserDefinedContainerInfo abstractionTarget("list", "iterator", "foo");
     translationUtility.addAbstractionTarget(abstractionTarget);

     TreeVisitorTraversal visitorTraversal(translationUtility);
     visitorTraversal.traverseInputFiles(project,preorder);

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

     return backend(project);
  // return backend(frontend(argc,argv));
   }











