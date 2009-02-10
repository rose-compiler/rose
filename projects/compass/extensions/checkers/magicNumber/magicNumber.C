// Magic Number
// Author: Gergo Barany
// Date: 19-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_MAGIC_NUMBER_H
#define COMPASS_MAGIC_NUMBER_H

namespace CompassAnalyses
   { 
     namespace MagicNumber
        { 
        /*! \brief Magic Number: Checks for integer or floating point
         *         literals occurring outside of initializer expressions. The
         *         user may choose to ignore some such constants (0 and 1 are
         *         likely candidates), this can be configured via the
         *         parameters.
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
                 std::vector<int> allowedIntegers;
                 std::vector<double> allowedFloats;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_MAGIC_NUMBER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Magic Number Analysis
// Author: Gergo Barany
// Date: 19-July-2007

#include "compass.h"
// #include "magicNumber.h"

namespace CompassAnalyses
   { 
     namespace MagicNumber
        { 
          const std::string checkerName      = "MagicNumber";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Occurrence of integer or floating constant.";
          const std::string longDescription  = "Occurrence of an integer or "
            "floating-point constant outside of an initializer expression. "
            "Permitted constants can be defined in the parameters.";
        } //End of namespace MagicNumber.
   } //End of namespace CompassAnalyses.

CompassAnalyses::MagicNumber::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::MagicNumber::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
     allowedIntegers = Compass::parseIntegerList(inputParameters["MagicNumber.allowedIntegers"]);
     allowedFloats = Compass::parseDoubleList(inputParameters["MagicNumber.allowedFloats"]);
   }

void
CompassAnalyses::MagicNumber::Traversal::
visit(SgNode* n)
   { 
      if (SgValueExp* val = isSgValueExp(n))
      {
          // Only care about value expressions where the original expression
          // tree is NULL, i.e. where the expression is an actual constant
          // in the source rather than something that was constant folded.
        if (val->get_originalExpressionTree() == NULL)
        {
          // Follow parent pointers to find out whether we are inside an
          // initializer statement.
          SgNode *p = val->get_parent();
          while (isSgExpression(p) && !isSgInitializer(p))
            p = p->get_parent();
          // Only traverse non-initializer expressions and constructor
          // initializer expressions, i.e. ignore assign or aggregate
          // initializers.
          if (!isSgInitializer(p) || isSgConstructorInitializer(p))
          {
            if (val->get_type()->isIntegerType())
            {
              // Extract the value from the expression. This can either be done
              // by trying to downcast to one of the many possible types, or by
              // doing a brittle string-based value comparison. Guess which
              // one I chose.
              std::string valstr = val->unparseToString();
              long longval = std::strtol(valstr.c_str(), NULL, 10);
              if (std::find(allowedIntegers.begin(), allowedIntegers.end(), longval)
                    == allowedIntegers.end())
                output->addOutput(new CheckerOutput(val));
            }
            else if (val->get_type()->isFloatType())
            {
              // Extract float value.
              std::string valstr = val->unparseToString();
              double doubleval = std::strtod(valstr.c_str(), NULL);
              if (std::find(allowedFloats.begin(), allowedFloats.end(), doubleval)
                    == allowedFloats.end())
                output->addOutput(new CheckerOutput(val));
            }
          }
        }
      }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::MagicNumber::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::MagicNumber::Traversal(params, output);
}

extern const Compass::Checker* const magicNumberChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::MagicNumber::checkerName,
        CompassAnalyses::MagicNumber::shortDescription,
        CompassAnalyses::MagicNumber::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
