// -*- mode: c++; c-basic-offset: 4; -*-
// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.C,v 1.32 2009-02-11 10:03:44 gergo Exp $

#include <satire_rose.h>
#include <patternRewrite.h>
#include <cstring>

#include "ExprTransformer.h"
#include "IrCreation.h"

bool
ExprInfo::isPure() const
{
    return (entry == NULL && exit == NULL);
}

ExprInfo::ExprInfo(SgExpression *value)
  : value(value), entry(NULL), exit(NULL)
{
}

ExprInfo merge(ExprInfo a, ExprInfo b, SgExpression *newValue)
{
    ExprInfo result(newValue);

    if (!a.isPure() && !b.isPure())
    {
        add_link(a.exit, b.entry, NORMAL_EDGE);
        result.entry = a.entry;
        result.exit = b.exit;
    }
    else if (a.isPure() && !b.isPure())
    {
        result.entry = b.entry;
        result.exit = b.exit;
    }
    else if (!a.isPure() && b.isPure())
    {
        result.entry = a.entry;
        result.exit = a.exit;
    }

    return result;
}

#if 0
ExprTransformer::ExprTransformer(int node_id_, int procnum_, int expnum_,
                 CFG *cfg_, BasicBlock *after_)
  : node_id(node_id_), procnum(procnum_), expnum(expnum_), cfg(cfg_),
    after(after_), retval(after_), root_var(NULL)
{
 // GB (2008-05-05): Refactoring has made this constructor deprecated. It
 // should be removed soon.
    std::cerr << "*** warning: called deprecated constructor of class "
        << "ExprTransformer" << std::endl;
}
#endif

ExprTransformer::ExprTransformer(int node_id, int procnum, int expnum,
        CFG *cfg, BasicBlock *after, SgStatement *stmt)
  : node_id(node_id), procnum(procnum), expnum(expnum), cfg(cfg),
    after(after), retval(after), stmt(stmt),
    el(expnum, cfg, (*cfg->procedures)[procnum]),
    stmt_start(NULL),
    stmt_end(new StatementAttribute(after, POS_POST))
{
}

SgExpression *
ExprTransformer::transformExpression(SgExpression *expr,
                                     SgExpression *original_expr)
{
    ExprInfo result = traverse(expr);

    if (!result.isPure())
    {
        add_link(result.exit, after, NORMAL_EDGE);
        after = result.entry;
        last = result.exit;
    }
    else
    {
        last = after;
    }

    if (stmt != NULL)
    {
        stmt_start = new StatementAttribute(after, POS_PRE);
     // stmt_end was initialized in the constructor
        if (!stmt->attributeExists("PAG statement start"))
            stmt->addNewAttribute("PAG statement start", stmt_start);
        if (!stmt->attributeExists("PAG statement end"))
            stmt->addNewAttribute("PAG statement end", stmt_end);
    }

    CallSiteAnnotator callSiteAnnotator(callSites);
    callSiteAnnotator.traverse(original_expr, preorder);

    if (result.value == NULL)
    {
        std::cout
            << "oops! NULL result expr for expr "
            << Ir::fragmentToString(expr)
            << " (original: " << Ir::fragmentToString(original_expr) << ")"
            << std::endl;
    }

    return result.value;
}

ExprInfo
ExprTransformer::defaultSynthesizedAttribute()
{
    return ExprInfo(NULL);
}

BasicBlock *
ExprTransformer::newBasicBlock()
{
    cfg->registerStatementLabel(node_id, stmt);
    BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
    cfg->nodes.push_back(b);
    return b;
}

CallBlock *
ExprTransformer::newCallBlock()
{
    cfg->registerStatementLabel(node_id, stmt);
    CallBlock *b = new CallBlock(node_id++, CALL, procnum,
                                 new std::vector<SgVariableSymbol *>(),
                                 "<unknown function>",
                                 /* add call stmt = */ false);
    cfg->nodes.push_back(b);
    cfg->calls.push_back(b);
    callSites.push_back(b);
    return b;
}

CallBlock *
ExprTransformer::newReturnBlock()
{
    cfg->registerStatementLabel(node_id, stmt);
    CallBlock *b = new CallBlock(node_id++, RETURN, procnum,
                                 new std::vector<SgVariableSymbol *>(),
                                 "<unknown function>",
                                 /* add call stmt = */ false);
    cfg->nodes.push_back(b);
    cfg->returns.push_back(b);
    return b;
}

SgVariableSymbol *
ExprTransformer::newReturnVariable(std::string funcname)
{
    std::stringstream varname;
    varname << "$tmpvar$" << funcname << "$return_" << expnum++;
    return Ir::createVariableSymbol(varname.str(), cfg->global_unknown_type);
}

SgVariableSymbol *
ExprTransformer::newLogicalVariable()
{
    std::stringstream varname;
    varname << "$tmpvar$logical_" << expnum++;
    return Ir::createVariableSymbol(varname.str(), cfg->global_unknown_type);
}

SgVariableSymbol *
ExprTransformer::icfgArgumentVarSym(unsigned int i)
{
    unsigned int n = i;
    while (i >= cfg->global_argument_variable_symbols.size())
    {
        std::stringstream varname;
        varname << "$tmpvar$arg_" << n++;
        SgVariableSymbol *varsym
            = Ir::createVariableSymbol(varname.str(),
                                       cfg->global_unknown_type);
        cfg->global_argument_variable_symbols.push_back(varsym);
    }

    SgVariableSymbol *varsym = cfg->global_argument_variable_symbols[i];
    return varsym;
}

SgVarRefExp *
ExprTransformer::icfgArgumentVarRef(unsigned int i)
{
    return Ir::createVarRefExp(icfgArgumentVarSym(i));
}

BasicBlock *
ExprTransformer::newAssignBlock(SgExpression *lhs, SgExpression *rhs)
{
    BasicBlock *b = newBasicBlock();
    b->statements.push_back(
            Ir::createExprStatement(Ir::createAssignOp(lhs, rhs)));
    return b;
}

BasicBlock *
ExprTransformer::newArgumentAssignmentBlock(unsigned int i, SgExpression *e)
{
    SgVarRefExp *arg_var = icfgArgumentVarRef(i);
    BasicBlock *b = newBasicBlock();
    b->statements.push_back(Ir::createArgumentAssignment(arg_var, e));
    return b;
}

std::vector<SgVariableSymbol *> *
ExprTransformer::newArgumentSymbolList(SgFunctionCallExp *call)
{
    unsigned int n = call->get_args()->get_expressions().size();
    std::vector<SgVariableSymbol *> *args
        = new std::vector<SgVariableSymbol *>();
    for (int i = 0; i < n; i++)
        args->push_back(icfgArgumentVarSym(i));
    return args;
}

