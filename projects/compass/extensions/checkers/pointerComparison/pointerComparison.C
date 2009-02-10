// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Pointer Comparison Analysis
// Author: Chunhua Liao
// Date: 30-October-2008

#include "rose.h"
#include "compass.h"

extern const Compass::Checker* const pointerComparisonChecker;

namespace CompassAnalyses
   { 
     namespace PointerComparison
        { 
        /*! \brief Pointer Comparison: Add your description here 
         */

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
                    Compass::OutputObject* output;
            // Checker specific parameters should be allocated here.

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

CompassAnalyses::PointerComparison::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,::pointerComparisonChecker->checkerName,::pointerComparisonChecker->shortDescription)
   {}

CompassAnalyses::PointerComparison::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PointerComparison.YourParameter"]);


   }

void
CompassAnalyses::PointerComparison::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
    SgBinaryOp* bin_op = isSgBinaryOp(node); 
     if (bin_op)
     {
       if (isSgGreaterThanOp(node)||
          isSgGreaterOrEqualOp(node)||
          isSgLessThanOp(node)||
          isSgLessOrEqualOp(node))
        {
          SgType* lhs_type = bin_op->get_lhs_operand()->get_type(); 
          SgType* rhs_type = bin_op->get_rhs_operand()->get_type(); 
          if (isSgPointerType(lhs_type)||isSgPointerType(rhs_type))
            output->addOutput(new CheckerOutput(bin_op));
        } 
      }// end if bin_op
   } //End of the visit function.

// Checker main run function and metadata

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PointerComparison::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PointerComparison::Traversal(params, output);
}

extern const Compass::Checker* const pointerComparisonChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        "PointerComparison",
     // Descriptions should not include the newline character "\n".
        "Warning: Error-prone pointer comparison using <,<=,>,or >=",
        "Test if relational operations such as <,<=,>,>= are used to compare data of pointer types. Pointer comparisons depend on memory locations of data, which are not consistent across different executions or platforms and can cause bugs which are very difficult to find.",
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
   
