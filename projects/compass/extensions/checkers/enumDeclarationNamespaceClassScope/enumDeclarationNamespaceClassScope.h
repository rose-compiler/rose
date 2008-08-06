// Enum Declaration Namespace Class Scope
// Author: Gary M. Yuan
// Date: 25-July-2007

#include "compass.h"

#ifndef COMPASS_ENUM_DECLARATION_NAMESPACE_CLASS_SCOPE_H
#define COMPASS_ENUM_DECLARATION_NAMESPACE_CLASS_SCOPE_H

namespace CompassAnalyses
   { 
     namespace EnumDeclarationNamespaceClassScope
        { 
          /// \brief checkerName is a std::string containing the name of this
          /// checker
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's pattern
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed
          /// description of this checker's pattern and purpose
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////
          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                 /// The constructor
                 /// \param node is a SgNode*
                 CheckerOutput(SgNode* node);
             }; //class CheckerOutput

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class implements a simple AST traversal seeking out
          /// enum declarations then visiting parent nodes until a namespace
          /// or class declaration is found. If no class or namespace
          /// declaration is found then a violation is flagged.
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
               public:
                    /// The constructor
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    /// visit function
                    /// \param n is a SgNode
                    void visit(SgNode* n);
             }; //class Traversal
        }
   }

// COMPASS_ENUM_DECLARATION_NAMESPACE_CLASS_SCOPE_H
#endif 