ExprInfo
ExprTransformer::evaluateSynthesizedAttribute(
        SgNode *node,
        ExprTransformer::SynthesizedAttributesList synList)
{
    ExprInfo result(NULL);

    if (SgThisExp *thisExp = isSgThisExp(node))
    {
     // TODO later
        result.value = isSgExpression(node);
    }
    else if (SgFunctionCallExp *call = isSgFunctionCallExp(node))
    {
     // This ExprInfo encapsulates all the stuff needed to evaluate the
     // function expression and assign the values of the argument
     // expressions to the argument tmpvars.
        ExprInfo prelude = merge(synList[SgFunctionCallExp_function],
                                 synList[SgFunctionCallExp_args]);

     // Set up basic structure.
        CallBlock *callBlock = newCallBlock();
        CallBlock *returnBlock = newReturnBlock();
        callBlock->partner = returnBlock;
        returnBlock->partner = callBlock;

        SgExpression *call_target_expr = call->get_function();
        callBlock->call_target = call_target_expr;
        cfg->call_target_call_block[call_target_expr] = callBlock;
        returnBlock->call_target = call_target_expr;

        if (!prelude.isPure())
        {
            result.entry = prelude.entry;
            add_link(prelude.exit, callBlock, NORMAL_EDGE);
        }
        else
            result.entry = callBlock;

        std::string *name = find_func_name(call);
        bool functionMayReturn = true;
        if (name != NULL && (*name == "exit"
                          || *name == "abort"
                          || *name == "__assert_fail"))
        {
            functionMayReturn = false;
        }
        if (functionMayReturn)
            add_link(callBlock, returnBlock, LOCAL);

     // Set call and return edges, put the appropriate call/return
     // statements into the call/return blocks.
        const std::vector<CallBlock *> *entries = find_entries(call);
        if (entries != NULL && !entries->empty())
        {
            std::vector<CallBlock *>::const_iterator e;
            for (e = entries->begin(); e != entries->end(); ++e)
            {
                CallBlock *funcEntry = *e;
                CallBlock *funcExit = funcEntry->partner;
                add_link(callBlock, funcEntry, CALL_EDGE);
                add_link(funcExit, returnBlock, RETURN_EDGE);
            }
            callBlock->statements.push_back(
                    Ir::createFunctionCall(CALL, *name, callBlock));
            returnBlock->statements.push_back(
                    Ir::createFunctionReturn(RETURN, *name, returnBlock));
        }
        else
        {
            callBlock->statements.push_back(
                    Ir::createExternalCall(call->get_function(),
                                           newArgumentSymbolList(call),
                                           call->get_type()));
            returnBlock->statements.push_back(
                    Ir::createExternalReturn(call->get_function(),
                                             newArgumentSymbolList(call),
                                             call->get_type()));
        }

     // Make a new return variable and a ReturnAssignment.
        SgVariableSymbol *var
            = newReturnVariable(name != NULL ? *name : "unknown_func");
        SgVariableSymbol *retvar = cfg->global_return_variable_symbol;
        ReturnAssignment *ra = Ir::createReturnAssignment(var, retvar);

        BasicBlock *retAssign = newBasicBlock();
        retAssign->statements.push_back(ra);
        retAssign->call_target = call_target_expr;
        add_link(returnBlock, retAssign, NORMAL_EDGE);
        result.exit = retAssign;

        SgVarRefExp *retvarref = Ir::createVarRefExp(var);
        result.value = retvarref;

     // Annotate the return variable (both the symbol and the expression),
     // and the specific global retvar expression, with the function call's
     // target.
        var->addNewAttribute("SATIrE: call target",
                             new CallAttribute(call_target_expr));
        retvarref->addNewAttribute("SATIrE: call target",
                                   new CallAttribute(call_target_expr));
        SgVarRefExp *glob_retvar_expr = ra->get_rhsVarRefExp();
        glob_retvar_expr->addNewAttribute("SATIrE: call target",
                                          new CallAttribute(call_target_expr));
    }
    else if (SgExprListExp *args = isSgExprListExp(node))
    {
        if (SgFunctionCallExp *call = isSgFunctionCallExp(args->get_parent()))
        {
            SgExpression *call_target_expr = call->get_function();
            SynthesizedAttributesList::iterator syn;
            BasicBlock *prev = NULL;
            unsigned int i = 0;
         // TODO later: handle 'this' pointer
            for (syn = synList.begin(); syn != synList.end(); ++syn)
            {
                ExprInfo info = *syn;

                BasicBlock *arg_block
                    = newArgumentAssignmentBlock(i++, info.value);
                arg_block->call_target = call_target_expr;
                if (!info.isPure())
                {
                    if (prev == NULL)
                        result.entry = prev = info.entry;
                    else
                        add_link(prev, info.entry, NORMAL_EDGE);
                    add_link(info.exit, arg_block, NORMAL_EDGE);
                }
                else
                {
                    if (prev == NULL)
                        result.entry = prev = arg_block;
                    else
                        add_link(prev, arg_block, NORMAL_EDGE);
                }

                prev = arg_block;
            }
         // If the list is empty, this assigns NULL, which is fine.
            result.exit = prev;
         // This is NULL anyway, but just to make really really explicit
         // that this ExprListExp is evaluated only for side-effects and not
         // for a value...
            result.value = NULL;
        }
        else
        {
#if 0
            std::cout
                << "* in new ExprTransformer: "
                << "SgExprListExp not in function call? parent is: "
                << args->get_parent()->class_name()
                << std::endl;
#endif
         // TODO: If the expressions in the list are impure, they should be
         // merged, preserving effects!
            result.value = isSgExpression(node);
        }
    }
    else if (SgConstructorInitializer *ci = isSgConstructorInitializer(node))
    {
     // TODO later
        result.value = isSgExpression(node);
    }
    else if (isSgNewExp(node) || isSgDeleteExp(node)
          || isSgMemberFunctionRefExp(node))
    {
     // TODO later
        result.value = isSgExpression(node);
    }
    else if (SgAndOp *conj = isSgAndOp(node))
    {
        ExprInfo lhsInfo = synList[SgBinaryOp_lhs_operand_i];
        ExprInfo rhsInfo = synList[SgBinaryOp_rhs_operand_i];

        if (lhsInfo.isPure() && rhsInfo.isPure())
        {
            result.value = conj;
        }
        else
        {
            BasicBlock *ifBlock = newBasicBlock();
            BasicBlock *joinBlock = newBasicBlock();
            joinBlock->statements.push_back(Ir::createIfJoin());
            SgVariableSymbol *logical = newLogicalVariable();
            SgVarRefExp *logical_varref = Ir::createVarRefExp(logical);

            BasicBlock *lhsAssign
                = newAssignBlock(logical_varref, Ir::createBoolValExp(false));
            add_link(ifBlock, lhsAssign, FALSE_EDGE);
            add_link(lhsAssign, joinBlock, NORMAL_EDGE);
            if (!lhsInfo.isPure())
            {
                result.entry = lhsInfo.entry;
                add_link(lhsInfo.exit, ifBlock, NORMAL_EDGE);
            }
            else
            {
                result.entry = ifBlock;
            }
            ifBlock->statements.push_back(Ir::createLogicalIf(lhsInfo.value));

            BasicBlock *rhsAssign
                = newAssignBlock(logical_varref, rhsInfo.value);
            if (!rhsInfo.isPure())
            {
                add_link(ifBlock, rhsInfo.entry, TRUE_EDGE);
                add_link(rhsInfo.exit, rhsAssign, NORMAL_EDGE);
            }
            else
            {
                add_link(ifBlock, rhsAssign, TRUE_EDGE);
            }
            add_link(rhsAssign, joinBlock, NORMAL_EDGE);

            result.value = logical_varref;
            result.exit = joinBlock;
        }
    }
    else if (SgOrOp *disj = isSgOrOp(node))
    {
        ExprInfo lhsInfo = synList[SgBinaryOp_lhs_operand_i];
        ExprInfo rhsInfo = synList[SgBinaryOp_rhs_operand_i];

        if (lhsInfo.isPure() && rhsInfo.isPure())
        {
            result.value = disj;
        }
        else
        {
            BasicBlock *ifBlock = newBasicBlock();
            BasicBlock *joinBlock = newBasicBlock();
            joinBlock->statements.push_back(Ir::createIfJoin());
            SgVariableSymbol *logical = newLogicalVariable();
            SgVarRefExp *logical_varref = Ir::createVarRefExp(logical);

            BasicBlock *lhsAssign
                = newAssignBlock(logical_varref, Ir::createBoolValExp(true));
            add_link(ifBlock, lhsAssign, TRUE_EDGE);
            add_link(lhsAssign, joinBlock, NORMAL_EDGE);
            if (!lhsInfo.isPure())
            {
                result.entry = lhsInfo.entry;
                add_link(lhsInfo.exit, ifBlock, NORMAL_EDGE);
            }
            else
            {
                result.entry = ifBlock;
            }
            ifBlock->statements.push_back(Ir::createLogicalIf(lhsInfo.value));

            BasicBlock *rhsAssign
                = newAssignBlock(logical_varref, rhsInfo.value);
            if (!rhsInfo.isPure())
            {
                add_link(ifBlock, rhsInfo.entry, FALSE_EDGE);
                add_link(rhsInfo.exit, rhsAssign, NORMAL_EDGE);
            }
            else
            {
                add_link(ifBlock, rhsAssign, FALSE_EDGE);
            }
            add_link(rhsAssign, joinBlock, NORMAL_EDGE);

            result.value = logical_varref;
            result.exit = joinBlock;
        }
    }
    else if (SgConditionalExp *cond = isSgConditionalExp(node))
    {
        ExprInfo condInfo = synList[SgConditionalExp_conditional_exp];
        ExprInfo trueInfo = synList[SgConditionalExp_true_exp];
        ExprInfo falseInfo = synList[SgConditionalExp_false_exp];

        if (condInfo.isPure() && trueInfo.isPure() && falseInfo.isPure())
        {
            result.value = cond;
        }
        else if (!condInfo.isPure() && trueInfo.isPure() && falseInfo.isPure())
        {
         // If the condition is impure, but both result expressions are
         // pure, we can generate code like this:
         //     ...evaluate cond...
         //     ($cond$value ? trueExp : falseExp)
         // by simply replacing the condition expression by condInfo's value
         // expression.
            result.entry = condInfo.entry;
            result.exit = condInfo.exit;
            if (cond->get_conditional_exp() != condInfo.value)
            {
                satireReplaceChild(cond, cond->get_conditional_exp(),
                                   condInfo.value);
            }
            result.value = cond;
        }
        else
        {
            BasicBlock *ifBlock = newBasicBlock();
            ifBlock->statements.push_back(Ir::createLogicalIf(condInfo.value));
            if (!condInfo.isPure())
            {
                result.entry = condInfo.entry;
                add_link(condInfo.exit, ifBlock, NORMAL_EDGE);
            }
            else
            {
                result.entry = ifBlock;
            }

            BasicBlock *joinBlock = newBasicBlock();
            joinBlock->statements.push_back(Ir::createIfJoin());
            result.exit = joinBlock;

            SgVariableSymbol *resvalsym = newLogicalVariable();
            SgVarRefExp *resval = Ir::createVarRefExp(resvalsym);

            BasicBlock *trueAssign
                = newAssignBlock(resval, trueInfo.value);
            BasicBlock *falseAssign
                = newAssignBlock(resval, falseInfo.value);

            if (!trueInfo.isPure())
            {
                add_link(ifBlock, trueInfo.entry, TRUE_EDGE);
                add_link(trueInfo.exit, trueAssign, NORMAL_EDGE);
            }
            else
            {
                add_link(ifBlock, trueAssign, TRUE_EDGE);
            }
            add_link(trueAssign, joinBlock, NORMAL_EDGE);

            if (!falseInfo.isPure())
            {
                add_link(ifBlock, falseInfo.entry, FALSE_EDGE);
                add_link(falseInfo.exit, falseAssign, NORMAL_EDGE);
            }
            else
            {
                add_link(ifBlock, falseAssign, FALSE_EDGE);
            }
            add_link(falseAssign, joinBlock, NORMAL_EDGE);

            result.value = resval;
        }
    }
    else if (SgBinaryOp *binOp = isSgBinaryOp(node))
    {
     // replace impure children, merge
        ExprInfo lhsInfo = synList[SgBinaryOp_lhs_operand_i];
        ExprInfo rhsInfo = synList[SgBinaryOp_rhs_operand_i];
        if (!lhsInfo.isPure() && binOp->get_lhs_operand() != lhsInfo.value)
            satireReplaceChild(binOp, binOp->get_lhs_operand(), lhsInfo.value);
        if (!rhsInfo.isPure() && binOp->get_rhs_operand() != rhsInfo.value)
            satireReplaceChild(binOp, binOp->get_rhs_operand(), rhsInfo.value);
        result = merge(lhsInfo, rhsInfo, binOp);
    }
    else if (SgUnaryOp *unOp = isSgUnaryOp(node))
    {
        ExprInfo childInfo = synList[SgUnaryOp_operand_i];
        if (!childInfo.isPure() && unOp->get_operand() != childInfo.value)
        {
            result = childInfo;
            satireReplaceChild(unOp, unOp->get_operand(), childInfo.value);
        }
        result.value = unOp;
    }
    else if (isSgAssignInitializer(node))
    {
        result = synList[SgAssignInitializer_operand_i];
    }
    else if (isSgAggregateInitializer(node))
    {
        result = synList[SgAggregateInitializer_initializers];
    }
    else if (isSgVarRefExp(node) || isSgValueExp(node)
          || isSgFunctionRefExp(node) || isSgNullExpression(node))
    {
        result.value = isSgExpression(node);
    }
    else
    {
#if 0
        std::cout
            << "* in new ExprTransformer: unhandled expression "
            << node->class_name()
            << std::endl;
        std::abort();
#endif
        result.value = isSgExpression(node);
    }

    return result;
}

