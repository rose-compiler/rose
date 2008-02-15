// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Magic Number Analysis
// Author: Gergo Barany
// Date: 19-July-2007

#include "compass.h"
#include "magicNumber.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
