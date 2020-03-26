
//#pragma once
#ifndef SSA_GENERATOR_C
#define SSA_GENERATOR_C
#include"sage3basic.h"

#include"SSAGenerator.h"
#include<iostream>
#include"Analyzer.h"
#include"staticSingleAssignment.h"
#include"Flow.h"
#include<assert.h>
#include<set>
#include<map>
#include"PhiAttribute.h"
#include"PhiStatement.h"
#include"ContNodeAttribute.h"
#include<sstream>
#include"Sawyer/CommandLineBoost.h"
#include<time.h>

using namespace std;


//Returns the current SSA form number for the variable s that is referenced in the inTrueBranch branch of the if node labeled with *condLabel
int SSAGenerator::currentNumber(string s, Label* condLabel, bool inTrueBranch)
{
	if(condLabel == NULL) //No condition node exists 
	{	
		map<string, int>::iterator i = currentNumberMap.find(s);
		//If the variable has not been declared yet its current number is -1
		if(i == currentNumberMap.end())
		{
			logger[Sawyer::Message::WARN] << "WARNING: currentNumber(" << s << ") called although there exists no current number for " << s << " yet. -1 is returned." << endl; 			
			return -1;			
		}			
		//Else: The variable's current number is its newest number
		else
		{
			return i->second;	
		}
	}
	else //Condition node exists
	{
		SgNode* condNode = labeler->getNode(*condLabel);
		AstAttribute* condAtt = condNode->getAttribute("PHI");
		assert(condAtt != NULL);
		PhiAttribute* condPhiAtt = dynamic_cast<PhiAttribute*>(condAtt);
		assert(condPhiAtt != NULL);

		//If the variable is declared locally, its current number is its newest number
		bool declaredLocally = (inTrueBranch && (condPhiAtt->varsDeclaredInTrueBranch.find(s) != condPhiAtt->varsDeclaredInTrueBranch.end())) || 
					(!inTrueBranch && (condPhiAtt->varsDeclaredInFalseBranch.find(s) != condPhiAtt->varsDeclaredInFalseBranch.end()));
		if(declaredLocally)
		{
			return currentNumber(s, NULL, inTrueBranch);
		}	

		//Else: If there is a phi statement for the variable in the condition node its current number can be read out of it		
		PhiStatement* phi = condPhiAtt->getPhiFor(s);
		if(phi != NULL)
		{
			if(inTrueBranch) return phi->trueNumber;
			else return phi->falseNumber;
		}

		//Else: If there is a reaching number for the variable in the condition node that is the variable's current number
		int number = condPhiAtt->getReachingNumberFor(s); 
		if(number != -1) return number;

		//Else: The variable is not in scope here
		logger[Sawyer::Message::WARN]<< "WARNING: currentNumber(" << s << ") called although " << s << " seems to be out of scope. -1 is returned." << endl;
		return -1;
	}
}


//Returns the next SSA form number for the variable s that is declared or assigned a value in the inTrueBranch branch of the if node labeled with *condLabel
//Generates/updates the phi statement for the variable in the node labeled with *condLabel
int SSAGenerator::nextNumber(string s, Label* condLabel, bool inTrueBranch)
{
	map<string, int>::iterator i = currentNumberMap.find(s);
	if(i == currentNumberMap.end()) //Variable has never been declared yet so no current number exists for the variable 
	{
		currentNumberMap.insert(pair<string, int>(s, 0));
		return 0;			
	}
	else //Variable has been declared before so a current number for the variable exists
	{
		i->second = i->second + 1;
		if(condLabel != NULL) //Condition node exists
		{		
			SgNode* condNode = labeler->getNode(*condLabel);
			AstAttribute* condAtt = condNode->getAttribute("PHI");
			assert(condAtt != NULL);
			PhiAttribute* condPhiAtt = dynamic_cast<PhiAttribute*>(condAtt);
			assert(condPhiAtt != NULL);
			
			//Generate/update the phi statement for the variable in the condition node if it is not declared locally in the current branch 
			bool declaredLocally = (inTrueBranch && (condPhiAtt->varsDeclaredInTrueBranch.find(s) != condPhiAtt->varsDeclaredInTrueBranch.end())) || 
						(!inTrueBranch && (condPhiAtt->varsDeclaredInFalseBranch.find(s) != condPhiAtt->varsDeclaredInFalseBranch.end()));
			if(!declaredLocally)
			{					
				PhiStatement* phi = condPhiAtt->getPhiFor(s);
				if(phi == NULL){ 
					phi = condPhiAtt->generatePhiFor(s);
				}
		       		if(inTrueBranch) phi->trueNumber = i->second;
				else phi->falseNumber = i->second;
			}
		}
		return i->second;	
	}
}


ContNodeAttribute* SSAGenerator::getContinueNodeAttr(Label label)
{
	SgNode* node = labeler->getNode(label);
	AstAttribute* astAttr = node->getAttribute("CONTINUE_NODE"); 
	if(astAttr == NULL) 
	{
		//logger[Sawyer::Message::WARN] << "WARNING: Function getContinueNodeAttr() could not find a ContinueNodeAttribute for the CFG node " << label << endl;		
		return NULL;
	}
	ContNodeAttribute* contNodeAttr = dynamic_cast<ContNodeAttribute*>(astAttr);
	assert(contNodeAttr != NULL);
	return contNodeAttr;		
}