#if OLD_AND_UNUSED

SgExpression *
ExprTransformer::labelAndTransformExpression(SgExpression *expr,
                                             SgExpression *orig_expr)
{
    int original_node_id = node_id;
 // Label expression
    el.traverse(expr, preorder);
 // Transform expression
    traverse(expr, preorder);
 // Remember what statement this expression comes from
    if (stmt != NULL)
    {
        for (int z = original_node_id; z < node_id; ++z)
            cfg->registerStatementLabel(z, stmt);

        stmt_start = new StatementAttribute(after, POS_PRE);
     // We saved the old "after" value in stmt_end in the constructor. That
     // after block is the one that will hold the root of the transformed
     // expression. So it is really only an "after" block if the expression
     // transformer generated code -- otherwise, it is really the
     // "new_block" that will hold the expression. Therefore, if after has
     // not changed, we modify stmt_end to really be the post info of that
     // block, rather than the pre info of the successor.
        if (stmt_end->get_bb() == after)
        {
            delete stmt_end;
            stmt_end = new StatementAttribute(after, POS_POST);
        }

        if (!stmt->attributeExists("PAG statement start"))
            stmt->addNewAttribute("PAG statement start", stmt_start);
        if (!stmt->attributeExists("PAG statement end"))
            stmt->addNewAttribute("PAG statement end", stmt_end);
    }
 // Set the new expnum computed by the ExprLabeler
    expnum = el.get_expnum();

 // If root_var is set and arose from a call, we need to create a new var
 // ref exp for it; otherwise, return the original expression pointer (which
 // may point to a transformed expression). We check whether this came from
 // a call by checking if the root_var has an attribute associated with it
 // because there is no other good way to double-check whether this is a
 // call-related expression (I think).
    if (root_var != NULL && root_var->attributeExists("SATIrE: call target"))
    {
        expr = Ir::createVarRefExp(root_var);
        CallAttribute *c
            = (CallAttribute *) root_var->getAttribute("SATIrE: call target");
        assert(c != NULL);
        expr->addNewAttribute("SATIrE: call target",
                              new CallAttribute(c->call_target));
    }

    CallSiteAnnotator callSiteAnnotator(callSites);
    callSiteAnnotator.traverse(orig_expr, preorder);

    return expr;
}

#endif

int 
ExprTransformer::get_node_id() const {
  return node_id;
}

int 
ExprTransformer::get_expnum() const {
    return expnum;
}

BasicBlock*
ExprTransformer::get_after() const {
#if 0
    std::cout
        << "ExprTransformer returning after = "
        << (after != NULL ? after->id : -1)
        << " for stmt " << Ir::fragmentToString(stmt)
        << std::endl;
#endif
    return after;
}

BasicBlock*
ExprTransformer::get_last() const {
    return last;
}

BasicBlock* 
ExprTransformer::get_retval() const {
    return retval;
}

#if OLD_AND_UNUSED

