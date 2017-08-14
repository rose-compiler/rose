#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_Z3

#ifndef REACHABILITY_Analyzer_Z3_C
#define REACHABILITY_Analyzer_Z3_C

#include "ReachabilityAnalyzerZ3.h"

#include <iostream>
#include "z3++.h"
#include "Analyzer.h"
#include "staticSingleAssignment.h"
#include "Flow.h"
#include <assert.h>
#include <set>
#include <map>
#include "PhiAttribute.h"
#include "PhiStatement.h"
#include "ContNodeAttribute.h"
#include <sstream>
#include "Sawyer/CommandLineBoost.h"
#include <time.h>

using namespace std;
using namespace z3;

expr ReachabilityAnalyzerZ3::conditionToExpr(Condition* cond)
{
	CondAtomic* atomCond = dynamic_cast<CondAtomic*>(cond);
	CondTrue* trueCond = dynamic_cast<CondTrue*>(cond);
	CondFalse* falseCond = dynamic_cast<CondFalse*>(cond);
	CondNeg* negCond = dynamic_cast<CondNeg*>(cond);
	CondConj* conjCond = dynamic_cast<CondConj*>(cond);
	CondDisj* disjCond = dynamic_cast<CondDisj*>(cond);
	
	if(atomCond != NULL) //Atomic condition
	{
		//Identify the condition node's expression and process it
		Label label = atomCond->label;
		SgNode* node = labeler->getNode(label);
		SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(node);
		assert(exprStmt != NULL);
		SgExpression* ex = exprStmt->get_expression();
		expr condZ3 = processSgExpression(ex);		

		//Cast condition to bool if necessary
		assert(condZ3.is_bool() || condZ3.is_int());
		if(condZ3.is_int()) condZ3 = (condZ3 != contextZ3.int_val(0));

		return condZ3;
	}
	else if(trueCond != NULL) //True
	{
		return contextZ3.bool_val(true);
	}
	else if(falseCond != NULL) //False
	{
		return contextZ3.bool_val(false);
	}
	else if(negCond != NULL) //Negation of a condition
	{
		return !conditionToExpr(negCond->cond);
	}
	else if(conjCond != NULL) //Conjunction of two conditions
	{
		return (conditionToExpr(conjCond->cond1) && conditionToExpr(conjCond->cond1));
	}
	else if(disjCond != NULL) //Disjunction of two conditions
	{
		return (conditionToExpr(conjCond->cond1) || conditionToExpr(conjCond->cond1));
	}
	else //Should not be reachable
	{
		assert(false);
	}
}

expr ReachabilityAnalyzerZ3::toNumberedVar(string varName, int varNumber)
{
	stringstream numberedVarName;
	numberedVarName << varName << "__" << varNumber;
	//expr numberedVarZ3(contextZ3);
	if(varName == "RERSErrorOccured") return contextZ3.bool_const(numberedVarName.str().c_str()); //RERS specific; RERSErrorOccured is the only bool variable
	else return contextZ3.int_const(numberedVarName.str().c_str());
}


int ReachabilityAnalyzerZ3::getSSANumber(SgNode* node)
{
	AstAttribute* att = node->getAttribute("SSA_NUMBER");
	assert(att != NULL);
	AstValueAttribute<int>* varNumberAtt = dynamic_cast<AstValueAttribute<int>*>(att);
	assert(varNumberAtt != NULL);
	return varNumberAtt->get(); 
}


LabelSet* ReachabilityAnalyzerZ3::getReachableNodes(Label label)
{
	LabelSet* result = new LabelSet();
	vector<Label> workList;
	workList.push_back(label);
	for(unsigned i = 0; i < workList.size(); i++)
	{
		Label current = workList[i];
		result->insert(current);
		LabelSet successors = flow->succ(current);
		LabelSet::iterator j = successors.begin();
		while(j != successors.end())
		{
			workList.push_back(*j);
			j++;
		}
	}
	return result;
}