Label* SSAGenerator::getContinueLabel(Label label)
{
	ContNodeAttribute* contAttr = getContinueNodeAttr(label);
	if(contAttr == NULL) 
	{
		return NULL;
	}
	else
	{
		return &(contAttr->contLabel); 
	}
}


//Returns the conpound condition that has to be true so that the node marked with "label" is reached
Condition* SSAGenerator::getReachCond(Label* label)
{
	map<Label, Condition*>::iterator i = reachingConditionMap.find(*label);
	if(i == reachingConditionMap.end()) return new CondFalse();
	else return i->second;
}


//Forms the disjunction of "label"'s current reaching condition and cond; Saves the result as "label"'s new reaching condition
void SSAGenerator::updateReachCond(Label* label, Condition* cond)
{
	map<Label, Condition*>::iterator i = reachingConditionMap.find(*label);
	if(i == reachingConditionMap.end()) reachingConditionMap.insert(pair<Label, Condition*>(*label, cond)); 
	else i->second = (*(i->second) || *cond); 
		
	logger[Sawyer::Message::DEBUG] << "Current reaching cond for node " << *label << ": " << cond->toString() << endl;		
}


void SSAGenerator::findConditionNodes()
{	
	Flow::node_iterator i = flow->nodes_begin();
	while(i != flow->nodes_end())
	{
		SgNode* node = labeler->getNode(*i);
		SgNode* current = node;
		while(true)
		{
			assert(current != NULL);
			SgFunctionDefinition* funcDef = dynamic_cast<SgFunctionDefinition*>(current);
			SgIfStmt* ifStmt = dynamic_cast<SgIfStmt*>(current);	
			if(funcDef != NULL) //*node is not enclosed in an if statement
			{
				break;
			}
			else if(ifStmt != NULL)
			{
				SgNode* condition = ifStmt->get_conditional();
				if(node == condition) //*node is the condition of the found if statement; Keep searching upwards
				{
					current = current->get_parent();					
					continue; 
				}	
				else //*node is enclosed in the if statement with the condition *condition
				{
					Label conditionLabel = labeler->getLabel(condition);
					conditionMap.insert(pair<Label, Label>(*i, conditionLabel));
					break;
				}
			}
			current = current->get_parent();
		}
		i++;
	}

	//Print
	map<Label, Label>::iterator j = conditionMap.begin(); 
	while(j != conditionMap.end())
	{
		logger[Sawyer::Message::DEBUG] << "(" << j->first << ", " << j->second << ")" << endl;	
		j++;
	}
}


//Calculates the continue node for each condition node among the CFG nodes that are reachable from the CFG node marked with *label (the "current node")
//Identifies branches that definitely or maybe end with a return statement
//Attaches continue attributes that contain this information to the related condition nodes; Exception: No such attribute is added to condition nodes for which both branches definitely return 
//*callingCondLabel denotes the current node's recoursive predecessor (It is a condition node that contains the current node in one of its two branches)
//Definition continue node:
	//The continue node cx for a condition node x is defined as the direct successor of x's true and false branch (the two branches' cont node) if that node is uniquely defined
	//If there is not one unique direct successor of the two branches (possible due to return statements), there are two: An inner CFG node and the CFG's exit node
	//In that case cx is defined as the inner CFG node		
