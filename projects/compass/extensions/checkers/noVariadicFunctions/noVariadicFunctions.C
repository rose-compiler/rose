// No Variadic Functions
// Author: Gary M. Yuan
// Date: 15-July-2008

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_VARIADIC_FUNCTIONS_H
#define COMPASS_NO_VARIADIC_FUNCTIONS_H

namespace CompassAnalyses
   { 
     namespace NoVariadicFunctions
        { 
        /*! \brief No Variadic Functions: Add your description here 
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

                    void functionDeclarationHandler(
                      const SgFunctionDeclaration *fdecl, SgNode *node );

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

// COMPASS_NO_VARIADIC_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Variadic Functions Analysis
// Author: Gary M. Yuan
// Date: 15-July-2008

#include "compass.h"
// #include "noVariadicFunctions.h"

namespace CompassAnalyses
   { 
     namespace NoVariadicFunctions
        { 
          const std::string checkerName      = "NoVariadicFunctions";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker forbids the definition of variadic functions.";
          const std::string longDescription  = "This checker forbids the definition of variadic functions. A variadic function – a function declared with a parameter list ending with ellipsis (...) – can accept a varying number of arguments of differing types. Variadic functions are flexible, but they are also hazardous. The compiler can't verify that a given call to a variadic function passes an appropriate number of arguments or that those arguments have appropriate types. Consequently, a runtime call to a variadic function that passes inappropriate arguments yields undefined behavior. Such undefined behavior could be exploited to run arbitrary code.";
        } //End of namespace NoVariadicFunctions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoVariadicFunctions::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoVariadicFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoVariadicFunctions.YourParameter"]);


   }

void
CompassAnalyses::NoVariadicFunctions::Traversal::
functionDeclarationHandler( const SgFunctionDeclaration *fdecl, SgNode *node )
{
  ROSE_ASSERT( fdecl != NULL );

  if( fdecl->get_definition() != NULL )
  {
    const SgInitializedNamePtrList &arguments = fdecl->get_args();

    for( SgInitializedNamePtrList::const_iterator itr = arguments.begin();
         itr != arguments.end(); itr++ )
    {
      SgType *type = (*itr)->get_type();

      if( isSgTypeEllipse(type) != NULL )
      {
        output->addOutput( new CheckerOutput(node) );
      } //if( isSgTypeEllipse(type) != NULL )
    } //for, itr
  } //if( fdecl->get_definition() != NULL )

  return;
}

void
CompassAnalyses::NoVariadicFunctions::Traversal::
visit(SgNode* node)
   { 
     switch( node->variantT() )
     {
       case V_SgFunctionRefExp:
       {
         SgFunctionRefExp *fref = isSgFunctionRefExp(node);

         ROSE_ASSERT(fref != NULL);

         SgFunctionDeclaration *fdecl = fref->get_symbol()->get_declaration();

         this->functionDeclarationHandler(
           fdecl, fref );
       } break; //case V_SgFunctionRefExp
       case V_SgMemberFunctionRefExp:
       {
         SgMemberFunctionRefExp *fref = isSgMemberFunctionRefExp(node);

         ROSE_ASSERT(fref != NULL);

         SgFunctionDeclaration *fdecl = fref->get_symbol()->get_declaration();

         this->functionDeclarationHandler(
           fdecl, fref );
       } break; //case V_SgMemberFunctionRefExp
       default: break;
     } //switch( node->variantT() )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoVariadicFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoVariadicFunctions::Traversal(params, output);
}

extern const Compass::Checker* const noVariadicFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoVariadicFunctions::checkerName,
        CompassAnalyses::NoVariadicFunctions::shortDescription,
        CompassAnalyses::NoVariadicFunctions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