//Generates a Z3 expression that corresponds to *ex and returns it
expr ReachabilityAnalyzerZ3::processSgExpression(SgExpression* ex)
{
	SgIntVal* intVal = dynamic_cast<SgIntVal*>(ex);
	SgMinusOp* minusOp = dynamic_cast<SgMinusOp*>(ex);
	SgVarRefExp* varRef = dynamic_cast<SgVarRefExp*>(ex);
	SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(ex);
	SgFunctionCallExp* funcCall = dynamic_cast<SgFunctionCallExp*>(ex);
	
	if(intVal) //Int value 
	{	
		return contextZ3.int_val(intVal->get_value());
	}
	else if(minusOp)
	{
		expr exZ3 = processSgExpression(minusOp->get_operand());
		return (-exZ3);
	}
	else if(varRef) //Reference to variable
	{
		string varName = varRef->get_symbol()->get_name().getString();
		int varNumber = getSSANumber(varRef); 
		return toNumberedVar(varName, varNumber);
	}
	else if(binOp) //Binary operation
	{	
		//Process both sides
		SgExpression* lhs = binOp->get_lhs_operand();
		SgExpression* rhs = binOp->get_rhs_operand();
		expr lhsZ3 = processSgExpression(lhs); 
		expr rhsZ3 = processSgExpression(rhs);

		//Try to cast binOp to a variable assignment and return a corresponding Z3 formula
		SgAssignOp* assignOp = dynamic_cast<SgAssignOp*>(binOp); if(assignOp) return (lhsZ3 == rhsZ3);	

		//Try to cast binOp to an arithmetic operation and return a corresponding Z3 formula
		SgAddOp* addOp = dynamic_cast<SgAddOp*>(binOp); if(addOp) return (lhsZ3 + rhsZ3);
		SgSubtractOp* subOp = dynamic_cast<SgSubtractOp*>(binOp); if(subOp) return (lhsZ3 - rhsZ3);
		SgMultiplyOp* multOp = dynamic_cast<SgMultiplyOp*>(binOp); if(multOp) return (lhsZ3 * rhsZ3);
		SgDivideOp* divOp = dynamic_cast<SgDivideOp*>(binOp); if(divOp) return (lhsZ3 / rhsZ3);

		//Try to cast binOp to a boolean operation and return a corresponding Z3 formula
		SgEqualityOp* eqOp = dynamic_cast<SgEqualityOp*>(binOp); if(eqOp) return (lhsZ3 == rhsZ3);
		SgNotEqualOp* neqOp = dynamic_cast<SgNotEqualOp*>(binOp); if(neqOp) return (lhsZ3 != rhsZ3);
		SgLessThanOp* ltOp = dynamic_cast<SgLessThanOp*>(binOp); if(ltOp) return (lhsZ3 < rhsZ3);
		SgLessOrEqualOp* leOp = dynamic_cast<SgLessOrEqualOp*>(binOp); if(leOp) return (lhsZ3 <= rhsZ3);
		SgGreaterThanOp* gtOp = dynamic_cast<SgGreaterThanOp*>(binOp); if(gtOp) return (lhsZ3 > rhsZ3);
		SgGreaterOrEqualOp* geOp = dynamic_cast<SgGreaterOrEqualOp*>(binOp); if(geOp) return (lhsZ3 >= rhsZ3); 		
		SgAndOp* andOp = dynamic_cast<SgAndOp*>(binOp); if(andOp) return (lhsZ3 && rhsZ3);
		SgOrOp* orOp = dynamic_cast<SgOrOp*>(binOp); if(orOp) return (lhsZ3 || rhsZ3);	

		//If all casts failded: Error
		logger[Sawyer::Message::ERROR] << "ERROR: Binary operator could not be handled: " << binOp->class_name() << endl;
		return contextZ3.int_const("ERROR");	
	}
	else if(funcCall)//Call to a function
			 //RERS specific; Only two function call types are supported: 
			 //(1) scanf("%d",&...); An input from {1, ..., RERSUpperBoundForInput} is assumed;
			 //(2) __VERIFIER_error(RERSVerifierErrorNumber); The artificial bool variable RERSErrorOccured has to be updated 
	{
		expr retZ3 = contextZ3.bool_val(true);
		string funcName = funcCall->getAssociatedFunctionSymbol()->get_name().getString();
		if(funcName == "scanf") //(1)
		{
			SgExprListExp* funcArgs = funcCall->get_args();
			SgExpressionPtrList funcArgsPtrs = funcArgs->get_expressions();
			SgExpressionPtrList::iterator i = funcArgsPtrs.begin();
			while(i != funcArgsPtrs.end())
			{
				SgAddressOfOp* addrOp = dynamic_cast<SgAddressOfOp*>(*i);
				if(addrOp) 
				{
					SgVarRefExp* varRef = dynamic_cast<SgVarRefExp*>(addrOp->get_operand());
					if(varRef)
					{
						expr varZ3 = processSgExpression(varRef);
						retZ3 = varZ3 >= 1 && varZ3 <= RERSUpperBoundForInput;
					}
					else logger[Sawyer::Message::ERROR] << "FOUND NO REFERENCE TO VARIABLE" << endl;
				}
				i++;	
			}	
		}
		else if(funcName == "__VERIFIER_error")
		{
			SgExprListExp* funcArgs = funcCall->get_args();
			SgExpressionPtrList funcArgsPtrs = funcArgs->get_expressions();
			assert(funcArgsPtrs.size() == 1);
			SgExpression* argument = *funcArgsPtrs.begin();
			SgIntVal* intArgument = dynamic_cast<SgIntVal*>(argument);
			assert(intArgument != NULL);
			if(intArgument->get_value() == RERSVerifierErrorNumber) //(2) 
			{
				int RERSErrorOccuredNumber = getSSANumber(funcCall);
				expr RERSErrorOccuredZ3 = toNumberedVar("RERSErrorOccured", RERSErrorOccuredNumber);
				retZ3 = (RERSErrorOccuredZ3 == contextZ3.bool_val(true));
			}
		}
		else logger[Sawyer::Message::DEBUG] << "Cast unsuccessful" << endl;
		return retZ3;
	}
	logger[Sawyer::Message::ERROR] << "ERROR: SgExpression could not be handled: " << ex->class_name() << endl;
	return contextZ3.int_const("ERROR");
}


