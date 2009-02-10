// Const String Literals
// Author: Gergo  Barany
// Date: 07-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_CONST_STRING_LITERALS_H
#define COMPASS_CONST_STRING_LITERALS_H

namespace CompassAnalyses
   { 
     namespace ConstStringLiterals
        { 
        /*! \brief Const String Literals: Add your description here 
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

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_CONST_STRING_LITERALS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Const String Literals Analysis
// Author: Gergo Barany
// Date: 07-September-2007

#include "compass.h"
// #include "constStringLiterals.h"

namespace CompassAnalyses
   { 
     namespace ConstStringLiterals
        { 
          const std::string checkerName      = "ConstStringLiterals";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "String literals should be treated as const char *";
          const std::string longDescription  = "String literals should be treated as const char *";
        } //End of namespace ConstStringLiterals.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ConstStringLiterals::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ConstStringLiterals::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ConstStringLiterals.YourParameter"]);


   }

void
CompassAnalyses::ConstStringLiterals::Traversal::
visit(SgNode* node)
   { 
     SgStringVal *str = isSgStringVal(node);
     if (str)
     {
       SgNode *parent = str->get_parent();
       SgCastExp *cast = isSgCastExp(parent);
       if (cast && !isSgCastExp(cast->get_parent()))
         output->addOutput(new CheckerOutput(cast->get_parent()));
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ConstStringLiterals::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ConstStringLiterals::Traversal(params, output);
}

extern const Compass::Checker* const constStringLiteralsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ConstStringLiterals::checkerName,
        CompassAnalyses::ConstStringLiterals::shortDescription,
        CompassAnalyses::ConstStringLiterals::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
