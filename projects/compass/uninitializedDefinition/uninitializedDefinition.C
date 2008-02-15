// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Uninitialized Definition Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
#include "uninitializedDefinition.h"

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
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::UninitializedDefinition::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
              output->addOutput(new CheckerOutput(n));
            }
          }
        }
      }
   } //End of the visit function.