void ReachabilityAnalyzerZ3::processPhiAttribute(PhiAttribute* phiAtt)
{
	assert(phiAtt != NULL);
	expr condZ3 = conditionToExpr(phiAtt->condition); 
	vector<PhiStatement*>::iterator i = phiAtt->phiStatements.begin();
	while (i != phiAtt->phiStatements.end())
	{
		if((*i)->trueNumber != (*i)->falseNumber) 
		{	
			//Generate Z3 phi statement and add it to the solver	
			expr phiNewZ3 = toNumberedVar((*i)->varName, (*i)->newNumber);
			expr phiTrueZ3 = toNumberedVar((*i)->varName, (*i)->trueNumber);
			expr phiFalseZ3 = toNumberedVar((*i)->varName, (*i)->falseNumber);
			expr phiZ3 = ( phiNewZ3 == to_expr(contextZ3, Z3_mk_ite(contextZ3, condZ3, phiTrueZ3, phiFalseZ3)) );
			solverZ3.add(phiZ3);
			logger[Sawyer::Message::DEBUG] << "*Generated Z3 phi statement: " << phiZ3 << endl;
		}
		i++;
	}					
}


void ReachabilityAnalyzerZ3::processNode(Label* label)
{
	//Skip the current node if it is just a return node for a called function
	if(labeler->isFunctionCallReturnLabel(*label)) return;

	logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
	logger[Sawyer::Message::DEBUG] << "processNode() called for label " << *label << endl;

	SgNode* node = labeler->getNode(*label);
	LabelSet successors = flow->succ(*label);	
	SgVariableDeclaration* varDec = dynamic_cast<SgVariableDeclaration*>(node);
	SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(node);

	if(varDec) //Variable declaration
	{
		SgInitializedNamePtrList varNames = varDec->get_variables();
		SgInitializedNamePtrList::iterator i = varNames.begin();
		while(i != varNames.end())
		{	
			expr lhsZ3(contextZ3);
			expr rhsZ3(contextZ3);
			string name = (*i)->get_qualified_name();

			SgAssignInitializer* assignInit = dynamic_cast<SgAssignInitializer*>((*i)->get_initializer());
			if(assignInit) //Declaration with initialization
			{
				//Generate formula corresponding to right hand side
				SgExpression* ex = assignInit->get_operand();
				rhsZ3 = processSgExpression(ex);

				//Generate formula corresponding to right hand side
				AstAttribute* att = (*i)->getAttribute("SSA_NUMBER");
				assert(att != NULL);				
				AstValueAttribute<int>* numberAtt = dynamic_cast<AstValueAttribute<int>*>(att);
				assert(numberAtt != NULL);
				int number = numberAtt->get();
				lhsZ3 = toNumberedVar(name, number);

				//Generate formula (= lhsZ3 rhsZ3) and add it to the solver
				solverZ3.add(lhsZ3 == rhsZ3);
				logger[Sawyer::Message::DEBUG] << "*Generated Z3 formula: " << (lhsZ3 == rhsZ3) << endl;
			}
			else //Declaration without initialization 
			{
				assert((*i)->get_initializer() == NULL);
			}
			i++;
		}
	}
	else if(exprStmt) //Assignment to variable or if statement or function call or ...
	{
		SgExpression* ex = exprStmt->get_expression();
		
		if(successors.size() == 1) //Current node is a regular node (not an if node); Generate a corresponding formula and add it to the solver 
		{
			expr exZ3 = processSgExpression(ex);	
			solverZ3.add(exZ3); 
			logger[Sawyer::Message::DEBUG] << "*Generated Z3 formula: " << exZ3 << endl;
		}
		if(successors.size() == 2) //Current node is an if node; Generate formulas corresponding to its phi statement and add them to the solver
		{
			AstAttribute* att = exprStmt->getAttribute("PHI");
			PhiAttribute* phiAtt = dynamic_cast<PhiAttribute*>(att);
			processPhiAttribute(phiAtt);
		}
		
	}
	else //CFG node that is not a variable declaration and not an expression statement; Should only be the case for the first node (Entry), the last node (Exit) and return nodes
	{ 
		SgReturnStmt* retStmt = dynamic_cast<SgReturnStmt*>(node);
		assert(labeler->isFunctionEntryLabel(*label) || labeler->isFunctionExitLabel(*label) || retStmt != NULL); 
		return;
	}
	
}


