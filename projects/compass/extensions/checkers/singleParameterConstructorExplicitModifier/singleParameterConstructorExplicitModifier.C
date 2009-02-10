// Single Parameter Constructor Explicit Modifier
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_SINGLE_PARAMETER_CONSTRUCTOR_EXPLICIT_MODIFIER_H
#define COMPASS_SINGLE_PARAMETER_CONSTRUCTOR_EXPLICIT_MODIFIER_H

namespace CompassAnalyses
   { 
     namespace SingleParameterConstructorExplicitModifier
        { 
          /// \brief checkerName is a std::string containing the name of this
          /// checker.
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's pattern
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed
          /// description of this checker's pattern and purpose.
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               const char *what;

               public:
                 /// The constructor
                 /// \param node is a SgNode*
                 CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class performs a simple AST traversal seeking out
          /// single parameter constructors that are not modified with the
          /// explicit keyword; this is flagged as a violation. 
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    /// The constructor
                    /// \param out is a Compass::OutputObject*
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    /// visit function
                    /// \param n is a SgNode
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_SINGLE_PARAMETER_CONSTRUCTOR_EXPLICIT_MODIFIER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Single Parameter Constructor Explicit Modifier Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
// #include "singleParameterConstructorExplicitModifier.h"

namespace CompassAnalyses
   { 
     namespace SingleParameterConstructorExplicitModifier
        { 
          const std::string checkerName      = "SingleParameterConstructorExplicitModifier";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that single paramater constructors are declared using the explicit keyword.";
          const std::string longDescription  = "Declare single parameter constructors as explicit to avoid unexpected type conversion. A compiler can use a single paramter constructor for type conversions. While this is natural in some situations; it may be unexpected in others.";
        } //End of namespace SingleParameterConstructorExplicitModifier.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SingleParameterConstructorExplicitModifier::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SingleParameterConstructorExplicitModifier.YourParameter"]);


   }

void
CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal::
visit(SgNode* node)
   { 
     SgFunctionDeclaration *fd = isSgFunctionDeclaration(node);

     if( fd != NULL )
     {
       SgSpecialFunctionModifier sfm = fd->get_specialFunctionModifier();

       if( sfm.isConstructor() == true )
       {
         int argc = fd->get_parameterList()->get_args().size();

         if( argc == 1 )
         {
           if( fd->get_functionModifier().isExplicit() == false )
           {
             output->addOutput( new CheckerOutput( node ) );
           } //if( fd->get_functionModifier().isExplicit() == false )
         } //if( argc == 1 )
       } //if( sfm.isConstructor() == true )
     } //if( fd != NULL )

     return;
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal(params, output);
}

extern const Compass::Checker* const singleParameterConstructorExplicitModifierChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::SingleParameterConstructorExplicitModifier::checkerName,
        CompassAnalyses::SingleParameterConstructorExplicitModifier::shortDescription,
        CompassAnalyses::SingleParameterConstructorExplicitModifier::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
