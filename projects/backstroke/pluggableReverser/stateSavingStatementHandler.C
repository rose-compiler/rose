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

bool isPureVirtualClass(SgType* type, const ClassHierarchyWrapper& classHierarchy)
{
    SgClassType* classType = isSgClassType(type);
    if (classType == NULL)
        return false;
    
    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
    ROSE_ASSERT(classDeclaration != NULL);
    
    classDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
    if (classDeclaration == NULL)
    {
        //There's no defining declaration. We really can't tell
        return false;
    }
    SgClassDefinition* classDefinition = classDeclaration->get_definition();
    set<SgMemberFunctionDeclaration*> classFunctions;
        
    //Find all superclasses
    const ClassHierarchyWrapper::ClassDefSet& ancestors =  classHierarchy.getAncestorClasses(classDefinition);
    set<SgClassDefinition*> inclusiveAncestors(ancestors.begin(), ancestors.end());
    inclusiveAncestors.insert(classDefinition);
    
    //Find all virtual and concrete functions
    set<SgMemberFunctionDeclaration*> concreteFunctions, pureVirtualFunctions;
    foreach(SgClassDefinition* c, inclusiveAncestors)
    {
        foreach(SgDeclarationStatement* memberDeclaration, c->get_members())
        {
            if (SgMemberFunctionDeclaration* memberFunction = isSgMemberFunctionDeclaration(memberDeclaration))
            {
                if (memberFunction->get_functionModifier().isPureVirtual())
                {
                    pureVirtualFunctions.insert(memberFunction);
                }
                else
                {
                    concreteFunctions.insert(memberFunction);
                }
            }
        }
    }
    
    //Check if each virtual function is implemented somewhere
    foreach (SgMemberFunctionDeclaration* virtualFunction, pureVirtualFunctions)
    {
        bool foundConcrete = false;
        
        foreach (SgMemberFunctionDeclaration* concreteFunction, concreteFunctions)
        {
            if (concreteFunction->get_name() != virtualFunction->get_name())
                continue;
            
            if (concreteFunction->get_args().size() != virtualFunction->get_args().size())
                continue;
            
            bool argsMatch = true;
            for(size_t i = 0; i < concreteFunction->get_args().size(); i++)
            {
                if (concreteFunction->get_args()[i]->get_type() != virtualFunction->get_args()[i]->get_type())
                {
                    argsMatch = false;
                    break;
                }
            }
            
            if (!argsMatch)
                continue;
            
            foundConcrete = true;
            break;
        }
        
        //If there's a pure virtual function with no corresponding concrete function, the type is pure virtual
        if (!foundConcrete)
        {
            return true;
        }
    }
    
    return false;
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
	foreach (const VariableRenaming::VarName& var_name, modified_vars)
	{
		//We will build a push expression and a pop expression.
		SgExpression* valueToBePushedExpression = VariableRenaming::buildVariableReference(var_name);
        SgExpression* assignedVarExpression = SageInterface::copyExpression(valueToBePushedExpression);
		
		SgType* varType = var_name.back()->get_type();
		if (SageInterface::isPointerType(varType))
		{
			valueToBePushedExpression = SageBuilder::buildPointerDerefExp(valueToBePushedExpression);
            assignedVarExpression = SageBuilder::buildPointerDerefExp(assignedVarExpression);
		}
        
        //If it's an enum type, we want to cast the value to int
        SgType* underlyingType = valueToBePushedExpression->get_type();
        while (true)
        {
            if (isSgModifierType(underlyingType))
            {
                underlyingType = isSgModifierType(underlyingType)->get_base_type();
                continue;
            }
            else if (isSgTypedefType(underlyingType))
            {
                underlyingType = isSgTypedefType(underlyingType)->get_base_type();
                continue;
            }
            break;
        }
        if (isSgEnumType(underlyingType))
        {
            valueToBePushedExpression = SageBuilder::buildCastExp(valueToBePushedExpression, SageBuilder::buildIntType());
        }
                
		if (!SageInterface::isCopyConstructible(valueToBePushedExpression->get_type()))
		{
			printf("OH NO THE TYPE '%s' is not copy constructible!\n", valueToBePushedExpression->get_type()->unparseToString().c_str());
            printf("The type %s abstract\n", isPureVirtualClass(valueToBePushedExpression->get_type(), classHierarchy) ? "IS" : "IS NOT");
			continue;
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
        SageInterface::prependStatement(buildExprStatement(commitExpression), commitBody);
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
