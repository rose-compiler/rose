// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astPostProcessing/markOverloadedTemplateInstantiations.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void markOverloadedTemplateInstantiations( SgNode* node )
   {
  // This simplifies how the traversal is called!
     MarkOverloadedTemplateInstantiations astTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astTraversal.traverse(node,preorder);
   }


void
MarkOverloadedTemplateInstantiations::visit ( SgNode *node )
   {
  // This function marks member functions that are overloaded with templated member function
  // the output of the non-template member function will bring out a bug in GNU g++.
  // This processing needs to be done after all template instatiations are marked for output 
  // so that it can turn off the output of specific overloaded member function template 
  // instantiations that would confuse g++.

  // Note that this bug in g++ prevents the transformation of non-template member functions 
  // in template classes that are overloaded with template member function in the same templated class.

  // printf ("MarkOverloadedTemplateInstantiations::visit(%p = %s) \n",node,node->class_name().c_str());

     SgTemplateInstantiationMemberFunctionDecl* memberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(node);
     if (memberFunctionInstantiation != NULL)
        {
#if 0
          SgDeclarationStatement* definingDeclaration         = memberFunctionInstantiation->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = memberFunctionInstantiation->get_firstNondefiningDeclaration();

          bool isDefiningDeclaration         = definingDeclaration         == NULL ? false : (memberFunctionInstantiation == definingDeclaration);
          bool isfirstNondefiningDeclaration = firstNondefiningDeclaration == NULL ? false : (memberFunctionInstantiation == firstNondefiningDeclaration);

          printf ("\n\n");
          printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
          printf ("      Required declaration = %p = %s = %s (is defining decl = %p = %s) (is first non-defining decl = %p = %s) \n",
               memberFunctionInstantiation,memberFunctionInstantiation->sage_class_name(),SageInterface::get_name(memberFunctionInstantiation).c_str(),
               definingDeclaration,isDefiningDeclaration ? "true" : "false", 
               firstNondefiningDeclaration,isfirstNondefiningDeclaration ? "true" : "false");
          printf ("memberFunctionInstantiation->get_parent() = %p = %s \n",
               memberFunctionInstantiation->get_parent(),
               memberFunctionInstantiation->get_parent()->class_name().c_str());
          if (definingDeclaration != NULL)
               printf ("definingDeclaration->get_parent() = %p = %s \n",
                    definingDeclaration->get_parent(),
                    definingDeclaration->get_parent()->class_name().c_str());
          if (firstNondefiningDeclaration != NULL)
               printf ("firstNondefiningDeclaration->get_parent() = %p = %s \n",
                    firstNondefiningDeclaration->get_parent(),
                    firstNondefiningDeclaration->get_parent()->class_name().c_str());
          memberFunctionInstantiation->get_file_info()->display("required declaration: debug");
#endif
          bool searchForOverloadedMemberFunctions = false;
          if (memberFunctionInstantiation->get_file_info()->isOutputInCodeGeneration() == true)
             {
               searchForOverloadedMemberFunctions = true;
             }
#if 0
       // Since the defining and non-defining instantiated template declaration will be in global scope, we can't ignore these.
       // Skip the member function prototypes that we have added to global scope!
       // if ( memberFunctionInstantiation->get_parent() != memberFunctionInstantiation->get_scope() )
          if ( isSgGlobal(memberFunctionInstantiation->get_parent()) != NULL )
             {
               printf ("Ignoring member function found in global scope! \n");
               searchForOverloadedMemberFunctions = false;
             }
#endif
#if 0
          if (memberFunctionInstantiation->get_specialFunctionModifier().isConstructor() == true)
             {
               searchForOverloadedMemberFunctions = true;
             }
#endif

#if 0
          printf ("In MarkOverloadedTemplateInstantiations::visit(): searchForOverloadedMemberFunctions = %s \n",searchForOverloadedMemberFunctions ? "true" : "false");
#endif
          if (searchForOverloadedMemberFunctions == true)
             {
               bool isOverloaded =  SageInterface::isOverloaded(memberFunctionInstantiation);
#if 0
               printf ("In MarkOverloadedTemplateInstantiations::visit(): isOverloaded = %s \n",isOverloaded ? "true" : "false");
#endif
               if (isOverloaded == true)
                  {
                    bool istemplatedMemberFunction = false;
                    if (memberFunctionInstantiation->isTemplateFunction() == true)
                         istemplatedMemberFunction = true;
#if 0
                    printf ("      istemplatedMemberFunction = %s \n",istemplatedMemberFunction ? "true" : "false");
#endif
#if 0
                 // DQ (8/29/2005): Generate different code (marks different template instantiations for 
                 // output) depending upon the backend compiler.
                    string compilerType = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;

                    if (compilerType == "g++")
                       {
                      // Output different template instantiations depending upon 
                      // the version number of the compiler.
                         if ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 3) && 
                              (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER < 4) )
                            {
#if 0
                              printf ("      Calling #0 unsetOutputInCodeGeneration() \n");
#endif
                              if (memberFunctionInstantiation->isSpecialization() == false)
                                   memberFunctionInstantiation->get_file_info()->unsetOutputInCodeGeneration();
                            }
#if 1
                      // DQ (10/11/2007): This should not be reset for g++ 3.4.x! This is a problem related to AST copy. 
                      // This was a problem for test2004_76.C and test2004_142.C. 
                      // However it is required for test2004_143.C
                         if ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 3) ||
                              ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 3) &&
                                (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 4) ) )
                            {
#if 0
                              printf ("      Calling #1 unsetOutputInCodeGeneration() \n");
#endif
                              if (memberFunctionInstantiation->isSpecialization() == false)
                                   memberFunctionInstantiation->get_file_info()->unsetOutputInCodeGeneration();
                            }
#endif
                       }
                      else
                       {
                         printf ("Non GNU compilers not handled for case of generating code for gnu compilers ... \n");
                      // ROSE_ASSERT(false);

                      // DQ (8/10/2006): Treat other compilers as we would the g++ 3.4.x or later compilers
                         if (memberFunctionInstantiation->isSpecialization() == false)
                              memberFunctionInstantiation->get_file_info()->unsetOutputInCodeGeneration();
                       }
#else
                 // DQ (11/6/2007): The code above is equivalent to this simpler code below.
                    if (memberFunctionInstantiation->isSpecialization() == false)
                       {
                      // printf ("*** Calling unsetOutputInCodeGeneration on memberFunctionInstantiation = %p \n",memberFunctionInstantiation);
                         memberFunctionInstantiation->get_file_info()->unsetOutputInCodeGeneration();
                       }
#endif
                  }
             }
#if 0
          printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif
        }
   }
