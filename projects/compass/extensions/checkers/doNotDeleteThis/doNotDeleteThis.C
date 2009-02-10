// Do Not Delete This
// Author: Gary M. Yuan
// Date: 14-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DO_NOT_DELETE_THIS_H
#define COMPASS_DO_NOT_DELETE_THIS_H

namespace CompassAnalyses
   { 
     namespace DoNotDeleteThis
        { 
        /*! \brief Do Not Delete This: Add your description here 
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

// COMPASS_DO_NOT_DELETE_THIS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Do Not Delete This Analysis
// Author: Gary M. Yuan
// Date: 14-September-2007

#include "compass.h"
// #include "doNotDeleteThis.h"

namespace CompassAnalyses
   { 
     namespace DoNotDeleteThis
        { 
          const std::string checkerName      = "DoNotDeleteThis";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that 'this' is not the argument to 'delete'";
          const std::string longDescription  = "Deleting this leaves it as a \"dangling\" pointer, which leads to undefined behavior if it is accessed.";
        } //End of namespace DoNotDeleteThis.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DoNotDeleteThis::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DoNotDeleteThis::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DoNotDeleteThis.YourParameter"]);


   }

void
CompassAnalyses::DoNotDeleteThis::Traversal::
visit(SgNode* node)
   {
     SgDeleteExp *d = isSgDeleteExp( node );

     if( d != NULL )
     {
       SgThisExp *t = isSgThisExp( d->get_variable() );

       if( t != NULL )
         output->addOutput( new CheckerOutput( d ) );
     } //if( d != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DoNotDeleteThis::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DoNotDeleteThis::Traversal(params, output);
}

extern const Compass::Checker* const doNotDeleteThisChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DoNotDeleteThis::checkerName,
        CompassAnalyses::DoNotDeleteThis::shortDescription,
        CompassAnalyses::DoNotDeleteThis::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
