#include <sage3basic.h>
#include <BinaryZ3Solver.h>


namespace Rose {
namespace BinaryAnalysis {

// class method
unsigned
Z3Solver::availableLinkages() {
    unsigned retval = 0;
#ifdef ROSE_HAVE_Z3
    retval |= LM_LIBRARY;
#endif
#ifdef ROSE_Z3
    retval |= LM_EXECUTABLE;
#endif
    return retval;
}

void
Z3Solver::clearEvidence() {
#ifdef ROSE_HAVE_Z3
    ctxCses_.clear();
    ctxVarDecls_.clear();
    delete ctx_;
    ctx_ = NULL;
#endif
    SmtlibSolver::clearEvidence();
}

SmtSolver::Satisfiable
Z3Solver::satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    clearEvidence();
    Satisfiable retval = triviallySatisfiable(exprs);
    if (retval != SAT_UNKNOWN)
        return retval;

    if ((linkage() & LM_EXECUTABLE) != 0)
        return SmtSolver::satisfiable(exprs);

    requireLinkage(LM_LIBRARY);
    
#ifdef ROSE_HAVE_Z3
    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }

    delete ctx_;
    ctx_ = new z3::context;
    z3::solver solver(*ctx_);

    VariableSet vars = findVariables(exprs);
    ctxVarDecls_ = ctxVariableDeclarations(vars);
    ctxCses_ = ctxCommonSubexpressions(exprs);
    
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        solver.add(ctxExpression(expr, BOOLEAN));

    switch (solver.check()) {
        case z3::unsat:
            return SAT_NO;
        case z3::sat:
            return SAT_YES;
        case z3::unknown:
            return SAT_UNKNOWN;
    }
#endif
    ASSERT_not_reachable("library linkage accepted but ROSE_HAVE_Z3 not defined");
}

// No need to emit anything since Z3 already has a "bvxor" function.
void
Z3Solver::outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) {}

// No need to emit anything since Z3 has a full complement of signed and unsigned comparison functions.
void
Z3Solver::outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) {}

