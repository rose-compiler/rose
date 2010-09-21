#include "Utilities.h"
#include "CPPDefinesAndNamespaces.h"

#include "rose.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace SageInterface;
using namespace SageBuilder;

/** Generate a name that is unique in the current scope and any parent and children scopes.
 * @param baseName the word to be included in the variable names. */
string backstroke_util::GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName)
{
	//This implementation tends to generate numbers that are unnecessarily high.
	static int counter = 0;

	string name;
	bool collision = false;
	do
	{
		name = "__" + baseName + lexical_cast<string > (counter++) + "__";

		//Look up the name in the parent scopes
		SgSymbol* nameSymbol = SageInterface::lookupSymbolInParentScopes(SgName(name), scope);
		collision = (nameSymbol != NULL);

		//Look up the name in the children scopes
		Rose_STL_Container<SgNode*> childScopes = NodeQuery::querySubTree(scope, V_SgScopeStatement);


		foreach(SgNode* childScope, childScopes)
		{
			SgScopeStatement* childScopeStatement = isSgScopeStatement(childScope);
			nameSymbol = childScopeStatement->lookup_symbol(SgName(name));
			collision = collision || (nameSymbol != NULL);
		}
	} while (collision);

	return name;
}


/** Returns true if the given expression refers to a variable. This could include using the
 * dot and arrow operator to access member variables. A comma op conunts as a variable references
 * if all its members are variable references (not just the last expression in the list). */
bool backstroke_util::IsVariableReference(SgExpression* expression)
{
	if (isSgVarRefExp(expression))
	{
		return true;
	}
	else if (isSgDotExp(expression))
	{
		SgDotExp* dotExpression = isSgDotExp(expression);
		return IsVariableReference(dotExpression->get_lhs_operand()) &&
				IsVariableReference(dotExpression->get_rhs_operand());
	}
	else if (isSgArrowExp(expression))
	{
		SgArrowExp* arrowExpression = isSgArrowExp(expression);
		return IsVariableReference(arrowExpression->get_lhs_operand()) &&
				IsVariableReference(arrowExpression->get_rhs_operand());
	}
	else if (isSgCommaOpExp(expression))
	{
		//Comma op where both the lhs and th rhs are variable references.
		//The lhs would be semantically meaningless since it doesn't have any side effects
		SgCommaOpExp* commaOp = isSgCommaOpExp(expression);
		return IsVariableReference(commaOp->get_lhs_operand()) &&
				IsVariableReference(commaOp->get_rhs_operand());
	}
	else
	{
		return false;
	}
}


/** Given an expression, generates a temporary variable whose initializer optionally evaluates
 * that expression. Then, the var reference expression returned can be used instead of the original
 * expression. The temporary variable created can be reassigned to the expression by the returned SgAssignOp;
 * this can be used when the expression the variable represents needs to be evaluated. NOTE: This handles
 * reference types correctly by using pointer types for the temporary.
 * @param expression Expression which will be replaced by a variable
 * @param scope scope in which the temporary variable will be generated
 * @return declaration of the temporary variable, an assignment op to
 *			reevaluate the expression, and a a variable reference expression to use instead of
 *         the original expression. Delete the results that you don't need! */
tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> backstroke_util::CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope, bool initializeInDeclaration)
{
	SgTreeCopy copyHelp;
	SgType* expressionType = expression->get_type();
	SgType* variableType = expressionType;

	//If the expression has a reference type, we need to use a pointer type for the temporary variable.
	//Else, re-assigning the variable is not possible
	bool isReferenceType = isSgReferenceType(expressionType);
	if (isReferenceType)
	{
		SgType* expressionBaseType = isSgReferenceType(expressionType)->get_base_type();
		variableType = SageBuilder::buildPointerType(expressionBaseType);
	}

	//Generate a unique variable name
	string name = backstroke_util::GenerateUniqueVariableName(scope);

	//Initialize the temporary variable to an evaluation of the expression
	SgAssignInitializer* initializer = NULL;
	SgExpression* tempVarInitExpression = isSgExpression(expression->copy(copyHelp));
	ROSE_ASSERT(tempVarInitExpression != NULL);
	if (isReferenceType)
	{
		//FIXME: the next line is hiding a bug in ROSE. Remove this line and talk to Dan about the resulting assert
		tempVarInitExpression->set_lvalue(false);

		tempVarInitExpression = SageBuilder::buildAddressOfOp(tempVarInitExpression);
	}

	//Optionally initialize the variable in its declaration
	if (initializeInDeclaration)
	{
		SgExpression* initExpressionCopy = isSgExpression(tempVarInitExpression->copy(copyHelp));
		initializer = SageBuilder::buildAssignInitializer(initExpressionCopy);
	}

	SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, variableType, initializer, scope);
	ROSE_ASSERT(tempVarDeclaration != NULL);

	//Now create the assignment op for reevaluating the expression
	SgVarRefExp* tempVarReference = SageBuilder::buildVarRefExp(tempVarDeclaration);
	SgAssignOp* assignment = SageBuilder::buildAssignOp(tempVarReference, tempVarInitExpression);

	//Build the variable reference expression that can be used in place of the original expresion
	SgExpression* varRefExpression = SageBuilder::buildVarRefExp(tempVarDeclaration);
	if (isReferenceType)
	{
		//The temp variable is a pointer type, so dereference it before usint it
		varRefExpression = SageBuilder::buildPointerDerefExp(varRefExpression);
	}

	return make_tuple(tempVarDeclaration, assignment, varRefExpression);
}


