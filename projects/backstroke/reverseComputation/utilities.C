#include "utilities.h"
#include <sageInterface.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace SageInterface;
using namespace boost;

#define foreach BOOST_FOREACH

#define ISZERO(value, ValType) \
    if (ValType* val = is##ValType(value)) \
return val->get_value() == 0;

// Return if the value in a SgValueExp object is zero.
bool isZero(SgValueExp* value)
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
SgUnaryOp::Sgop_mode reverseOpMode(SgUnaryOp::Sgop_mode mode)
{
    if (mode == SgUnaryOp::prefix)
        return SgUnaryOp::postfix;
    else
        return SgUnaryOp::prefix;
}

// Check if there is another used variable with the same name in the current scope.
// If yes, alter the name until it does not conflict with any other variable name.
void validateName(string& name, SgNode* root)
{
    Rose_STL_Container<SgNode*> ref_list = NodeQuery::querySubTree(root, V_SgVarRefExp);
    foreach (SgNode* node, ref_list)
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
bool areSameVariable(SgExpression* exp1, SgExpression* exp2)
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
bool containsVariable(SgExpression* exp, SgExpression* var)
{
    Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(exp, V_SgExpression);
    foreach (SgNode* node, exp_list)
        if (areSameVariable(isSgExpression(node), var))
            return true;
    return false;
}


// Return whether a basic block contains a break statement. 
bool hasBreakStmt(SgBasicBlock* body)
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
bool canBeReordered(SgExpression* exp1, SgExpression* exp2)
{
    return false;
}

// If a type is a STL container type.
bool isSTLContainer(SgType* type)
{
    SgType* real_type = type->stripTypedefsAndModifiers();
    SgClassType* class_t = isSgClassType(real_type);
    if (class_t == NULL)
        return false;

    // Check the namespace.
    if (SgNamespaceDefinitionStatement* ns_def = enclosingNamespaceScope(class_t->get_declaration()))
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
std::vector<SgMemberFunctionDeclaration*> 
getCopyConstructors(SgClassDeclaration* class_decl)
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
    foreach (SgDeclarationStatement* decl, class_def->get_members())
    {
        if (SgMemberFunctionDeclaration* mem_decl = isSgMemberFunctionDeclaration(decl))
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

                if (SgReferenceType* ref_t = isSgReferenceType(t))
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

// Returns a boolean value to indicate whether the return value of the given expression is used.
bool isReturnValueUsed(SgExpression* exp)
{
    SgNode* parent_node = exp->get_parent();

    // If the expression is a full expression in an expression statement.
    if (isSgExprStatement(parent_node))
        return false;

    if (SgCommaOpExp* comma_op = isSgCommaOpExp(parent_node))
    {
        if (comma_op->get_lhs_operand() == exp)
            return false;
        if (comma_op->get_rhs_operand() == exp)
            return isReturnValueUsed(comma_op);
    }

#if 0
    if (SgExpression* parent_exp = isSgExpression(parent_node))
        return true;
#endif

    return true;
}
