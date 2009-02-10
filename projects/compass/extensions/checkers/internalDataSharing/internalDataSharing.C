// Internal Data Sharing
// Author: Gergo Barany
// Date: 31-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_INTERNAL_DATA_SHARING_H
#define COMPASS_INTERNAL_DATA_SHARING_H

namespace CompassAnalyses
   { 
     namespace InternalDataSharing
        { 
        /*! \brief Internal Data Sharing: Add your description here 
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
                 bool withinMemberFunctionReturningPtrOrRef;
                 bool operatorsExcepted;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_INTERNAL_DATA_SHARING_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Internal Data Sharing Analysis
// Author: Gergo Barany
// Date: 31-July-2007

#include "compass.h"
// #include "internalDataSharing.h"

namespace CompassAnalyses
   { 
     namespace InternalDataSharing
        { 
          const std::string checkerName      = "InternalDataSharing";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Member function returns non-const pointer or reference to member";
          const std::string longDescription  = "A member function returns a "
            "non-const pointer or reference to a member, thus making it "
            "possible for the caller to modify internal data.";
        } //End of namespace InternalDataSharing.
   } //End of namespace CompassAnalyses.

CompassAnalyses::InternalDataSharing::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::InternalDataSharing::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output),
     withinMemberFunctionReturningPtrOrRef(false)
   {
     operatorsExcepted = Compass::parseBool(inputParameters["InternalDataSharing.operatorsExcepted"]);
   }

void
CompassAnalyses::InternalDataSharing::Traversal::
visit(SgNode* node)
   { 
     if (SgFunctionDefinition *fd = isSgFunctionDefinition(node))
     {
       withinMemberFunctionReturningPtrOrRef = false;
       if (isSgMemberFunctionDeclaration(fd->get_declaration()))
       {
         // if operators are allowed to be messy and this is an operator, nevermind
         if (operatorsExcepted && fd->get_declaration()->get_specialFunctionModifier().isOperator())
             return;

         SgFunctionType *ft = fd->get_declaration()->get_type();
         SgType *rt = ft->get_return_type();
         if (isSgPointerType(rt) || isSgReferenceType(rt))
         {
           SgModifierType *mt = isSgModifierType(rt->dereference());
           if (!mt || !mt->get_typeModifier().get_constVolatileModifier().isConst())
             withinMemberFunctionReturningPtrOrRef = true;
         }
       }
     }
     else if (SgReturnStmt *ret = isSgReturnStmt(node))
     {
       if (withinMemberFunctionReturningPtrOrRef)
       {
         if (SgArrowExp *ae = isSgArrowExp(ret->get_expression()))
         {
           // expression is of the form this->member
           if (isSgThisExp(ae->get_lhs_operand()))
             output->addOutput(new CheckerOutput(ret));
         }
         else if (SgPointerDerefExp *deref = isSgPointerDerefExp(ret->get_expression()))
         {
           // see if the this is the dereference of a member variable
           if (SgArrowExp *ae = isSgArrowExp(deref->get_operand()))
           {
             // expression is of the form this->member
             if (isSgThisExp(ae->get_lhs_operand()))
               output->addOutput(new CheckerOutput(ret));
           }
         }
       }
     }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::InternalDataSharing::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::InternalDataSharing::Traversal(params, output);
}

extern const Compass::Checker* const internalDataSharingChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::InternalDataSharing::checkerName,
        CompassAnalyses::InternalDataSharing::shortDescription,
        CompassAnalyses::InternalDataSharing::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