pair<Label, BranchReturns> SSAGenerator::findContinueNodes(Label* label, Label* callingCondLabel)
{
	SgNode* node = labeler->getNode(*label);
	
	//Identify the current node's actual condition node if it exists
	Label* condLabel = NULL;
	map<Label, Label>::iterator i = conditionMap.find(*label);
	if(i != conditionMap.end())
	{ 
		condLabel = &(i->second);
	}

	//Do not process the current node and its successors in the CFG if *callingCondLabel does not denote the current node's actual condition node; Return the current node's label as continue node label; The current branch does not return
	bool process = (condLabel == NULL && callingCondLabel == NULL) || (condLabel != NULL && callingCondLabel != NULL && *condLabel == *callingCondLabel);		
	if(!process) 
	{
		return pair<Label, BranchReturns>(*label, NO);
	}

	//Do not process the current node and its successors in the CFG if *label denotes a return statement; Return the current node's label as continue node label; The current branch definitely returns
	SgReturnStmt* retStmt = dynamic_cast<SgReturnStmt*>(node);		
	if(retStmt != NULL)
	{
		return pair<Label, BranchReturns>(*label, YES);
	}

	//Process the current node's successors if they exist
	LabelSet successors = flow->succ(*label);
	assert(successors.size() <= 2);
	if(successors.size() == 0) //Current node is the exit node; Return its label; Current branch does not return
	{
		assert(labeler->isFunctionExitLabel(*label));
		return pair<Label, BranchReturns>(*label, NO);
	}
	if(successors.size() == 1) //Current node is a regular node (not an if node or the exit node); Process its successor	
	{
		Label succLabel = *(successors.begin());
		return findContinueNodes(&succLabel, callingCondLabel);
	}			
	if(successors.size() == 2) //Current node is an if node
	{
		//Identify true successor and false successor 
		Flow trueOutEdges = flow->outEdgesOfType(*label, EDGE_TRUE);
		Flow falseOutEdges = flow->outEdgesOfType(*label, EDGE_FALSE);
		assert( (trueOutEdges.size() == 1) && (falseOutEdges.size() == 1) );			
		Edge outTrue = *trueOutEdges.begin();
		Edge outFalse = *falseOutEdges.begin();
		Label nextLabelTrue = outTrue.target();
		Label nextLabelFalse = outFalse.target();

		//Process true successor and false successor
		pair<Label, BranchReturns> trueRet = findContinueNodes(&nextLabelTrue, label);
		pair<Label, BranchReturns> falseRet = findContinueNodes(&nextLabelFalse, label);
		
		//If necessary: Attach a continue node attribute to the current node, determine its continue node, and process that continue node
		ContNodeAttribute* contNodeAtt;
		if(trueRet.second == YES && falseRet.second == YES) //Both branches definitely return
		{
			logger[Sawyer::Message::DEBUG] << "Continue node for node " << *label << ": None" << endl;
			return pair<Label, BranchReturns>(*label, YES); //The current branch definitely returns
		}
		else if(trueRet.second == YES) //Only the true branch definitely returns
		{
			contNodeAtt = new ContNodeAttribute(falseRet.first, trueRet.second, falseRet.second);						
			node->setAttribute("CONTINUE_NODE", contNodeAtt);
			logger[Sawyer::Message::DEBUG] << "Continue node for node " << *label << ": " << falseRet.first << endl;
			logger[Sawyer::Message::DEBUG] << "trueBranchReturns: " << trueRet.second << "; falseBranchReturns: " << falseRet.second << endl;
			pair<Label, BranchReturns> contRet = findContinueNodes(&falseRet.first, callingCondLabel); //Process the continue node and its successors
			if(contRet.second == YES) return pair<Label, BranchReturns>(contRet.first, YES); //The current branch definitely returns
			else return pair<Label, BranchReturns>(contRet.first, MAYBE); //The current branch may return
		}
		else if(falseRet.second == YES) //Only the false branch definitely returns
		{
			contNodeAtt = new ContNodeAttribute(trueRet.first, trueRet.second, falseRet.second);						
			node->setAttribute("CONTINUE_NODE", contNodeAtt);
			logger[Sawyer::Message::DEBUG] << "Continue node for node " << *label << ": " << trueRet.first << endl;	
			logger[Sawyer::Message::DEBUG] << "trueBranchReturns: " << trueRet.second << "; falseBranchReturns: " << falseRet.second << endl;		
			pair<Label, BranchReturns> contRet = findContinueNodes(&trueRet.first, callingCondLabel);
			if(contRet.second == YES) return pair<Label, BranchReturns>(contRet.first, YES); //The current branch definitely returns
			else return pair<Label, BranchReturns>(contRet.first, MAYBE); //The current branch may return
		}
		else if(trueRet.second == NO && falseRet.second == NO) //Neither the true branch nor the false branch may return 
		{
			assert(trueRet.first == falseRet.first); //The calls for both branches should yield the same continue node

			contNodeAtt = new ContNodeAttribute(trueRet.first, trueRet.second, falseRet.second);						
			node->setAttribute("CONTINUE_NODE", contNodeAtt);
			logger[Sawyer::Message::DEBUG] << "Continue node for node " << *label << ": " << trueRet.first << endl;
			logger[Sawyer::Message::DEBUG] << "trueBranchReturns: " << trueRet.second << "; falseBranchReturns: " << falseRet.second << endl;
			return findContinueNodes(&trueRet.first, callingCondLabel); //Process the continue node and its successors; Whether the current branch returns depends on the outcome
		}
		else //Neither the true branch nor the false branch definitely returns but at least one of them may return
		{
			assert(trueRet.first == falseRet.first); //The calls for both branches should yield the same continue node

			contNodeAtt = new ContNodeAttribute(trueRet.first, trueRet.second, falseRet.second);						
			node->setAttribute("CONTINUE_NODE", contNodeAtt);
			logger[Sawyer::Message::DEBUG] << "Continue node for node " << *label << ": " << trueRet.first << endl;
			logger[Sawyer::Message::DEBUG] << "trueBranchReturns: " << trueRet.second << "; falseBranchReturns: " << falseRet.second << endl;
			pair<Label, BranchReturns> contRet = findContinueNodes(&trueRet.first, callingCondLabel); //Process the continue node and its successors
			if(contRet.second == YES) return pair<Label, BranchReturns>(contRet.first, YES); //The current branch definitely returns
			return pair<Label, BranchReturns>(contRet.first, MAYBE); //The current branch may return				
		}
	}
        ROSE_ASSERT(false);
}


