// Explicit Char Sign
// Author: Gary M. Yuan
// Date: 24-August-2007


#include "rose.h"
#include "compass.h"

#ifndef COMPASS_EXPLICIT_CHAR_SIGN_H
#define COMPASS_EXPLICIT_CHAR_SIGN_H

namespace CompassAnalyses
   { 
     namespace ExplicitCharSign
        { 
        /*! \brief Explicit Char Sign: Add your description here 
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

// COMPASS_EXPLICIT_CHAR_SIGN_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Explicit Char Sign Analysis
// Author: Gary M. Yuan
// Date: 1-December-2007

#include "compass.h"
// #include "explicitCharSign.h"

namespace CompassAnalyses
   { 
     namespace ExplicitCharSign
        { 
          const std::string checkerName      = "ExplicitCharSign";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that all char declarations initialized with type int are explitly signed or unsigned.";
          const std::string longDescription  = "The three types char, signed char, and unsigned char are collectively called the character types. Compilers have the latitude to define char to have the same range, representation, and behavior as either signed char or unsigned char. Irrespective of the choice made, char is a separate type from the other two and is not compatible with either.";
        } //End of namespace ExplicitCharSign.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ExplicitCharSign::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ExplicitCharSign::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ExplicitCharSign.YourParameter"]);


   }

void
CompassAnalyses::ExplicitCharSign::Traversal::
visit(SgNode* node)
   {
     SgAssignInitializer *sgAssign = isSgAssignInitializer(node);

     if( sgAssign && isSgTypeChar(sgAssign->get_type()) )
     {
       SgCastExp *sgCast = isSgCastExp(sgAssign->get_operand());
       SgCharVal *sgChar = isSgCharVal(sgAssign->get_operand());

       if( (sgCast && isSgTypeInt(sgCast->get_operand()->get_type())) ||
           (sgChar && (int)sgChar->get_value() < 0) )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if
     } //if( sgAssign && isSgTypeChar(sgAssign->get_type()) )

     return;
   } //End of the visit function. 

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ExplicitCharSign::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ExplicitCharSign::Traversal(params, output);
}

extern const Compass::Checker* const explicitCharSignChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ExplicitCharSign::checkerName,
        CompassAnalyses::ExplicitCharSign::shortDescription,
        CompassAnalyses::ExplicitCharSign::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
