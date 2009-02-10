// Name All Parameters
// Author: Valentin  David
// Date: 03-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NAME_ALL_PARAMETERS_H
#define COMPASS_NAME_ALL_PARAMETERS_H

namespace CompassAnalyses {
  namespace NameAllParameters {
    /*! \brief Name All Parameters: Add your description here
     */

     extern const std::string checkerName;
     extern const std::string shortDescription;
     extern const std::string longDescription;

     class CheckerOutput: public Compass::OutputViolationBase
        {
          public:
               CheckerOutput(SgInitializedName* arg, SgFunctionDeclaration* fun);
        };

    // Specification of Checker Traversal Implementation
     class Traversal : public Compass::AstSimpleProcessingWithRunFunction
        {
               Compass::OutputObject* output;
          public:
               Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

               void run(SgNode* n)
                  {
                    this->traverse(n, preorder);
                  }

               void visit(SgNode* n);
        };
  }
}

// COMPASS_NAME_ALL_PARAMETERS_H
#endif
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Name All Parameters Analysis
// Author: Valentin  David
// Date: 03-August-2007

// History:
// DQ (1/15/2008): Fixed this checker to skip warning for SgTypeEllipse.

#include "compass.h"
// #include "nameAllParameters.h"

namespace CompassAnalyses
   {
     namespace NameAllParameters
        {
          const std::string checkerName      = "NameAllParameters";

          const std::string shortDescription = "Check that all parameters are named";
          const std::string longDescription  = "This checker check that all "
               "parameters are named. If one is not used, at static_cast<void> "
               "should be used to disable compiler warnings.";
        } //End of namespace NameAllParameters.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NameAllParameters::CheckerOutput::
CheckerOutput(SgInitializedName* arg, SgFunctionDeclaration* fun)
   : OutputViolationBase(arg, checkerName,"function " + fun->get_name() + " has an unnamed parameter.")
   {
   }

CompassAnalyses::NameAllParameters::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::NameAllParameters::Traversal::
visit(SgNode* n)
   {
  // This function only looks for function declarations
     SgFunctionDeclaration *decl = isSgFunctionDeclaration(n);
     if (decl != NULL)
        {
       // Get the list of function arguments
          SgInitializedNamePtrList args = decl->get_args();
          for (SgInitializedNamePtrList::iterator i = args.begin(); i != args.end(); ++i)
             {
            // Check if the name is empty
               if ((*i)->get_name().str() == std::string(""))
                  {
                    SgTypeEllipse* ellipseType = isSgTypeEllipse((*i)->get_type());
                    if (ellipseType != NULL)
                       {
                      // If this is an elipse type ("...") then there never should be a name, so an empty name is OK!
                       }
                      else
                       {
                      // For all other types there should be a valid name so issue the warning...
                         output->addOutput(new CheckerOutput(*i, decl));
                       }                    
                  }
             }
        }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NameAllParameters::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NameAllParameters::Traversal(params, output);
}

extern const Compass::Checker* const nameAllParametersChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NameAllParameters::checkerName,
        CompassAnalyses::NameAllParameters::shortDescription,
        CompassAnalyses::NameAllParameters::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