void SSAGenerator::processAssignmentTo(SgVarRefExp* varRef, Label* condLabel, bool inTrueBranch)
{
	//Assign next number to variable
	string varName = varRef->get_symbol()->get_name().getString();
	int varNumber = nextNumber(varName, condLabel, inTrueBranch);
	logger[Sawyer::Message::DEBUG] << "Next number for variable " << varName << ": " << varNumber << endl;
	AstValueAttribute<int>* varNumberAtt = new AstValueAttribute<int>(varNumber);
	varRef->setAttribute("SSA_NUMBER", varNumberAtt);
}


//Generates SSA form numbers for the variables contained in *ex and attaches them as AstValueAttributes to the related SgNodes
//Assumption: *ex is located in in the inTrueBranch branch of the if node labeled *condLabel (These two arguments are required to generate the SSA form numbers)  
void SSAGenerator::processSgExpression(SgExpression* ex, Label* condLabel, bool inTrueBranch)
{
	SgIntVal* intVal = dynamic_cast<SgIntVal*>(ex);
	SgMinusOp* minusOp = dynamic_cast<SgMinusOp*>(ex);
	SgVarRefExp* varRef = dynamic_cast<SgVarRefExp*>(ex);
	SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(ex);
	SgFunctionCallExp* funcCall = dynamic_cast<SgFunctionCallExp*>(ex);
	
	if(intVal) //Int value
	{
		//Nothing needs to be done; Case is listed here to have a collection of all expected cases
	}
	else if(minusOp)
	{
		processSgExpression(minusOp->get_operand(), condLabel, inTrueBranch);
	}
	else if(varRef) //Reference to variable that is NOT on the left hand side of an assignment
	{
		//Assign number to variable
		string varName = varRef->get_symbol()->get_name().getString();
		int varNumber = currentNumber(varName, condLabel, inTrueBranch);  
		logger[Sawyer::Message::DEBUG] << "Current number for variable " << varName << ": " << varNumber << endl;
		AstValueAttribute<int>* varNumberAtt = new AstValueAttribute<int>(varNumber);
		varRef->setAttribute("SSA_NUMBER", varNumberAtt);	
	}
	else if(binOp) //Binary operation
	{	
		SgExpression* lhs = binOp->get_lhs_operand();
		SgExpression* rhs = binOp->get_rhs_operand();

		//Process right hand side first
		processSgExpression(rhs, condLabel, inTrueBranch);
		
		//Process left hand side second
		SgAssignOp* assignOp = dynamic_cast<SgAssignOp*>(binOp);	
		if(assignOp) //Assignment to a variable
		{
			//Assign new number to that variable  
			SgVarRefExp* lhsVarRef = dynamic_cast<SgVarRefExp*>(lhs);
			assert(lhsVarRef != NULL);
			processAssignmentTo(lhsVarRef, condLabel, inTrueBranch);
		}
		else //Arithmetic operation or boolean operation (or something unexpected)
		{
			processSgExpression(lhs, condLabel, inTrueBranch);	
		}	
		
	}
	else if(funcCall) //Call to a function
			  //RERS specific; Only two function call types are supported: 
			  //(1) scanf("%d",&...); 
			  //(2) __VERIFIER_error(RERSVerifierErrorNumber); The artificial bool variable RERSErrorOccured has to be updated 
	{
		string funcName = funcCall->getAssociatedFunctionSymbol()->get_name().getString();
		logger[Sawyer::Message::DEBUG] << "Call to function: " << funcName << endl;
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
						processAssignmentTo(varRef, condLabel, inTrueBranch);
					}
					else logger[Sawyer::Message::DEBUG] << "FOUND NO REFERENCE TO VARIABLE" << endl;
				}
				i++;	
			}	
		}
		else if(funcName == "__VERIFIER_error" && prepareReachabilityAnalysisZ3)
		{
			SgExprListExp* funcArgs = funcCall->get_args();
			SgExpressionPtrList funcArgsPtrs = funcArgs->get_expressions();
			assert(funcArgsPtrs.size() == 1);
			SgExpression* argument = *funcArgsPtrs.begin();
			SgIntVal* intArgument = dynamic_cast<SgIntVal*>(argument);
			assert(intArgument != NULL);
			if(intArgument->get_value() == RERSVerifierErrorNumber) //(2) 
			{
				int RERSErrorOccuredNumber = nextNumber("RERSErrorOccured", condLabel, inTrueBranch); 
				logger[Sawyer::Message::DEBUG] << "Next number for variable RERSErrorOccured: " << RERSErrorOccuredNumber << endl;
				AstValueAttribute<int>* numberAtt = new AstValueAttribute<int>(RERSErrorOccuredNumber);
				funcCall->setAttribute("SSA_NUMBER", numberAtt); 
			}
		}
		else logger[Sawyer::Message::DEBUG] << "Ignoring function call" << endl;
	}
	else //Unexpected
	{
		logger[Sawyer::Message::ERROR] << "ERROR: SgExpression could not be handled: " << ex->class_name() << endl;
		assert(false);
	}
}


