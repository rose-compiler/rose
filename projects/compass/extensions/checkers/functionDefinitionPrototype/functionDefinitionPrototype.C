// Function Definition Prototype
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FUNCTION_DEFINITION_PROTOTYPE_H
#define COMPASS_FUNCTION_DEFINITION_PROTOTYPE_H

namespace CompassAnalyses
   { 
     namespace FunctionDefinitionPrototype
        { 
          /// \brief checkerName is a std::string containing the name of this 
          /// checker.
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's pattern.
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed 
          /// description of this checker's pattern and purpose.
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////
          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput : public Compass::OutputViolationBase
             { 
               private:
                 const char *what;
               public:
                 CheckerOutput(SgNode* node, const char *w);

              // DQ (1/16/2008): I don't think this is needed since it is defined 
              // well enough in the base class: Compass::OutputViolationBase
              // std::string getString() const;
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class implements a simple AST traversal seeking out
          /// SgDeclarationStatement instances that match function declarations.
          /// Matching declarations are checked if they have a non-defining
          /// declaration, i.e. a function prototype. Functions without
          /// prototypes are flagged as violations.
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    /// \brief The constructor
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// \brief run, starts AST traversal
                    void run(SgNode* n){ this->traverse(n, preorder); };
                    /// \brief visit, pattern for AST traversal
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FUNCTION_DEFINITION_PROTOTYPE_H
#endif 
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Definition Prototype Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
// #include "functionDefinitionPrototype.h"

namespace CompassAnalyses
   { 
     namespace FunctionDefinitionPrototype
        { 
          const std::string checkerName      = "FunctionDefinitionPrototype";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "matching function prototype not available";
          const std::string longDescription  = "This checker checks that function definitions have matching function prototypes";
        } //End of namespace FunctionDefinitionPrototype.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FunctionDefinitionPrototype::
CheckerOutput::CheckerOutput ( SgNode* node, const char *w )
   : OutputViolationBase(node,checkerName,shortDescription), what(w)
   {
   }

#if 0
// This function is not requied since the base class function is available and is preferred (for conformity).
std::string
CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput::getString() const
{
     ROSE_ASSERT(getNodeArray().size() <= 1);

  // Default implementation for getString
     SgLocatedNode* locatedNode = isSgLocatedNode(getNode());
     std::string sourceCodeLocation;
     if (locatedNode != NULL)
        {
          Sg_File_Info* start = locatedNode->get_startOfConstruct();
          Sg_File_Info* end   = locatedNode->get_endOfConstruct();
          sourceCodeLocation = (end ? Compass::formatStandardSourcePosition(start, end) 
                                    : Compass::formatStandardSourcePosition(start));
       }
      else
       {
      // Else this could be a SgInitializedName or SgTemplateArgument (not yet moved to be a SgLocatedNode)
         Sg_File_Info* start = getNode()->get_file_info();
         ROSE_ASSERT(start != NULL);
         sourceCodeLocation = Compass::formatStandardSourcePosition(start);
       }

     std::string nodeName = getNode()->class_name();

  // The short description used here needs to be put into a separate function (can this be part of what is filled in by the script?)
  // return loc + ": " + nodeName + ": variable requiring static constructor initialization";

     return m_checkerName + ": " + sourceCodeLocation + ": " + nodeName + ": " + m_shortDescription + ".\n\"" + what + "\" does not have a prototype";
} //CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput::getString()
#endif

CompassAnalyses::FunctionDefinitionPrototype::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionDefinitionPrototype.YourParameter"]);


   }

void
CompassAnalyses::FunctionDefinitionPrototype::Traversal::
visit(SgNode* node)
   { 
     SgDeclarationStatement *sgdecl = isSgDeclarationStatement(node);

     if( sgdecl != NULL )
     {
       SgFunctionDeclaration *fd = isSgFunctionDeclaration(node);

       if( fd != NULL )
       {
         if( sgdecl->get_firstNondefiningDeclaration() == NULL )
         {
           output->addOutput( new CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput( node, fd->get_name().getString().c_str() ) );
         }
       } //if( fd != NULL )
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FunctionDefinitionPrototype::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FunctionDefinitionPrototype::Traversal(params, output);
}

extern const Compass::Checker* const functionDefinitionPrototypeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FunctionDefinitionPrototype::checkerName,
        CompassAnalyses::FunctionDefinitionPrototype::shortDescription,
        CompassAnalyses::FunctionDefinitionPrototype::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
