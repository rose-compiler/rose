#include "stateSavingStatementHandler.h"
#include "ssa/staticSingleAssignment.h"
#include "eventProcessor.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/bind.hpp>
#include <utilities/utilities.h>
#include <utilities/cppDefinesAndNamespaces.h>

using namespace SageBuilder;
using namespace std;

//KNOWN BUGS
//-If a modified variable is a pointer, we only dereference it once. Modifications made through a pointer-to-pointer are not
//	saved correctly.
//
//PERFORMANCE HITS
//-For polymorphic classes, instead of if-then-else, instrument the classes with an extra static int and use a switch
//-Don't just call exit() if we encounter a class we can't save. We could be down an incorrect computation path



vector<VariableRenaming::VarName> StateSavingStatementHandler::getAllDefsAtNode(SgNode* node)
{
	const IVariableFilter* filter = getVariableFilter();
	vector<VariableRenaming::VarName> modified_vars;
	foreach (const StaticSingleAssignment::VarName& var_name, getSsa()->getOriginalVarsDefinedInSubtree(node))
	{
		// Get the declaration of this variable to see if it's declared inside of the given statement.
		// If so, we don't have to store this variable.
		if (!SageInterface::isAncestor(node, var_name[0]->get_declaration()) && filter->isVariableInteresting(var_name))
			modified_vars.push_back(var_name);
	}

	// Sort those names in lexicographical order.
	using namespace boost::lambda;
	std::sort(modified_vars.begin(), modified_vars.end(), 
			bind(ll::lexicographical_compare(),
			bind(call_begin(), _1), bind(call_end(), _1),
			bind(call_begin(), _2), bind(call_end(), _2)));

	// Here if a def is a member of another def, we only include the latter one. For example, if both a and a.i
	// are modified, we only include a in the results.
	modified_vars.erase(
		std::unique(modified_vars.begin(), modified_vars.end(), bind(BackstrokeUtility::isMemberOf, _2, _1)),
		modified_vars.end());

	return modified_vars;
}

bool StateSavingStatementHandler::checkStatement(SgStatement* stmt) const
{
	if (isSgWhileStmt(stmt) ||
		isSgIfStmt(stmt) ||
		isSgDoWhileStmt(stmt) ||
		isSgForStatement(stmt) ||
		isSgSwitchStatement(stmt))
		return true;

	if (isSgBasicBlock(stmt))
	{
		SgNode* parent_stmt = stmt->get_parent();
		if (isSgWhileStmt(parent_stmt) ||
			isSgDoWhileStmt(parent_stmt) ||
			isSgForStatement(parent_stmt) ||
			isSgSwitchStatement(parent_stmt))
			return false;
		else
			return true;
	}
	return false;
}

/** Given a type, remove all outer layers of SgModiferType and SgTypeDefType. */
SgType* cleanModifersAndTypeDefs(SgType* t)
{
	while (true)
	{
		if (isSgModifierType(t))
		{
			t = isSgModifierType(t)->get_base_type();
			continue;
		}
		else if (isSgTypedefType(t))
		{
			t = isSgTypedefType(t)->get_base_type();
			continue;
		}
		return t;
	}
}

SgType* removePointerOrReferenceType(SgType* t)
{
	t = cleanModifersAndTypeDefs(t);
	if (isSgPointerType(t))
		t = isSgPointerType(t)->get_base_type();
	else if (isSgReferenceType(t))
		t = isSgReferenceType(t)->get_base_type();
	
	t = cleanModifersAndTypeDefs(t);
	return t;
}


