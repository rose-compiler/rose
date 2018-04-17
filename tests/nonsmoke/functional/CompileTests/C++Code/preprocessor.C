// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iomanip>

#include "AstTests.h"

#include <algorithm>

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class dqInheritedAttribute
   {
     public:
      //! Specific constructors are required
          dqInheritedAttribute () {};
          dqInheritedAttribute ( const dqInheritedAttribute & X ) {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class dqSynthesizedAttribute
   {
     public:
         dqSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class dqTraversal
   : public SgTopDownBottomUpProcessing<dqInheritedAttribute,dqSynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

       // list of types that have been traversed
          static list<SgNode*> listOfTraversedTypes;
         
          dqTraversal (): traversalNodeCounter(0) {};

       // Functions required by the rewrite mechanism
          dqInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             dqInheritedAttribute inheritedAttribute );

          dqSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             dqInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Allocation of space for listOfTraversedTypes declared in dqTraversal
list<SgNode*> dqTraversal::listOfTraversedTypes;

// Functions required by the tree traversal mechanism
dqInheritedAttribute
dqTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     dqInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("!!!!! In evaluateInheritedAttribute() \n");
     printf ("     (traversalNodeCounter=%d) astNode->sage_class_name() = %s \n",
          traversalNodeCounter,astNode->sage_class_name());
     printf ("     astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
     vector<string> successorNames = astNode->get_traversalSuccessorNamesContainer();
     printf ("     #of successors = %zu successorNames = ",successorNames.size());
     for (vector<string>::iterator s = successorNames.begin(); s != successorNames.end(); s++)
        {
           printf ("%s ",(*s).c_str());
        }
     printf ("\n");
#endif

     traversalNodeCounter++;

#if 0
     SgStatement* currentStatement = isSgStatement(astNode);
     if (currentStatement != NULL)
        {
          printf ("currentStatement = %s currentStatement = %s \n",currentStatement->sage_class_name(),currentStatement->unparseToString().c_str());
        }
#endif

     switch(astNode->variantT())
        {
          case V_SgTypedefDeclaration:
             {
//             printf ("Found a SgTypedefDeclaration \n");
               break;
             }
          case V_SgTypedefType:
             {
//             printf ("Found a SgTypedefType \n");
               SgTypedefType* typedefType = isSgTypedefType(astNode);
               ROSE_ASSERT (typedefType != NULL);
//             printf ("typedefType->get_name() = %s \n",
//                  (typedefType->get_name().str() != NULL) ? typedefType->get_name().str() : "<Null String>");

               SgType* baseType = typedefType->get_base_type();
//             printf ("typedefType->get_base_type()->sage_class_name() = %s \n",
//                  (baseType != NULL) ? baseType->sage_class_name() : "<Null Pointer>");

               if (baseType != NULL)
                  {
//                  printf ("listOfTraversedTypes.size() = %zu \n",listOfTraversedTypes.size());
                    list<SgNode*>::iterator previouslyTraversedType =
                         find(listOfTraversedTypes.begin(),listOfTraversedTypes.end(),baseType);
                    bool traverseBaseType = (previouslyTraversedType == listOfTraversedTypes.end());
//                  printf ("traverseBaseType = %s \n",(traverseBaseType) ? "true" : "false");
                    if (traverseBaseType == true)
                       {
                      // Add to list of traversed types
                         listOfTraversedTypes.push_back(baseType);

//                       printf ("########################################################## \n");
//                       printf ("Traverse the base type \n");
                      // traverse the base type (skipped by the traversal mechanism, seems to be a bug!)
                         dqTraversal treeTraversal;
                         dqInheritedAttribute inheritedAttribute;

                      // Ignore the return value since we don't need it
                         treeTraversal.traverse(baseType,inheritedAttribute);

//                       printf ("########################################################## \n");

                      // Add to list of traversed types
//                       listOfTraversedTypes.push_back(baseType);
                       }
#if 0
                      else
                       {
                         printf ("Skip traversal of the base type \n");
                       }
#endif
                  }

               break;
             }
          case V_SgTypedefSeq:
             {
//             printf ("Found a SgTypedefSeq \n");
               SgTypedefSeq* typedefSequence = isSgTypedefSeq(astNode);
               ROSE_ASSERT (typedefSequence != NULL);
               break;
             }
          case V_SgClassType:
             {
//             printf ("Found a SgClassType \n");
               SgClassType* classType = isSgClassType(astNode);
               ROSE_ASSERT (classType != NULL);
//             printf ("classType->get_name() = %s \n",
//                  (classType->get_name().str() != NULL) ? classType->get_name().str() : "<Null String>");
               break;
             }
          case V_SgVariableDeclaration:
             {
//             printf ("Found a SgVariableDeclaration \n");
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
               ROSE_ASSERT (variableDeclaration != NULL);
//             variableDeclaration->get_file_info()->display("Called from SgVariableDeclaration case ... ");
               break;
             }
        }

     return inheritedAttribute;
   }

dqSynthesizedAttribute
dqTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     dqInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     dqSynthesizedAttribute returnAttribute;

     switch(astNode->variantT())
        {
          case V_SgTypedefType:
             {
               break;
             }
        }

     return returnAttribute;
   }

#include "sageCommonSourceHeader.h"

extern an_il_header il_header;

int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     printf ("In preprocessor.C: main() \n");

#if 0
     list<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // testing removeArgs
     CommandlineProcessing::removeArgs (argc,argv,"-edg:");
     CommandlineProcessing::removeArgs (argc,argv,"--edg:");
     CommandlineProcessing::removeArgsWithParameters (argc,argv,"-edg_parameter:");
     CommandlineProcessing::removeArgsWithParameters (argc,argv,"--edg_parameter:");

     printf ("argc = %d \n",argc);
     l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("l.size() = %zu \n",l.size());
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());

     printf ("Exiting in main! \n");
     ROSE_ASSERT(1 == 2);