void
Z3Solver::outputExpression(std::ostream &o, const SymbolicExpr::Ptr &expr, Type needType) {
    ASSERT_not_null(expr);
    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    std::string subExprName;
    if (termNames_.getOptional(expr).assignTo(subExprName)) {
        if (BOOLEAN == needType) {
            o <<"(= " <<subExprName <<" #b1)";
        } else {
            o <<subExprName;                            // bit vector or memory state
        }
    } else if (leaf) {
        outputLeaf(o, leaf, needType);
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_ADD:
                ASSERT_require(BIT_VECTOR == needType);
                outputList(o, "bvadd", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_AND:
                ASSERT_require(BOOLEAN == needType);
                outputList(o, "and", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(BIT_VECTOR == needType);
                outputArithmeticShiftRight(o, inode);
                break;
            case SymbolicExpr::OP_BV_AND:
                ASSERT_require(BIT_VECTOR == needType);
                outputList(o, "bvand", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_OR:
                ASSERT_require(BIT_VECTOR == needType);
                outputList(o, "bvor",  inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_XOR:
                ASSERT_require(BIT_VECTOR == needType);
                outputList(o, "bvxor", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "=", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_CONCAT:
                ASSERT_require(BIT_VECTOR == needType);
                outputList(o, "concat", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EXTRACT:
                ASSERT_require(BIT_VECTOR == needType);
                outputExtract(o, inode);
                break;
            case SymbolicExpr::OP_INVERT:
                outputUnary(o, (BOOLEAN==needType?"not":"bvnot"), inode, needType);
                break;
            case SymbolicExpr::OP_ITE:
                outputIte(o, inode, needType);
                break;
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                ASSERT_require(BOOLEAN == needType);
                outputNotEqual(o, inode);
                break;
            case SymbolicExpr::OP_NEGATE:
                ASSERT_require(BIT_VECTOR == needType);
                outputUnary(o, "bvneg", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NOOP:
                outputExpression(o, SymbolicExpr::makeInteger(inode->nBits(), 0), needType);
                break;
            case SymbolicExpr::OP_OR:
                ASSERT_require(BOOLEAN == needType);
                outputList(o, "or", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_READ:
                ASSERT_require(BIT_VECTOR == needType);
                outputRead(o, inode);
                break;
            case SymbolicExpr::OP_ROL:
                ASSERT_require(BIT_VECTOR == needType);
                outputRotateLeft(o, inode);
                break;
            case SymbolicExpr::OP_ROR:
                ASSERT_require(BIT_VECTOR == needType);
                outputRotateRight(o, inode);
                break;
            case SymbolicExpr::OP_SDIV:
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:
                ASSERT_require(BIT_VECTOR == needType);
                outputSet(o, inode);
                break;
            case SymbolicExpr::OP_SEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                outputSignExtend(o, inode);
                break;
            case SymbolicExpr::OP_SLT:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvslt", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvsle", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(BIT_VECTOR == needType);
                outputShiftLeft(o, inode);
                break;
            case SymbolicExpr::OP_SHL1:
                ASSERT_require(BIT_VECTOR == needType);
                outputShiftLeft(o, inode);
                break;
            case SymbolicExpr::OP_SHR0:
                ASSERT_require(BIT_VECTOR == needType);
                outputLogicalShiftRight(o, inode);
                break;
            case SymbolicExpr::OP_SHR1:
                ASSERT_require(BIT_VECTOR == needType);
                outputLogicalShiftRight(o, inode);
                break;
            case SymbolicExpr::OP_SGE:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvsge", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvsgt", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_SMOD:
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                ASSERT_require(BIT_VECTOR == needType);
                outputMultiply(o, inode);
                break;
            case SymbolicExpr::OP_UDIV:
                ASSERT_require(BIT_VECTOR == needType);
                outputUnsignedDivide(o, inode);
                break;
            case SymbolicExpr::OP_UEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                outputUnsignedExtend(o, inode);
                break;
            case SymbolicExpr::OP_UGE:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvuge", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvugt", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvule", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "bvult", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UMOD:
                ASSERT_require(BIT_VECTOR == needType);
                outputUnsignedModulo(o, inode);
                break;
            case SymbolicExpr::OP_UMUL:
                ASSERT_require(BIT_VECTOR == needType);
                outputMultiply(o, inode);
                break;
            case SymbolicExpr::OP_WRITE:
                ASSERT_require(MEM_STATE == needType);
                outputWrite(o, inode);
                break;
            case SymbolicExpr::OP_ZEROP:
                ASSERT_require(BOOLEAN == needType);
                outputZerop(o, inode);
                break;
        }
    }
}

#ifdef ROSE_HAVE_Z3
Z3Solver::VariableDeclarations
Z3Solver::ctxVariableDeclarations(const VariableSet &vars) {
    VariableDeclarations retval;
    ASSERT_not_null(ctx_);
    BOOST_FOREACH (const SymbolicExpr::LeafPtr &leaf, vars.values()) {
        ASSERT_require(leaf->isVariable() || leaf->isMemory());
        if (leaf->isScalar()) {
            z3::sort range = ctx_->bv_sort(leaf->nBits());
            z3::func_decl decl = z3::function(leaf->toString().c_str(), 0, NULL, range);
            retval.insert(leaf, decl);
        } else {
            ASSERT_require(leaf->domainWidth() > 0);
            z3::sort addr = ctx_->bv_sort(leaf->domainWidth());
            z3::sort value = ctx_->bv_sort(leaf->nBits());
            z3::sort range = ctx_->array_sort(addr, value);
            z3::func_decl decl = z3::function(leaf->toString().c_str(), 0, NULL, range);
            retval.insert(leaf, decl);
        }
    }
    return retval;
}

Z3Solver::CommonSubexpressions
Z3Solver::ctxCommonSubexpressions(const std::vector<SymbolicExpr::Ptr> &exprs) {
    CommonSubexpressions retval;
    std::vector<SymbolicExpr::Ptr> cses = findCommonSubexpressions(exprs);
    BOOST_FOREACH (const SymbolicExpr::Ptr &cse, cses)
        retval.insert(cse, ctxExpression(cse, BIT_VECTOR));
    return retval;
}

z3::expr
Z3Solver::ctxExpression(const SymbolicExpr::Ptr &expr, Type needType) {
    ASSERT_not_null(expr);
    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    ASSERT_not_null(ctx_);
    z3::expr retval(*ctx_);
    if (ctxCses_.getOptional(expr).assignTo(retval)) {
        if (BOOLEAN == needType) {
            ASSERT_require(expr->nBits() == 1);
            return retval == ctx_->bv_val(1, 1);
        } else {
            return retval;
        }
    } else if (leaf != NULL) {
        if (leaf->isNumber()) {
            if (BOOLEAN == needType) {
                ASSERT_require(expr->nBits() == 1);
                return ctx_->bool_val(leaf->toInt() != 0);
            } else if (leaf->nBits() <= 64) {
                return ctx_->bv_val((unsigned long long)leaf->toInt(), (unsigned)leaf->nBits());
            } else {
                return ctx_->bv_val(("#x" + leaf->bits().toHex()).c_str(), leaf->nBits());
            }
        } else if (leaf->isVariable()) {
            z3::func_decl decl = ctxVarDecls_.get(leaf);
            if (BOOLEAN == needType) {
                ASSERT_require(leaf->nBits() == 1);
                return decl() == ctx_->bv_val(1, 1);
            } else {
                return decl();
            }
        } else {
            ASSERT_require(leaf->isMemory());
            z3::func_decl decl = ctxVarDecls_.get(leaf);
            return decl();
        }
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_ADD:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval + ctxExpression(inode->child(i), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_AND:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BOOLEAN);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval && ctxExpression(inode->child(i), BOOLEAN);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxArithmeticShiftRight(inode);
                break;
            case SymbolicExpr::OP_BV_AND:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval & ctxExpression(inode->child(i), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_OR:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval | ctxExpression(inode->child(i), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_XOR:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval ^ ctxExpression(inode->child(i), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) == ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_CONCAT:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = z3::concat(retval, ctxExpression(inode->child(i), BIT_VECTOR));
                break;
            case SymbolicExpr::OP_EXTRACT:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxExtract(inode);
                break;
            case SymbolicExpr::OP_INVERT:
                ASSERT_require(inode->nChildren() == 1);
                if (BOOLEAN == needType) {
                    retval = !ctxExpression(inode->child(0), BOOLEAN);
                } else {
                    retval = ~ctxExpression(inode->child(0), BIT_VECTOR);
                }
                break;
            case SymbolicExpr::OP_ITE:
                ASSERT_require(inode->nChildren() == 3);
                retval = z3::ite(ctxExpression(inode->child(0), BOOLEAN),
                                 ctxExpression(inode->child(1), needType),
                                 ctxExpression(inode->child(2), needType));
                break;
            case SymbolicExpr::OP_LSSB:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) != ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NEGATE:
                ASSERT_require(BIT_VECTOR == needType);
                ASSERT_require(inode->nChildren() == 1);
                retval = -ctxExpression(inode->child(0), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NOOP:
                if (BOOLEAN == needType) {
                    retval = ctx_->bool_val(false);
                } else {
                    retval = ctx_->bv_val(0, inode->nBits());
                }
                break;
            case SymbolicExpr::OP_OR:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() >= 2);
                retval = ctxExpression(inode->child(0), BOOLEAN);
                for (size_t i = 1; i < inode->nChildren(); ++i)
                    retval = retval || ctxExpression(inode->child(i), BOOLEAN);
                break;
            case SymbolicExpr::OP_READ:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxRead(inode);
                break;
            case SymbolicExpr::OP_ROL:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxRotateLeft(inode);
                break;
            case SymbolicExpr::OP_ROR:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxRotateRight(inode);
                break;
            case SymbolicExpr::OP_SDIV:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxSet(inode);
                break;
            case SymbolicExpr::OP_SEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxSignExtend(inode);
                break;
            case SymbolicExpr::OP_SLT:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) < ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) <= ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxShiftLeft(inode);
                break;
            case SymbolicExpr::OP_SHL1:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxShiftLeft(inode);
                break;
            case SymbolicExpr::OP_SHR0:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxShiftRight(inode);
                break;
            case SymbolicExpr::OP_SHR1:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxShiftRight(inode);
                break;
            case SymbolicExpr::OP_SGE:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) >= ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = ctxExpression(inode->child(0), BIT_VECTOR) > ctxExpression(inode->child(1), BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SMOD:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxMultiply(inode);
                break;
            case SymbolicExpr::OP_UDIV:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxUnsignedDivide(inode);
                break;
            case SymbolicExpr::OP_UEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxUnsignedExtend(inode);
                break;
            case SymbolicExpr::OP_UGE:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = z3::uge(ctxExpression(inode->child(0), BIT_VECTOR), ctxExpression(inode->child(1), BIT_VECTOR));
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = z3::ugt(ctxExpression(inode->child(0), BIT_VECTOR), ctxExpression(inode->child(1), BIT_VECTOR));
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = z3::ule(ctxExpression(inode->child(0), BIT_VECTOR), ctxExpression(inode->child(1), BIT_VECTOR));
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(BOOLEAN == needType);
                ASSERT_require(inode->nChildren() == 2);
                retval = z3::ult(ctxExpression(inode->child(0), BIT_VECTOR), ctxExpression(inode->child(1), BIT_VECTOR));
                break;
            case SymbolicExpr::OP_UMOD:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxUnsignedModulo(inode);
                break;
            case SymbolicExpr::OP_UMUL:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctxMultiply(inode);
                break;
            case SymbolicExpr::OP_WRITE:
                ASSERT_require(MEM_STATE == needType);
                retval = ctxWrite(inode);
                break;
            case SymbolicExpr::OP_ZEROP:
                ASSERT_require(BOOLEAN == needType);
                retval = ctxZerop(inode);
                break;
        }
    }
    return retval;
}
#endif

// ROSE (asr amount expr) =>
// SMT-LIB (bvashr (uextend [expr.size] amount) expr)
//
// Shift amount needs to be extended or truncated to be the same width as the expression.
void
Z3Solver::outputArithmeticShiftRight(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa);
    o <<"(bvashr" <<" ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<" ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<")";
}

#ifdef ROSE_HAVE_Z3
z3::expr
Z3Solver::ctxArithmeticShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa);

    return z3::ashr(ctxExpression(expr, BIT_VECTOR), ctxExpression(sa, BIT_VECTOR));
}

z3::expr
Z3Solver::ctxExtract(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(1)->isNumber());
    size_t begin = inode->child(0)->toInt();            // low, inclusive
    size_t end = inode->child(1)->toInt();              // high, exclusive
    ASSERT_require(end > begin);
    ASSERT_require(end <= inode->child(2)->nBits());
    return ctxExpression(inode->child(2), BIT_VECTOR).extract(end-1, begin);
}

z3::expr
Z3Solver::ctxRead(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren());
    return z3::select(ctxExpression(inode->child(0), MEM_STATE), ctxExpression(inode->child(1), BIT_VECTOR));
}

