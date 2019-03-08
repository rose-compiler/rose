// Multiple Public Inheritance
// Author: Gergo  Barany
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_MULTIPLE_PUBLIC_INHERITANCE_H
#define COMPASS_MULTIPLE_PUBLIC_INHERITANCE_H

namespace CompassAnalyses
   { 
     namespace MultiplePublicInheritance
        { 
        /*! \brief Multiple Public Inheritance: Add your description here 
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

// COMPASS_MULTIPLE_PUBLIC_INHERITANCE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Multiple Public Inheritance Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
// #include "multiplePublicInheritance.h"

namespace CompassAnalyses
   { 
     namespace MultiplePublicInheritance
        { 
          const std::string checkerName      = "MultiplePublicInheritance";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "A class is defined with more than one public superclass.";
          const std::string longDescription  = "A class is defined with more than one public superclass.";
        } //End of namespace MultiplePublicInheritance.
   } //End of namespace CompassAnalyses.

CompassAnalyses::MultiplePublicInheritance::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::MultiplePublicInheritance::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::MultiplePublicInheritance::Traversal::
visit(SgNode* node)
   { 
     if (SgClassDefinition *cd = isSgClassDefinition(node))
     {
       SgBaseClassPtrList bases = cd->get_inheritances();
       SgBaseClassPtrList::iterator itr;
       size_t publics = 0;
       for (itr = bases.begin(); itr != bases.end(); ++itr)
       {
         SgAccessModifier &am = (*itr)->get_baseClassModifier()->get_accessModifier();
         if (am.isPublic())
           publics++;
       }
       if (publics > 1)
         output->addOutput(new CheckerOutput(cd));
     }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::MultiplePublicInheritance::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::MultiplePublicInheritance::Traversal(params, output);
}

extern const Compass::Checker* const multiplePublicInheritanceChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::MultiplePublicInheritance::checkerName,
        CompassAnalyses::MultiplePublicInheritance::shortDescription,
        CompassAnalyses::MultiplePublicInheritance::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
