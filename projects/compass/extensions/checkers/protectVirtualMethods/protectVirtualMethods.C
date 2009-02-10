// Protect Virtual Methods
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_PROTECT_VIRTUAL_METHODS_H
#define COMPASS_PROTECT_VIRTUAL_METHODS_H

namespace CompassAnalyses
   { 
     namespace ProtectVirtualMethods
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
               public:
                 /// The constructor
                 /// \param node is a SgNode*
                 CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class performs a simple AST traversal seeking out
          /// virtual member functions that are declared with public access.
          /// These are flagged as violations and are suggested to be protected
          /// with public accessor methods instead.
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

// COMPASS_PROTECT_VIRTUAL_METHODS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Protect Virtual Methods Analysis
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "compass.h"
// #include "protectVirtualMethods.h"

namespace CompassAnalyses
   { 
     namespace ProtectVirtualMethods
        { 
          const std::string checkerName      = "ProtectVirtualMethods";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that virtual member function are not public. Virtual member functions should be protected with public member accessor functions.";
          const std::string longDescription  = "Declare virtual member functions as protected and call them from public accessor member functions. This ensures that the virtual member function is not exposed to the public interface of the class.";
        } //End of namespace ProtectVirtualMethods.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ProtectVirtualMethods::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ProtectVirtualMethods::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ProtectVirtualMethods.YourParameter"]);


   }

void
CompassAnalyses::ProtectVirtualMethods::Traversal::
visit(SgNode* node)
   { 
     SgMemberFunctionDeclaration *sgmf = isSgMemberFunctionDeclaration(node);

     if( sgmf != NULL )
     {
       SgFunctionModifier sgfm = sgmf->get_functionModifier();
       SgAccessModifier sgam = 
         sgmf->get_declarationModifier().get_accessModifier();

       if( sgfm.isVirtual() == true && sgam.isPublic() == true )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if( sgfm.isVirtual() == true && sgam.isPublic() == true )
     } //if( sgmf != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ProtectVirtualMethods::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ProtectVirtualMethods::Traversal(params, output);
}

extern const Compass::Checker* const protectVirtualMethodsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ProtectVirtualMethods::checkerName,
        CompassAnalyses::ProtectVirtualMethods::shortDescription,
        CompassAnalyses::ProtectVirtualMethods::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