z3::expr
Z3Solver::ctxRotateLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, 2*w), sa);
    return z3::shl(z3::concat(ctxExpression(expr, BIT_VECTOR), ctxExpression(expr, BIT_VECTOR)),
                   ctxExpression(sa, BIT_VECTOR)).extract(2*w-1, w);
}

z3::expr
Z3Solver::ctxRotateRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, 2*w), sa);
    return z3::lshr(z3::concat(ctxExpression(expr, BIT_VECTOR), ctxExpression(expr, BIT_VECTOR)),
                    ctxExpression(sa, BIT_VECTOR)).extract(w-1, 0);
}

z3::expr
Z3Solver::ctxSet(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(inode->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(inode);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(inode, var);
    return ctxExpression(ite, BIT_VECTOR);
}

z3::expr
Z3Solver::ctxSignExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(0)->toInt() > inode->child(1)->nBits());
    SymbolicExpr::Ptr newSize = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    size_t signBitIdx = expr->nBits() - 1;
    size_t growth = newSize->toInt() - expr->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(growth, false));

    return z3::concat(z3::ite(ctxExpression(expr, BIT_VECTOR).extract(signBitIdx, signBitIdx) == ctx_->bv_val(1, 1),
                              ~ctxExpression(zeros, BIT_VECTOR),
                              ctxExpression(zeros, BIT_VECTOR)),
                      ctxExpression(expr, BIT_VECTOR));
}

