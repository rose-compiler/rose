// Boolean Is Has
// Author: pants,,,
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_BOOLEAN_IS_HAS_H
#define COMPASS_BOOLEAN_IS_HAS_H

namespace CompassAnalyses
   { 
     namespace BooleanIsHas
        { 
        /*! \brief Boolean Is Has: Add your description here 
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

// COMPASS_BOOLEAN_IS_HAS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Boolean Is Has Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
// #include "booleanIsHas.h"

namespace CompassAnalyses
   { 
     namespace BooleanIsHas
        { 
          const std::string checkerName      = "BooleanIsHas";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding boolean variables or boolean returning functions that do not begin with 'is_' or 'has_'";
          const std::string longDescription  = "Finds all boolean variables or boolean returning functions whose name does not begin with 'is_' or 'has_'";
        } //End of namespace BooleanIsHas.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BooleanIsHas::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::BooleanIsHas::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BooleanIsHas.YourParameter"]);


   }

void
CompassAnalyses::BooleanIsHas::Traversal::
visit(SgNode* node)
   {
        SgFunctionDeclaration* fn = isSgFunctionDeclaration(node);
        SgVariableDeclaration* var = isSgVariableDeclaration(node);
        if (!fn and !var) return;
        SgType *t;
        std::string thingName;
        if (fn)
          {
#if ROSE_GCC_OMP
#pragma omp critical (BooleanIsHasvisit)
#endif
            thingName = fn->get_name().str();
            SgFunctionType* s = fn->get_type();
            t = s->get_return_type();
          }
        else
          { 
            SgInitializedNamePtrList vars =  var->get_variables();

            if (!vars.empty())
              {
                //I cut out the iterator loop because it seemed to
                //never be used.  Yell at me if this is wrong... why
                //else would you return a list?
                SgInitializedNamePtrList::iterator j = vars.begin();
                SgInitializedName* initName = isSgInitializedName (*j);
                if(!initName) {return;}
                t = initName->get_type();
#if ROSE_GCC_OMP
#pragma omp critical (BooleanIsHasthingname)
#endif
                thingName  = initName->get_qualified_name().str();
              }
          }
        if (isSgTypeBool(t) and not (thingName.rfind("is_", 0) != std::string::npos or thingName.rfind("has_", 0) != std::string::npos))
          {
            output->addOutput(new CheckerOutput(node));
          }
   }
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::BooleanIsHas::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::BooleanIsHas::Traversal(params, output);
}

extern const Compass::Checker* const booleanIsHasChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::BooleanIsHas::checkerName,
        CompassAnalyses::BooleanIsHas::shortDescription,
        CompassAnalyses::BooleanIsHas::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
