#include <sage3basic.h>
#include <BinaryZ3Solver.h>

#include <boost/lexical_cast.hpp>
#include <rose_strtoull.h>
#include <stringify.h>
#include <Sawyer/Stopwatch.h>

using namespace Sawyer::Message::Common;

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
    if (ROSE_Z3 && ROSE_Z3[0] != '\0')
        retval |= LM_EXECUTABLE;
#endif
    return retval;
}

void
Z3Solver::reset() {
    SmtlibSolver::reset();
#ifdef ROSE_HAVE_Z3
    if (linkage() == LM_LIBRARY) {
        z3Stack_.clear();
        z3Stack_.push_back(std::vector<z3::expr>());
        delete solver_;
        delete ctx_;
        ctx_ = new z3::context;
        solver_ = new z3::solver(*ctx_);
    }
#endif
}

void
Z3Solver::clearEvidence() {
    SmtlibSolver::clearEvidence();
#ifdef ROSE_HAVE_Z3
    if (linkage() == LM_LIBRARY) {
        ctxCses_.clear();
        ctxVarDecls_.clear();
    }
#endif
}

void
Z3Solver::pop() {
    SmtlibSolver::pop();
#ifdef ROSE_HAVE_Z3
    if (nLevels() < z3Stack_.size()) {
        ASSERT_require(nLevels() + 1 == z3Stack_.size());
        solver_->pop();
        z3Stack_.pop_back();
    }
#endif
}

void
Z3Solver::z3Update() {
#ifdef ROSE_HAVE_Z3
    if (linkage() == LM_LIBRARY) {
        ASSERT_not_null(ctx_);
        ASSERT_not_null(solver_);
        ASSERT_forbid(z3Stack_.empty());
        ASSERT_require(z3Stack_.size() <= nLevels());
        Sawyer::Stopwatch prepareTimer;

        while (z3Stack_.size() < nLevels() || z3Stack_.back().size() < assertions(nLevels()-1).size()) {

            // Push z3 expressions onto the top of the z3 stack
            size_t level = z3Stack_.size() - 1;
            std::vector<SymbolicExpr::Ptr> exprs = assertions(level);
            while (z3Stack_.back().size() < exprs.size()) {
                size_t i = z3Stack_[level].size();
                SymbolicExpr::Ptr expr = exprs[i];

                // Create the Z3 expression for this ROSE expression
                VariableSet vars;
                findVariables(expr, vars /*out*/);
                ctxVariableDeclarations(vars);
                ctxCommonSubexpressions(expr);
                z3::expr z3expr = ctxCast(ctxExpression(expr), BOOLEAN).first;
                solver_->add(z3expr);
                z3Stack_.back().push_back(z3expr);
            }

            // Push another level onto the z3 stack
            if (z3Stack_.size() < nLevels()) {
                solver_->push();
                z3Stack_.push_back(std::vector<z3::expr>());
            }
        }

#ifndef NDEBUG
        ASSERT_require(z3Stack_.size() == nLevels());
        for (size_t i=0; i<nLevels(); ++i)
            ASSERT_require(z3Stack_[i].size() == assertions(i).size());
#endif

        stats.prepareTime += prepareTimer.stop();
    }
#endif
}

