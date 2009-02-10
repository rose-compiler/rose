// Fopen Format Parameter
// Author: Gary M. Yuan
// Date: 06-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FOPEN_FORMAT_PARAMETER_H
#define COMPASS_FOPEN_FORMAT_PARAMETER_H

namespace CompassAnalyses
   { 
     namespace FopenFormatParameter
        { 
        /*! \brief Fopen Format Parameter: Add your description here 
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

// COMPASS_FOPEN_FORMAT_PARAMETER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Fopen Format Parameter Analysis
// Author: Gary M. Yuan
// Date: 06-September-2007

#include "compass.h"
// #include "fopenFormatParameter.h"

namespace CompassAnalyses
   { 
     namespace FopenFormatParameter
        { 
          const std::string checkerName      = "FopenFormatParameter";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that only C standard specified strings are used in the mode argument to fopen().";
          const std::string longDescription  = "The C standard specifies specific strings to use for the mode for the function fopen(). An implementation may define extra strings that define additional modes, but only the modes in the following table (adapted from the C99 standard) are fully portable and C99 compliant";
        } //End of namespace FopenFormatParameter.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FopenFormatParameter::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FopenFormatParameter::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FopenFormatParameter.YourParameter"]);


   }

void
CompassAnalyses::FopenFormatParameter::Traversal::
visit(SgNode* node)
   {
     SgFunctionCallExp *call = isSgFunctionCallExp( node );

     if( call != NULL )
     {
       SgFunctionRefExp *fref = isSgFunctionRefExp( call->get_function() );

       if( fref != NULL )
       {
         std::string fname( fref->get_symbol()->get_name().getString() );

         if( fname == "fopen" )
         {
           const SgExpressionPtrList arguments = call->get_args()->get_expressions();

           for( SgExpressionPtrList::const_iterator itr = arguments.begin();
                itr != arguments.end(); itr++ )
           {
             SgStringVal *format = isSgStringVal( *itr );

             if( format != NULL )
             {
               std::string mode( format->get_value() );

               if( mode != "r" && mode != "w" && mode != "a" &&
                   mode != "rb" && mode != "wb" && mode != "ab" &&
                   mode != "r+" && mode != "w+" && mode != "a+" &&
                   mode != "r+b" && mode != "rb+" &&
                   mode != "w+b" && mode != "wb+" &&
                   mode != "a+b" && mode != "ab+" )
               {
                 output->addOutput( new CheckerOutput( node ) );
               } //if, mode

               break;
             } //if( format != NULL )
           } //for, itr
         } //if( fname == "fopen" )
       } //if( fref != NULL )
     } //if( call != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FopenFormatParameter::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FopenFormatParameter::Traversal(params, output);
}

extern const Compass::Checker* const fopenFormatParameterChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FopenFormatParameter::checkerName,
        CompassAnalyses::FopenFormatParameter::shortDescription,
        CompassAnalyses::FopenFormatParameter::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