//Determines whether *label is the continue label of *condLabel 
bool SSAGenerator::isCont(Label* label, Label* condLabel)
{
	if(condLabel != NULL)
	{
		Label* condContLabel = getContinueLabel(*condLabel);
		if( (condContLabel != NULL && *label == *condContLabel))
		{
			return true;
		}		
	}
	return false;
}

//Determines whether *label1 is the continue label of any label that is a predecessor (not necessarily a direct predecessor) of *label2 
bool SSAGenerator::isContOfPred(Label* label1, Label* label2) 
{
	if(label1 == NULL || label2 == NULL) return false;
	map<Label, Label>::iterator i = conditionMap.find(*label2);
	while(i != conditionMap.end())
	{ 
		Label condLabel = i->second;
		if(isCont(label1, &condLabel)) return true;
		else i = conditionMap.find(condLabel);
	}
	return false;
}


//Determines whether *label1 is the exit label or the continue label of any label that is a predecessor (not necessarily a direct predecessor) of *label2 
bool SSAGenerator::isExitOrContOfPred(Label* label1, Label* label2)
{
	if(label1 == NULL) return false;
	if(labeler->isFunctionExitLabel(*label1)) return true;
	else if(isContOfPred(label1, label2)) return true;
	else return false;
}


//Determines whether *label1 is the exit label or the continue label of *label2 or of any label that is a predecessor (not necessarily a direct predecessor) of *label2 
bool SSAGenerator::isExitOrContOrContOfPred(Label* label1, Label* label2)
{
	if(isCont(label1, label2)) return true;
	else if(isExitOrContOfPred(label1, label2)) return true;
	else return false;
}


void SSAGenerator::findReachingConditions(Label* label, Label* condLabel, bool inTrueBranch, bool updateContNode)
{
	logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
	logger[Sawyer::Message::DEBUG] << "findReachingConditions() called for label " << *label << endl;

	SgNode* node = labeler->getNode(*label);
	LabelSet successors = flow->succ(*label);
	assert(successors.size() <= 2);	

	if(successors.size() == 1) //Current node is a regular node (not an if node)
	{
		Label nextLabel = *successors.begin();
		if(labeler->isFunctionExitLabel(nextLabel)) //Next node is exit node
		{
			return;
		}
		else if(isCont(&nextLabel, condLabel)) //Next node is the continue node of the condition node
		{
			if(updateContNode) updateReachCond(&nextLabel, getReachCond(label));
			else return;
		}
		else //Next node is neither the continue node of the condition node nor the exit node
		{
			updateReachCond(&nextLabel, getReachCond(label));
			findReachingConditions(&nextLabel, condLabel, inTrueBranch, updateContNode);
			return;				
		}
	}
	else if(successors.size() == 2) //Current node is an if node
	{
		//Identify true successor, false successor, and continue node
		Flow trueOutEdges = flow->outEdgesOfType(*label, EDGE_TRUE);
		Flow falseOutEdges = flow->outEdgesOfType(*label, EDGE_FALSE);
		assert( (trueOutEdges.size() == 1) && (falseOutEdges.size() == 1) );			
		Edge outTrue = *trueOutEdges.begin();
		Edge outFalse = *falseOutEdges.begin();
		Label nextLabelTrue = outTrue.target();
		Label nextLabelFalse = outFalse.target();
		Label* contLabel = NULL;
		ContNodeAttribute* contAttr = getContinueNodeAttr(*label);
		if(contAttr != NULL) contLabel = &(contAttr->contLabel);

		//Identify condition
		AstAttribute* condAtt = node->getAttribute("PHI");
		assert(condAtt != NULL);
		PhiAttribute* phiAtt = dynamic_cast<PhiAttribute*>(condAtt);
		assert(phiAtt != NULL);
		Condition* cond = phiAtt->condition;

		//Determine whether the reaching condition of current node's continue node has to be updated by the recoursive calls
		bool updateContNodeRek = true;
		if (contAttr != NULL && contAttr->trueBranchReturns == NO && contAttr->falseBranchReturns == NO) //None of the two branches might return; Thus they both definitely end at the continue node
		{
			updateContNodeRek = false;
		}

		//Update both successors' reaching condition and process both branches
		Condition* reachCond = getReachCond(label);
		if(!labeler->isFunctionExitLabel(nextLabelTrue)) //True successor is not the exit node
		{
			if(isCont(&nextLabelTrue, condLabel)) //True successor is the condition node's continue node
			{
				if(updateContNode) updateReachCond(&nextLabelTrue, *reachCond && *cond); 
			} 
			else //True successor is actually in the condition node's inTrueBranch branch
			{
				if(!isCont(&nextLabelTrue, label)) //True successor is not the current node's continue node but actually in its true branch
				{
					updateReachCond(&nextLabelTrue, *reachCond && *cond);
					findReachingConditions(&nextLabelTrue, label, true, updateContNodeRek); 
				}
			}
		}	
		if(!labeler->isFunctionExitLabel(nextLabelFalse)) //False successor is not the exit node
		{
			if(isCont(&nextLabelFalse, condLabel)) //False successor is the condition node's continue node
			{
				if(updateContNode) updateReachCond(&nextLabelFalse, (*reachCond && *!*cond));  
			} 
			else //False successor is actually in the condition node's false branch
			{
				if(!isCont(&nextLabelFalse, label)) //False successor is not the current node's continue node but actually in its false branch
				{
					updateReachCond(&nextLabelFalse, (*reachCond && *!*cond)); 
					findReachingConditions(&nextLabelFalse, label, false, updateContNodeRek); 
				}			
			}
		}	
		
		//Update the continue node's reaching condition if it has not been updated by the recoursive calls and process it
		if(contLabel != NULL)
		{
			assert(!labeler->isFunctionExitLabel(*contLabel)); 

			if(isCont(contLabel, condLabel)) //Continue node is also the condition node's continue node
			{
				if(updateContNode && !updateContNodeRek) updateReachCond(contLabel, reachCond); 
			} 
			else //Continue node is actually in the condition node's inTrueBranch branch
			{
				if(!updateContNodeRek) updateReachCond(contLabel, reachCond);
				findReachingConditions(contLabel, condLabel, inTrueBranch, updateContNode); 
			}
		}	
	}
}