SmtSolver::Satisfiable
Z3Solver::checkLib() {
    requireLinkage(LM_LIBRARY);

#ifdef ROSE_HAVE_Z3
    z3Update();

    Sawyer::Stopwatch timer;
    z3::check_result result = solver_->check();
    stats.solveTime += timer.stop();

    switch (result) {
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

SmtSolver::SExprTypePair
Z3Solver::outputExpression(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    typedef std::vector<SExprTypePair> Etv;
    SExprTypePair retval;

    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    StringTypePair st;
    if (termNames_.getOptional(expr).assignTo(st)) {
        return SExprTypePair(SExpr::instance(st.first), st.second);
    } else if (leaf) {
        return outputLeaf(leaf);
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case SymbolicExpr::OP_ADD:
                retval = outputList("bvadd", inode);
                break;
            case SymbolicExpr::OP_AND: {
                Etv children = outputExpressions(inode->children());
                Type type = mostType(children);
                children = outputCast(children, type);
                if (BOOLEAN == type) {
                    retval = outputList("and", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    retval = outputList("bvand", children);
                }
                break;
            }
            case SymbolicExpr::OP_ASR:
                retval = outputArithmeticShiftRight(inode);
                break;
            case SymbolicExpr::OP_XOR: {
                Etv children = outputExpressions(inode->children());
                Type type = mostType(children);
                children = outputCast(children, type);
                if (BOOLEAN == type) {
                    retval = outputList("xor", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    retval = outputList("bvxor", children);
                }
                break;
            }
            case SymbolicExpr::OP_EQ:
                retval = outputBinary("=", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_CONCAT: {
                Etv children = outputCast(outputExpressions(inode->children()), BIT_VECTOR);
                retval = outputList("concat", children);
                break;
            }
            case SymbolicExpr::OP_EXTRACT:
                retval = outputExtract(inode);
                break;
            case SymbolicExpr::OP_INVERT: {
                ASSERT_require(inode->nChildren() == 1);
                SExprTypePair child = outputExpression(inode->child(0));
                retval = outputUnary((BOOLEAN==child.second?"not":"bvnot"), child);
                break;
            }
            case SymbolicExpr::OP_ITE:
                retval = outputIte(inode);
                break;
            case SymbolicExpr::OP_LET:
                throw Exception("OP_LET not implemented");
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                retval = outputNotEqual(inode);
                break;
            case SymbolicExpr::OP_NEGATE: {
                ASSERT_require(inode->nChildren() == 1);
                SExprTypePair child = outputCast(outputExpression(inode->child(0)), BIT_VECTOR);
                retval = outputUnary("bvneg", child);
                break;
            }
            case SymbolicExpr::OP_NOOP:
                retval = outputExpression(SymbolicExpr::makeIntegerConstant(inode->nBits(), 0));
                break;
            case SymbolicExpr::OP_OR: {
                Etv children = outputExpressions(inode->children());
                Type type = mostType(children);
                children = outputCast(children, type);
                if (BOOLEAN == type) {
                    retval = outputList("or", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    retval = outputList("bvor",  children);
                }
                break;
            }
            case SymbolicExpr::OP_READ:
                retval = outputRead(inode);
                break;
            case SymbolicExpr::OP_ROL:
                retval = outputRotateLeft(inode);
                break;
            case SymbolicExpr::OP_ROR:
                retval = outputRotateRight(inode);
                break;
            case SymbolicExpr::OP_SDIV:
                retval = outputDivide(inode, "bvsdiv");
                break;
            case SymbolicExpr::OP_SET:
                retval = outputSet(inode);
                break;
            case SymbolicExpr::OP_SEXTEND:
                retval = outputSignExtend(inode);
                break;
            case SymbolicExpr::OP_SLT:
                retval = outputBinary("bvslt", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_SLE:
                retval = outputBinary("bvsle", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_SHL0:
                retval = outputShiftLeft(inode);
                break;
            case SymbolicExpr::OP_SHL1:
                retval = outputShiftLeft(inode);
                break;
            case SymbolicExpr::OP_SHR0:
                retval = outputLogicalShiftRight(inode);
                break;
            case SymbolicExpr::OP_SHR1:
                retval = outputLogicalShiftRight(inode);
                break;
            case SymbolicExpr::OP_SGE:
                retval = outputBinary("bvsge", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_SGT:
                retval = outputBinary("bvsgt", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_SMOD:
                retval = outputModulo(inode, "bvsrem");
                break;
            case SymbolicExpr::OP_SMUL:
                retval = outputMultiply(inode);
                break;
            case SymbolicExpr::OP_UDIV:
                retval = outputDivide(inode, "bvudiv");
                break;
            case SymbolicExpr::OP_UEXTEND:
                retval = outputUnsignedExtend(inode);
                break;
            case SymbolicExpr::OP_UGE:
                retval = outputBinary("bvuge", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_UGT:
                retval = outputBinary("bvugt", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_ULE:
                retval = outputBinary("bvule", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_ULT:
                retval = outputBinary("bvult", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_UMOD:
                retval = outputModulo(inode, "bvurem");
                break;
            case SymbolicExpr::OP_UMUL:
                retval = outputMultiply(inode);
                break;
            case SymbolicExpr::OP_WRITE:
                retval = outputWrite(inode);
                break;
            case SymbolicExpr::OP_ZEROP:
                retval = outputZerop(inode);
                break;
        }
    }

    ASSERT_not_null(retval.first);
    ASSERT_forbid(NO_TYPE == retval.second);
    return retval;
}

#ifdef ROSE_HAVE_Z3
z3::context*
Z3Solver::z3Context() const {
    requireLinkage(LM_LIBRARY);
    ASSERT_not_null(ctx_);
    return ctx_;
}

z3::solver*
Z3Solver::z3Solver() const {
    requireLinkage(LM_LIBRARY);
    ASSERT_not_null(solver_);
    return solver_;
}

std::vector<z3::expr>
Z3Solver::z3Assertions(size_t level) const {
    ASSERT_require(level < z3Stack_.size());
    return z3Stack_[level];
}

std::vector<z3::expr>
Z3Solver::z3Assertions() const {
    std::vector<z3::expr> retval;
    BOOST_FOREACH (const std::vector<z3::expr> &level, z3Stack_)
        retval.insert(retval.end(), level.begin(), level.end());
    return retval;
}

static z3::expr
portable_z3_bv_val(z3::context *ctx, uint64_t value, size_t nBits) {
    // Z3 prior to 4.8 didn't have a way to check the version at compile time, however a workaround for ROSE users was to
    // define the Z3_*_VERSION constants themselves for these older Z3 versions.
#if defined(Z3_MAJOR_VERSION) && defined(Z3_MINOR_VERSION) && defined(Z3_BUILD_NUMBER)
    #if Z3_MAJOR_VERSION < 4
        // z3 < 4.0.0
        #error "Z3 version < 4.x.x are not supported"
    #elif Z3_MAJOR_VERSION == 4 && Z3_MINOR_VERSION < 7
        // z3 >= 4.0.0 && z3 < 4.7.0
        return ctx->bv_val((unsigned long long)value, (unsigned)nBits);
    #else
        // z3 >= 4.7.0
        return ctx->bv_val((uint64_t)value, (unsigned)nBits);
    #endif
#else
    // Not all ROSE users manually defined the Z3_*_VERSION constants. Therefore, if the compiler is compiling this part of the
    // code the only thing we know is that it's a version before 4.8.0 where the version numbers were first defined in the Z3
    // source code. See [https://github.com/Z3Prover/z3/issues/1833].
    //
    // The Z3 API for z3::context::bv_val changed in version 4.7 that makes it incompatible with earlier versions, but since we
    // have no Z3_*_VERSION constants we can't tell whether this is z3 4.7 or an earlier version. We assume that it's an earlier
    // version since that's more likely.
    //
    // If you get an error here it's probably because we assumed wrong and you're actually using Z3 4.7. Instead of
    // changing the ROSE source code, define the Z3_*_VERSION values in your compiler command-line.
    return ctx->bv_val((unsigned long long)value, (unsigned)nBits); // DO NOT CHANGE; this assumes z3 < 4.7.
#endif
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxLeaf(const SymbolicExpr::LeafPtr &leaf) {
    ASSERT_not_null(leaf);
    if (leaf->isIntegerConstant()) {
        if (leaf->nBits() <= 64) {
            z3::expr z3expr = portable_z3_bv_val(ctx_, *leaf->toUnsigned(), leaf->nBits());
            return Z3ExprTypePair(z3expr, BIT_VECTOR);
        } else {
            // Z3-4.5.0 and 4.6.0 lack an interface for creating a bit vector with more than 64 bits from a hexadecimal
            // string. A function taking a bool[] has been added to z3's development branch but is not yet
            // available. Therefore, we construct a wider-than-64-bit constant by concatenating up to 64 bits at a time.
            z3::expr z3expr(*ctx_);
            for (size_t offset=0; offset < leaf->bits().size(); offset += 64) {
                size_t windowSize = std::min((size_t)64, leaf->bits().size() - offset);
                Sawyer::Container::BitVector windowBits = leaf->bits();
                windowBits.shiftRight(offset).resize(windowSize);
                z3::expr window = portable_z3_bv_val(ctx_, windowBits.toInteger(), windowSize);
                if (0 == offset) {
                    z3expr = window;
                } else {
                    z3expr = z3::concat(window, z3expr);
                }
            }
            return Z3ExprTypePair(z3expr, BIT_VECTOR);
        }
    } else if (leaf->isIntegerVariable()) {
        z3::func_decl decl = ctxVarDecls_.get(leaf);
        return Z3ExprTypePair(decl(), BIT_VECTOR);
    } else {
        ASSERT_require(leaf->isMemoryVariable());
        z3::func_decl decl = ctxVarDecls_.get(leaf);
        return Z3ExprTypePair(decl(), MEM_STATE);
    }
}

SmtSolver::Type
Z3Solver::mostType(const std::vector<Z3ExprTypePair> &ets) {
    typedef Sawyer::Container::Map<Type, size_t> Histogram;
    Histogram histogram;
    BOOST_FOREACH (const Z3ExprTypePair &et, ets)
        ++histogram.insertMaybe(et.second, 0);
    Type bestType = NO_TYPE;
    size_t bestCount = 0;
    BOOST_FOREACH (const Histogram::Node &node, histogram.nodes()) {
        if (node.value() > bestCount) {
            bestType = node.key();
            bestCount = node.value();
        }
    }
    return bestType;
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxExpression(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    typedef std::vector<Z3ExprTypePair> Etv;

    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    ASSERT_not_null(ctx_);
    z3::expr z3expr(*ctx_);
    if (ctxCses_.exists(expr)) {
        return ctxCses_[expr];
    } else if (leaf != NULL) {
        return ctxLeaf(leaf);
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case SymbolicExpr::OP_ADD: {
                Etv children = ctxCast(ctxExpressions(inode->children()), BIT_VECTOR);
                ASSERT_forbid(children.empty());
                z3expr = children[0].first;
                for (size_t i = 1; i < children.size(); ++i)
                    z3expr = z3expr + children[i].first;
                return Z3ExprTypePair(z3expr, BIT_VECTOR);
            }
            case SymbolicExpr::OP_AND: {
                Etv children = ctxExpressions(inode->children());
                Type type = mostType(children);
                children = ctxCast(children, type);
                ASSERT_require(children.size() >= 2);
                if (BOOLEAN == type) {
                    z3expr = children[0].first;
                    for (size_t i = 1; i < children.size(); ++i)
                        z3expr = z3expr && children[i].first;
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    z3expr = children[0].first;
                    for (size_t i = 1; i < children.size(); ++i)
                        z3expr = z3expr & children[i].first;
                }
                return Z3ExprTypePair(z3expr, type);
            }
            case SymbolicExpr::OP_ASR:
                return ctxArithmeticShiftRight(inode);
            case SymbolicExpr::OP_XOR: {
                // Z3 doesn't have an XOR operator for Boolean arguments, only for bit vectors. Therefore, we cast
                // the arguments to bit vectors and do a bit-wise XOR. We can leave the result a bit vector because
                // if a Boolean result is needed, the caller will cast the result to Boolean.
                Etv children = ctxCast(ctxExpressions(inode->children()), BIT_VECTOR);
                ASSERT_require(children.size() >= 2);
                z3expr = children[0].first;
                for (size_t i = 1; i < children.size(); ++i)
                    z3expr = z3expr ^ children[i].first;
                return Z3ExprTypePair(z3expr, BIT_VECTOR);
            }
            case SymbolicExpr::OP_EQ: {
                ASSERT_require(inode->nChildren() == 2);
                Etv children = ctxExpressions(inode->children());
                Type type = mostType(children);
                children = ctxCast(children, type);
                z3expr = children[0].first == children[1].first;
                return Z3ExprTypePair(z3expr, BOOLEAN);
            }
            case SymbolicExpr::OP_CONCAT: {
                Etv children = ctxCast(ctxExpressions(inode->children()), BIT_VECTOR);
                ASSERT_forbid(children.empty());
                z3expr = children[0].first;
                for (size_t i = 1; i < children.size(); ++i)
                    z3expr = z3::concat(z3expr, children[i].first);
                return Z3ExprTypePair(z3expr, BIT_VECTOR);
            }
            case SymbolicExpr::OP_EXTRACT:
                return ctxExtract(inode);
            case SymbolicExpr::OP_INVERT: {
                ASSERT_require(inode->nChildren() == 1);
                Z3ExprTypePair child = ctxExpression(inode->child(0));
                if (BOOLEAN == child.second) {
                    z3expr = !child.first;
                } else {
                    ASSERT_require(BIT_VECTOR == child.second);
                    z3expr = ~child.first;
                }
                return Z3ExprTypePair(z3expr, child.second);
            }
            case SymbolicExpr::OP_ITE: {
                ASSERT_require(inode->nChildren() == 3);
                Etv alternatives;
                alternatives.push_back(ctxExpression(inode->child(1)));
                alternatives.push_back(ctxExpression(inode->child(2)));
                Type type = mostType(alternatives);
                alternatives = ctxCast(alternatives, type);
                z3expr = z3::ite(ctxCast(ctxExpression(inode->child(0)), BOOLEAN).first,
                                 alternatives[0].first,
                                 alternatives[1].first);
                return Z3ExprTypePair(z3expr, type);
            }
            case SymbolicExpr::OP_LET:
                throw Exception("OP_LET not implemented");
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE: {
                ASSERT_require(inode->nChildren() == 2);
                Etv children = ctxExpressions(inode->children());
                Type type = mostType(children);
                children = ctxCast(children, type);
                z3expr = children[0].first != children[1].first;
                return Z3ExprTypePair(z3expr, BOOLEAN);
            }
            case SymbolicExpr::OP_NEGATE: {
                ASSERT_require(inode->nChildren() == 1);
                Z3ExprTypePair child = ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR);
                z3expr = -child.first;
                return Z3ExprTypePair(z3expr, BIT_VECTOR);
            }
            case SymbolicExpr::OP_NOOP:
                return ctxExpression(SymbolicExpr::makeIntegerConstant(inode->nBits(), 0));
            case SymbolicExpr::OP_OR: {
                ASSERT_require(inode->nChildren() >= 2);
                Etv children = ctxExpressions(inode->children());
                Type type = mostType(children);
                children = ctxCast(children, type);
                if (BOOLEAN == type) {
                    z3expr = children[0].first;
                    for (size_t i = 1; i < children.size(); ++i)
                        z3expr = z3expr || children[i].first;
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    z3expr = children[0].first;
                    for (size_t i = 1; i < children.size(); ++i)
                        z3expr = z3expr | children[i].first;
                }
                return Z3ExprTypePair(z3expr, type);
            }
            case SymbolicExpr::OP_READ:
                return ctxRead(inode);
            case SymbolicExpr::OP_ROL:
                return ctxRotateLeft(inode);
            case SymbolicExpr::OP_ROR:
                return ctxRotateRight(inode);
            case SymbolicExpr::OP_SDIV:
                return ctxSignedDivide(inode);
            case SymbolicExpr::OP_SET:
                return ctxSet(inode);
            case SymbolicExpr::OP_SEXTEND:
                return ctxSignExtend(inode);
            case SymbolicExpr::OP_SLT:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = (ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first <
                          ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_SLE:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = (ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first <=
                          ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_SHL0:
                return ctxShiftLeft(inode);
            case SymbolicExpr::OP_SHL1:
                return ctxShiftLeft(inode);
            case SymbolicExpr::OP_SHR0:
                return ctxShiftRight(inode);
            case SymbolicExpr::OP_SHR1:
                return ctxShiftRight(inode);
            case SymbolicExpr::OP_SGE:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = (ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first >=
                          ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_SGT:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = (ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first >
                          ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_SMOD:
                return ctxSignedModulo(inode);
            case SymbolicExpr::OP_SMUL:
                return ctxMultiply(inode);
            case SymbolicExpr::OP_UDIV:
                return ctxUnsignedDivide(inode);
            case SymbolicExpr::OP_UEXTEND:
                return ctxUnsignedExtend(inode);
            case SymbolicExpr::OP_UGE:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = z3::uge(ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first,
                                 ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_UGT:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = z3::ugt(ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first,
                                 ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_ULE:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = z3::ule(ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first,
                                 ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_ULT:
                ASSERT_require(inode->nChildren() == 2);
                z3expr = z3::ult(ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first,
                                 ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
                return Z3ExprTypePair(z3expr, BOOLEAN);
            case SymbolicExpr::OP_UMOD:
                return ctxUnsignedModulo(inode);
            case SymbolicExpr::OP_UMUL:
                return ctxMultiply(inode);
            case SymbolicExpr::OP_WRITE:
                return ctxWrite(inode);
            case SymbolicExpr::OP_ZEROP:
                return ctxZerop(inode);
        }
    }
    ASSERT_not_reachable("unhandled case " + stringify::Rose::BinaryAnalysis::SymbolicExpr::Operator(inode->getOperator(), "") +
                         " (" + boost::lexical_cast<std::string>(inode->getOperator()) + ")");
}

std::vector<Z3Solver::Z3ExprTypePair>
Z3Solver::ctxExpressions(const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<Z3Solver::Z3ExprTypePair> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(ctxExpression(expr));
    return retval;
}
#endif

SmtSolver::SExprTypePair
Z3Solver::outputList(const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type rettype) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);

    std::vector<SExprTypePair> children = outputExpressions(inode->children());
    return outputList(name, children, rettype);
}

SmtSolver::SExprTypePair
Z3Solver::outputList(const std::string &name, const std::vector<SExprTypePair> &children, Type rettype) {
    Type childType = mostType(children);
    SExpr::Ptr retval = SExpr::instance(SExpr::instance(name));
    std::vector<SExprTypePair> castChildren = outputCast(children, childType);
    BOOST_FOREACH (const SExprTypePair child, castChildren)
        retval->children().push_back(child.first);

    if (NO_TYPE == rettype)
        rettype = childType;
    return SExprTypePair(retval, rettype);
}

// ROSE (asr amount expr) =>
// SMT-LIB (bvashr (uextend [expr.size] amount) expr)
//
// Shift amount needs to be extended or truncated to be the same width as the expression.
SmtSolver::SExprTypePair
Z3Solver::outputArithmeticShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, expr->nBits()), sa);

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("bvashr"),
                        outputCast(outputExpression(expr), BIT_VECTOR).first,
                        outputCast(outputExpression(sa), BIT_VECTOR).first);

    return SExprTypePair(retval, BIT_VECTOR);
}

#ifdef ROSE_HAVE_Z3
void
Z3Solver::ctxVariableDeclarations(const VariableSet &vars) {
    BOOST_FOREACH (const SymbolicExpr::LeafPtr &var, vars.values()) {
        ASSERT_not_null(var);
        ASSERT_require(var->isVariable2());
        if (ctxVarDecls_.exists(var)) {
            // already emitted a declaration for this variable
        } else if (var->isScalar()) {
            z3::sort range = ctx_->bv_sort(var->nBits());
            z3::func_decl decl = z3::function(var->toString().c_str(), 0, NULL, range);
            ctxVarDecls_.insert(var, decl);
        } else {
            ASSERT_require(var->domainWidth() > 0);
            z3::sort addr = ctx_->bv_sort(var->domainWidth());
            z3::sort value = ctx_->bv_sort(var->nBits());
            z3::sort range = ctx_->array_sort(addr, value);
            z3::func_decl decl = z3::function(var->toString().c_str(), 0, NULL, range);
            ctxVarDecls_.insert(var, decl);
        }
    }
}

void
Z3Solver::ctxCommonSubexpressions(const SymbolicExpr::Ptr &expr) {
    std::vector<SymbolicExpr::Ptr> cses = expr->findCommonSubexpressions();
    BOOST_FOREACH (const SymbolicExpr::Ptr &cse, cses) {
        if (!ctxCses_.exists(cse))
            ctxCses_.insert(cse, ctxExpression(cse));
    }
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxCast(const Z3ExprTypePair &et, Type toType) {
    Type fromType = et.second;
    if (fromType == toType) {
        return et;
    } else if (BOOLEAN == fromType && BIT_VECTOR == toType) {
        z3::expr e = z3::ite(et.first, ctx_->bv_val(1, 1), ctx_->bv_val(0, 1));
        return Z3ExprTypePair(e, toType);
    } else if (BIT_VECTOR == fromType && BOOLEAN == toType) {
        z3::expr e = et.first == ctx_->bv_val(1, 1);
        return Z3ExprTypePair(e, toType);
    } else {
        Stringifier string(stringifyBinaryAnalysisSmtSolverType);
        ASSERT_not_reachable("invalid cast from " + string(et.second) + " to " + string(toType));
    }
}

std::vector<Z3Solver::Z3ExprTypePair>
Z3Solver::ctxCast(const std::vector<Z3ExprTypePair> &ets, Type toType) {
    std::vector<Z3ExprTypePair> retval;
    retval.reserve(ets.size());
    BOOST_FOREACH (const Z3ExprTypePair &et, ets)
        retval.push_back(ctxCast(et, toType));
    return retval;
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxArithmeticShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, expr->nBits()), sa);
    z3::expr e = z3::ashr(ctxCast(ctxExpression(expr), BIT_VECTOR).first,
                          ctxCast(ctxExpression(sa), BIT_VECTOR).first);
    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxExtract(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(1)->isIntegerConstant());
    size_t begin = inode->child(0)->toUnsigned().get(); // low, inclusive
    size_t end = inode->child(1)->toUnsigned().get();   // high, exclusive
    ASSERT_require(end > begin);
    ASSERT_require(end <= inode->child(2)->nBits());

    z3::expr e = ctxCast(ctxExpression(inode->child(2)), BIT_VECTOR).first.extract(end-1, begin);
    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxRead(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren());
    z3::expr e = z3::select(ctxCast(ctxExpression(inode->child(0)), MEM_STATE).first,
                            ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);
    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxRotateLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*w), sa);
    z3::expr e = z3::shl(z3::concat(ctxCast(ctxExpression(expr), BIT_VECTOR).first,
                                    ctxCast(ctxExpression(expr), BIT_VECTOR).first),
                         ctxCast(ctxExpression(sa), BIT_VECTOR).first)
                 .extract(2*w-1, w);
    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxRotateRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*w), sa);
    z3::expr e = z3::lshr(z3::concat(ctxCast(ctxExpression(expr), BIT_VECTOR).first,
                                     ctxCast(ctxExpression(expr), BIT_VECTOR).first),
                          ctxCast(ctxExpression(sa), BIT_VECTOR).first)
                 .extract(w-1, 0);
    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxSet(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(inode->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(inode);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(inode, SmtSolverPtr(), var);
    return ctxExpression(ite);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxSignExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    SymbolicExpr::Ptr newSize = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t signBitIdx = expr->nBits() - 1;
    size_t growth = newSize->toUnsigned().get() - expr->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(growth, false));

    z3::expr isNegative =
        ctxCast(ctxExpression(expr), BIT_VECTOR).first.extract(signBitIdx, signBitIdx) == ctx_->bv_val(1, 1);

    z3::expr e =
        z3::concat(z3::ite(isNegative,
                           ~ctxCast(ctxExpression(zeros), BIT_VECTOR).first,
                           ctxCast(ctxExpression(zeros), BIT_VECTOR).first),
                   ctxCast(ctxExpression(expr), BIT_VECTOR).first);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxShiftLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHL0 || inode->getOperator() == SymbolicExpr::OP_SHL1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*expr->nBits()), sa); // widen sa same as shifted expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHL1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    z3::expr e =
        z3::shl(z3::concat(ctxCast(ctxExpression(expr), BIT_VECTOR).first,
                           ctxCast(ctxExpression(zerosOrOnes), BIT_VECTOR).first),
                ctxCast(ctxExpression(sa), BIT_VECTOR).first)
        .extract(2*expr->nBits()-1, expr->nBits());

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHR0 || inode->getOperator() == SymbolicExpr::OP_SHR1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*expr->nBits()), sa); // widen sa same as shifted expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHR1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    z3::expr e =
        z3::lshr(z3::concat(ctxCast(ctxExpression(zerosOrOnes), BIT_VECTOR).first,
                            ctxCast(ctxExpression(expr), BIT_VECTOR).first),
                 ctxCast(ctxExpression(sa), BIT_VECTOR).first)
        .extract(expr->nBits()-1, 0);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxMultiply(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr a = inode->child(0);
    SymbolicExpr::Ptr b = inode->child(1);
    SymbolicExpr::Ptr resultSize = SymbolicExpr::makeIntegerConstant(32, a->nBits() + b->nBits());

    SymbolicExpr::Ptr aExtended, bExtended;
    if (inode->getOperator() == SymbolicExpr::OP_SMUL) {
        aExtended = SymbolicExpr::makeSignExtend(resultSize, a);
        bExtended = SymbolicExpr::makeSignExtend(resultSize, b);
    } else {
        ASSERT_require(inode->getOperator() == SymbolicExpr::OP_UMUL);
        aExtended = SymbolicExpr::makeExtend(resultSize, a);
        bExtended = SymbolicExpr::makeExtend(resultSize, b);
    }

    z3::expr e = ctxCast(ctxExpression(aExtended), BIT_VECTOR).first *
                 ctxCast(ctxExpression(bExtended), BIT_VECTOR).first;

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxUnsignedDivide(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    z3::expr e =
        z3::udiv(ctxCast(ctxExpression(aExtended), BIT_VECTOR).first,
                 ctxCast(ctxExpression(bExtended), BIT_VECTOR).first)
        .extract(inode->child(0)->nBits()-1, 0);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxSignedDivide(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    z3::expr e =
        (ctxCast(ctxExpression(aExtended), BIT_VECTOR).first /
         ctxCast(ctxExpression(bExtended), BIT_VECTOR).first)
        .extract(inode->child(0)->nBits()-1, 0);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxUnsignedExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    size_t newWidth = inode->child(0)->toUnsigned().get();
    size_t needBits = newWidth - inode->child(1)->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(needBits, false));

    z3::expr e = z3::concat(ctxCast(ctxExpression(zeros), BIT_VECTOR).first,
                            ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxUnsignedModulo(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    z3::expr e =
        z3::urem(ctxCast(ctxExpression(aExtended), BIT_VECTOR).first,
                 ctxCast(ctxExpression(bExtended), BIT_VECTOR).first)
        .extract(inode->child(1)->nBits()-1, 0);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxSignedModulo(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    z3::expr e =
        z3::srem(ctxCast(ctxExpression(aExtended), BIT_VECTOR).first,
                 ctxCast(ctxExpression(bExtended), BIT_VECTOR).first)
        .extract(inode->child(1)->nBits()-1, 0);

    return Z3ExprTypePair(e, BIT_VECTOR);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxWrite(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);

    z3::expr e =  z3::store(ctxCast(ctxExpression(inode->child(0)), MEM_STATE).first,
                            ctxCast(ctxExpression(inode->child(1)), BIT_VECTOR).first,
                            ctxCast(ctxExpression(inode->child(2)), BIT_VECTOR).first);

    return Z3ExprTypePair(e, MEM_STATE);
}

Z3Solver::Z3ExprTypePair
Z3Solver::ctxZerop(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);

    SymbolicExpr::Ptr zeros = SymbolicExpr::makeIntegerConstant(inode->child(0)->nBits(), 0);
    z3::expr e = ctxCast(ctxExpression(inode->child(0)), BIT_VECTOR).first ==
                 ctxCast(ctxExpression(zeros), BIT_VECTOR).first;

    return Z3ExprTypePair(e, BOOLEAN);
}
#endif

void
Z3Solver::parseEvidence() {
    if (linkage() != LM_LIBRARY)
        return SmtlibSolver::parseEvidence();

#ifdef ROSE_HAVE_Z3
    Sawyer::Stopwatch evidenceTimer;

    // If memoization is being used and we have a previous result, then use the previous result. However, we need to undo the
    // variable renaming. That is, the memoized result is in terms of renumbered variables, so we need to use the
    // latestMemoizationRewrite_ to rename the memoized variables back to the variable names used in the actual query from the
    // caller.
    SymbolicExpr::Hash memoId = latestMemoizationId();
    if (memoId > 0) {
        MemoizedEvidence::iterator found = memoizedEvidence.find(memoId);
        if (found != memoizedEvidence.end()) {
            evidence.clear();
            SymbolicExpr::ExprExprHashMap undo = latestMemoizationRewrite_.invert();
            evidence.clear();
            BOOST_FOREACH (const ExprExprMap::Node &node, found->second.nodes())
                evidence.insert(node.key()->substituteMultiple(undo),
                                node.value()->substituteMultiple(undo));
            stats.evidenceTime += evidenceTimer.stop();
            return;
        }
    }

    // If there are no assertions, then there is no evidence.
    bool hasAssertions = false;
    for (size_t i=0; i<z3Stack_.size() && !hasAssertions; ++i)
        hasAssertions = !z3Stack_[i].empty();
    if (!hasAssertions)
        return;

    // Get all the variables in all the assertions regardless of transaction level. This is somewhat expensive but is needed
    // below because the Z3 interface lacks a way to get type information from the variables returned as part of the evidence.
    VariableSet allVariables;
    std::vector<SymbolicExpr::Ptr> allAssertions = assertions();
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, allAssertions)
        findVariables(expr, allVariables /*in,out*/);

    // Parse the evidence
    ASSERT_not_null(solver_);
    z3::model model = solver_->get_model();
    for (size_t i=0; i<model.size(); ++i) {
        z3::func_decl fdecl = model[i];

        if (fdecl.arity() != 0)
            continue;

        // There's got to be a better way to get information about a z3::expr, but I haven't found it yet.  For bit vectors, we
        // need to know the number of bits and the value, even if the value is wider than 64 bits. Threfore, we obtain the list
        // of all variables from above (regardless of transaction level) and try to match of the z3 variable name with the ROSE
        // variable name and obtain the type information from the ROSE variable.
        SymbolicExpr::LeafPtr var;
        BOOST_FOREACH (const SymbolicExpr::LeafPtr &v, allVariables.values()) {
            if (v->toString() == fdecl.name().str()) {
                var = v;
                break;
            }
        }
        if (NULL == var) {
            mlog[WARN] <<"cannot find evidence variable " <<fdecl.name() <<"\n";
            continue;
        }

        // Get the value
        SymbolicExpr::Ptr val;
        z3::expr interp = model.get_const_interp(fdecl);
        if (interp.is_bv()) {
            if (var->nBits() <= 64) {
                val = SymbolicExpr::makeIntegerConstant(var->nBits(), interp.get_numeral_uint64());
            } else {
                // Z3 doesn't have an API function to obtain the bits of a constant if the constant is wider than 64 bits. We
                // can't use the trick of splitting the Z3 bit vector into 64-bit chunks and converting each to a uint64_t and
                // concatenating the results because "bv.extract(hi,lo)" is no longer a number but rather an extraction
                // expression.  What we can do is get a string of decimal digits and parse it to binary. This isn't
                // particularly efficient since the conversions to/from string require divide/multiply by 10.
                std::string digits = interp.get_decimal_string(0);
                Sawyer::Container::BitVector bits(var->nBits());
                bits.fromDecimal(bits.hull(), digits);
                val = SymbolicExpr::makeIntegerConstant(bits);
                ASSERT_require(val->nBits() == var->nBits());
                ASSERT_require(val->isLeafNode() && val->isLeafNode()->isIntegerConstant());
            }
        } else if (interp.is_bool()) {
            val = SymbolicExpr::makeBooleanConstant(interp.get_numeral_uint() != 0);
        } else {
            mlog[WARN] <<"cannot parse evidence expression for " <<*var <<"\n";
            continue;
        }

        ASSERT_not_null(var);
        ASSERT_not_null(val);
        evidence.insert(var, val);
    }

    // Cache the evidence. We need to cache the evidence in terms of the normalized variable names.
    if (memoId > 0) {
        ExprExprMap &me = memoizedEvidence[memoId];
        BOOST_FOREACH (const ExprExprMap::Node &node, evidence.nodes()) {
            me.insert(node.key()->substituteMultiple(latestMemoizationRewrite_),
                      node.value()->substituteMultiple(latestMemoizationRewrite_));
        }
    }

    stats.evidenceTime += evidenceTimer.stop();
#else
    ASSERT_not_reachable("z3 not enabled");
#endif
}

void
Z3Solver::selfTest() {
    SmtlibSolver::selfTest();

    using namespace SymbolicExpr;
    typedef SymbolicExpr::Ptr Expr;

    Stream trace(mlog[TRACE] <<"Z3-specific unit tests");

    reset();
    ASSERT_always_require(nLevels() == 1);
    ASSERT_always_require(assertions().empty());
    ASSERT_always_require(assertions(0).empty());

    //-----
    // This unit test answers the following question:
    //   * Given a 32-bit variable, what values when rotate left three bits are equal to the same value
    //     rotated right three bits?
    //-----

    // Create the expression
    Expr a = makeIntegerVariable(32, "a");
    Expr rol = makeRol(makeIntegerConstant(32, 3), a);
    Expr ror = makeRor(makeIntegerConstant(32, 3), a);
    Expr expr1 = makeEq(rol, ror);

    // Insert the expression into the solver
    mlog[DEBUG] <<"insert " <<*expr1 <<"\n";
    insert(expr1);
    ASSERT_always_require(nLevels() == 1);
    ASSERT_always_require(assertions().size() == 1);
    ASSERT_always_require(assertions(0).size() == 1);
    ASSERT_always_require(assertions()[0] == assertions(0)[0]);

    // Check that it's satisfiable
    mlog[DEBUG] <<"checking\n";
    Satisfiable sat = check();
    ASSERT_always_require(SAT_YES == sat);

    // Get an answer
    mlog[DEBUG] <<"parsing evidence\n";
    parseEvidence();
    std::vector<std::string> enames = evidenceNames();
    BOOST_FOREACH (const std::string &name, enames) {
        mlog[DEBUG] <<"evidence name = " <<name <<"\n";
        Expr expr = evidenceForName(name);
        ASSERT_always_not_null(expr);
        mlog[DEBUG] <<"  " <<name <<" = " <<*expr <<"\n";
    }

    ASSERT_always_require(enames.size() == 1);
    ASSERT_always_require(enames[0] == a->isLeafNode()->toString());
    Expr aEvidence = evidenceForName(a->isLeafNode()->toString());
    ASSERT_always_not_null(aEvidence);
    ASSERT_always_require(aEvidence->nBits() == a->nBits());
    ASSERT_always_require(aEvidence->isLeafNode());
    ASSERT_always_require(aEvidence->isLeafNode()->isIntegerConstant());
    uint32_t aVal = aEvidence->isLeafNode()->toUnsigned().get();
    ASSERT_always_require2(aVal == 0x00000000 || aVal == 0x11111111 || aVal == 0xaaaaaaaa || aVal == 0x55555555,
                           StringUtility::addrToString(aVal));

    // Augment by requiring that the answer is not certain values
    push();
    insert(makeNe(a, makeIntegerConstant(32, 0x00000000)));
    insert(makeNe(a, makeIntegerConstant(32, 0xffffffff)));
    insert(makeNe(a, makeIntegerConstant(32, 0xaaaaaaaa)));

    // Check again and get an answer
    sat = check();
    ASSERT_always_require(SAT_YES == sat);
    parseEvidence();
    aEvidence = evidenceForName(a->isLeafNode()->toString());
    ASSERT_always_not_null(aEvidence);
    ASSERT_always_require(aEvidence->isLeafNode());
    ASSERT_always_require(aEvidence->isLeafNode()->isIntegerConstant());
    aVal = aEvidence->isLeafNode()->toUnsigned().get();
    ASSERT_always_require2(aVal == 0x55555555, StringUtility::addrToString(aVal));

    trace <<"; pass\n";
}

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Z3Solver);
#endif