void ReachabilityAnalyzerZ3::processReachableNodes(Label* label, Label* condLabel)
{
	LabelSet successors = flow->succ(*label);
	assert(successors.size() <= 2);
	
	//Do not process the current node if it is the exit node or the continue node of either the condition node or one of its predecessors
	if(label == NULL || ssaGen->isExitOrContOrContOfPred(label, condLabel)) return;
	else processNode(label);

	//Continue CFG traversal
	if(successors.size() == 1) //Current node is a regular node (not an if node)
	{
		Label nextLabel = *successors.begin();
		processReachableNodes(&nextLabel, condLabel);
	}
	else if(successors.size() == 2) //Current node is an if node
	{
		//Identify true successor, false successor and continue node
		Flow trueOutEdges = flow->outEdgesOfType(*label, EDGE_TRUE);
		Flow falseOutEdges = flow->outEdgesOfType(*label, EDGE_FALSE);
		assert( (trueOutEdges.size() == 1) && (falseOutEdges.size() == 1) );			
		Edge outTrue = *trueOutEdges.begin();
		Edge outFalse = *falseOutEdges.begin();
		Label nextLabelTrue = outTrue.target();
		Label nextLabelFalse = outFalse.target();
		Label* contLabel = ssaGen->getContinueLabel(*label);

		//Process true and false branch
		processReachableNodes(&nextLabelTrue, label); 
		processReachableNodes(&nextLabelFalse, label); 

		//Process continue node
		processReachableNodes(contLabel, condLabel); 
	}
}


