// Enum Declaration Namespace Class Scope
// Author: Gary M. Yuan
// Date: 25-July-2007

#include "rose.h"
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
             : public Compass::AstSimpleProcessingWithRunFunction
             {
                    Compass::OutputObject* output;
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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Enum Declaration Namespace Class Scope Analysis
// Author: Gary M. Yuan
// Date: 25-July-2007

#include "compass.h"
// #include "enumDeclarationNamespaceClassScope.h"

namespace CompassAnalyses
   { 
     namespace EnumDeclarationNamespaceClassScope
        { 
          const std::string checkerName      = "EnumDeclarationNamespaceClassScope";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that enumerations are declared in a namespace or class scope";
          const std::string longDescription  = "Declare enumerations in namespace or class scope to avoid symbolic name conflicts; one should nest these declarations in the closest class or namespace.";
        } //End of namespace EnumDeclarationNamespaceClassScope.
   } //End of namespace CompassAnalyses.

CompassAnalyses::EnumDeclarationNamespaceClassScope::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["EnumDeclarationNamespaceClassScope.YourParameter"]);


   }

void
CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal::
visit(SgNode* node)
   {
     bool violation=true;
     SgEnumDeclaration *eNum = isSgEnumDeclaration(node);

     if( eNum != NULL )
     {
       for( SgNode *parent = node->get_parent(); parent != NULL;
            parent = parent->get_parent() )
       {
         if( isSgClassDeclaration(parent) != NULL || 
             isSgNamespaceDeclarationStatement(parent) != NULL )
         {
//           std::cout << parent->unparseToString() << std::endl; //debug
           violation=false;
           break;
         } //if( isSgClassDeclaration(parent) != NULL )
       } //for, visit all parent nodes

       if( violation == true )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if( violation == true )
     } //if( eNum != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal(params, output);
}

extern const Compass::Checker* const enumDeclarationNamespaceClassScopeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::EnumDeclarationNamespaceClassScope::checkerName,
        CompassAnalyses::EnumDeclarationNamespaceClassScope::shortDescription,
        CompassAnalyses::EnumDeclarationNamespaceClassScope::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
