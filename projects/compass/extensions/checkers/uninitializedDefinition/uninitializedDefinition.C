// Uninitialized Definition
// Author: Gergo  Barany
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_UNINITIALIZED_DEFINITION_H
#define COMPASS_UNINITIALIZED_DEFINITION_H

namespace CompassAnalyses
   { 
     namespace UninitializedDefinition
        { 
        /*! \brief Uninitialized Definition: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
	       CheckerOutput(SgNode* node, const std::string &);
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

// COMPASS_UNINITIALIZED_DEFINITION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Uninitialized Definition Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
// #include "uninitializedDefinition.h"

namespace CompassAnalyses
   { 
     namespace UninitializedDefinition
        { 
          const std::string checkerName      = "UninitializedDefinition";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Variable definition without an initializer.";
          const std::string longDescription  = "A variable is defined without "
            "an initializer expression. Some variable declarations without "
            "initializers are not detected: Class objects and static objects "
            "have default initializers, extern declarations are not "
            "definitions, class members should be initialized by the class "
            "constructor.";
        } //End of namespace UninitializedDefinition.
   } //End of namespace CompassAnalyses.

CompassAnalyses::UninitializedDefinition::
CheckerOutput::CheckerOutput ( SgNode* node, const std::string &reason )
  : OutputViolationBase(node,checkerName,reason+shortDescription)
   {}

CompassAnalyses::UninitializedDefinition::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::UninitializedDefinition::Traversal::
visit(SgNode* n)
   { 
      if (SgVariableDeclaration* vardecl = isSgVariableDeclaration(n))
      {
        SgInitializedNamePtrList& vars = vardecl->get_variables();
        SgInitializedNamePtrList::iterator var;
        for (var = vars.begin(); var != vars.end(); ++var)
        {
          SgInitializedName* initName = *var;
          if (initName->get_initializer() == NULL)
          {
          /* Warn about every variable declaration with a NULL initializer except:
             - extern declarations (these are not definitions)
             - static declarations (these are initialized implicitly)
             - declarations of class members (initializing these is the
               constructor's business)
             - declarations of variables of class type (they have constructors)
             - declarations at global scope (these are implicitly static and
               can only be changed to extern)
           */
            if (!vardecl->get_declarationModifier().get_storageModifier().isExtern()
             && !vardecl->get_declarationModifier().get_storageModifier().isStatic()
             && !isSgClassDefinition(vardecl->get_parent())
             && !isSgClassType(initName->get_type()->stripTypedefsAndModifiers())
             && !isSgGlobal(vardecl->get_parent()))
            {

	      SgNode* parent = n->get_parent();
	      while (!isSgFunctionDeclaration(parent) && !isSgGlobal(parent)) 
		parent=parent->get_parent();
	      std::string funcname="";
	      if (isSgFunctionDeclaration(parent))
		funcname=isSgFunctionDeclaration(parent)->get_name();
	      std::string reason="\tin function: "+funcname+" \t";
	      
              output->addOutput(new CheckerOutput(n,reason));
            }
          }
        }
      }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::UninitializedDefinition::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::UninitializedDefinition::Traversal(params, output);
}

extern const Compass::Checker* const uninitializedDefinitionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::UninitializedDefinition::checkerName,
        CompassAnalyses::UninitializedDefinition::shortDescription,
        CompassAnalyses::UninitializedDefinition::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