//Generates SSA form numbers for the variables contained in the CFG node labeled *label and attaches them as AstValueAttributes to the related SgNodes
//Generates phi statements for the node if it is an if node; Collects those phi statements in a phi attribute and attaches it to the related SgNode
//Continues the traversal of the CFG; The CFG nodes are traversed in the topological order that treats if nodes as follows: 
	//if node -> true branch -> false branch -> (phi statements for the if node are now finished) -> if node's associated continue node and its successors
//Assumption: The node is located in in the inTrueBranch branch of the if node labeled *condLabel (These two arguments are required to generate the SSA form numbers) 
void SSAGenerator::processNode(Label* label, Label* condLabel, bool inTrueBranch)
{
	SgNode* node = labeler->getNode(*label);
	LabelSet successors = flow->succ(*label);
	assert(successors.size() <= 2);


	//Skip the current node if it is just a return node for a called function
	if(labeler->isFunctionCallReturnLabel(*label)) 
	{
		logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
		logger[Sawyer::Message::DEBUG] << "Ignoring function call return node " << *label << endl;
		assert(successors.size() == 1);
		Label nextLabel = *successors.begin();	

		//If the next node is not the continue node to any of the enclosing condition nodes and not the exit node: Process it 
		if(!isExitOrContOfPred(&nextLabel, label)) processNode(&nextLabel, condLabel, inTrueBranch);

		return;
	}				

	logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
	logger[Sawyer::Message::DEBUG] << "processNode() called for label " << *label << endl;
	
	SgVariableDeclaration* varDec = dynamic_cast<SgVariableDeclaration*>(node);
	SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(node); 		
	if(varDec) //Variable declaration
	{
		SgInitializedNamePtrList varNames = varDec->get_variables();
		SgInitializedNamePtrList::iterator i = varNames.begin();
		while(i != varNames.end())
		{	
			string name = (*i)->get_qualified_name();

			//Update the varsDeclaredInTrueBranch/varsDeclaredInFalseBranch attribute in the PhiAttribute of the condition node
			if(condLabel != NULL) 
			{
				SgNode* condNode = labeler->getNode(*condLabel);
				AstAttribute* condAtt = condNode->getAttribute("PHI");
				assert(condAtt != NULL);
				PhiAttribute* condPhiAtt = dynamic_cast<PhiAttribute*>(condAtt);
				assert(condPhiAtt != NULL);
				if(inTrueBranch) condPhiAtt->varsDeclaredInTrueBranch.insert(name);
				else condPhiAtt->varsDeclaredInFalseBranch.insert(name);
			}

			SgAssignInitializer* assignInit = dynamic_cast<SgAssignInitializer*>((*i)->get_initializer());
			if(assignInit) //Declaration with initialization
			{
				SgExpression* ex = assignInit->get_operand();
				processSgExpression(ex, condLabel, inTrueBranch); 
			}
			else //Declaration without initialization 
			{
				assert((*i)->get_initializer() == NULL);
			}

			//Assign number to declared variable
			int number = nextNumber(name, condLabel, inTrueBranch);
			logger[Sawyer::Message::DEBUG] << "Next number for variable " << name << ": " << number << endl;
			AstValueAttribute<int>* numberAtt = new AstValueAttribute<int>(number);
			(*i)->setAttribute("SSA_NUMBER", numberAtt); 

			i++;	
		}
	}
	else if(exprStmt) //Assignment to variable or if statement or function call or ...
	{
		SgExpression* ex = exprStmt->get_expression();
		processSgExpression(ex, condLabel, inTrueBranch);		
	}
	else //CFG node that is not a variable declaration and not an expression statement; Should only be the case for the first node (Entry), the last node (Exit) and return nodes
	{ 
		logger[Sawyer::Message::DEBUG] << "Node is not a variable declaration and not an expression statement" << endl;
		SgReturnStmt* retStmt = dynamic_cast<SgReturnStmt*>(node);
		assert(labeler->isFunctionEntryLabel(*label) || labeler->isFunctionExitLabel(*label) || retStmt != NULL); 
	}		

	//Continue traversal of CFG
	if(successors.size() == 1) //Current node is a regular node (not an if node)
	{
		Label nextLabel = *successors.begin();
		
		//If the next node is not the continue node to any of the enclosing condition nodes and not the exit node: Process it 
		if(!isExitOrContOfPred(&nextLabel, label)) processNode(&nextLabel, condLabel, inTrueBranch);
	}
	else if(successors.size() == 2) //Current node is an if node
	{
		assert(exprStmt != NULL);

		//Attach PhiAttribute to node that (for now) only includes its reaching variable numbers  
		map<string, int> reachingNumbers = currentNumberMap;
		if(condLabel != NULL)
		{
			SgNode* condNode = labeler->getNode(*condLabel);
			AstAttribute* condAtt = condNode->getAttribute("PHI");
			assert(condAtt != NULL);
			PhiAttribute* condPhiAtt = dynamic_cast<PhiAttribute*>(condAtt);
			assert(condPhiAtt != NULL);
			map<string, int>::iterator m = reachingNumbers.begin();
			while(m != reachingNumbers.end()) 
			{
				//m->first is in scope at the current node only if it is in scope at the condition node or it is declared locally in the current node's branch of the condition node
				bool inScope = (condPhiAtt->reachingNumbers.find(m->first) != condPhiAtt->reachingNumbers.end()) ||
						(inTrueBranch && condPhiAtt->varsDeclaredInTrueBranch.find(m->first) != condPhiAtt->varsDeclaredInTrueBranch.end()) || 
						(!inTrueBranch && condPhiAtt->varsDeclaredInFalseBranch.find(m->first) != condPhiAtt->varsDeclaredInFalseBranch.end());
				if(!inScope) 
				{
					m = reachingNumbers.erase(m);
					continue;
				}

				//Reaching number for m->first has to be updated  //TODO: Makes no sense to take reaching numbers from current numbers in the first place					
				m->second = currentNumber(m->first, condLabel, inTrueBranch);
				m++;
			}		
		}			
		CondAtomic* cond = new CondAtomic(*label);
		PhiAttribute* phiAtt = new PhiAttribute(reachingNumbers, cond);				
		exprStmt->setAttribute("PHI", phiAtt);	

		//Identify true successor, false successor and continue node
		Flow trueOutEdges = flow->outEdgesOfType(*label, EDGE_TRUE);
		Flow falseOutEdges = flow->outEdgesOfType(*label, EDGE_FALSE);
		assert( (trueOutEdges.size() == 1) && (falseOutEdges.size() == 1) );			
		Edge outTrue = *trueOutEdges.begin();
		Edge outFalse = *falseOutEdges.begin();
		Label nextLabelTrue = outTrue.target();
		Label nextLabelFalse = outFalse.target();
		Label* contLabel = getContinueLabel(*label);
					
		//Process true and false branch
		ContNodeAttribute* contAttr = getContinueNodeAttr(*label);
		bool commitPhiStatements = true; //"Hack": 
						 //If one or both of the two branches definitely return there will be phi statements created for the current node although the SSA form that is being created here does not require them in that case
						 //They are however required to find out current variable numbers in the branch/branches that definitely return 
						 //Therefore in that case the phi statements will be created but not committed
		if (contAttr == NULL) //Both branches definitely return 
		{
			if(!isExitOrContOrContOfPred(&nextLabelTrue, label)) processNode(&nextLabelTrue, label, true); //"Hack"
			if(!isExitOrContOrContOfPred(&nextLabelFalse, label)) processNode(&nextLabelFalse, label, false); //"Hack"
			commitPhiStatements = false;
		}
		else if (contAttr->trueBranchReturns == YES && contAttr->falseBranchReturns != YES) //Only the true branch definitely returns
		{
			if(!isExitOrContOrContOfPred(&nextLabelTrue, label)) processNode(&nextLabelTrue, label, true); //"Hack"
			if(condLabel == NULL) //No enclosing condition node exists 
			{
				//"Hack"-phi-statement needs to be used to determine current variable numbers because processing the true branch modified currentNumberMap 
				if(!isExitOrContOrContOfPred(&nextLabelFalse, label)) processNode(&nextLabelFalse, label, inTrueBranch); 			
			}
			else if(!isExitOrContOrContOfPred(&nextLabelFalse, label)) processNode(&nextLabelFalse, condLabel, inTrueBranch); 			
			commitPhiStatements = false;
		}
		else if (contAttr->trueBranchReturns != YES && contAttr->falseBranchReturns == YES) //Only the false branch definitely returns
		{
			if(!isExitOrContOrContOfPred(&nextLabelTrue, label)) processNode(&nextLabelTrue, condLabel, inTrueBranch); 
			if(!isExitOrContOrContOfPred(&nextLabelFalse, label)) processNode(&nextLabelFalse, label, false); //"Hack"
			commitPhiStatements = false;
		}
		else //Neither of the two branches definitely returns
		{	
			assert(!(contAttr->trueBranchReturns == YES && contAttr->falseBranchReturns == YES));

			if(!isExitOrContOrContOfPred(&nextLabelTrue, label)) processNode(&nextLabelTrue, label, true);
			if(!isExitOrContOrContOfPred(&nextLabelFalse, label)) processNode(&nextLabelFalse, label, false);
			commitPhiStatements = true;
		}
		if(commitPhiStatements)	//Commit phi statements: Generate a new number for the variable of each phi statement and save that number in its respective newNumber attribute
		{	
			vector<PhiStatement*>::iterator i = phiAtt->phiStatements.begin();
			logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
			logger[Sawyer::Message::DEBUG] << "Phi statements created for node with label " << *label << ":" << endl;
			while (i != phiAtt->phiStatements.end())
			{
				if((*i)->trueNumber != (*i)->falseNumber) 
				{	
					//Generate new number for phi statement's variable
					int newNumber = nextNumber((*i)->varName, condLabel, inTrueBranch);
					(*i)->newNumber = newNumber;	
					logger[Sawyer::Message::DEBUG] << (*i)->toString() << endl;
				}
				i++;
			}					
			logger[Sawyer::Message::DEBUG] << "- - - - - - - - - - - - - - - - - - - - - - - - -" << endl;	
			logger[Sawyer::Message::DEBUG] << "COMPLETE PHI ATTRIBUTE:" << endl << phiAtt->toString() << endl;
		}
		else //Delete phi statements ("Hack") 
		{
			phiAtt->phiStatements.clear(); 
		}
		
		//If the continue node exists and is not the continue node to any of the enclosing condition nodes and not the exit node: Process it 
		if(contLabel != NULL && !isExitOrContOfPred(contLabel, label))
		{
			processNode(contLabel, condLabel, inTrueBranch); 
		}
	}
}


