// Non Associative Relational Operators
// Author: Gary Yuan
// Date: 02-January-2008

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H
#define COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H

namespace CompassAnalyses
   { 
     namespace NonAssociativeRelationalOperators
        { 
        /*! \brief Non Associative Relational Operators: Add your description here 
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

          bool isRelationalOperator( SgNode *n );

          class ExpressionTraversal : public AstSimpleProcessing
          {
            private:
              int count;
            public:
              ExpressionTraversal();
              int run(SgNode *n);
              void visit(SgNode *n);
          }; //class ExpressionTraversal

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
               private:
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

// COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Non Associative Relational Operators Analysis
// Author: Gary Yuan
// Date: 02-January-2008

#include "compass.h"
// #include "nonAssociativeRelationalOperators.h"

namespace CompassAnalyses
   { 
     namespace NonAssociativeRelationalOperators
        { 
          const std::string checkerName      = "NonAssociativeRelationalOperators";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that relational binary operators (==,!=,<,>,<=,>=) are not treated as if they were non-associative.";
          const std::string longDescription  = "This checker checks that relational binary operators (==,!=,<,>,<=,>=) are not treated as if they were non-associative. The relational and equality operators are left-associative, not non-associative as they often are in other languages. This allows a C++ programmer to write an expression (particularly an expression used as a condition) that can be easily misinterpreted.";
        } //End of namespace NonAssociativeRelationalOperators.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NonAssociativeRelationalOperators::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NonAssociativeRelationalOperators::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NonAssociativeRelationalOperators.YourParameter"]);


   }

bool
CompassAnalyses::NonAssociativeRelationalOperators::
isRelationalOperator( SgNode *node )
{
  switch(node->variantT())
  {
    case V_SgEqualityOp:
    case V_SgNotEqualOp:
    case V_SgLessThanOp:
    case V_SgLessOrEqualOp:
    case V_SgGreaterThanOp:
    case V_SgGreaterOrEqualOp:
      return true;
    default: return false;
  } //switch(node->variantT()) 
} //isRelationalOperator( SgNode *node )

CompassAnalyses::NonAssociativeRelationalOperators::ExpressionTraversal::
ExpressionTraversal() : count(0)
{
} //ExpressionTraversal::ExpressionTraversal()

int CompassAnalyses::NonAssociativeRelationalOperators::ExpressionTraversal::
run(SgNode *n)
{
  this->traverse(n,preorder);
  return this->count;
} //ExpressionTraversal::run(SgNode *n)

void CompassAnalyses::NonAssociativeRelationalOperators::ExpressionTraversal::
visit(SgNode *node)
{
  if( isRelationalOperator(node) )
    this->count++;

  return;
} //ExpressionTraversal::visit(SgNode *node)

void
CompassAnalyses::NonAssociativeRelationalOperators::Traversal::
visit(SgNode* node)
   {
     SgBinaryOp *relOperator = isSgBinaryOp(node);

     if( relOperator != NULL && 
         isRelationalOperator(relOperator) )
     {
       SgExpression *lhs = relOperator->get_lhs_operand();
       SgExpression *rhs = relOperator->get_rhs_operand();

       if( lhs != NULL && rhs != NULL )
       {
         CompassAnalyses::NonAssociativeRelationalOperators::ExpressionTraversal expressionTraversal;

         if( expressionTraversal.run(lhs->get_parent()) > 1 ) 
         {
           output->addOutput( new CheckerOutput(relOperator) );
         } //if( expressionTraversal.run(lhs->get_parent()) > 1 ) 

       } //if( lhs != NULL && rhs != NULL )
     } //if( relOperator != NULL && isRelationalOperator(node) )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NonAssociativeRelationalOperators::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NonAssociativeRelationalOperators::Traversal(params, output);
}

extern const Compass::Checker* const nonAssociativeRelationalOperatorsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NonAssociativeRelationalOperators::checkerName,
        CompassAnalyses::NonAssociativeRelationalOperators::shortDescription,
        CompassAnalyses::NonAssociativeRelationalOperators::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