void StateSavingStatementHandler::saveOneVariable(const VariableRenaming::VarName& varName, SgBasicBlock* forwardBody, 
		SgBasicBlock* reverseBody, SgBasicBlock* commitBody, const ClassHierarchyWrapper& classHierarchy)
{
	SgType* varType = varName.back()->get_type();
	bool isVarPointer = SageInterface::isPointerType(varType);
	bool isVarReference = SageInterface::isReferenceType(varType);

	if (isVarPointer || isVarReference)
	{
		//If the variable has a class type and it's accessed through a pointer or a reference,
		//we have to find the concrete type to save.
		SgType* dereferencedType = removePointerOrReferenceType(varType);

		if (SgClassType* classType = isSgClassType(dereferencedType))
		{
			//Get the class definition from the class type
			SgClassDeclaration* definingDeclaration = isSgClassDeclaration(classType->get_declaration());
			ROSE_ASSERT(definingDeclaration != NULL);
			definingDeclaration = isSgClassDeclaration(definingDeclaration->get_definingDeclaration());
			ROSE_ASSERT(definingDeclaration != NULL);
			SgClassDefinition* classDef = definingDeclaration->get_definition();
			ROSE_ASSERT(classDef != NULL);

			//Get all subclasses of the class
			const ClassHierarchyWrapper::ClassDefSet& subclasses = classHierarchy.getSubclasses(classDef);

			if (subclasses.size() > 0)
			{
				//Ok, this class has subclasses. We need to generate a sequence of if/else if's to cast to the right type
				set<SgClassDefinition*> concreteSubclasses;
				foreach (SgClassDefinition* subclass, subclasses)
				{
					if (!SageInterface::isPureVirtualClass(subclass->get_declaration()->get_type(), classHierarchy))
						concreteSubclasses.insert(subclass);
				}

				//Construct the appropriate cast for each concrete subclass.
				vector<SgExpression*> castedVars;
				foreach (SgClassDefinition* subclass, concreteSubclasses)
				{
					//Cast the variable to its subclass
					SgType* subclassType = NULL;
					if (isVarPointer)
						subclassType = SageBuilder::buildPointerType(subclass->get_declaration()->get_type());
					else if (isVarReference)
						subclassType = SageBuilder::buildReferenceType(subclass->get_declaration()->get_type());

					SgExpression* castedVarExp = VariableRenaming::buildVariableReference(varName);
					castedVarExp = SageBuilder::buildCastExp(castedVarExp, subclassType, SgCastExp::e_static_cast);

					//If the variable is a pointer, we dereference it so we save the value, not the pointer
					if (isVarPointer)
						castedVarExp = SageBuilder::buildPointerDerefExp(castedVarExp);

					castedVars.push_back(castedVarExp);
				}

				//Construct the boolean expression that checks if the variable is of the given subclass.
				//We can use dynamic_cast or typeid
				vector<SgExpression*> typeComparisonExpressions;
				foreach(SgClassDefinition* subclass, concreteSubclasses)
				{
					//We have to construct an expression like dynamic_cast<var> != NULL
					SgExpression* pointerVar = VariableRenaming::buildVariableReference(varName);
					if (isVarReference)
					{
						pointerVar = SageBuilder::buildAddressOfOp(pointerVar);
					}

					SgType* classPointerType = SageBuilder::buildPointerType(subclass->get_declaration()->get_type());
					SgExpression* dynamicCast = 
								SageBuilder::buildCastExp(pointerVar, classPointerType, SgCastExp::e_dynamic_cast);

					//Now check if the dynamic cast is null
					SgExpression* comparison = SageBuilder::buildNotEqualOp(dynamicCast, SageBuilder::buildLongIntVal(0));
					typeComparisonExpressions.push_back(comparison);
				}

				//Now we have all the casted variables and the expressions that check for the dynamic type
				//Now we just have to build a chain of if-else if to push the appropriate type
				vector<SgIfStmt*> pushIfStatements;
				ROSE_ASSERT(castedVars.size() == typeComparisonExpressions.size());
				for (size_t i = 0; i < castedVars.size(); i++)
				{
					SgStatement* pushTrueBody = NULL;

					if (SageInterface::isCopyConstructible(castedVars[i]->get_type()))
					{
						//We push two things. First, we push an integer which identifies which class this was.
						//Then, we push the class itself
						SgStatement* pushClassId = SageBuilder::buildExprStatement(pushVal(SageBuilder::buildIntVal(i)));
						SgStatement* pushClassValue = SageBuilder::buildExprStatement(pushVal(castedVars[i]));
						pushTrueBody = SageBuilder::buildBasicBlock(pushClassId, pushClassValue);
					}
					else
					{
						printf("OH NO THE TYPE '%s' is not copy constructible!\n", castedVars[i]->get_type()->unparseToString().c_str());
						printf("The type %s abstract\n", 
								SageInterface::isPureVirtualClass(castedVars[i]->get_type(), classHierarchy) ? "IS" : "IS NOT");

						//We insert exit(1). The simulation will exit if it finds a runtime type that it cannot save
						SgExprListExp* params = SageBuilder::buildExprListExp(SageBuilder::buildIntVal(1));
						SgType* returnType = SageBuilder::buildVoidType();
						SgScopeStatement* scope = SageInterface::getFirstGlobalScope(SageInterface::getProject());
						pushTrueBody = SageBuilder::buildFunctionCallStmt("exit", returnType, params, scope);
					}

					SgExpression* conditional = typeComparisonExpressions[i];
					SgStatement* falseBody = SageBuilder::buildBasicBlock();
					SgIfStmt* latestCheck = SageBuilder::buildIfStmt(conditional, pushTrueBody, falseBody);

					//All this if-statement to the chain
					if (!pushIfStatements.empty())
					{
						SageInterface::replaceStatement(pushIfStatements.back()->get_false_body(), latestCheck);
					}

					pushIfStatements.push_back(latestCheck);
				}

				if (!pushIfStatements.empty())
				{
					SageInterface::prependStatement(pushIfStatements.front(), forwardBody);
				}

				//Now, we have to build the reverse part, where we cast appropriately when we pop the class
				//We can use a switch statement because we pushed the id of the concrete class
				vector<SgStatement*> casesWithAssignments, casesWithoutAssignment;
				for (size_t i = 0; i < castedVars.size(); i++)
				{
					//We want to build an assignment like "var = pop<var type>()";
					SgExpression* varExpression = SageInterface::copyExpression(castedVars[i]);
					SgExpression* poppedVal = popVal(varExpression->get_type());
					SgAssignOp* assignment = SageBuilder::buildAssignOp(varExpression, poppedVal);

					//Perform the assignment then break (for the reverse function)
					SgStatement* caseBody = SageBuilder::buildBasicBlock(SageBuilder::buildExprStatement(assignment),
							SageBuilder::buildBreakStmt());

					SgCaseOptionStmt* caseStmt = SageBuilder::buildCaseOptionStmt(SageBuilder::buildIntVal(i), caseBody);
					casesWithAssignments.push_back(caseStmt);

					//Pop without performing an assignment (for the commit function)
					SgStatement* popStatement = SageBuilder::buildExprStatement(SageInterface::copyExpression(poppedVal));
					SgStatement* commitCaseBody = SageBuilder::buildBasicBlock(popStatement, SageBuilder::buildBreakStmt());
					casesWithoutAssignment.push_back(commitCaseBody);
				}

				//Build & insert the switch statement for the reverse function
				SgExpression* selectorExpression = popVal(SageBuilder::buildIntType());
				SgBasicBlock* switchBody = SageBuilder::buildBasicBlock();
				SageInterface::appendStatementList(casesWithAssignments, switchBody);       
				SgSwitchStatement* reverseSwitch = SageBuilder::buildSwitchStatement(selectorExpression, switchBody);
				SageInterface::appendStatement(reverseSwitch, reverseBody);

				//Build & insert the switch statement for the commit function
				selectorExpression = SageInterface::copyExpression(selectorExpression);
				switchBody = SageBuilder::buildBasicBlock();
				SageInterface::appendStatementList(casesWithoutAssignment, switchBody);
				SgSwitchStatement* commitSwitch = SageBuilder::buildSwitchStatement(selectorExpression, switchBody);
				SageInterface::appendStatement(commitSwitch, commitBody);

				return;
			} //end if there are any subclasses
		} //end if the var has class type
	} //end if var is pointer or reference

	//We will build a push expression and a pop expression.
	SgExpression* valueToBePushedExpression = VariableRenaming::buildVariableReference(varName);
	SgExpression* assignedVarExpression = SageInterface::copyExpression(valueToBePushedExpression);
	if (SageInterface::isPointerType(varType))
	{
		valueToBePushedExpression = SageBuilder::buildPointerDerefExp(valueToBePushedExpression);
		assignedVarExpression = SageBuilder::buildPointerDerefExp(assignedVarExpression);
	}

	//If it's an enum type, we want to cast the value to int
	SgType* underlyingType = valueToBePushedExpression->get_type();
	underlyingType = cleanModifersAndTypeDefs(underlyingType);

	if (isSgEnumType(underlyingType))
	{
		valueToBePushedExpression = SageBuilder::buildCastExp(valueToBePushedExpression, SageBuilder::buildIntType());
	}

	if (!SageInterface::isCopyConstructible(valueToBePushedExpression->get_type()))
	{
		printf("OH NO THE TYPE '%s' is not copy constructible!\n", valueToBePushedExpression->get_type()->unparseToString().c_str());
		printf("The type %s abstract\n", 
				SageInterface::isPureVirtualClass(valueToBePushedExpression->get_type(), classHierarchy) ? "IS" : "IS NOT");
		return;
	}

	SgExpression* fwd_exp = pushVal(valueToBePushedExpression);

	//Now, restore the value in the reverse code
	SgExpression* poppedExpression = popVal(valueToBePushedExpression->get_type());

	//C++ requires ints to be explictly cased to enums
	if (isSgEnumType(underlyingType))
	{
		poppedExpression = SageBuilder::buildCastExp(poppedExpression, underlyingType);
	}
	SgExpression* rvs_exp = SageBuilder::buildAssignOp(assignedVarExpression, poppedExpression);

	SgExpression* commitExpression = popVal(valueToBePushedExpression->get_type());

	SageInterface::prependStatement(buildExprStatement(fwd_exp), forwardBody);
	SageInterface::appendStatement(buildExprStatement(rvs_exp), reverseBody);
	SageInterface::appendStatement(buildExprStatement(commitExpression), commitBody);
}