//Generates an SSA form for the CFG flow 
	//Stores the SSA form numbers for the variables in AstValueAttributes and attaches those to the variables's corresponding SgNodes
	//Stores the phi statements in phi attributes and attaches those to the SgNodes corresponding to if nodes
//Assumptions about the CFG: Only one function, no loops, no pointers, no calls to external functions except printf, scanf and __VERIFIER_error (RERS specific)  
void SSAGenerator::generateSSAForm()
{
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "Executing generateSSAForm() ..." << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;	

	logger[Sawyer::Message::DEBUG] << "Executing findConditionNodes() ..." << endl;
	findConditionNodes();		
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;

	//Update the current number of the RERS specific artificial variable RERSErrorOccured to 1; 
	//If the generated SSA form will be used for Z3 reachability analysis the formula (= RERSErrorOccured__0 false) will be automatically generated by the class ReachabilityAnalyzerZ3
	if(prepareReachabilityAnalysisZ3)	
	{
		int RERSErrorOccuredNumber = nextNumber("RERSErrorOccured", NULL, false);
		logger[Sawyer::Message::DEBUG] << "Next number for variable RERSErrorOccured: " << RERSErrorOccuredNumber << endl;
		logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	}
			
	Label startLabel = analyzer->getTransitionGraph()->getStartLabel();
	logger[Sawyer::Message::DEBUG] << "Executing findContinueNodes() ..." << endl;
	findContinueNodes(&startLabel, NULL);
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	
	//Execute CFG traversal to generate SSA form numbers and phi statements
	processNode(&startLabel, NULL, false); 

	//Execute CFG traversal to generate the reaching condition for each node
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "Executing findReachingConditions() ..." << endl; 
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	logger[Sawyer::Message::DEBUG] << "----------------------------------------------" << endl;
	updateReachCond(&startLabel, new CondTrue());
	findReachingConditions(&startLabel, NULL, false, false);  
}


string CondAtomic::toString()
{
	stringstream s;
	s << label;
	return s.str();
}


string CondTrue::toString()
{
	return "true";
}


string CondFalse::toString()
{
	return "false";
}


string CondNeg::toString()
{
	stringstream s;
	s << "!(" << cond->toString() << ")";
	return s.str();
}


string CondConj::toString()
{
	stringstream s;
	s << "(" << cond1->toString() << ") && (" << cond2->toString() << ")";
	return s.str();
}


string CondDisj::toString()
{
	stringstream s;
	s << "(" << cond1->toString() << ") || (" << cond2->toString() << ")";
	return s.str();
}


Condition* Condition::operator&&(Condition& other)
{
	CondTrue* trueCondThis = dynamic_cast<CondTrue*>(this);
	CondTrue* trueCondOther = dynamic_cast<CondTrue*>(&other);
	if(trueCondThis) return &other;
	else if(trueCondOther) return this;
	else return new CondConj(this, &other);
}


Condition* Condition::operator||(Condition& other)
{
	CondDisj* disjCond = new CondDisj(this, &other);
	return disjCond;
}


Condition* Condition::operator!()
{
	CondNeg* negCond = new CondNeg(this);
	return negCond;
}

#endif