#endif

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
  // See if we can access the EDG AST directly
     ROSE_ASSERT (il_header.primary_source_file != NULL);
     ROSE_ASSERT (il_header.primary_source_file->file_name != NULL);
     printf ("##### il_header.primary_source_file->file_name = %s \n",il_header.primary_source_file->file_name);
#endif

  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
     AstTests::runAllTests(const_cast<SgProject*>(project));

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

     printf ("Generate the DOT output of the SAGE III AST \n");
     generateDOT ( *project );
     printf ("DONE: Generate the DOT output of the SAGE III AST \n");

#if 0
     dqTraversal treeTraversal;
     dqInheritedAttribute inheritedAttribute;

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(project,inheritedAttribute);
#endif

#if 0
     printf ("\n\n");
     printf ("############################################# \n");
     printf ("############################################# \n");
     list<SgNode*> nodeList;
  // nodeList = NodeQuery::querySubTree (project,V_SgTypeInt,NodeQuery::ExtractTypes);
     nodeList = NodeQuery::querySubTree (project,V_SgType,NodeQuery::ExtractTypes);
     printf ("\nnodeList.size() = %zu \n",nodeList.size());

     list<SgNode*>::iterator i = nodeList.begin();
     while (i != nodeList.end())
        {
       // printf ("Query node = %p = %s \n",*i,(*i)->sage_class_name());
       // printf ("Query node = %s \n",(*i)->unparseToString().c_str());
          printf ("Query node = %p = %s = %s \n",*i,(*i)->sage_class_name(),(*i)->unparseToString().c_str());
          i++;
        }
     printf ("############################################# \n");
     printf ("############################################# \n");
     printf ("\n\n");

     printf ("Exiting ... \n");
     ROSE_ASSERT (false);
#endif

     printf ("Calling the backend() \n");

     return backend(project);
  // return backend(frontend(argc,argv));
   }