StatementReversal StateSavingStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	SgBasicBlock* forwardBody = buildBasicBlock();
    SgBasicBlock* reverseBody = buildBasicBlock();
    SgBasicBlock* commitBody = SageBuilder::buildBasicBlock();
    
    ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

	// If the following child result is empty, we don't have to reverse the target statement.
	vector<EvaluationResult> child_result = eval_result.getChildResults();
	if (!child_result.empty())
	{
		StatementReversal child_reversal = child_result[0].generateReverseStatement();
		SageInterface::prependStatement(child_reversal.fwd_stmt, forwardBody);
		SageInterface::appendStatement(child_reversal.rvs_stmt, reverseBody);
	}
	else
	{
		//In the forward code, include a copy of the original statement
		SageInterface::prependStatement(SageInterface::copyStatement(stmt), forwardBody);
	}

	//Now, in the forward code, push all variables on the stack. Pop them in the reverse code
	vector<VariableRenaming::VarName> modified_vars = eval_result.getAttribute<vector<VariableRenaming::VarName> >();
	foreach (const VariableRenaming::VarName& varName, modified_vars)
	{
		SgType* varType = varName.back()->get_type();
		bool isVarPointer = SageInterface::isPointerType(varType);

		//If the variable to be saved is a pointer, we only save it if it is non-null
		if (isVarPointer)
		{
			//Build the if-statement for the forward body
			SgExpression* var = VariableRenaming::buildVariableReference(varName);
			SgExpression* varNotNullCondition = SageBuilder::buildNotEqualOp(var, SageBuilder::buildLongIntVal(0));
			SgBasicBlock* nonNullBody = SageBuilder::buildBasicBlock();
			SgIfStmt* forwardNullCheck = SageBuilder::buildIfStmt(varNotNullCondition, nonNullBody, NULL);
			SageInterface::prependStatement(forwardNullCheck, forwardBody);
			
			//In the forward body, we also push one bit to indicate whether the pointer was null or not
			SgExpression* pushNullBit = pushVal(SageInterface::copyExpression(varNotNullCondition));
			SageInterface::insertStatementAfter(forwardNullCheck, SageBuilder::buildExprStatement(pushNullBit));
			
			//Build the if-statement for the reverse body
			SgExpression* reverseCondition = popVal(SageBuilder::buildBoolType());
			SgIfStmt* reverseNullCheck = SageBuilder::buildIfStmt(reverseCondition, SageBuilder::buildBasicBlock(), 
					SageBuilder::buildBasicBlock());
			SageInterface::appendStatement(reverseNullCheck, reverseBody);
			
			//In the reverse body, assign the pointer to NULL if it was null originally
			SgExpression* assignVarToNull = 
					SageBuilder::buildAssignOp(VariableRenaming::buildVariableReference(varName), SageBuilder::buildLongIntVal(0));
			SageInterface::appendStatement(SageBuilder::buildExprStatement(assignVarToNull), 
					(SgBasicBlock*)reverseNullCheck->get_false_body());

			//Build the if-statement for the commit body
			SgIfStmt* commitNullCheck = (SgIfStmt*)SageInterface::copyStatement(reverseNullCheck);
			SageInterface::appendStatement(commitNullCheck, commitBody);
			
			//Now, actually generate the code to save / restore the variable (inside the body of the NULL guards)
			saveOneVariable(varName, 
					(SgBasicBlock*)forwardNullCheck->get_true_body(), 
					(SgBasicBlock*)reverseNullCheck->get_true_body(), 
					(SgBasicBlock*)commitNullCheck->get_true_body(), classHierarchy);
		}
		else
		{
			saveOneVariable(varName, forwardBody, reverseBody, commitBody, classHierarchy);
		}
	}

	return StatementReversal(forwardBody, reverseBody, commitBody);
}

