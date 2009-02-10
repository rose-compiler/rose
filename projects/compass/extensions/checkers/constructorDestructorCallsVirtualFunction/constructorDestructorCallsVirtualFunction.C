// Constructor Destructor Calls Virtual Function
// Author: Gary Yuan
// Date: 20-December-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_CONSTRUCTOR_DESTRUCTOR_CALLS_VIRTUAL_FUNCTION_H
#define COMPASS_CONSTRUCTOR_DESTRUCTOR_CALLS_VIRTUAL_FUNCTION_H

namespace CompassAnalyses
   { 
     namespace ConstructorDestructorCallsVirtualFunction
        { 
          SgMemberFunctionRefExp* isMemberFunctionRefExp(SgExpression *exp);
          SgFunctionRefExp* isFunctionRefExp(SgExpression *exp);

          class NestedTraversal : public AstSimpleProcessing
          {
            private:
              Compass::OutputObject *output;
            public:
              NestedTraversal( Compass::OutputObject *out ) : output(out){} 
              void visit( SgNode *n );
          }; //class NestedTraversal

        /*! \brief Constructor Destructor Calls Virtual Function: Add your description here 
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

// COMPASS_CONSTRUCTOR_DESTRUCTOR_CALLS_VIRTUAL_FUNCTION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Constructor Destructor Calls Virtual Function Analysis
// Author: Gary Yuan
// Date: 20-December-2007

#include "compass.h"
// #include "constructorDestructorCallsVirtualFunction.h"

namespace CompassAnalyses
   { 
     namespace ConstructorDestructorCallsVirtualFunction
        { 
          const std::string checkerName      = "ConstructorDestructorCallsVirtualFunction";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks if a call to a virtual function occurs in either class constructors or destructors";
          const std::string longDescription  = "This checker checks if a call to a virtual function occurs in either class constructors or destructors";
        } //End of namespace ConstructorDestructorCallsVirtualFunction.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ConstructorDestructorCallsVirtualFunction::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ConstructorDestructorCallsVirtualFunction.YourParameter"]);


   }

SgMemberFunctionRefExp* CompassAnalyses::ConstructorDestructorCallsVirtualFunction::isMemberFunctionRefExp(SgExpression *exp)
{
  return isSgMemberFunctionRefExp(exp);
}

SgFunctionRefExp* CompassAnalyses::ConstructorDestructorCallsVirtualFunction::isFunctionRefExp(SgExpression *exp)
{
  return isSgFunctionRefExp(exp);
}

void CompassAnalyses::ConstructorDestructorCallsVirtualFunction::NestedTraversal::visit(SgNode *node)
{
  SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
  SgMemberFunctionRefExp *fmem = NULL;
  SgFunctionRefExp *fref = NULL;

  if( fcall != NULL )
  {
    SgExpression *exp = fcall->get_function();

    if( isSgBinaryOp(exp) != NULL )
      exp = isSgBinaryOp(exp)->get_rhs_operand();

    fmem = CompassAnalyses::ConstructorDestructorCallsVirtualFunction::isMemberFunctionRefExp(exp);
    fref =  CompassAnalyses::ConstructorDestructorCallsVirtualFunction::isFunctionRefExp(exp);

    if( (fmem != NULL && fmem->get_symbol()->get_declaration()->get_functionModifier().isVirtual()) || (fref != NULL && fref->get_symbol()->get_declaration()->get_functionModifier().isVirtual()) )
    {
      output->addOutput( new CheckerOutput(fmem) );
    } //if( fmem != NULL )

  } //if( fcall != NULL )

  return;
}

void
CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal::
visit(SgNode* node)
   {
     SgFunctionDefinition *fdef = isSgFunctionDefinition(node);

     if(fdef != NULL)
     {
       SgFunctionDeclaration *fdecl = fdef->get_declaration();

       if( fdecl->get_specialFunctionModifier().isConstructor() ||
           fdecl->get_specialFunctionModifier().isDestructor() )
       {
         CompassAnalyses::ConstructorDestructorCallsVirtualFunction::NestedTraversal nest(output);
         nest.traverse(fdef, postorder);
       } //if
     } //if
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal(params, output);
}

extern const Compass::Checker* const constructorDestructorCallsVirtualFunctionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ConstructorDestructorCallsVirtualFunction::checkerName,
        CompassAnalyses::ConstructorDestructorCallsVirtualFunction::shortDescription,
        CompassAnalyses::ConstructorDestructorCallsVirtualFunction::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