void ExprTransformer::visit(SgNode *node)
{
    /*
     * This traverses the expression bottom-up (i.e., postorder) and
     * successively replaces function call and logical expressions
     * by appropriate sequences of statements.
     * Calls are unfolded as required by PAG, logical expressions
     * are taken apart to simulate the control flow forced by the
     * short-circuit operators &&, ||, and ?: (the comma operator
     * should also be implemented some day).
     */
 // GB (2008-03-10): Replaced all calls to replaceChild by
 // satireReplaceChild. This function handles SgValueExps correctly.
    if (SgThisExp* thisExp=isSgThisExp(node))
    {
   // GB (2008-05-26): The this pointer is now stored in the procedure.
   // SgVarRefExp* varRefExp=Ir::createVarRefExp("this",thisExp->get_type());
      SgVarRefExp* varRefExp = (*cfg->procedures)[procnum]->this_exp;
      satireReplaceChild(node->get_parent(), node, varRefExp);
    }
    else if (isSgFunctionCallExp(node))
    {
        SgFunctionCallExp *call = isSgFunctionCallExp(node);
        SgExpression *call_target_expr = call->get_function();
        std::string *name = find_func_name(call);
        const std::vector<CallBlock *> *entries = find_entries(call);
        SgExpressionPtrList elist;
        if (name == NULL)
            name = new std::string("unknown_func");

        if (entries != NULL && !entries->empty()) {
          Procedure *p = (*cfg->procedures)[entries->front()->procnum];
       // SgInitializedNamePtrList params = p->params->get_args();
          SgInitializedNamePtrList default_params = p->default_params->get_args();
          SgExpressionPtrList &alist
            = call->get_args()->get_expressions();
          SgInitializedNamePtrList::const_iterator ni = default_params.begin();
          SgExpressionPtrList::const_iterator ei;
          /* if this is a member function, put the this pointer as
           * first argument; this does not appear explicitly
           * anywhere */
          if (find_called_memberfunc(call->get_function())) {
         // GB (2008-03-05): calling_object_address returns the expression
         // that we assign to the "this" pointer. We pass a null pointer
         // constant, that is, a valid SgExpression that represents a null
         // pointer, if the member function is static.
         // calling_object_address should never return NULL since every
         // member function call is either static or associated with an
         // object the member function is invoked on.
            SgExpression *e = calling_object_address(call->get_function());
            if (e != NULL) {
              elist.push_back(e);
            } else {
              std::cout << __FILE__ << ":" << __LINE__
                  << ": unknown object in member function call:" << std::endl
                  << dumpTreeFragmentToString(call)
                  << std::endl
                  << "'" << Ir::fragmentToString(call) << "'" << std::endl;
              exit(EXIT_FAILURE);
            }
          }
       // Add the argument expressions to the list.
          for (ei = alist.begin() ; ei != alist.end(); ++ei) {
            elist.push_back(*ei);
            if (ni != default_params.end())
                ++ni;
          }
       // If ni is not at the end of params, the function has variadic
       // arguments or default arguments. If the function is variadic, we
       // don't need to do anything else, just ignore the ellipse parameter.
       // If the function has default arguments, add their initializers as
       // explicit arguments to the call.
          while (ni != default_params.end()) {
            if (*ni != NULL)
            {
             // GB (2008-05-14): The default arguments come from the
             // default_params list, not from params anymore.
                SgInitializedName *initname = *ni;
                if (isSgAssignInitializer(initname->get_initptr()))
                {
                    elist.push_back(
                            isSgAssignInitializer(initname->get_initptr())
                            ->get_operand_i());
                }
             // GB (2008-04-08): Implemented the ellipse case.
                else if (!isSgTypeEllipse(initname->get_type()))
                {
                    std::cerr << "ICFG builder error: "
                        << __FILE__ << ":" << __LINE__
                        << ": not enough parameters in function call: "
                        << Ir::fragmentToString(call)
                        << std::endl;
                    abort();
                }
            }
            ++ni;
          }
        } else {
          /* evaluate args for external functions */
          SgExpressionPtrList &alist
            = call->get_args()->get_expressions();
          SgExpressionPtrList::const_iterator ei;
          for (ei = alist.begin(); ei != alist.end(); ++ei)
            elist.push_back(*ei);
        }
        /*
         * create:
         * 1. blocks for argument assignments
         * 2. a call block
         * 3. a return block
         * 4. a block for return value assignment
         */
        BasicBlock *first_arg_block = NULL, *last_arg_block = NULL;
        if (!elist.empty())
        {
            int i;
            BasicBlock *prev = NULL;
            for (i = 0; i < elist.size(); i++)
            {
                BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
                cfg->nodes.push_back(b);
                b->call_target = call_target_expr;
                b->call_index = i;
                if (first_arg_block == NULL)
                    first_arg_block = b;
                if (prev != NULL)
                    add_link(prev, b, NORMAL_EDGE);
                prev = b;
            }
            last_arg_block = prev;
        }
        BasicBlock *retval_block;
        /* FIXME: if no retval_block is set, links are wrong */
        if (true || !isSgTypeVoid(call->get_type()))
        {
            retval_block = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(retval_block);
            retval_block->call_target = call_target_expr;
        }
        else
            retval_block = NULL;
        CallBlock *call_block = NULL, *return_block = NULL;
        CallBlock *ext_call_block = NULL, *ext_return_block = NULL;
        ExternalCall *external_call = NULL;
        ExternalReturn *external_return = NULL;
        if (entries != NULL && !entries->empty())
        {
            call_block = new CallBlock(node_id++, CALL, procnum,
                    new std::vector<SgVariableSymbol *>()
                    /*entries->front()->paramlist*/, *name);
            callSites.push_back(call_block);
            return_block = new CallBlock(node_id++, RETURN,
                    procnum, new std::vector<SgVariableSymbol *>()
                    /*entries->front()->paramlist*/,
                    *name);
            cfg->nodes.push_back(call_block);
            cfg->calls.push_back(call_block);
            cfg->nodes.push_back(return_block);
            cfg->returns.push_back(return_block);
            call_block->partner = return_block;
            return_block->partner = call_block;
            call_block->call_target = call_target_expr;
            cfg->call_target_call_block[call_target_expr] = call_block;
            return_block->call_target = call_target_expr;

            /* set links */
            std::vector<CallBlock *>::const_iterator i;
            if (last_arg_block != NULL)
                add_link(last_arg_block, call_block, NORMAL_EDGE);
            for (i = entries->begin(); i != entries->end(); ++i)
            {
                add_link(call_block, *i, CALL_EDGE);
                add_link((*i)->partner, return_block, RETURN_EDGE);
            }
            add_link(call_block, return_block, LOCAL);
            if (retval_block != NULL)
            {
                add_link(return_block, retval_block, NORMAL_EDGE);
                add_link(retval_block, after, NORMAL_EDGE);
                retval = retval_block;
            }
            else
            {
                add_link(return_block, after, NORMAL_EDGE);
                retval = return_block;
            }
            after = call_block;
            last = retval;
        }
        else
        {
      /* external call */
            ext_call_block
                = new CallBlock(node_id++, CALL, procnum,
                        new std::vector<SgVariableSymbol *>(), "<unknown function>",
                        /* add call stmt = */ false);
            callSites.push_back(ext_call_block);
            cfg->nodes.push_back(ext_call_block);
            cfg->calls.push_back(ext_call_block);
            ext_call_block->call_target = call_target_expr;
            cfg->call_target_call_block[call_target_expr] = ext_call_block;
         // GB (2007-10-23): This now records the expression referring to
         // the called function and the parameter list. Because the
         // parameter list has not been computed yet, pass an empty dummy
         // list for now.
            external_call = 
                    Ir::createExternalCall(call->get_function(),
                                           new std::vector<SgVariableSymbol *>,
                                           call->get_type());
            ext_call_block->statements.push_front(external_call);

         // GB (2008-04-23): Added external return blocks.
            ext_return_block
                = new CallBlock(node_id++, RETURN, procnum,
                        new std::vector<SgVariableSymbol *>(), "<unknown function>",
                        /* add call stmt = */ false);
            cfg->nodes.push_back(ext_return_block);
            cfg->returns.push_back(ext_return_block);
            ext_return_block->call_target = call_target_expr;
            external_return =
                    Ir::createExternalReturn(call->get_function(),
                                             new std::vector<SgVariableSymbol *>,
                                             call->get_type());
            ext_return_block->statements.push_front(external_return);

            ext_call_block->partner = ext_return_block;
            ext_return_block->partner = ext_call_block;

            /* set links */
         // GB (2009-04-17): Usually, we add an edge from the external call
         // to the return. However, for some functions, we (think we) know
         // that they never return. This can be modeled by simply omitting
         // the edge in these cases.
            bool functionMayReturn = true;
            if (name != NULL && (*name == "exit"
                              || *name == "abort"
                              /*|| *name == "__assert_fail"*/))
            {
                functionMayReturn = false;
            }

            if (functionMayReturn)
                add_link(ext_call_block, ext_return_block, LOCAL);

            if (last_arg_block != NULL)
                add_link(last_arg_block, ext_call_block, NORMAL_EDGE);
            if (retval_block != NULL)
            {
             // add_link(call_block, retval_block, NORMAL_EDGE);
                add_link(ext_return_block, retval_block, NORMAL_EDGE);
                add_link(retval_block, after, NORMAL_EDGE);
                retval = retval_block;
            }
            else
            {
             // add_link(call_block, after, NORMAL_EDGE);
                add_link(ext_return_block, after, NORMAL_EDGE);
                retval = ext_call_block;
            }
            after = ext_call_block;
            last = retval;
        }
        /* fill blocks */
        if (first_arg_block != NULL)
        {
         // GB (2008-03-26): Generate the "$this" parameter of the function
         // only if this is a non-external member function call.
            bool generateThisParam
                = !external_call && find_called_memberfunc(call->get_function());
            std::vector<SgVariableSymbol *> *call_params =
                evaluate_arguments(*name, elist, first_arg_block,
                    generateThisParam);
            std::vector<SgVariableSymbol *> *ret_params =
                new std::vector<SgVariableSymbol *>(*call_params);
            std::vector<SgVariableSymbol *> *ext_params =
                new std::vector<SgVariableSymbol *>(*call_params);
         // Set the parameter list for whatever kind of CFG nodes we
         // computed above.
            if (call_block != NULL)
            {
                call_block->set_params(call_params);
                call_block->stmt->update_infolabel();
            }
            else
                delete call_params;
            if (return_block != NULL)
            {
                return_block->set_params(ret_params);
                return_block->stmt->update_infolabel();
            }
            else
                delete ret_params;
            if (external_call != NULL)
            {
                external_call->set_params(ext_params);
             // external_call != NULL iff external_return != NULL
                external_return->set_params(new std::vector<SgVariableSymbol *>(*ext_params));
             // ... iff ext_call_block != NULL iff ext_return_block != NULL
                ext_call_block->set_params(new std::vector<SgVariableSymbol *>(*ext_params));
             // ext_call_block->stmt->update_infolabel();
                ext_return_block->set_params(new std::vector<SgVariableSymbol *>(*ext_params));
             // ext_return_block->stmt->update_infolabel();
            }
            else
                delete ext_params;

        }
        /* replace call by its result */
        if (retval_block != NULL)
            assign_retval(*name, call, retval_block);
        if (first_arg_block != NULL)
            after = first_arg_block;
     // GB (2008-05-05): ExprTransformer should never modify expnum; it
     // doesn't even use it! Everything is done via attributes.
     // expnum++;

        delete entries;
        delete name;
    }
    else if (isSgConstructorInitializer(node)) {
      SgConstructorInitializer* ci = isSgConstructorInitializer(node);

      SgClassDefinition* class_type = (ci->get_class_decl()
                       ? ci->get_class_decl()->get_definition() : NULL);
      std::vector<CallBlock *> blocks(0);
      std::string name = "";
      std::string mangled_name = "";
   // GB (2008-04-29): ROSE generates SgConstructorInitializer nodes for new
   // expressions that allocate (arrays of) basic types. But in these cases,
   // we don't want to generate a constructor call, because basic types are
   // not constructed on allocation. Thus, we "bail out", i.e., bypass the
   // whole call generation stuff.
      bool bailOut = false;
      if (SgNewExp *ne = isSgNewExp(ci->get_parent()))
      {
          SgType *type = ne->get_type();
       // These lines are copied from ExprLabeler.h. Keep them consistent!
          if (isSgArrayType(type))
              type = isSgArrayType(type)->get_base_type();
          if (isSgPointerType(type))
              type = isSgPointerType(type)->get_base_type();
          if (!isSgNamedType(type))
          {
#if 0
              std::cout << "not generating constructor call for type "
                  << type->class_name() << " (" << Ir::fragmentToString(type) << ")"
                  << std::endl;
#endif
              bailOut = true;
          }
      }
      if (!bailOut) {
        if (ci->get_declaration() != NULL) {
          name = ci->get_declaration()->get_name().str();
          mangled_name = ci->get_declaration()->get_mangled_name().str();
          /* find constructor implementations */
          int num = 0;
          std::deque<Procedure *>::const_iterator i;
       // GB (2008-03-13): Default constructors can be called without
       // parentheses, in which case the mangled name of the call is different
       // from the mangled name of the constructor that is called. We thus
       // make a distinction between default and non-default constructors. For
       // non-default constructors, comparing the mangled name is fine; for
       // default constructors, we look at the non-mangled name and the number
       // of args.
       // All of this should be handled by some much smarter logic based an
       // defining declarations or something.
          if (ci->get_declaration()->get_args().empty() && ci->get_args()->get_expressions().size() == 1) {
           // GB (2008-03-27): This is a fun case: The constructor
           // initializer's declaration has no arguments, but the call has one
           // argument. This appears to be the case for default copy
           // constructors. Since the copy constructor is default, we don't
           // have a definition, so we want the call to be external. Thus we
           // simply do: Nothing! The blocks list will remain empty, and an
           // external constructor call will be generated below.

#if 0
              std::cout << " ***" << std::endl;
              std::cout << " *** arg numbers do not match! ***" << std::endl;
              std::cout << " *** constructor: " << name << "/" << mangled_name << std::endl;
              std::cout << " *** params: " << ci->get_declaration()->get_parameterList()->get_args().size() << std::endl;
              std::cout << " *** arg: " << Ir::fragmentToString(ci->get_args()->get_expressions().front()) << std::endl;
              std::cout << " ***" << std::endl;
#endif
          } else if (ci->get_declaration()->get_args().empty()) {
#if 0
              for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
                std::string i_name = (*i)->name;
                if (name == i_name && (*i)->decl->get_args().empty()) {
                  blocks.push_back((*cfg->procedures)[num]->entry);
                }
                num++;
              }
#else
              std::multimap<std::string, Procedure *>::iterator first = cfg->proc_map.lower_bound(name);
              std::multimap<std::string, Procedure *>::iterator last = cfg->proc_map.upper_bound(name);
              std::multimap<std::string, Procedure *>::iterator i;
              for (i = first; i != last; ++i)
              {
                  Procedure *p = i->second;
                  if (p->decl->get_args().empty())
                      blocks.push_back(p->entry);
              }
#endif
          } else {
#if 0
              for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
                std::string i_mangled_name = (*i)->mangled_name;
                if (mangled_name == i_mangled_name) {
                  blocks.push_back((*cfg->procedures)[num]->entry);
                }
                num++;
              }
#else
              std::multimap<std::string, Procedure *>::iterator first = cfg->mangled_proc_map.lower_bound(mangled_name);
              std::multimap<std::string, Procedure *>::iterator last = cfg->mangled_proc_map.upper_bound(mangled_name);
              std::multimap<std::string, Procedure *>::iterator i;
              for (i = first; i != last; ++i)
              {
                  Procedure *p = i->second;
                  blocks.push_back(p->entry);
              }
#endif
          }
       // GB (2008-03-13): Overloading must be uniquely resolvable.
          if (blocks.size() > 1) {
           // GB (2008-03-27): We cannot make this an error (yet?). Some STL
           // headers define constructors and maybe even functions; if such
           // headers are included several times, we get this error, although
           // the implementations are the same. This is yet another instance
           // of the problem that identical sub-ASTs are not shared. So we
           // let this go for now. Maybe in the future the ProcTraversal could
           // be made to only traverse unique function definitions from the
           // AST matcher's hash table...
#if 0
              std::cout << __FILE__ << ":" << __LINE__
                  << ": error during ICFG construction: ";
              std::cout << "found more than one "
                  << "constructor implementation for initializer '"
                  << Ir::fragmentToString(ci) << "'" << std::endl;
              std::cout << "procedures:";
              std::vector<CallBlock *>::iterator block;
              for (block = blocks.begin(); block != blocks.end(); ++block) {
                  std::cout << " " << (*block)->procnum;
              }
              std::cout << std::endl;
              std::exit(EXIT_FAILURE);
#endif
          }
        }
     // GB (2008-03-25): Oops, we missed setting the type for implicit
     // constructor calls (copy constructors).
        if (name == "" && isSgNamedType(ci->get_type())) {
            SgNamedType *t = isSgNamedType(ci->get_type());
            name = t->get_name().str();
        }

        /* setup argument expressions */
        SgExpressionPtrList elist;
        Procedure *p = NULL;
     // SgInitializedNamePtrList params;
        SgInitializedNamePtrList default_params;
        SgExpressionPtrList& alist = ci->get_args()->get_expressions();
        SgInitializedNamePtrList::const_iterator ni;
        SgExpressionPtrList::const_iterator ei;
        if (!blocks.empty()) {
          p = (*cfg->procedures)[blocks.front()->procnum];
       // params = p->params->get_args();
          default_params = p->default_params->get_args();
          ni = default_params.begin();
        }
        if (SgInitializedName* initializedName=isSgInitializedName(ci->get_parent())) {
          /* some member is initialized, pass the address
           * of the object as this pointer */
          //initializedName->set_file_info(FILEINFO);
          SgVarRefExp *ref = Ir::createVarRefExp(initializedName);
          SgAddressOfOp* a = Ir::createAddressOfOp(ref,Ir::createPointerType(ref->get_type()));
          elist.push_back(a);
        } else if (SgNewExp* newExp0=isSgNewExp(ci->get_parent())) {
          SgType *t = newExp0->get_type();
          if (isSgPointerType(t))
            t = isSgPointerType(t)->get_base_type();
          if (isSgNamedType(t))
            name = isSgNamedType(t)->get_name();

          elist.push_back(newExp0);

          if (!ci->attributeExists("return variable"))
          {
              std::cerr << __FILE__ << ":" << __LINE__
                  << ": new expression " << Ir::fragmentToString(newExp0)
                  << " has no return var attribute; bailOut = " << bailOut
                  << std::endl;
              std::cerr << "type: " << newExp0->get_type()->class_name()
                  << std::endl;
           // If we got into this mess, the following getAttribute call will abort.
          }
          RetvalAttribute *ra = (RetvalAttribute *) ci->getAttribute("return variable");

       // SgVariableSymbol *var = Ir::createVariableSymbol(ra->get_str(),newExp0->get_type());
          SgVariableSymbol *var = ra->get_variable_symbol();

          if (isSgExpression(newExp0->get_parent())) {
            SgVarRefExp* varRefExp=Ir::createVarRefExp(var);
            satireReplaceChild(newExp0->get_parent(),newExp0,varRefExp);
          }
        } else {
          RetvalAttribute* ra = (RetvalAttribute *) ci->getAttribute("anonymous variable");
       // SgVarRefExp* ref = Ir::createVarRefExp(ra->get_str(), ci->get_type());
          SgVarRefExp* ref = Ir::createVarRefExp(ra->get_variable_symbol());
          elist.push_back(Ir::createAddressOfOp(ref, Ir::createPointerType(ref->get_type())));
        }
        if (!blocks.empty() && default_params.size() < alist.size()) {
            std::cout << __FILE__ << ":" << __LINE__
                << ": error during ICFG construction: "
                << "constructor has more arguments than parameters!"
                << std::endl;
#if 0
            std::cout
                << "function call: "
                << Ir::fragmentToString(ci->get_parent()) << std::endl
                << "               "
                << dumpTreeFragmentToString(ci->get_parent()) << std::endl;
            std::cout
                << "candicate function: "
                << (void *) p
                << " " << p->name << " (" << p->mangled_name << ")"
                << std::endl;
            std::cout << "params: [";
            SgInitializedNamePtrList::iterator pi;
            for (pi = params.begin(); pi != params.end(); ++pi) {
                std::cout << Ir::fragmentToString(*pi);
                if (pi + 1 != params.end())
                    std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            std::cout << "alist:  [";
            SgExpressionPtrList::iterator ai;
            for (ai = alist.begin(); ai != alist.end(); ++ai) {
                std::cout << Ir::fragmentToString(*ai);
                if (ai + 1 != alist.end())
                    std::cout << ", ";
            }
            std::cout << "]" << std::endl;
#endif
            exit(EXIT_FAILURE);
        }
        for (ei = alist.begin(); ei != alist.end(); ++ei) {
          elist.push_back(*ei);
          if (!blocks.empty() && ni != default_params.end())
            ++ni;
        }
        if (!blocks.empty()) {
          while (ni != default_params.end()) {
            if (*ni != NULL)
            {
             // GB (2008-04-29): This used to cast the initialized name itself
             // to an initializer, which did not make sense at all... also,
             // cleaned up the error messages.
                SgInitializedName *initname = *ni;
                SgInitializer *initializer = initname->get_initptr();
                if (isSgAssignInitializer(initializer))
                {
                    elist.push_back(isSgAssignInitializer(initializer)
                      ->get_operand_i());
                }
#if 0
             // GB (2008-03-12): This cannot be right. It adds the parameter
             // name (i.e. the name of a variable internal to the called
             // function) to the list of function call arguments.
                else if (isSgInitializedName(*ni))
                    elist.push_back(Ir::createVarRefExp(isSgInitializedName(*ni)));
#endif
                else if (initializer != NULL)
                {
                    std::cerr
                        << __FILE__ << ":" << __LINE__ << ": error: "
                        << "found initializer of type: " << initializer->class_name()
                        << " in constructor call with default arguments (?)"
                        << ", this is not handled yet" << std::endl;
                    exit(EXIT_FAILURE);
                }
                else
                {
                    std::cerr
                        << __FILE__ << ":" << __LINE__ << ": error: "
                        << "found NULL initializer in constructor call: ";
                    Sg_File_Info *where = ci->get_file_info();
                    std::cerr
                        << where->get_filenameString() << ":"
                        << where->get_line() << ":" << where->get_col() << ": "
                        << Ir::fragmentToString(ci->get_parent()) << std::endl
                        << "Giving up, sorry." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            ++ni;
          }
        }
        BasicBlock *first_arg_block = NULL, *last_arg_block = NULL;
        if (!elist.empty()) {
          int i;
          BasicBlock *prev = NULL;
          for (i = 0; i < elist.size(); i++) {
            BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(b);
            if (first_arg_block == NULL)
              first_arg_block = b;
            if (prev != NULL)
              add_link(prev, b, NORMAL_EDGE);
            prev = b;
          }
          last_arg_block = prev;
        }
        /* FIXME: is this correct? */
        BasicBlock *retval_block = NULL;
        CallBlock *call_block = NULL, *return_block = NULL;
        if (!blocks.empty()) {
          call_block
              = new CallBlock(node_id++, CALL, procnum,
                              new std::vector<SgVariableSymbol *>(
                                  *blocks.front()->get_params()),
                              name);
          callSites.push_back(call_block);
          return_block
              = new CallBlock(node_id++, RETURN, procnum,
                              new std::vector<SgVariableSymbol *>(
                                  *blocks.front()->get_params()),
                              name);
          cfg->nodes.push_back(call_block);
          cfg->calls.push_back(call_block);
          cfg->nodes.push_back(return_block);
          cfg->returns.push_back(return_block);
          call_block->partner = return_block;
          return_block->partner = call_block;

          /* set links */
          std::vector<CallBlock *> *exits = new std::vector<CallBlock *>();
          std::vector<CallBlock *>::const_iterator i;
          for (i = blocks.begin(); i != blocks.end(); ++i)
            exits->push_back((*i)->partner);
          if (last_arg_block != NULL)
            add_link(last_arg_block, call_block, NORMAL_EDGE);
          for (i = blocks.begin(); i != blocks.end(); ++i)
            add_link(call_block, *i, CALL_EDGE);
          add_link(call_block, return_block, LOCAL);
          for (i = exits->begin(); i != exits->end(); ++i)
            add_link(*i, return_block, RETURN_EDGE);
          delete exits;
          if (retval_block != NULL) {
            add_link(return_block, retval_block, NORMAL_EDGE);
            add_link(retval_block, after, NORMAL_EDGE);
            retval = retval_block;
          } else {
            add_link(return_block, after, NORMAL_EDGE);
            retval = return_block;
          }
          after = call_block;
          last = retval;
        } else {
          /* call to external constructor */
          BasicBlock *call_block
            = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(call_block);
          call_block->statements.push_front(
                  Ir::createConstructorCall(name, ci->get_type()));

          /* set links */
          if (last_arg_block != NULL)
            add_link(last_arg_block, call_block, NORMAL_EDGE);
          if (retval_block != NULL) {
            add_link(call_block, retval_block, NORMAL_EDGE);
            add_link(retval_block, after, NORMAL_EDGE);
            retval = retval_block;
          } else {
            add_link(call_block, after, NORMAL_EDGE);
            retval = call_block;
          }
          after = call_block;
          last = retval;
        }
        /* fill blocks */
        if (first_arg_block != NULL) {
          std::vector<SgVariableSymbol *> *call_params =
            evaluate_arguments(name, elist, first_arg_block, true);
          std::vector<SgVariableSymbol *> *ret_params =
              new std::vector<SgVariableSymbol *>(*call_params);
          if (call_block != NULL) {
            call_block->set_params(call_params);
            call_block->stmt->update_infolabel();
          }
          else
              delete call_params;
       // GB (2008-03-12): We had forgotten to update the list of variables in
       // the return block.
          if (return_block != NULL) {
         // GB (2008-03-13): If this is a constructor call that resulted from
         // application of the 'new' operator, we need to remove the $A$this
         // pointer from the list of variables in the ReturnStmt. The reason
         // is that this pointer is used one more time in the subsequent
         // statement; two occurrences break the general rule of exactly one
         // use of temporary variables.
            if (isSgNewExp(ci->get_parent())) {
                std::vector<SgVariableSymbol *> *pop_params =
                    new std::vector<SgVariableSymbol *>(
                            ret_params->begin()+1, ret_params->end());
                return_block->set_params(pop_params);
                delete ret_params;
            } else {
                return_block->set_params(ret_params);
            }
            return_block->stmt->update_infolabel();
          }
          else
              delete ret_params;
        }
        /* replace call by its result */
        // if (retval_block != NULL)
        //     assign_retval(name, call, retval_block);
        if (first_arg_block != NULL)
          after = first_arg_block;
     // GB (2008-05-05): ExprTransformer should never modify expnum; it
     // doesn't even use it! Everything is done via attributes.
     // expnum++;
      }
    }
    else if (isSgDeleteExp(node)) {
      SgDeleteExp *de = isSgDeleteExp(node);
      SgPointerType *type = isSgPointerType(de->get_variable()->get_type());
   // GB (2008-03-17): Added a check for more than one layer of pointers.
   // We shouldn't generate destructor calls for those.
      if (!de->get_is_array() && type && !isSgPointerType(type->get_base_type())) {
#if 0
        SgType *delete_type = isSgPointerType(de->get_variable()
                          ->get_type())->get_base_type();
        while (isSgTypedefType(delete_type))
          delete_type = isSgTypedefType(delete_type)->get_base_type();
#endif
        SgType *delete_type = type->findBaseType();
        SgClassType *ct = isSgClassType(delete_type);
        std::string class_name(ct->get_name().str());
        // std::string destructor_name = class_name + "::~" + class_name;
        // std::string this_var_name
        //    = std::string() + "$~" + class_name + "$this";
        const std::vector<CallBlock *> *d_entries
          = find_destructor_entries(ct);
        std::vector<std::string> *d_class_names = find_destructor_names(ct);
     // std::vector<std::string> *d_this_names
     //   = find_destructor_this_names(ct);
        std::vector<std::string>::iterator destr_name
          = d_class_names->begin();
     // std::vector<std::string>::iterator this_name
     //   = d_this_names->begin();
        if (d_entries != NULL && !d_entries->empty()) {
          std::vector<BasicBlock *> afters, lasts;
          std::vector<CallBlock *>::const_iterator d;
          for (d = d_entries->begin(); d != d_entries->end(); ++d) {
            SgVariableSymbol *this_var_sym
           // = Ir::createVariableSymbol(*this_name++, de->get_variable()->get_type());
              = cfg->global_this_variable_symbol;

            std::string destructor_name = *destr_name++;
            CallBlock *call_block = new CallBlock(node_id++, CALL,
                          procnum, NULL, destructor_name);
            callSites.push_back(call_block);
            CallBlock *return_block = new CallBlock(node_id++, RETURN,
                            procnum, NULL, destructor_name);
            cfg->nodes.push_back(call_block);
            cfg->calls.push_back(call_block);
            cfg->nodes.push_back(return_block);
            cfg->returns.push_back(return_block);
            call_block->partner = return_block;
            return_block->partner = call_block;
            BasicBlock *this_block
              = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(this_block);
            this_block->statements.push_back(Ir::createArgumentAssignment(Ir::createVarRefExp(this_var_sym),
                                    de->get_variable()));
            call_block->set_params(new std::vector<SgVariableSymbol *>());
            call_block->get_params()->push_back(this_var_sym);
            return_block->set_params(new std::vector<SgVariableSymbol *>(
                        *call_block->get_params()));

            /* set links */
            add_link(this_block, call_block, NORMAL_EDGE);

            add_link(call_block, *d, CALL_EDGE);
            add_link((*d)->partner, return_block, RETURN_EDGE);
        
            add_link(call_block, return_block, LOCAL);
            // add_link(return_block, after, NORMAL_EDGE);
            afters.push_back(this_block);
            lasts.push_back(return_block);
            // after = this_block;
                    // last = return_block;
          }
          // after: kill this_vars
          BasicBlock* kill_this_vars = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(kill_this_vars);
          std::vector<SgVariableSymbol *>* this_syms = new std::vector<SgVariableSymbol *>();
#if 0
          std::vector<std::string>::iterator dtn;
          for (dtn = d_this_names->begin(); dtn != d_this_names->end();
               ++dtn) {
            this_syms->push_back(Ir::createVariableSymbol(*dtn,Ir::createClassType()));
          }
#else
       // kill only the global this variable
          this_syms->push_back(cfg->global_this_variable_symbol);
#endif
          kill_this_vars->statements.push_back(Ir::createUndeclareStmt(this_syms));
          add_link(kill_this_vars, after, NORMAL_EDGE);
          after = kill_this_vars;
          // set links
          if (afters.size() > 1) {
            BasicBlock *afterblock
              = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(afterblock);
            afterblock->statements.push_back(Ir::createNullStatement());
            std::vector<BasicBlock *>::iterator ai, li;
            ai = afters.begin();
            li = lasts.begin();
            while (ai != afters.end() && li != lasts.end()) {
              add_link(afterblock, *ai++, NORMAL_EDGE);
              add_link(*li++, after, NORMAL_EDGE);
            }
            after = afterblock;
          } else {
            add_link(*lasts.begin(), after, NORMAL_EDGE);
            after = *afters.begin();
          }
        } else {
          /* No destructor found. Generating an external call
           * (there might be a destructor whose implementation
           * is not accessible to us). */
          BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(b);
          b->statements.push_front(Ir::createDestructorCall(class_name, ct));
          add_link(b, after, NORMAL_EDGE);
          after = b;
          last = b;
        }
      }
    } else if (isSgAndOp(node) || isSgOrOp(node)) {
      SgBinaryOp *logical_op = isSgBinaryOp(node);
      RetvalAttribute *varnameattr
        = (RetvalAttribute *) logical_op->getAttribute("logical variable");
   // SgVariableSymbol *var = Ir::createVariableSymbol(varnameattr->get_str(),
   //                          logical_op->get_type());
      SgVariableSymbol *var = varnameattr->get_variable_symbol();
      
      BasicBlock *if_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(if_block);
      if (logical_op->get_lhs_operand_i()->attributeExists("logical variable")) {
          RetvalAttribute* vna = (RetvalAttribute *) logical_op->get_lhs_operand_i()->getAttribute("logical variable");
       // SgVarRefExp* varexp = Ir::createVarRefExp(vna->get_str(),logical_op->get_lhs_operand_i()->get_type());
          SgVarRefExp* varexp = Ir::createVarRefExp(vna->get_variable_symbol());
          LogicalIf* logicalIf=Ir::createLogicalIf(varexp);
          if_block->statements.push_front(logicalIf);
      } else {
          LogicalIf* logicalIf= Ir::createLogicalIf(logical_op->get_lhs_operand_i());
          if_block->statements.push_front(logicalIf);
      }

      BasicBlock *t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(t_block);
      BasicBlock *f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(f_block);
   // GB (2007-10-23): Create blocks for a nested branch to evaluate the rhs
   // operand. These blocks are linked below; they are successors of the
   // true or the false branch depending on whether the operator is && or ||.
      BasicBlock *nested_t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(nested_t_block);
      nested_t_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                                                    Ir::createBoolValExp(true))));
      BasicBlock *nested_f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(nested_f_block);
      nested_f_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                                                    Ir::createBoolValExp(false))));

   // GB (2007-10-23): Compute the rhs operand of the logical operation.
   // This is the expression in the AST or a reference to the logical
   // variable associated with that expression, if any.
      SgExpression *rhs_operand = logical_op->get_rhs_operand_i();
      if (rhs_operand->attributeExists("logical variable"))
      {
          RetvalAttribute* vna = (RetvalAttribute *) rhs_operand->getAttribute("logical variable");
       // rhs_operand = Ir::createVarRefExp(vna->get_str(), rhs_operand->get_type());
          rhs_operand = Ir::createVarRefExp(vna->get_variable_symbol());
      }
      
      if(isSgAndOp(logical_op)) {
        t_block->statements.push_front(Ir::createLogicalIf(rhs_operand));
        add_link(t_block, nested_t_block, TRUE_EDGE);
        add_link(t_block, nested_f_block, FALSE_EDGE);
        
        SgAssignOp* assignOp2=Ir::createAssignOp(Ir::createVarRefExp(var),Ir::createBoolValExp(false));
        f_block->statements.push_front(Ir::createExprStatement(assignOp2));
        add_link(f_block, after, NORMAL_EDGE);
      } else if(isSgOrOp(logical_op)) {
        t_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                                                                  Ir::createBoolValExp(true))));
        add_link(t_block, after, NORMAL_EDGE);

        f_block->statements.push_front(Ir::createLogicalIf(rhs_operand));
        add_link(f_block, nested_t_block, TRUE_EDGE);
        add_link(f_block, nested_f_block, FALSE_EDGE);
      } else {
        assert(false); // impossible if outer 'if' remains unchanged
      }
      
      add_link(if_block, t_block, TRUE_EDGE);
      add_link(if_block, f_block, FALSE_EDGE);
      add_link(nested_t_block, after, NORMAL_EDGE);
      add_link(nested_f_block, after, NORMAL_EDGE);
      after = if_block;
      
      if (isSgExpression(logical_op->get_parent())) {
        satireReplaceChild(logical_op->get_parent(), logical_op,
                           Ir::createVarRefExp(var));
      }
   // GB (2008-05-05): ExprTransformer should never modify expnum; it
   // doesn't even use it! Everything is done via attributes.
   // expnum++;
    } else if (isSgConditionalExp(node)) {
      SgConditionalExp* cond = isSgConditionalExp(node);
      RetvalAttribute* varnameattr
        = (RetvalAttribute *) cond->getAttribute("logical variable");
   // SgVariableSymbol* var = Ir::createVariableSymbol(varnameattr->get_str(),
   //                                                  cond->get_type());
      SgVariableSymbol* var = varnameattr->get_variable_symbol();
      BasicBlock *if_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(if_block);
      if_block->statements.push_front(Ir::createLogicalIf(
                  cond->get_conditional_exp()));

   // Create ICFG nodes for assignment of appropriate values to the logical
   // variable in the "true" and "false" branches.
      BasicBlock *t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(t_block);
      t_block->statements.push_front(Ir::createExprStatement(
        Ir::createAssignOp(Ir::createVarRefExp(var),cond->get_true_exp())));
      BasicBlock *f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(f_block);
      f_block->statements.push_front(Ir::createExprStatement(
        Ir::createAssignOp(Ir::createVarRefExp(var),cond->get_false_exp())));
      
   // Generate code for the "true" and "false" branches, ending with the
   // respective logical variable assignment block.
      add_link(if_block, t_block, TRUE_EDGE);
      add_link(if_block, f_block, FALSE_EDGE);

      add_link(t_block, after, NORMAL_EDGE);
      add_link(f_block, after, NORMAL_EDGE);
      after = if_block;
      
      if (isSgExpression(cond->get_parent())) {
        satireReplaceChild(cond->get_parent(), cond, Ir::createVarRefExp(var));
      }
   // GB (2008-05-05): ExprTransformer should never modify expnum; it
   // doesn't even use it! Everything is done via attributes.
   // expnum++;
    }
}