std::vector<EvaluationResult> StateSavingStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;

	// Currently, we just perform this state saving handler on if/while/for/do-while/switch statements and pure
	// basic block which is not the body of if/while/for/do-while/switch statements.
	if (!checkStatement(stmt))
		return results;

	// In case of infinite calling to this function.
	if (evaluating_stmts_.count(stmt) > 0)
		return results;

	vector<VariableRenaming::VarName> modified_vars = getAllDefsAtNode(stmt);

#if 0
	string name;
	if (isSgFunctionDefinition(stmt->get_parent()))
		name = isSgFunctionDefinition(stmt->get_parent())->get_declaration()->get_name();
	cout << "Modified vars in " << name << ":\n";
	foreach (const VariableRenaming::VarName& name, modified_vars)
		cout << VariableRenaming::keyToString(name) << endl;
	cout << "^^^\n";
#endif
	
	VariableVersionTable new_table = var_table;
	new_table.reverseVersionAtStatementStart(stmt);

#if 0
	cout << "\n\n";
	new_table.print();
	cout << "\n\n";
#endif

#if USE_OTHER_HANDLERS
	// Reverse the target statement using other handlers.
	evaluating_stmts_.insert(stmt);
	vector<EvaluationResult> eval_results = evaluateStatement(stmt, var_table);
	evaluating_stmts_.erase(stmt);

	// We combine both state saving and reversed target statement together.
	// In a following analysis on generated code, those extra store and restores will be removed.
	foreach (const EvaluationResult& eval_result, eval_results)
	{
		EvaluationResult result(this, stmt, new_table);
		result.addChildEvaluationResult(eval_result);
		// Add the attribute to the result.
		result.setAttribute(modified_vars);
		results.push_back(result);
	}
#endif

	// Here we just use state saving.
	EvaluationResult result(this, stmt, new_table);
	// Add the attribute to the result.
	result.setAttribute(modified_vars);
	results.push_back(result);

	return results;
}