//Assumes "flow" to be in SSA form generated by a SSAGenerator object 
//Generates Z3 expressions corresponding to the SSA form statements, adds them to the Z3 solver
void ReachabilityAnalyzerZ3::generateZ3Formulas()
{
		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
		logger[Sawyer::Message::DEBUG] << "Executing generateZ3Formulas() ..." << endl;
		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;

		//Customize the Z3 solver by utilizing Z3 tactics
		tactic solveEqs(contextZ3, "solve-eqs");
		tactic simplify(contextZ3, "simplify");
		tactic splitClause20 = repeat(tactic(contextZ3, "split-clause") | tactic(contextZ3, "skip"), 20) ;
		tactic smt(contextZ3, "smt");
		tactic RERSTactic = solveEqs & smt;
		/*solverZ3 = RERSTactic.mk_solver();*/


		//Generate z3 formula for each reachable CFG node and add it to the solver
		Label startLabel = analyzer->getTransitionGraph()->getStartLabel();
		processReachableNodes(&startLabel, NULL);
		/*
		LabelSet::iterator i = reachableNodes->begin();
		while(i != reachableNodes->end())
		{
			Label currentLabel = *i;
			processNode(&currentLabel);
			i++;		
		}
		*/ //TODO: The lower version is nicer for debugging but inefficient; Comment the lower version out and the upper version in; Update: got replaced by processReachableNodes()
		/*		
		Flow::node_iterator i = flow->nodes_begin();
		while(i != flow->nodes_end())
		{
			cout << "process?" << endl;
			if(reachableNodes->find(*i) != reachableNodes->end()) 
			{
				cout << "yes" << endl;
				processNode(&*i);
			}
			else cout << "no" << endl;
			i++;		
		}
		*/



		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
}


ReachResult ReachabilityAnalyzerZ3::checkReachability()
{
	//Generate a formula corresponding to the initialization of the RERS specific artificial variable RERSErrorOccured with false; Add the formula to the solver
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;	
	expr RERSErrorOccuredZ3 = toNumberedVar("RERSErrorOccured", 0);
	solverZ3.add(RERSErrorOccuredZ3 == contextZ3.bool_val(false));
	logger[Sawyer::Message::DEBUG] << "*Generated Z3 formula: " << (RERSErrorOccuredZ3 == contextZ3.bool_val(false)) << endl;

	//Generate formulas representing the analyzed program; Add them to the solver
	cout << "generateZ3Formulas()" << endl;
	generateZ3Formulas();

	//Generate a formula stating that RERSErrorOccured is true when the analyzed program terminates; Add the formula to the solver
	int highestNumber = ssaGen->currentNumber("RERSErrorOccured", NULL, false);
	expr testZ3 = ( toNumberedVar("RERSErrorOccured", highestNumber) == contextZ3.bool_val(true) );
	solverZ3.add(testZ3);
	logger[Sawyer::Message::DEBUG] << "*Z3 test formula added: " << testZ3 << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;	
	
	//Check the feasibility of the conjunction of all generated formulas	
	cout << "Starting Z3 solver" << endl;
	time_t startTime, endTime;
	time(&startTime);
	check_result result = solverZ3.check();
	time(&endTime);
	cout << "Elapsed time: " << difftime(endTime, startTime) << " seconds" << endl;
	if(result == sat)
	{
		cout << "Result: Reachable" << endl;
		cout << "Model: " << endl;
		return REACHABLE;
	}
	else
	{
		if(result == unsat)
		{
			cout << "Result: Unreachable" << endl;
			return UNREACHABLE;
		}
		else 
		{
			cout << "Result: Unknown" << endl;
			return UNKNOWN;
		}
				
	}
}



#endif 

#endif // end of "#ifdef HAVE_Z3"












