// Variable Name Equals Database Name
// Author: Andreas Saebjoernsen,,,
// Date: 12-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H
#define COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H

namespace CompassAnalyses
   { 
     namespace VariableNameEqualsDatabaseName
        { 
        /*! \brief Variable Name Equals Database Name: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
                    SgNode* violatingDeclaration;
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
                    std::string classToLookFor;
                    std::string memberFunctionToLookFor;
                    SgNode* assignExp;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Variable Name Equals Database Name Analysis
// Author: Andreas Saebjoernsen,,,
// Date: 12-August-2007

#include "compass.h"
// #include "variableNameEqualsDatabaseName.h"

namespace CompassAnalyses
   { 
     namespace VariableNameEqualsDatabaseName
        { 
          const std::string checkerName      = "VariableNameEqualsDatabaseName";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "The name of the local variable should be the same as the name in the database.";
          const std::string longDescription  = "When looking up a name in the database the result should be put into a variable of the same name.";
        } //End of namespace VariableNameEqualsDatabaseName.
   } //End of namespace CompassAnalyses.

CompassAnalyses::VariableNameEqualsDatabaseName::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription),
     violatingDeclaration(node)
   {}

CompassAnalyses::VariableNameEqualsDatabaseName::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  //
  // The class and member function in the class are located based on names. These names are found through
  // looking up strings from compass parameters.
     classToLookFor          = Compass::parseString(inputParameters["VariableNameEqualsDatabaseName.ClassName"]);
     memberFunctionToLookFor = Compass::parseString(inputParameters["VariableNameEqualsDatabaseName.MemberFunctionName"]);
     assignExp = NULL;
   }

void
CompassAnalyses::VariableNameEqualsDatabaseName::Traversal::
visit(SgNode* node)
   { 
     if( isSgAssignInitializer(node) != NULL )
          assignExp = node;
  
     if( isSgAssignOp(node) != NULL )
          assignExp = node; 

     SgFunctionCallExp* funcCall = isSgFunctionCallExp(node);

  // See if we have a dot expression or arrow expression which
  // accesses the desired member function in the class we are looking for.
     if ( funcCall != NULL  )
        {
          SgExpression* funcExp = funcCall->get_function();

          if ( ( isSgDotExp(funcExp) != NULL ) | ( isSgArrowExp(funcExp) != NULL ) )
             {
               SgBinaryOp*     binOp = isSgBinaryOp(funcExp);
               SgExpression*   rhsOp = binOp->get_rhs_operand();
            // SgExpression*   lhsOp = binOp->get_lhs_operand();
               if ( SgMemberFunctionRefExp* funcRef = isSgMemberFunctionRefExp(rhsOp)  )
                  {
                 // std::cout << "c1\n" ;

                    SgMemberFunctionSymbol*      funcSymbol = funcRef->get_symbol();
                    ROSE_ASSERT(funcSymbol->get_declaration() != NULL);

                 // DQ (1/16/2008): Note that the defining declaration need not exist (see test2001_11.C)
                 // ROSE_ASSERT(funcSymbol->get_declaration()->get_definingDeclaration() != NULL);
                    if (funcSymbol->get_declaration()->get_definingDeclaration() != NULL)
                       {
                         SgMemberFunctionDeclaration* funcDecl   = isSgMemberFunctionDeclaration(funcSymbol->get_declaration()->get_definingDeclaration());
                         ROSE_ASSERT( funcDecl != NULL );

                         SgClassDefinition* clDef = isSgClassDefinition(funcDecl->get_scope());
                         SgClassDeclaration*          clDecl     = isSgClassDeclaration(clDef->get_declaration());  

                      // SgClassDeclaration*          clDecl     = funcDecl->get_associatedClassDeclaration();

                         ROSE_ASSERT( clDecl != NULL );
                         std::string className    = clDecl->get_name().getString();

                         ROSE_ASSERT(funcDecl != NULL);
                         std::string functionName = funcDecl->get_name().getString();
 
                      // If the class is the class we are looking for see if the member function
                      // access is to the member function we are interested in.
                      // std::cout << "className = " << className << std::endl;
                      // std::cout << "functionName = " << functionName << std::endl;

                         if ( (className == classToLookFor) && ( functionName == memberFunctionToLookFor ) )
                            {
                              SgExprListExp*         actualArgs    = funcCall->get_args();
                              SgExpressionPtrList&   actualExpArgs = actualArgs->get_expressions ();

                              ROSE_ASSERT(actualExpArgs.size() == 1);
                              Rose_STL_Container<SgNode*> nodeLst = NodeQuery::querySubTree(*actualExpArgs.begin(), V_SgStringVal);

                              ROSE_ASSERT( nodeLst.size() > 0);
                              SgStringVal* actualArg = isSgStringVal(*nodeLst.begin());
                              ROSE_ASSERT(actualArg != NULL);

                              std::string  stringArg = actualArg->get_value();

                              std::cout << "arg:" << stringArg << std::endl;

                              std::string varName;

                           // SgInitializedName* initName = NULL; 
                              if ( SgAssignInitializer* assignInit =  isSgAssignInitializer(assignExp) )
                                 {
                                   SgInitializedName* initName = isSgInitializedName(assignInit->get_parent());
                                   ROSE_ASSERT(initName != NULL);
                                 
                                   varName = initName->get_name().getString();
                                 }
                                else
                                 {
                                   if ( SgAssignOp* assignOp = isSgAssignOp(assignExp) )
                                      {
                                        SgExpression*     lhsOp  = assignOp->get_lhs_operand();
                                        SgVarRefExp*      varRef = isSgVarRefExp(lhsOp);
                                        ROSE_ASSERT(varRef!=NULL);
                                        SgVariableSymbol* varSymbol = varRef->get_symbol();
                                        ROSE_ASSERT(varSymbol != NULL);
                                        SgInitializedName* initName = varSymbol->get_declaration();
                                        varName = initName->get_name().getString();
                                      }
                                 }
 
                              if (varName != "")
                                 {
                                // we are only interested in the part of the argument after the last ":"
                                // Database scopes in ALE3D are separated by ":"

                                   size_t posCol = stringArg.find_last_of(':');
                                 
                                   if (posCol != std::string::npos)
                                        stringArg = stringArg.substr(posCol+1);

                                 //Find violations to the rule
                                   if ( stringArg != varName)
                                      {
                                        output->addOutput(new CheckerOutput(assignExp));
                                        std::cout << "violation" << varName << std::endl;
                                      }
                                     else 
                                      {
                                        std::cout << "non=violation" << varName << std::endl;
                                      }
                                 }
                            }
                       }
                  }
             } 
        }
   } // End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::VariableNameEqualsDatabaseName::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::VariableNameEqualsDatabaseName::Traversal(params, output);
}

extern const Compass::Checker* const variableNameEqualsDatabaseNameChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::VariableNameEqualsDatabaseName::checkerName,
        CompassAnalyses::VariableNameEqualsDatabaseName::shortDescription,
        CompassAnalyses::VariableNameEqualsDatabaseName::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
