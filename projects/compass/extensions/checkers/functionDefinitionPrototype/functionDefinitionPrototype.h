// Function Definition Prototype
// Author: Gary M. Yuan
// Date: 23-July-2007

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
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

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
