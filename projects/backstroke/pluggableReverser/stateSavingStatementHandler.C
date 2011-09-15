#include "stateSavingStatementHandler.h"
#include <staticSingleAssignment.h>
#include "eventProcessor.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/tuple/tuple.hpp>
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



SgExpression* StateSavingStatementHandler::restoreOneVariable(const VariableRenaming::VarName& varName, SgType* pushedType)
{
	SgType* varDeclaredType = varName.back()->get_type();
	SgType* dereferencedType = BackstrokeUtility::removePointerOrReferenceType(varDeclaredType);

	SgExpression* assignedVarExpression = VariableRenaming::buildVariableReference(varName);

	//Now, restore the value in the reverse code
	SgExpression* poppedExpression = popVal(pushedType);

	//C++ requires ints to be explictly cased to enums
	if (isSgEnumType(dereferencedType))
	{
		poppedExpression = SageBuilder::buildCastExp(poppedExpression, dereferencedType);
	}
	
	SgExpression* result = NULL;
	
	//For pointers, we have to call the copy constructor explicitly. We can't rely on the assignment operator,
	//because the lhs pointer may be null
	if (SageInterface::isPointerType(varDeclaredType))
	{
		SgExprListExp* constructorParam = SageBuilder::buildExprListExp(poppedExpression);
		SgConstructorInitializer* copyConstructor = 
				SageBuilder::buildConstructorInitializer(NULL, constructorParam, pushedType, false, true, true, true);
		
		SgNewExp* copiedVar = SageBuilder::buildNewExp(pushedType, NULL, copyConstructor, NULL, 0, NULL);
		
		SgAssignOp* assignCopy = SageBuilder::buildAssignOp(assignedVarExpression, copiedVar);
		result = assignCopy;
	}
	else
	{
		result = SageBuilder::buildAssignOp(assignedVarExpression, poppedExpression);
	}
	
	return result;
}

SgExpression* buildNULL()
{
    return SageBuilder::buildLongIntVal(0);
}