#endif

std::string
ExprTransformer::find_mangled_func_name(SgFunctionRefExp *fr) const {
  // fr->get_symbol()->get_declaration()->get_mangled_name();  
  SgDeclarationStatement* declaration= fr->get_symbol()->get_declaration();
  ROSE_ASSERT(declaration != NULL);
  SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
  ROSE_ASSERT(functionDeclaration != NULL);
  //cannot use following code because the pointer is NULL
  //SgFunctionDeclaration* nondefiningFunctionDeclaration
  //  = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
  SgName mname=functionDeclaration->get_mangled_name();
  //std::cerr<<mname.str()<<std::endl;
  return std::string(mname.str());
}

//SgName 
//ExprTransformer::find_mangled_memberf_name(SgMemberFunctionRefExp *mfr) const {
//  return mfr->get_symbol()->get_declaration()->get_mangled_name();
//}

CallBlock* 
ExprTransformer::find_entry(SgFunctionCallExp *call)
{
    /* ATTENTION: this is deprecated, do not use */
    std::cerr << "warning: "
        << "call to deprecated function ExprTransformer::find_entry"
        << std::endl;

    SgFunctionRefExp *func_ref = isSgFunctionRefExp(call->get_function());
    SgMemberFunctionRefExp *member_func_ref
        = isSgMemberFunctionRefExp(call->get_function());

    if (func_ref)
    {
        std::string *sgname = find_func_name(call);
        std::string name = (sgname != NULL ? *sgname : "unknown_func");
        int num = 0;
        std::deque<Procedure *>::const_iterator i;

        for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i)
        {
            if (name == (*i)->name)
                return (*cfg->procedures)[num]->entry;
            num++;
        }
        return NULL;
    }

    return NULL;
} 

