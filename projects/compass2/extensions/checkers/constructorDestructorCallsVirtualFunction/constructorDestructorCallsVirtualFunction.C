// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Constructor Destructor Calls Virtual Function Analysis
// Author: Gary Yuan
// Date: 20-December-2007

#include "compass.h"
#include "constructorDestructorCallsVirtualFunction.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
