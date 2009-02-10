// Assignment Operator Check Self
// Author: pants,,,
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_ASSIGNMENT_OPERATOR_CHECK_SELF_H
#define COMPASS_ASSIGNMENT_OPERATOR_CHECK_SELF_H

namespace CompassAnalyses
   { 
     namespace AssignmentOperatorCheckSelf
        { 
        /*! \brief Assignment Operator Check Self: Add your description here 
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
             };
        }
   }

// COMPASS_ASSIGNMENT_OPERATOR_CHECK_SELF_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Assignment Operator Check Self Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
// #include "assignmentOperatorCheckSelf.h"

namespace CompassAnalyses
   { 
     namespace AssignmentOperatorCheckSelf
        { 
          const std::string checkerName      = "AssignmentOperatorCheckSelf";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding Assignment Operators that do not check for self in first line";
          const std::string longDescription  = "Searches for assignment operators (operator=) that do not check for if (this == &righthandarg) as their first line of code.";
        } //End of namespace AssignmentOperatorCheckSelf.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AssignmentOperatorCheckSelf::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::AssignmentOperatorCheckSelf::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AssignmentOperatorCheckSelf.YourParameter"]);


   }

void
CompassAnalyses::AssignmentOperatorCheckSelf::Traversal::
visit(SgNode* node)
   {
     
	bool has_this, has_rhand;
	std::string inputVar;
        std::string op = "operator=";	
	int stmtCount = 0;
	 SgMemberFunctionDeclaration *mfn =  isSgMemberFunctionDeclaration(node);
	 if (!mfn) return;
	 std::string className = mfn->get_qualified_name().str();
	 int colon = className.rfind("::");
	 std::string fName = className.substr(colon+2);
	 className =  className.substr(2,colon-2);
	 
	 //fName = mfn->get_name().str();
	 //cout << "made: " << className <<  " " << fName << endl;
	 if (op == fName)
	   {
	     SgInitializedNamePtrList fnArgs = mfn->get_args();
	     if (fnArgs.size() > 1) return;
	     for (SgInitializedNamePtrList::iterator n=fnArgs.begin(); n!=fnArgs.end(); n++)
	       {
		 SgInitializedName* argVar = isSgInitializedName (*n);
		 if (!argVar) return;
		 //cout << "Input is: " << argVar->get_name().str() << endl;
		 inputVar = argVar->get_name().str();
	       }
	     const SgFunctionDefinition *fnDef = mfn->get_definition();
	     if (!fnDef) return;
	     SgBasicBlock *bb = fnDef->get_body();
	     SgStatementPtrList stmts = bb->get_statements();
	     //start int stmtcount here
	     for (SgStatementPtrList::iterator j = stmts.begin(); j != stmts.end(); j++)
	       {
		 stmtCount += 1;
		 has_this = false;
		 has_rhand = false;
		 SgIfStmt *iffy = isSgIfStmt(*j);
		 if (!iffy) continue;
		 //cout << "if statment!" <<  endl;
		 SgStatement *condee = iffy->get_conditional();
		 SgExprStatement *expr = isSgExprStatement(condee);
		 if (!expr) continue;
		 //cout << "and we have a expr" << endl;
		 SgExpression* myExp =expr->get_expression();
		 SgEqualityOp *eqOp = isSgEqualityOp(myExp);
		 if (!eqOp) continue;
		 std::vector<SgNode *> kids = eqOp->get_traversalSuccessorContainer();
		 //vector<std::string> kids = eqOp->get_traversalSuccessorNamesContainer();		 
		 //for (vector<std::string>::iterator k = kids.begin(); k != kids.end(); k++)
		 //cout << "looping: " << kids.size() << endl;
		 for (std::vector<SgNode*>::iterator k = kids.begin(); k != kids.end(); k++)
		   {
		     SgCastExp *go1 = isSgCastExp(*k);
		     if (go1)
		       {
			 //cout << "ok got past cast" << endl;
			 //vector<std::string> kids2 = go1->get_traversalSuccessorNamesContainer();
			 std::vector<SgNode *> kids2 = go1->get_traversalSuccessorContainer();
			 for (std::vector<SgNode*>::iterator l = kids2.begin(); l != kids2.end(); l++)
			   {
			     
			     SgThisExp *athis = isSgThisExp(*l);
			     if (!athis) continue;
			     //cout << "ok found a this" << endl;
			     has_this = true;
			   }		     
		       }
		     SgAddressOfOp *ops = isSgAddressOfOp(*k);
		     //cout << "checking for var ref and addresses" << endl;
		     if (ops)
		       {
			 //cout << "ok found an addressof" << endl;
			 std::vector<SgNode *> kids3 = ops->get_traversalSuccessorContainer();
			 for (std::vector<SgNode*>::iterator m = kids3.begin(); m != kids3.end(); m++)
			   {   
			     SgVarRefExp *aVar = isSgVarRefExp(*m);
			     if (!aVar) continue;
			     //cout << "ok found a var" << endl;
			     SgVariableSymbol *varS = aVar->get_symbol();
			     //cout << "name? " << varS->get_name().str() << endl;
			     std::string varName = varS->get_name().str();
			     if (varName == inputVar)
			       {
				 has_rhand = true;
			       }

			   }
		       }
		     if (has_rhand and has_this and stmtCount == 1)
		       {
			 //cout << "found the good one " << className << endl;
			 return;
		       }

		   }
		 
		 //RESUME here, figure out how to get the none this
		 //arg from the conditional.  remember to grab input
		 //arg at beginning from thing.
		 

		 //cout << condee->
		 //set value of found def as positive.
	       }
	     //code here should only be run in operator= member
		 //functions that are not previously returned out of
		 //(by positive example)
	     //cout << "here we should only see if this is a negative test case " << className << endl;
	     std::string errString = className + "'s operator= lacks proper if construct in first line. See docs.";
	     //Sg_File_Info* start = mfn->get_startOfConstruct();
	     output->addOutput(new CheckerOutput(node));//(new AssignmentOperatorCheckSelfOutput(node)); CheckerOutput
	   }
  // Implement your traversal here.  

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AssignmentOperatorCheckSelf::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AssignmentOperatorCheckSelf::Traversal(params, output);
}

extern const Compass::Checker* const assignmentOperatorCheckSelfChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AssignmentOperatorCheckSelf::checkerName,
        CompassAnalyses::AssignmentOperatorCheckSelf::shortDescription,
        CompassAnalyses::AssignmentOperatorCheckSelf::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
