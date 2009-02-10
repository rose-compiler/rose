// Time_t Direct Manipulation
// Author: Gary M. Yuan
// Date: 06-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_TIME_T_DIRECT_MANIPULATION_H
#define COMPASS_TIME_T_DIRECT_MANIPULATION_H

namespace CompassAnalyses
   { 
     namespace Time_tDirectManipulation
        { 
        /*! \brief Time_t Direct Manipulation: Add your description here 
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

// COMPASS_TIME_T_DIRECT_MANIPULATION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Time_t Direct Manipulation Analysis
// Author: Gary M. Yuan
// Date: 06-September-2007

#include "compass.h"
// #include "time_tDirectManipulation.h"

namespace CompassAnalyses
   { 
     namespace Time_tDirectManipulation
        { 
          const std::string checkerName      = "Time_tDirectManipulation";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that type time_t variables are not directly manipulated in arithmetic expressions.";
          const std::string longDescription  = "time_t is specified as an arithmetic type capable of representing times. However, how time is encoded within this arithmetic type is unspecified. Because the encoding is unspecified, there is no safe way to manually perform arithmetic on the type, and, as a result, the values should not be modified directly";
        } //End of namespace Time_tDirectManipulation.
   } //End of namespace CompassAnalyses.

CompassAnalyses::Time_tDirectManipulation::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::Time_tDirectManipulation::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["Time_tDirectManipulation.YourParameter"]);


   }

void
CompassAnalyses::Time_tDirectManipulation::Traversal::
visit(SgNode* node)
   { 
     SgBinaryOp *bop = isSgBinaryOp( node );

     if( bop != NULL )
     {
       SgExpression *lhs = bop->get_lhs_operand_i();
       SgExpression *rhs = bop->get_rhs_operand_i();

       if( isSgAssignOp( bop ) != NULL || 
           isSgAndOp( bop ) != NULL ||
           isSgCommaOpExp( bop ) != NULL ||
           isSgEqualityOp( bop ) != NULL ||
           isSgGreaterOrEqualOp( bop) != NULL ||
           isSgGreaterThanOp( bop ) != NULL ||
           isSgLessOrEqualOp( bop ) != NULL ||
           isSgLessThanOp( bop ) != NULL ||
           isSgNotEqualOp( bop ) != NULL ||
           isSgOrOp( bop ) != NULL ||
           isSgScopeOp( bop ) != NULL ||
           isSgPntrArrRefExp( bop ) != NULL )
       {
         return;
       } //if

       SgNamedType *lhstype = isSgNamedType( lhs->get_type() );
       SgNamedType *rhstype = isSgNamedType( rhs->get_type() );

       SgNamedType *ptr = lhstype != NULL ? lhstype : rhstype;

       if( ptr != NULL )
       {
         if( ptr->get_name().getString() == "time_t" )
           output->addOutput( new CheckerOutput( bop ) );
       } //if( lhstype != NULL && rhstype != NULL )
     } //if( bop != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::Time_tDirectManipulation::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::Time_tDirectManipulation::Traversal(params, output);
}

extern const Compass::Checker* const time_tDirectManipulationChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::Time_tDirectManipulation::checkerName,
        CompassAnalyses::Time_tDirectManipulation::shortDescription,
        CompassAnalyses::Time_tDirectManipulation::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