z3::expr
Z3Solver::ctxShiftLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHL0 || inode->getOperator() == SymbolicExpr::OP_SHL1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHL1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    return z3::shl(z3::concat(ctxExpression(expr, BIT_VECTOR), ctxExpression(zerosOrOnes, BIT_VECTOR)),
                   ctxExpression(sa, BIT_VECTOR))
        .extract(2*expr->nBits()-1, expr->nBits());
}

z3::expr
Z3Solver::ctxShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHL0 || inode->getOperator() == SymbolicExpr::OP_SHL1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHL1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    return z3::shl(z3::concat(ctxExpression(zerosOrOnes, BIT_VECTOR), ctxExpression(expr, BIT_VECTOR)),
                   ctxExpression(sa, BIT_VECTOR))
        .extract(expr->nBits()-1, 0);
}

z3::expr
Z3Solver::ctxMultiply(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr a = inode->child(0);
    SymbolicExpr::Ptr b = inode->child(1);
    SymbolicExpr::Ptr resultSize = SymbolicExpr::makeInteger(32, a->nBits() + b->nBits());

    SymbolicExpr::Ptr aExtended, bExtended;
    if (inode->getOperator() == SymbolicExpr::OP_SMUL) {
        aExtended = SymbolicExpr::makeSignExtend(resultSize, a);
        bExtended = SymbolicExpr::makeSignExtend(resultSize, b);
    } else {
        ASSERT_require(inode->getOperator() == SymbolicExpr::OP_UMUL);
        aExtended = SymbolicExpr::makeExtend(resultSize, a);
        bExtended = SymbolicExpr::makeExtend(resultSize, b);
    }

    return ctxExpression(aExtended, BIT_VECTOR) * ctxExpression(bExtended, BIT_VECTOR);
}