SgStatement*  generateErrorHandling(const char* message)
{
	//For now we just generate exit(1)
	SgExprListExp* params = SageBuilder::buildExprListExp(SageBuilder::buildIntVal(1));
	SgType* returnType = SageBuilder::buildVoidType();
	SgScopeStatement* scope = SageInterface::getFirstGlobalScope(SageInterface::getProject());
	SgStatement* exitFunctionCall = SageBuilder::buildFunctionCallStmt("exit", returnType, params, scope);
	
	//Attach the message as a comment
	SageInterface::attachComment(exitFunctionCall, message);
	
	return exitFunctionCall;
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
		SgType* dereferencedType = BackstrokeUtility::removePointerOrReferenceType(varType);

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

			//Ok, this class has subclasses. We need to generate a sequence of if/else if's to cast to the right type
			vector<SgClassDefinition*> concreteSubclasses;
			foreach (SgClassDefinition* subclass, subclasses)
			{
				if (!SageInterface::isPureVirtualClass(subclass->get_declaration()->get_type(), classHierarchy))
					concreteSubclasses.push_back(subclass);
			}

			//Don't forget to include the parent class itself
			if (!SageInterface::isPureVirtualClass(classType, classHierarchy))
			{
				concreteSubclasses.push_back(classDef);
			}

			//Construct the appropriate cast for each concrete subclass.
			vector<SgExpression*> castedVars;
			vector<SgType*> concreteTypes;
			foreach (SgClassDefinition* subclass, concreteSubclasses)
			{
				//Cast the variable to its subclass
				SgType* subclassType = NULL;
				if (isVarPointer)
					subclassType = SageBuilder::buildPointerType(subclass->get_declaration()->get_type());
				else if (isVarReference)
					subclassType = SageBuilder::buildReferenceType(subclass->get_declaration()->get_type());
				concreteTypes.push_back(subclass->get_declaration()->get_type());

				SgExpression* castedVarExp = VariableRenaming::buildVariableReference(varName);
				castedVarExp = SageBuilder::buildCastExp(castedVarExp, subclassType, SgCastExp::e_static_cast);

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
			//Now we just have to build a chain of if-else to instantiate the appropriate type
			vector<SgIfStmt*> forwardIfStatements;
			ROSE_ASSERT(castedVars.size() == typeComparisonExpressions.size());
			for (size_t i = 0; i < castedVars.size(); i++)
			{
				SgStatement* pushTrueBody = NULL;

				if (SageInterface::isCopyConstructible(concreteTypes[i]))
				{
					SgExpression* copiedVar = cloneValueExp(castedVars[i], castedVars[i]->get_type());

					//Now, push the resulting pointer
					pushTrueBody = SageBuilder::buildExprStatement(pushVal(copiedVar, varType));
				}
				else
				{
					printf("OH NO THE TYPE '%s' is not copy constructible!\n", 
							concreteTypes[i]->unparseToString().c_str());
					printf("The type %s abstract\n", 
							SageInterface::isPureVirtualClass(castedVars[i]->get_type(), classHierarchy) ? "IS" : "IS NOT");

					//We insert exit(1). The simulation will exit if it finds a runtime type that it cannot save
					pushTrueBody = generateErrorHandling("The type is not copy constructible.");
				}

				SgExpression* conditional = typeComparisonExpressions[i];
				SgStatement* falseBody = SageBuilder::buildBasicBlock();
				SgIfStmt* latestCheck = SageBuilder::buildIfStmt(conditional, pushTrueBody, falseBody);

				//All this if-statement to the chain
				if (!forwardIfStatements.empty())
				{
					SageInterface::replaceStatement(forwardIfStatements.back()->get_false_body(), latestCheck);
				}

				forwardIfStatements.push_back(latestCheck);
			}

			if (!forwardIfStatements.empty())
			{
				const char* errorMsg = "Encountered dynamic type that was not handled";
				SageInterface::replaceStatement(forwardIfStatements.back()->get_false_body(), generateErrorHandling(errorMsg));
				SageInterface::prependStatement(forwardIfStatements.front(), forwardBody);
			}
			else
			{
				const char* errorMsg = "Found no subclasses that were copy constructible!";
				SageInterface::prependStatement(generateErrorHandling(errorMsg), forwardBody);
			}
			
			//
			// Now, we have to build the reverse part,. We pop the value, test for its dynamic type, then call the 
			// assignment operator
			//
			
			//Declare the temporary variable to store the popped value
			SgVariableDeclaration* tempVarDecl;
			SgExpression* tempVarRef;
			SgAssignOp* reassignTempVar;
			boost::tie(tempVarDecl, reassignTempVar, tempVarRef) = 
					BackstrokeUtility::CreateTempVariableForExpression(popVal(varType), reverseBody, true);
			SageInterface::deepDelete(reassignTempVar);
			reassignTempVar = NULL;

			//Build if-statments that check for the actual type of the popped value
			vector<SgIfStmt*> reverseIfStatements;
			for (size_t i = 0; i < castedVars.size(); i++)
			{
				//We only push vars that are copy constructible, so there's no need to do extra checks in the commit function
				if (!SageInterface::isCopyConstructible(concreteTypes[i]))
					continue;

				SgExpression* poppedVarRef = SageInterface::copyExpression(tempVarRef);
				SgType* dynamicType = castedVars[i]->get_type();
				SgExpression* dynamicCast =
						SageBuilder::buildCastExp(poppedVarRef, dynamicType, SgCastExp::e_dynamic_cast);

				//Now check if the dynamic cast is not null
				SgExpression* comparison = SageBuilder::buildNotEqualOp(dynamicCast, buildNULL());


				//Perform a cast to the dynamic type of the popped value
				SgExpression* castedPoppedVal = SageBuilder::buildCastExp(SageInterface::copyExpression(tempVarRef), dynamicType);
				
				//Perform a cast of the original variable (and dereference)
				//The extra cast ensures that the most specific assignment operator that's applicable will be used
				SgExpression* varRef = VariableRenaming::buildVariableReference(varName);
				
				SgExpression* assignment = assignPointerExp(varRef, castedPoppedVal, dereferencedType, concreteTypes[i]);
				
				//Now build an if-statement that checks for the dynamic type, and if the dynamic type matches,
				//we do the assignment
				SgStatement* falseBody = SageBuilder::buildBasicBlock();
				SgIfStmt* latestCheck = 
						SageBuilder::buildIfStmt(comparison, SageBuilder::buildExprStatement(assignment), falseBody);

				//All this if-statement to the chain
				if (!reverseIfStatements.empty())
				{
					SageInterface::replaceStatement(reverseIfStatements.back()->get_false_body(), latestCheck);
				}

				reverseIfStatements.push_back(latestCheck);
			} //End loop over all subclasses
			

			//Put the if-statement in the reverse body
			SageInterface::appendStatement(tempVarDecl, reverseBody);
			if (!reverseIfStatements.empty())
			{
				//In the last 'else' block, we want to assert that the pointer is null since it didn't match any of the 
				//dynamic types we tried.
				SgExpression* tempVar = SageInterface::copyExpression(tempVarRef);
				SgExpression* nullAssert = BackstrokeUtility::buildAssert(SageBuilder::buildEqualityOp(tempVar, buildNULL()));
				
				//Assign the variable pointer to NULL
				SgExpression* varRef = VariableRenaming::buildVariableReference(varName);
				SgAssignOp* assignToNull = SageBuilder::buildAssignOp(varRef, buildNULL());
				
				SgBasicBlock* falseBody = SageBuilder::buildBasicBlock(
						SageBuilder::buildExprStatement(nullAssert), SageBuilder::buildExprStatement(assignToNull));
				SageInterface::replaceStatement(reverseIfStatements.back()->get_false_body(), falseBody);
				
				SageInterface::appendStatement(reverseIfStatements.front(), reverseBody);
			}
			SageInterface::deepDelete(tempVarRef);
			tempVarRef = NULL;


			//
			//In the commit function, we pop to a temporary variable. Then, we cast that variable to an appropriate
			//subclass and call delete
			//

			//Declare the temporary variable to store the popped value
			boost::tie(tempVarDecl, reassignTempVar, tempVarRef) = 
					BackstrokeUtility::CreateTempVariableForExpression(popVal_front(varType), commitBody, true);
			SageInterface::deepDelete(reassignTempVar);
			reassignTempVar = NULL;

			//Build if-statments that check for the actual type of the popped value
			vector<SgIfStmt*> commitIfStatements;
			for (size_t i = 0; i < castedVars.size(); i++)
			{
				//We only push vars that are copy constructible, so there's no need to do extra checks in the commit function
				if (!SageInterface::isCopyConstructible(concreteTypes[i]))
					continue;

				SgExpression* varRef = SageInterface::copyExpression(tempVarRef);
				SgType* dynamicType = SageBuilder::buildPointerType(concreteSubclasses[i]->get_declaration()->get_type());
				SgExpression* dynamicCast =
						SageBuilder::buildCastExp(varRef, dynamicType, SgCastExp::e_dynamic_cast);

				//Now check if the dynamic cast is not null
				SgExpression* comparison = SageBuilder::buildNotEqualOp(dynamicCast, buildNULL());


				//Perform a static cast to the dynamic type, then call delete
				SgExpression* staticCast = 
						SageBuilder::buildCastExp(SageInterface::copyExpression(tempVarRef), dynamicType);
				SgDeleteExp* deleteVar = new SgDeleteExp(staticCast, false, false, NULL);
				SageInterface::setOneSourcePositionForTransformation(deleteVar);


				//Now build an if-statement that checks for the dynamic type, and if the dynamic type matches,
				//we call delete
				SgStatement* falseBody = SageBuilder::buildBasicBlock();
				SgIfStmt* latestCheck = 
						SageBuilder::buildIfStmt(comparison, SageBuilder::buildExprStatement(deleteVar), falseBody);


				//All this if-statement to the chain
				if (!commitIfStatements.empty())
				{
					SageInterface::replaceStatement(commitIfStatements.back()->get_false_body(), latestCheck);
				}

				commitIfStatements.push_back(latestCheck);
			} //End loop over all subclasses


			if (!commitIfStatements.empty())
			{
				//In the last 'else' block, we want to assert that the pointer is null since it didn't match any of the 
				//dynamic types we tried
				SgExpression* varRef = SageInterface::copyExpression(tempVarRef);
				SgExpression* nullAssert = BackstrokeUtility::buildAssert(SageBuilder::buildEqualityOp(varRef, buildNULL()));
				SageInterface::replaceStatement(commitIfStatements.back()->get_false_body(), 
						SageBuilder::buildExprStatement(nullAssert));

				SageInterface::prependStatement(commitIfStatements.front(), commitBody);
			}
			SageInterface::prependStatement(tempVarDecl, commitBody);

			SageInterface::deepDelete(tempVarRef);
			tempVarRef = NULL;

			return;
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
	underlyingType = BackstrokeUtility::cleanModifersAndTypeDefs(underlyingType);	
	
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
    SgExpression* assign = restoreOneVariable(varName, valueToBePushedExpression->get_type());
	SgExpression* commitExpression = popVal_front(valueToBePushedExpression->get_type());

	SageInterface::prependStatement(buildExprStatement(fwd_exp), forwardBody);
	SageInterface::appendStatement(buildExprStatement(assign), reverseBody);
	SageInterface::prependStatement(buildExprStatement(commitExpression), commitBody);
}

StatementReversal StateSavingStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	SgBasicBlock* forwardBody = buildBasicBlock();
    SgBasicBlock* reverseBody = buildBasicBlock();
    SgBasicBlock* commitBody = SageBuilder::buildBasicBlock();
    
    static ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

	// If the following child result is empty, we don't have to reverse the target statement.
	vector<EvaluationResult> child_result = eval_result.getChildResults();
	if (!child_result.empty())
	{
		StatementReversal child_reversal = child_result[0].generateReverseStatement();
		SageInterface::prependStatement(child_reversal.forwardStatement, forwardBody);
		SageInterface::appendStatement(child_reversal.reverseStatement, reverseBody);
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
			SgBasicBlock* ifNonNullBody = SageBuilder::buildBasicBlock();
            
            //In the NULL case, we just push NULL to the stack
            SgStatement* assignNull = SageBuilder::buildExprStatement(pushVal(buildNULL(),  varType));
            SgStatement* ifNullBody = SageBuilder::buildBasicBlock(assignNull);
            
			SgIfStmt* forwardNullCheck = SageBuilder::buildIfStmt(varNotNullCondition, ifNonNullBody, ifNullBody);
			SageInterface::prependStatement(forwardNullCheck, forwardBody);
			
			
			//Now, actually generate the code to save / restore the variable (inside the body of the NULL guards)
			saveOneVariable(varName, 
					(SgBasicBlock*)forwardNullCheck->get_true_body(), reverseBody, commitBody, classHierarchy);
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