bool
ExprTransformer::definitelyNotTheSameType(SgType *a, SgType *b) const
{
    if (a == b)
        return false;

    if (a->variantT() != b->variantT())
        return true;

    a = a->findBaseType();
    b = b->findBaseType();

    if (a == b)
        return false;

    if (a->variantT() != b->variantT())
        return true;

    if (SgNamedType *na = isSgNamedType(a))
    {
        SgNamedType *nb = isSgNamedType(b);
        const char *sa = na->get_qualified_name().str();
        const char *sb = nb->get_qualified_name().str();
        if (sa == sb || std::strcmp(sa, sb) == 0)
            return false;
        else
            return true;
    }
 // I don't think we can ever reach this point. Still, if we do, we do not
 // *definitely* know that the types are different unless we unparse them.
    return Ir::fragmentToString(a) != Ir::fragmentToString(b);
}

const std::vector<CallBlock *>*
ExprTransformer::find_entries(SgFunctionCallExp *call)
{
    std::vector<CallBlock *> *blocks = new std::vector<CallBlock *>();
    SgFunctionRefExp* func_ref = find_called_func(call->get_function());
    SgMemberFunctionRefExp* member_func_ref = find_called_memberfunc(call->get_function());
    //std::cout << "FunctionCall: FunctionRefExp:" << func_ref << ", MemberFunctionRef:" << member_func_ref << std::endl;

    if (func_ref)
    {
        std::string name = find_mangled_func_name(func_ref);
#if 0
        int num = 0;
        std::deque<Procedure *>::const_iterator i;
        std::deque<Procedure *>::const_iterator procedures_end = cfg->procedures->end();
        for (i = cfg->procedures->begin(); i != procedures_end; ++i) {
        //std::cout<<"B: "<< (char*)((*i)->mangled_name) << std::endl;
        //std::cout<<"A: "<< name <<std::endl;

            if (name == (*i)->mangled_name)
                blocks->push_back((*cfg->procedures)[num]->entry);
            num++;
        }
#else
        std::multimap<std::string, Procedure *>::iterator first = cfg->mangled_proc_map.lower_bound(name);
        std::multimap<std::string, Procedure *>::iterator last = cfg->mangled_proc_map.upper_bound(name);
        std::multimap<std::string, Procedure *>::iterator i;
        for (i = first; i != last; ++i)
        {
            Procedure *p = i->second;
         // GB (2008-07-01): Added check for static functions. If the
         // function is not declared static, call it; if it is declared
         // static, only call it if the call and the definition are in the
         // same file.
            if (p->static_file == NULL)
            {
                blocks->push_back(p->entry);
#if 0
                std::cout << "pushed non-static call to function "
                    << p->name << "/" << p->procnum << std::endl;
#endif
            }
            else
            {
             // Trace back to enclosing file. There is probably a ROSE
             // function for this...
                SgNode *n = call->get_parent();
                while (n != NULL && !isSgFile(n))
                    n = n->get_parent();
#if 0
                if (n == NULL)
                {
                    std::cout << "interesting, call '"
                        << Ir::fragmentToString(call) << "' (" << (void *) call
                        << ") is from no file" << std::endl;
                }
#endif
                SgFile *call_file = isSgFile(n);

                if (p->static_file == call_file)
                {
                    blocks->push_back(p->entry);
#if 0
                    std::cout << "pushed static call to function "
                        << p->name << "/" << p->procnum << std::endl;
#endif
                }
#if 0
                else
                {
                    std::cout << "did NOT push static call to function "
                        << p->name << "/" << p->procnum << std::endl;
                }
#endif
            }
        }
#endif
        return blocks;
    }
    else if (member_func_ref)
    {
        SgMemberFunctionDeclaration *decl
            = isSgMemberFunctionDeclaration(
                    member_func_ref->get_symbol()->get_declaration());
        assert(decl);
        SgMemberFunctionDeclaration *fnddecl
            = isSgMemberFunctionDeclaration(decl
                    ->get_firstNondefiningDeclaration());
        if (fnddecl != NULL && fnddecl != decl)
            decl = fnddecl;
        SgClassDefinition *class_type
            = isSgClassDefinition(member_func_ref
                ->get_symbol_i()->get_declaration()->get_scope());
        std::string name = member_func_ref->get_symbol()->get_name().str();
#if 0
        int num = 0;
        std::deque<Procedure *>::const_iterator i;
        std::deque<Procedure *>::const_iterator procedures_end = cfg->procedures->end();
        for (i = cfg->procedures->begin(); i != procedures_end; ++i)
#else
        std::multimap<std::string, Procedure *>::iterator first = cfg->proc_map.lower_bound(name);
        std::multimap<std::string, Procedure *>::iterator last = cfg->proc_map.upper_bound(name);
        std::multimap<std::string, Procedure *>::iterator i;
        for (i = first; i != last; ++i)
#endif
        {
            Procedure *p = i->second;
#if 0
            if (name == (*i)->name && (*i)->class_type != NULL
                    && (class_type == (*i)->class_type
                        || (decl->get_functionModifier().isVirtual()
                        && subtype_of((*i)->class_type, class_type)))) {
#else
            if (name == p->name && p->class_type != NULL
                    && (class_type == p->class_type
                        || (decl->get_functionModifier().isVirtual()
                        && subtype_of(p->class_type, class_type)))) {
#endif
             // GB (2008-03-13): OK, so the functions have the same name and
             // are on the same class, or the call is virtual and there is
             // an appropriate subtype relation. But to be sure that this is
             // the right function to call, we still have to check the
             // parameter types; the member function might be overloaded.
                bool candidate = true;
             // call_decl_args is the argument list of the declaration found
             // from the call, proc_decl_args is the argument list of the
             // declaration found from our procedure entry in the CFG.
                SgInitializedNamePtrList &call_decl_args = decl->get_args();
             // SgInitializedNamePtrList &proc_decl_args = (*i)->decl->get_args();
                SgInitializedNamePtrList &proc_decl_args = p->decl->get_args();
                if (call_decl_args.size() == proc_decl_args.size()) {
                    SgInitializedNamePtrList::iterator c = call_decl_args.begin(),
                                                       p = proc_decl_args.begin();
                    while (c != call_decl_args.end() && p != proc_decl_args.end()) {
#if 0
                     // Comparing types by comparing pointers, this is
                     // supposedly correct for ROSE as types are shared.
                        if ((*c++)->get_type() != (*p++)->get_type()) {
                            candidate = false;
                            break;
                        }
#endif
                     // GB (2008-03-18): It looks like, at least in ROSE
                     // 0.9.1a, class types are NOT always shared. I had
                     // this problem when running the ICFG builder on
                     // grato/GBackEnd.C and grato/GCocoBackEnd.C at once. I
                     // think this should be solved using Mihai Ghete's AST
                     // matcher (TODO). For now, we use the good old ugly
                     // string comparison technique.
                     // GB (2008-05-05): Trying to avoid the string stuff if
                     // at all possible...
                        SgType *ctype = (*c++)->get_type();
                        SgType *ptype = (*p++)->get_type();
#if 0
                        if (ctype != ptype
                                && (ctype->variantT() != ptype->variantT()
                                    || ctype->unparseToString() != ptype->unparseToString()))
#else
                        if (definitelyNotTheSameType(ctype, ptype))
#endif
                        {
                            candidate = false;
                            break;
                        }
                    }
                } else 
                    candidate = false;
                if (candidate) {
                 // Yippie! Looks like this function can be the target of
                 // this function call.
#if 0
                    blocks->push_back((*cfg->procedures)[num]->entry);
#else
                    blocks->push_back(p->entry);
#endif
                }
            }
         // num++;
        }
        return blocks;
    }
    delete blocks;
    return NULL;
}

std::vector<SgVariableSymbol *>*
ExprTransformer::evaluate_arguments(std::string name,
                    SgExpressionPtrList &args, 
                    BasicBlock *block,
                    bool member_func) {
  std::vector<SgVariableSymbol *> *params
    = new std::vector<SgVariableSymbol *>();
  SgExpressionPtrList::const_iterator i = args.begin();
  if (member_func) {
 // std::string varname = std::string("$") + name + "$this";
    SgExpression *new_expr = *i;
    SgVariableSymbol *varsym = 
   // Ir::createVariableSymbol(varname, (*i)->get_type());
      cfg->global_this_variable_symbol;
    params->push_back(varsym);
    block->statements.push_back(Ir::createArgumentAssignment(varsym, new_expr));
    ++i;
    if (block->successors.size() > 0) {
      Edge next = block->successors[0];
      block = next.first;
    }
  }
  int n = 0;
  for ( ; i != args.end(); ++i) {
    std::stringstream varname;
 // varname << "$" << name << "$arg_" << n++;
    
    SgExpression *new_expr = *i;
    RetvalAttribute *varnameattr
      = (*i)->attributeExists("logical variable") ?
      (RetvalAttribute *) (*i)->getAttribute("logical variable")
      : (*i)->attributeExists("return variable") ?
      (RetvalAttribute *) (*i)->getAttribute("return variable")
      : NULL;
    
    if (varnameattr != NULL) {
   // new_expr = Ir::createVarRefExp(varnameattr->get_str(),(*i)->get_type());
      new_expr = Ir::createVarRefExp(varnameattr->get_variable_symbol());
    }
 // GB (2008-06-23): Moving to a single global argument variable list.
 // SgVariableSymbol *varsym=Ir::createVariableSymbol(varname.str(), (*i)->get_type());
    if (n >= cfg->global_argument_variable_symbols.size())
    {
        varname.str("");
        varname << "$tmpvar$arg_" << n;
        SgVariableSymbol *varsym
            = Ir::createVariableSymbol(varname.str(),
                                       cfg->global_unknown_type);
        cfg->global_argument_variable_symbols.push_back(varsym);
    }
    SgVariableSymbol *varsym = cfg->global_argument_variable_symbols[n++];
    params->push_back(varsym);
    block->statements.push_back(Ir::createArgumentAssignment(varsym, new_expr));
    if (block->successors.size() > 0) {
      Edge next = block->successors[0];
      block = next.first;
    }
  }
  return params;
}

void ExprTransformer::assign_retval(
        std::string name, SgFunctionCallExp *call, BasicBlock *block)
{
 // std::stringstream retname;
 // retname << "$" << name << "$return";
    RetvalAttribute *varnameattr
        = (RetvalAttribute *) call->getAttribute("return variable");
 // SgVariableSymbol *var
 //     = Ir::createVariableSymbol(varnameattr->get_str(),call->get_type());
    SgVariableSymbol *var = varnameattr->get_variable_symbol();

 // GB (2008-06-23): Toying around with using only one global retvar
 // everywhere. This should make it easier to kill.
 // SgVariableSymbol *retvar
 //     = Ir::createVariableSymbol(retname.str(),call->get_type());
    SgVariableSymbol *retvar = cfg->global_return_variable_symbol;

 // GB (2008-10-16): Annotate the return variable and the replaced expression
 // with the function call's target. This is needed for the points-to
 // analysis, which needs to resolve which function's return value each
 // occurrence of the retvar refers to.
    SgExpression *call_target_expr = call->get_function();
    var->addNewAttribute("SATIrE: call target",
                         new CallAttribute(call_target_expr));

    ReturnAssignment *ra = Ir::createReturnAssignment(var, retvar);
 // GB (2008-11-12): Annotate the global retvar expression of the return
 // assignment as well.
    SgVarRefExp *retvar_expr = ra->get_rhsVarRefExp();
    retvar_expr->addNewAttribute("SATIrE: call target",
                                 new CallAttribute(call_target_expr));
    block->statements.push_front(ra);

    if (isSgExpression(call->get_parent()))
    {
        SgVarRefExp *replacementExpression = Ir::createVarRefExp(var);
        replacementExpression->addNewAttribute(
                "SATIrE: call target", new CallAttribute(call_target_expr));
        satireReplaceChild(call->get_parent(), call, replacementExpression);
    }
}

static 
SgMemberFunctionDeclaration*
find_dest_impl(SgClassType *ct) {
  SgClassDefinition *classdef 
    = isSgClassDefinition(isSgClassDeclaration(ct->get_declaration())->get_definition());
  if (classdef == NULL)
      return NULL;
  SgDeclarationStatementPtrList decls = classdef->get_members();
  for (SgDeclarationStatementPtrList::iterator i = decls.begin(); i != decls.end(); ++i) {
    SgMemberFunctionDeclaration* mf = isSgMemberFunctionDeclaration(*i);
    if (mf != NULL) {
      SgMemberFunctionDeclaration *fnddecl
    = isSgMemberFunctionDeclaration(mf->get_firstNondefiningDeclaration());
      if (fnddecl != NULL && fnddecl != mf)
    mf = fnddecl;
      if (mf->get_specialFunctionModifier().isDestructor())
    return mf;
    }
  }
  return NULL;
}

const std::vector<CallBlock *>*
ExprTransformer::find_destructor_entries(SgClassType *ct) {
  SgClassDefinition *cd
    = isSgClassDeclaration(ct->get_declaration())->get_definition();
  std::vector<CallBlock *> *blocks = new std::vector<CallBlock *>();

  SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
  bool virtual_destructor;
  if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
    virtual_destructor = true;
  else
    virtual_destructor = false;

  std::deque<Procedure *>::const_iterator p;
  for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p) {
    if ((*p)->class_type != NULL
    && (cd == (*p)->class_type
        || (virtual_destructor
        && subtype_of((*p)->class_type, cd)))
 // && strchr((*p)->memberf_name, '~') != NULL)
    && (*p)->memberf_name.find('~') != std::string::npos)
      blocks->push_back((*p)->entry);
  }
  return blocks;
}