z3::expr
Z3Solver::ctxUnsignedDivide(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(1));

    return z3::udiv(ctxExpression(aExtended, BIT_VECTOR), ctxExpression(bExtended, BIT_VECTOR))
        .extract(inode->child(0)->nBits()-1, 0);
}

z3::expr
Z3Solver::ctxUnsignedExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(0)->toInt() > inode->child(1)->nBits());
    size_t newWidth = inode->child(0)->toInt();
    size_t needBits = newWidth - inode->child(1)->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(needBits, false));

    return z3::concat(ctxExpression(zeros, BIT_VECTOR), ctxExpression(inode->child(1), BIT_VECTOR));
}

z3::expr
Z3Solver::ctxUnsignedModulo(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(1));

    return z3::urem(ctxExpression(aExtended, BIT_VECTOR), ctxExpression(bExtended, BIT_VECTOR))
        .extract(inode->child(1)->nBits()-1, 0);
}

z3::expr
Z3Solver::ctxWrite(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);

    return z3::store(ctxExpression(inode->child(0), MEM_STATE),
                     ctxExpression(inode->child(1), BIT_VECTOR),
                     ctxExpression(inode->child(2), BIT_VECTOR));
}

z3::expr
Z3Solver::ctxZerop(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);

    SymbolicExpr::Ptr zeros = SymbolicExpr::makeInteger(inode->child(0)->nBits(), 0);
    return ctxExpression(inode->child(0), BIT_VECTOR) == ctxExpression(zeros, BIT_VECTOR);
}
#endif

SymbolicExpr::Ptr
Z3Solver::evidenceForName(const std::string&) {
    TODO("[Robb Matzke 2017-10-23]");
}

std::vector<std::string>
Z3Solver::evidenceNames() {
    TODO("[Robb Matzke 2017-10-23]");
}

} // namespace
} // namespace
