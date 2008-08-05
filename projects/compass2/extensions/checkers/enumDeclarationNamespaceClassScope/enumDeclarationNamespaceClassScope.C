// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Enum Declaration Namespace Class Scope Analysis
// Author: Gary M. Yuan
// Date: 25-July-2007

#include "compass.h"
#include "enumDeclarationNamespaceClassScope.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
