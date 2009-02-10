// Induction Variable Update
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_INDUCTION_VARIABLE_UPDATE_H
#define COMPASS_INDUCTION_VARIABLE_UPDATE_H

namespace CompassAnalyses
   { 
     namespace InductionVariableUpdate
        { 
        /*! \brief Induction Variable Update: Add your description here 
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

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);

							 private:
										void checkForStatement(SgNode* n);
										void checkWhileStmt(SgNode* n);
										void checkDoWhileStmt(SgNode* n);
										void findUpdateStmt(SgNode* subTreeRoot, std::string indVarName);
             };
        }
   }

// COMPASS_INDUCTION_VARIABLE_UPDATE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Induction Variable Update Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
// #include "inductionVariableUpdate.h"

namespace CompassAnalyses
   { 
     namespace InductionVariableUpdate
        { 
          const std::string checkerName      = "InductionVariableUpdate";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Induction variables should not be updated inside of its loop body.";
          const std::string longDescription  = "The behaviour of iteration statements with multiple modifications of control variables is difficult to maintain and understand.";
        } //End of namespace InductionVariableUpdate.
   } //End of namespace CompassAnalyses.

CompassAnalyses::InductionVariableUpdate::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::InductionVariableUpdate::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["InductionVariableUpdate.YourParameter"]);


   }

void
CompassAnalyses::InductionVariableUpdate::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
      //if (we are in an iterative loop)
      //and if (a statement is an update and the variable updated is the induction variable)
      // output->addOutput(new IndVarCheckerOutput(n));

      if(isSgForStatement(node) != NULL) {
        checkForStatement(node);
      }else if(isSgWhileStmt(node) != NULL) {
        checkWhileStmt(node);
      }else if(isSgDoWhileStmt(node) != NULL) {
        checkDoWhileStmt(node);
      }

   } //End of the visit function.
   

    
void 
CompassAnalyses::InductionVariableUpdate::Traversal::
checkForStatement(SgNode* n)
    {
      SgForStatement* forStmt = isSgForStatement(n);
      if(NULL != forStmt)
      {
        // i is in the init statement
        SgForInitStatement* for_init_stmt = forStmt->get_for_init_stmt();
        Rose_STL_Container<SgNode*> variableList = NodeQuery::querySubTree(for_init_stmt, V_SgVarRefExp);

        for(Rose_STL_Container<SgNode*>::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
          SgVarRefExp* indVar = isSgVarRefExp((*i));
          if(indVar != NULL) {
            std::string indVarName = indVar->get_symbol()->get_name().str();
            //** up to this point, we can know which variable is an induction variable

            findUpdateStmt(forStmt->get_loop_body(), indVarName);
          }
        }
      }
    }

    
void 
CompassAnalyses::InductionVariableUpdate::Traversal::
checkWhileStmt(SgNode* n)
    {
      SgWhileStmt* whileStmt = isSgWhileStmt(n);
      if(NULL != whileStmt)
      {
        Rose_STL_Container<SgNode*> variableList = NodeQuery::querySubTree(whileStmt->get_condition(), V_SgVarRefExp);

        for(Rose_STL_Container<SgNode*>::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
          SgVarRefExp* indVar = isSgVarRefExp(*i);
          if(indVar != NULL) {
            std::string indVarName = indVar->get_symbol()->get_name().str();
            //** up to this point, we can know which variable is an induction variable

            findUpdateStmt(whileStmt->get_body(), indVarName);
          }
        }
      }
    }

    
void 
CompassAnalyses::InductionVariableUpdate::Traversal::
checkDoWhileStmt(SgNode* n)
    {
      SgDoWhileStmt* doWhileStmt = isSgDoWhileStmt(n);

      if(NULL != doWhileStmt)
      {
        Rose_STL_Container<SgNode*> variableList = NodeQuery::querySubTree(doWhileStmt->get_condition(), V_SgVarRefExp);

        for(Rose_STL_Container<SgNode*>::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
          SgVarRefExp* indVar = isSgVarRefExp(*i);
          if(indVar != NULL) {
            std::string indVarName = indVar->get_symbol()->get_name().str();
            //** up to this point, we know which variable is an induction variable

            findUpdateStmt(doWhileStmt->get_body(), indVarName);
          }
        }
      }
    }

    
void 
CompassAnalyses::InductionVariableUpdate::Traversal::
findUpdateStmt(SgNode* subTreeRoot, std::string indVarName)
    {
      Rose_STL_Container<SgNode*> varRefExpList = NodeQuery::querySubTree(subTreeRoot, V_SgVarRefExp);
      for(Rose_STL_Container<SgNode*>::iterator j = varRefExpList.begin(); j != varRefExpList.end(); j++)
      {
        SgVarRefExp* var = isSgVarRefExp((*j));
        if(var != NULL)
        {
          // 1. check PlusPlusOp (e.g. i++)
          SgPlusPlusOp* plusPlusOpParent = isSgPlusPlusOp(var->get_parent());
          if(plusPlusOpParent != NULL && var->get_symbol()->get_name().str() == indVarName)
            output->addOutput(new CheckerOutput(var));

          // 2. check MinusMinusOp (e.g. i--)
          SgMinusMinusOp* minumMinusOpParent = isSgMinusMinusOp(var->get_parent());
          if(minumMinusOpParent != NULL && var->get_symbol()->get_name().str() == indVarName)
            output->addOutput(new CheckerOutput(var));

          // 3. check AssignOp (e.g. i = expr)
          SgAssignOp* assignOpParent = isSgAssignOp(var->get_parent());
          if(assignOpParent != NULL)
          {
            SgVarRefExp* child = isSgVarRefExp(assignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 4. check PlusAssignOp (e.g. i += 3)
          SgPlusAssignOp* plusAssignOpParent = isSgPlusAssignOp(var->get_parent());
          if(plusAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(plusAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 5. check MinusAssignOp (e.g. i -= 3)
          SgMinusAssignOp* minusAssignOpParent = isSgMinusAssignOp(var->get_parent());
          if(minusAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(minusAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 6. check MultAssignOp (e.g. i *= 3)
          SgMultAssignOp* multAssignOpParent = isSgMultAssignOp(var->get_parent());
          if(multAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(multAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 7. check DivAssignOp (e.g. i /= 3)
          SgDivAssignOp* divAssignOpParent = isSgDivAssignOp(var->get_parent());
          if(divAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(divAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 8. AndAssignOp (e.g. i &= 0x11)
          SgAndAssignOp* andAssignOpParent = isSgAndAssignOp(var->get_parent());
          if(andAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(andAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 9. IorAssignOp (e.g. i |= 0x00)
          SgIorAssignOp* iorAssignOpParent = isSgIorAssignOp(var->get_parent());
          if(iorAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(iorAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 10. LshiftAssignOp (e.g. i <<= 1)
          SgLshiftAssignOp* lshiftAssignOpParent = isSgLshiftAssignOp(var->get_parent());
          if(lshiftAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(lshiftAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 11. RshiftAssignOp (e.g. i >>= 1)
          SgRshiftAssignOp* rshiftAssignOpParent = isSgRshiftAssignOp(var->get_parent());
          if(rshiftAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(rshiftAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }

          // 12. XorAssignOp (e.g. i ^= 0x01)
          SgXorAssignOp* xorAssignOpParent = isSgXorAssignOp(var->get_parent());
          if(xorAssignOpParent != NULL )
          {
            SgVarRefExp* child = isSgVarRefExp(xorAssignOpParent->get_lhs_operand_i());
            if(child != NULL
                && child->get_symbol() == var->get_symbol()
                && var->get_symbol()->get_name().str() == indVarName)
              output->addOutput(new CheckerOutput(var));
          }
        }
      }
    }


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::InductionVariableUpdate::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::InductionVariableUpdate::Traversal(params, output);
}

extern const Compass::Checker* const inductionVariableUpdateChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::InductionVariableUpdate::checkerName,
        CompassAnalyses::InductionVariableUpdate::shortDescription,
        CompassAnalyses::InductionVariableUpdate::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
