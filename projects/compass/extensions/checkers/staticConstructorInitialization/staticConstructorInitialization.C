// Static Constructor Initialization
// Author: Daniel J. Quinlan
// Date: 20-July-2007

#include "compass.h"

#ifndef COMPASS_STATIC_CONSTRUCTOR_INITIALIZATION_H
#define COMPASS_STATIC_CONSTRUCTOR_INITIALIZATION_H

namespace CompassAnalyses
   { 
     namespace StaticConstructorInitialization
        { 
        /*! \brief Static Constructor Initialization: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_STATIC_CONSTRUCTOR_INITIALIZATION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Static Constructor Initialization Analysis
// Author: Daniel J. Quinlan
// Date: 20-July-2007

#include "compass.h"
// #include "staticConstructorInitialization.h"

namespace CompassAnalyses
   { 
     namespace StaticConstructorInitialization
        { 
          const std::string checkerName      = "StaticConstructorInitialization";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "variable requiring static constructor initialization";
          const std::string longDescription  = "Static initialization of classes can be a portability issue since different compiler define them to be called in different orders.";
        } //End of namespace StaticConstructorInitialization.
   } //End of namespace CompassAnalyses.

CompassAnalyses::StaticConstructorInitialization::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::StaticConstructorInitialization::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::StaticConstructorInitialization::Traversal::
visit(SgNode* node)
   { 
  // Test for static initialization of variables of type class, such initializations where they are 
  // static or appear in global scope can be called in an order dependent upon the compiler and this 
  // can lead to subtle bugs in large scale applications.

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);

     if (variableDeclaration != NULL)
        {
          SgInitializedNamePtrList::iterator i = variableDeclaration->get_variables().begin();
          while (i != variableDeclaration->get_variables().end())
             {
               SgInitializedName* initializedName = *i;

            // Check the type and see if it is a class (check for typedefs too)
               SgType* variableType = initializedName->get_type();

               SgClassType *classType = isSgClassType(variableType);
               if (classType != NULL)
                  {
                 // Now check if this is a global or namespace variable or an static class member
                 // This might also have to be a test for other scopes as well.
                    SgScopeStatement* scope = variableDeclaration->get_scope();
                    if (isSgGlobal(scope) != NULL || isSgNamespaceDefinitionStatement(scope) != NULL)
                       {
                      // printf ("Found a global variable defining a class = %p \n",initializedName);
                      // variableDeclaration->get_file_info()->display("global variable defining a class");
                         output->addOutput(new CheckerOutput(initializedName));
                       }

                    if (isSgClassDefinition(scope) != NULL)
                       {
                      // Now check if it is a static data member
                         if (variableDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true)
                            {
                           // printf ("Found a static data member defining a class = %p \n",initializedName);
                           // variableDeclaration->get_file_info()->display("static data member defining a class");
                              output->addOutput(new CheckerOutput(initializedName));
                            }
                       }
                  }

            // increment though the variables in the declaration (typically just one)
               i++;
             }
        }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::StaticConstructorInitialization::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::StaticConstructorInitialization::Traversal(params, output);
}

extern const Compass::Checker* const staticConstructorInitializationChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::StaticConstructorInitialization::checkerName,
        CompassAnalyses::StaticConstructorInitialization::shortDescription,
        CompassAnalyses::StaticConstructorInitialization::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
