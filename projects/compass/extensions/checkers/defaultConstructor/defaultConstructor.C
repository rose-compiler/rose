// Default Constructor
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DEFAULT_CONSTRUCTOR_H
#define COMPASS_DEFAULT_CONSTRUCTOR_H

namespace CompassAnalyses
   { 
     namespace DefaultConstructor
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
          /////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                 /// The constructor
                 /// \param node is a SgNode
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class implements a simple AST traversal seeking out
          /// SgClassDefinition nodes. The children of any such nodes are
          /// stored in a successor container and are looped over to find
          /// a default constructor public member function. If no default
          /// constructor is found then a violation is flagged.
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

// COMPASS_DEFAULT_CONSTRUCTOR_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Default Constructor Analysis
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "compass.h"
// #include "defaultConstructor.h"

namespace CompassAnalyses
   { 
     namespace DefaultConstructor
        { 
          const std::string checkerName      = "DefaultConstructor";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Class does not have a user-declared default constructor.";
          const std::string longDescription  = "Declare a default constructor for every class you create. Although some compilers may generate a more efficient implementation in some instances you should choose a default constructor because it is more clear.";
        } //End of namespace DefaultConstructor.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DefaultConstructor::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DefaultConstructor::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DefaultConstructor.YourParameter"]);


   }

void
CompassAnalyses::DefaultConstructor::Traversal::
visit(SgNode* node)
   { 
     SgClassDefinition *sgcd = isSgClassDefinition(node);

     if( sgcd != NULL )
     {
       bool foundDefaultConstructor = false;
       const std::vector<SgNode*> children = 
         sgcd->get_traversalSuccessorContainer();

       for( std::vector<SgNode*>::const_iterator itr = children.begin(); 
            itr != children.end(); itr++ )
       {
         SgMemberFunctionDeclaration *sgmf = 
           isSgMemberFunctionDeclaration( *itr );

         if( sgmf != NULL )
         {
           if( sgmf->get_specialFunctionModifier().isConstructor() == true && 
               sgmf->get_functionModifier().isDefault() == true )
           {
             foundDefaultConstructor = true;
             break;
           } //sgmf->get_specialFunctionModifier().isConstructor() == true ... 
         } //if( sgmf != NULL )
       } //for, itr

       if( foundDefaultConstructor == false )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if( foundDefaultConstructor == false )
     } //if( sgcd != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DefaultConstructor::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DefaultConstructor::Traversal(params, output);
}

extern const Compass::Checker* const defaultConstructorChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DefaultConstructor::checkerName,
        CompassAnalyses::DefaultConstructor::shortDescription,
        CompassAnalyses::DefaultConstructor::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