vector<SgExpression*> backstroke_util::findVarReferences(VariableRenaming::VarName var, SgNode* root)
{
	class SearchTraversal : public AstTopDownProcessing<bool>
	{
	public:
		VariableRenaming::VarName desiredVar;
		vector<SgExpression*> result;

		virtual bool evaluateInheritedAttribute(SgNode* node, bool isParentReference)
		{
			if (isParentReference)
			{
				return true;
			}

			if (VariableRenaming::getVarName(node) == desiredVar)
			{
				ROSE_ASSERT(isSgExpression(node)); //The variable name should always be attached to an expression
				result.push_back(isSgExpression(node));
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	SearchTraversal traversal;
	traversal.desiredVar = var;
	traversal.traverse(root, false);
	return traversal.result;
}

#define ISZERO(value, ValType) \
    if (ValType* val = is##ValType(value)) \
return val->get_value() == 0;

// Return if the value in a SgValueExp object is zero.
bool backstroke_util::isZero(SgValueExp* value)
{
	if (!value)
		return true;
	//ROSE_ASSERT(false);

	ISZERO(value, SgBoolValExp);
	ISZERO(value, SgCharVal);
	ISZERO(value, SgDoubleVal);
	ISZERO(value, SgEnumVal);
	ISZERO(value, SgFloatVal);
	ISZERO(value, SgIntVal);
	ISZERO(value, SgLongDoubleVal);
	ISZERO(value, SgLongIntVal);
	ISZERO(value, SgLongLongIntVal);
	ISZERO(value, SgShortVal);
	ISZERO(value, SgUnsignedCharVal);
	ISZERO(value, SgUnsignedIntVal);
	ISZERO(value, SgUnsignedLongLongIntVal);
	ISZERO(value, SgUnsignedLongVal);
	ISZERO(value, SgUnsignedShortVal);

	ROSE_ASSERT(false);
	return true;
}

// Reverse the Sgop_mode from prefix to postfix, or vice versa.
SgUnaryOp::Sgop_mode backstroke_util::reverseOpMode(SgUnaryOp::Sgop_mode mode)
{
	if (mode == SgUnaryOp::prefix)
		return SgUnaryOp::postfix;
	else
		return SgUnaryOp::prefix;
}

// Check if there is another used variable with the same name in the current scope.
// If yes, alter the name until it does not conflict with any other variable name.


void backstroke_util::validateName(string& name, SgNode* root)
{
	Rose_STL_Container<SgNode*> ref_list = NodeQuery::querySubTree(root, V_SgVarRefExp);


	foreach(SgNode* node, ref_list)
	{
		SgVarRefExp* var_ref = isSgVarRefExp(node);
		ROSE_ASSERT(var_ref);
		if (var_ref->get_symbol()->get_name() == name)
		{
			name += "_";
			validateName(name, root);
			return;
		}
	}
}

// If two variables are the same. A variable may be a SgVarRefExp object
// or a SgArrowExp object.
bool backstroke_util::areSameVariable(SgExpression* exp1, SgExpression* exp2)
{
	SgVarRefExp* var_ref1 = isSgVarRefExp(exp1);
	SgVarRefExp* var_ref2 = isSgVarRefExp(exp2);
	if (var_ref1 && var_ref2)
		return var_ref1->get_symbol() == var_ref2->get_symbol();

	SgArrowExp* arrow_exp1 = isSgArrowExp(exp1);
	SgArrowExp* arrow_exp2 = isSgArrowExp(exp2);
	if (arrow_exp1 && arrow_exp2)
		return areSameVariable(arrow_exp1->get_lhs_operand(), arrow_exp2->get_lhs_operand()) &&
		areSameVariable(arrow_exp1->get_rhs_operand(), arrow_exp2->get_rhs_operand());

	SgDotExp* dot_exp1 = isSgDotExp(exp1);
	SgDotExp* dot_exp2 = isSgDotExp(exp2);
	if (dot_exp1 && dot_exp2)
		return areSameVariable(dot_exp1->get_lhs_operand(), dot_exp2->get_lhs_operand()) &&
		areSameVariable(dot_exp1->get_rhs_operand(), dot_exp2->get_rhs_operand());

	return false;
}

// If the expression contains the given variable
bool backstroke_util::containsVariable(SgExpression* exp, SgExpression* var)
{
	Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(exp, V_SgExpression);
	foreach(SgNode* node, exp_list)
	if (areSameVariable(isSgExpression(node), var))
		return true;
	return false;
}


// Return whether a basic block contains a break statement.
bool backstroke_util::hasBreakStmt(SgBasicBlock* body)
{
	ROSE_ASSERT(body);

	if (body->get_statements().empty())
		return false;

	// Recursively retrieve the last SgBasicBlock statement in case of {...{...{...}}}.
	SgStatement* stmt = body->get_statements().back();
	SgBasicBlock* another_body = isSgBasicBlock(stmt);
	while (another_body)
	{
		body = another_body;
		another_body = isSgBasicBlock(another_body->get_statements().back());
	}
	return isSgBreakStmt(body->get_statements().back());
}

// If two expressions can be reorderd (in other word, reordering does not change the result).
bool backstroke_util::canBeReordered(SgExpression* exp1, SgExpression* exp2)
{
	return false;
}

// If a type is a STL container type.
bool backstroke_util::isSTLContainer(SgType* type)
{
	SgType* real_type = type->stripTypedefsAndModifiers();
	SgClassType* class_t = isSgClassType(real_type);
	if (class_t == NULL)
		return false;

	// Check the namespace.
	if (SgNamespaceDefinitionStatement * ns_def = SageInterface::enclosingNamespaceScope(class_t->get_declaration()))
	{
		if (ns_def->get_namespaceDeclaration()->get_name() != "std")
			return false;
	}
	else
		return false;

	// Check the class name
	string name = class_t->get_name();
	if (starts_with(name, "vector <") ||
			starts_with(name, "deque <") ||
			starts_with(name, "list <") ||
			starts_with(name, "set <") ||
			starts_with(name, "multiset <") ||
			starts_with(name, "map <") ||
			starts_with(name, "multimap <") ||
			starts_with(name, "stack <") ||
			starts_with(name, "queue <") ||
			starts_with(name, "priority_queue <") ||
			//starts_with(name, "pair <") ||
			starts_with(name, "valarray <") ||
			starts_with(name, "complex <") ||
			starts_with(name, "bitset <"))
		return true;

	return false;
}

// Get the defined copy constructor in a given class. Returns NULL if the copy constructor is implicit.
std::vector<SgMemberFunctionDeclaration*> backstroke_util::getCopyConstructors(SgClassDeclaration* class_decl)
{
#if 0
	SgClassDeclaration* class_decl =
			isSgClassDeclaration(class_t->get_declaration()->get_definingDeclaration());
#endif
	ROSE_ASSERT(class_decl);

	vector<SgMemberFunctionDeclaration*> copy_ctors;

	// The C++ Standard says: A non-template constructor for class X is a copy constructor
	// if its first parameter if of type X&, const X&, volatile X& or const volatile X&,
	// and either there are no other parameters or else all other parameters have default
	// arguments.

	SgClassDefinition* class_def = class_decl->get_definition();


	foreach(SgDeclarationStatement* decl, class_def->get_members())
	{
		if (SgMemberFunctionDeclaration * mem_decl = isSgMemberFunctionDeclaration(decl))
		{
			if (mem_decl->get_specialFunctionModifier().isConstructor())
			{
				SgInitializedNamePtrList para_list = mem_decl->get_args();
				if (para_list.empty())
					continue;

				// The type of the first argument.
				SgType* t = para_list[0]->get_type();
				// Strip all typedefs and modifiers.
				t = t->stripTypedefsAndModifiers();

				if (SgReferenceType * ref_t = isSgReferenceType(t))
				{
					t = ref_t->get_base_type();
					// Note that we have to strip the type twice.
					t = t->stripTypedefsAndModifiers();

					if (t == class_decl->get_type())
					{
						bool flag = true;
						for (size_t i = 1; i < para_list.size(); ++i)
						{
							if (para_list[i]->get_initializer() == NULL)
							{
								flag = false;
								break;
							}
						}
						if (flag)
							copy_ctors.push_back(mem_decl);
					}
				}
			}
		}
	}

	return copy_ctors;
}

/** Returns a boolean value to indicate whether the return value (rvalue) of the given expression is used. */
bool backstroke_util::isReturnValueUsed(SgExpression* exp)
{
    SgNode* parent_node = exp->get_parent();

    // If the expression is a full expression in an expression statement.
    if (SgExprStatement* expr_stmt = isSgExprStatement(parent_node))
    {
        SgNode* grandpa_node = expr_stmt->get_parent();

        // In Rose, the condition part of if, for, while and switch statement may be a SgExprStatement.

        if (SgIfStmt* if_stmt = isSgIfStmt(grandpa_node))
            if (if_stmt->get_conditional() == expr_stmt)
                return true;

        if (SgForStatement* for_stmt = isSgForStatement(grandpa_node))
            if (for_stmt->get_test() == expr_stmt)
                return true;

        if (SgWhileStmt* while_stmt = isSgWhileStmt(grandpa_node))
            if (while_stmt->get_condition() == expr_stmt)
                return true;

        if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(grandpa_node))
            if (switch_stmt->get_item_selector() == expr_stmt)
                return true;

        return false;
    }

    // In (a, b),  a is not used.
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(parent_node))
    {
        if (comma_op->get_lhs_operand() == exp)
            return false;
        if (comma_op->get_rhs_operand() == exp)
            return isReturnValueUsed(comma_op);
    }

    if (SgConditionalExp* cond_exp = isSgConditionalExp(parent_node))
    {
        if ((cond_exp->get_true_exp() == exp) ||
                (cond_exp->get_false_exp() == exp))
            return isReturnValueUsed(cond_exp);
    }

    if (SgForStatement* for_stmt = isSgForStatement(parent_node))
    {
        if (for_stmt->get_increment() == exp)
            return false;
    }


    //if (SgExpression* parent_exp = isSgExpression(parent_node))
      //  return true;

    return true;
}


/** Prints an error message associated with a certain node. Also outputs the file and location
  * of the node. */
void backstroke_util::printCompilerError(SgNode* badNode, const char * message)
{
	ROSE_ASSERT(badNode->get_file_info() != NULL && "Can't display line number for node without file info.");

	fprintf(stderr, "\"%s\", line %d: Error: %s\n\t%s\n", badNode->get_file_info()->get_filename(),
			badNode->get_file_info()->get_line(), message, badNode->unparseToString().c_str());
}

// Returns if an expression modifies any value.
bool backstroke_util::isModifyingExpression(SgExpression* exp)
{
    if (isAssignmentOp(exp))
        return true;
    if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
        return true;
    if (isSgFunctionCallExp(exp))
    {
        // FIXME: This part should be refined.
        return true;
    }

    return false;
}

// Returns if an expression contains any subexpression which modifies any value.
bool backstroke_util::containsModifyingExpression(SgExpression* exp)
{
    Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(exp, V_SgExpression);
    foreach (SgNode* node, exp_list)
    {
        SgExpression* e = isSgExpression(node);
        ROSE_ASSERT(e);
        if (isModifyingExpression(e))
            return true;
    }
    return false;
}

bool backstroke_util::isAssignmentOp(SgNode* e)
{
    return isSgAssignOp(e) ||
        isSgPlusAssignOp(e) ||
        isSgMinusAssignOp(e) ||
        isSgMultAssignOp(e) ||
        isSgDivAssignOp(e) ||
        isSgModAssignOp(e) ||
        isSgIorAssignOp(e) ||
        isSgAndAssignOp(e) ||
        isSgXorAssignOp(e) ||
        isSgLshiftAssignOp(e) ||
        isSgRshiftAssignOp(e);
}

void backstroke_util::removeUselessBraces(SgNode* root)
{
    vector<SgBasicBlock*> block_list = querySubTree<SgBasicBlock>(root, postorder);

    foreach (SgBasicBlock* block, block_list)
    {
        // Make sure this block is not the body of if, while, etc.
        if (!isSgBasicBlock(block->get_parent()))
            continue;

        // If there is no declaration in a basic block and this basic block 
        // belongs to another basic block, the braces can be removed.
        bool has_decl = false;
        foreach (SgStatement* stmt, block->get_statements())
        {
            if (isSgDeclarationStatement(stmt))
            {
                has_decl = true;
                break;
            }
        }

        if (!has_decl)
        {
            foreach (SgStatement* stmt, block->get_statements())
                insertStatement(block, copyStatement(stmt));
            replaceStatement(block, buildNullStatement(), true);
            //removeStatement(block);
        }
    }
}

void backstroke_util::removeUselessParen(SgNode* root)
{
    vector<SgExpression*> exps = querySubTree<SgExpression>(root, postorder);

    foreach (SgExpression* exp, exps)
    {
        // An expression in an expression statement, or comma expression does not
        // need a parenthesis.
        if (isSgExprStatement(exp->get_parent()) ||
                isSgCommaOpExp(exp->get_parent()) ||
                isSgVarRefExp(exp) ||
                isSgValueExp(exp))
            exp->set_need_paren(false);
    }
}

SgBasicBlock* backstroke_util::getFunctionBody(SgFunctionDeclaration* func_decl)
{
	SgFunctionDeclaration* func_defining_decl = isSgFunctionDeclaration(func_decl->get_definingDeclaration());
	if (func_defining_decl)
		return func_defining_decl->get_definition()->get_body();
	else
		return NULL;
}

SgStatement* backstroke_util::getEnclosingIfBody(SgNode* node)
{
	while (node)
	{
		if (SgIfStmt* if_stmt = isSgIfStmt(node->get_parent()))
			if (node == if_stmt->get_true_body() || node == if_stmt->get_false_body())
				return isSgStatement(node);
		node = node->get_parent();
	}
	return NULL;
}

SgStatement* backstroke_util::getEnclosingLoopBody(SgNode* node)
{
	while (node)
	{
		SgNode* parent = node->get_parent();
		if (SgForStatement* for_stmt = isSgForStatement(parent))
		{
			if (node == for_stmt->get_loop_body())
				return isSgStatement(node);
		}
		else if (SgWhileStmt* while_stmt = isSgWhileStmt(parent))
		{
			if (node == while_stmt->get_body())
				return isSgStatement(node);
		}
		else if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(parent))
		{
			if (node == do_while_stmt->get_body())
				return isSgStatement(node);
		}
		node = node->get_parent();
	}
	return NULL;
}

vector<SgExpression*> backstroke_util::getAllVariables(SgNode* node)
{
	vector<SgExpression*> vars;

	vector<SgExpression*> exps = querySubTree<SgExpression > (node);

	//ROSE_ASSERT(!exps.empty());

	foreach(SgExpression* exp, exps)
	{
		SgExpression* cand = NULL;
		if (isSgVarRefExp(exp))
			cand = exp;
		else if (isSgDotExp(exp) && isSgVarRefExp(isSgDotExp(exp)->get_rhs_operand()))
			cand = exp;
		else if (isSgArrowExp(exp) && isSgVarRefExp(isSgArrowExp(exp)->get_rhs_operand()))
			cand = exp;

		if (cand != NULL &&
				isSgDotExp(cand->get_parent()) == NULL &&
				isSgArrowExp(cand->get_parent()) == NULL)
		{
			vars.push_back(cand);
		}
	}

	return vars;
}