std::vector<std::string>*
ExprTransformer::find_destructor_names(SgClassType *ct) {
    SgClassDefinition *cd
        = isSgClassDeclaration(ct->get_declaration())->get_definition();
    std::vector<std::string> *names = new std::vector<std::string>();

    SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
    bool virtual_destructor;
    if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
        virtual_destructor = true;
    else
        virtual_destructor = false;

    std::deque<Procedure *>::const_iterator p;
    for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p)
    {
        if ((*p)->class_type != NULL
                && (cd == (*p)->class_type
                    || (virtual_destructor
                        && subtype_of((*p)->class_type, cd)))
             // && strchr((*p)->memberf_name, '~') != NULL)
                && (*p)->memberf_name.find('~') != std::string::npos)
        {
            std::string class_name((*p)->class_type->get_declaration()
                    ->get_name().str());
            std::string destructor_name = class_name + "::~" + class_name;
            names->push_back(destructor_name);
        }
    }

    return names;
}

#if 0
std::vector<std::string>*
ExprTransformer::find_destructor_this_names(SgClassType *ct) {
    SgClassDefinition *cd
        = isSgClassDeclaration(ct->get_declaration())->get_definition();
    std::vector<std::string> *names = new std::vector<std::string>();

    SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
    bool virtual_destructor;
    if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
        virtual_destructor = true;
    else
        virtual_destructor = false;

    std::deque<Procedure *>::const_iterator p;
    for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p)
    {
        if ((*p)->class_type != NULL
                && (cd == (*p)->class_type
                    || (virtual_destructor
                        && subtype_of((*p)->class_type, cd)))
             // && strchr((*p)->memberf_name, '~') != NULL)
                && (*p)->memberf_name.find('~') != std::string::npos)
        {
            std::string class_name((*p)->class_type->get_declaration()
                    ->get_name().str());
            std::string this_var_name 
         //     = std::string() + "$~" + class_name + "$this";
            names->push_back(this_var_name);
        }
    }

    return names;
}
#endif

// GB (2008-03-10): See comment in header file satire/ExprTransformer.h
void satireReplaceChild(SgNode *parent, SgNode *from, SgNode *to)
{
    if (isSgValueExp(parent))
    {
     // Do nothing on value exps.
        return;
    }
    else
        replaceChild(parent, from, to);
}

CallSiteAnnotator::CallSiteAnnotator(std::vector<BasicBlock *> &callSites)
  : callSites(callSites), callSite(callSites.begin())
{
}

void
CallSiteAnnotator::visit(SgNode *node)
{
    if (SgFunctionCallExp *call = isSgFunctionCallExp(node))
    {
        CallSiteAttribute *attribute = new CallSiteAttribute(*callSite);
        call->addNewAttribute("SATIrE ICFG call block", attribute);
        ++callSite;
    }
}
