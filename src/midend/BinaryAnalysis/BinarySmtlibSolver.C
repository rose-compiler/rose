#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinarySmtlibSolver.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <Diagnostics.h>
#include <Sawyer/Stopwatch.h>
#include <stringify.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

void
SmtlibSolver::reset() {
    SmtSolver::reset();
    varsForSets_.clear();
}

void
SmtlibSolver::clearEvidence() {
    SmtSolver::clearEvidence();
    evidence.clear();
}

void
SmtlibSolver::clearMemoization() {
    SmtSolver::clearMemoization();
    memoizedEvidence.clear();
}

void
SmtlibSolver::timeout(boost::chrono::duration<double> seconds) {
    timeout_ = seconds;
}

std::string
SmtlibSolver::getCommand(const std::string &configName) {
    std::string exe = executable_.empty() ? std::string("/bin/false") : executable_.string();
    return exe + " " + shellArgs_ + " " + configName;
}

void
SmtlibSolver::generateFile(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) {
    requireLinkage(LM_EXECUTABLE);

    if (timeout_) {
        // It's not well documented. Experimentally determined to be milliseconds using Z3.
        o <<"(set-option :timeout " <<(unsigned)::round(timeout_->count()*1000) <<")\n";
    }

    // Find all variables
    VariableSet vars;
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs) {
        VariableSet tmp;
        findVariables(expr, tmp);
        BOOST_FOREACH (const SymbolicExpr::LeafPtr &var, tmp.values())
            vars.insert(var);
    }

    // Output variable declarations
    if (!vars.isEmpty()) {
        o <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
          <<"; Uninterpreted variables\n"
          <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
        outputVariableDeclarations(o, vars);
    }

    o <<"\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Common subexpressions\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    outputCommonSubexpressions(o, exprs);

    outputComments(o, exprs);

    o <<"\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Functions\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n";
    outputBvxorFunctions(o, exprs);
    outputComparisonFunctions(o, exprs);

    o <<"\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Assertions\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs) {
        o <<"\n";
        const std::string &comment = expr->comment();
        if (!comment.empty())
            o <<StringUtility::prefixLines(comment, "; ") <<"\n";
        o <<"; effective size = " <<StringUtility::plural(expr->nNodes(), "nodes")
          <<", actual size = " <<StringUtility::plural(expr->nNodesUnique(), "nodes") <<"\n";
        outputAssertion(o, expr);
    }

    o <<"\n(check-sat)\n";
    o <<"(get-model)\n";
}

std::string
SmtlibSolver::getErrorMessage(int exitStatus) {
    BOOST_FOREACH (const SExpr::Ptr &sexpr, parsedOutput_) {
        if (sexpr->children().size() == 2 && sexpr->children()[0]->name() == "error") {
            if (sexpr->children()[1]->name().empty()) {
                std::ostringstream ss;
                printSExpression(ss, sexpr);
                return ss.str();
            } else {
                std::string s = sexpr->children()[1]->name();
                if (s.find("model is not available") != std::string::npos) {
                    // This is a non-error.  It is generally illegal to invoke "(get-model)" for an unsatisfiable state, but
                    // since we have to generate the input script before we know whether it's satisfiable or not, we have to
                    // unconditionally include the "(get-model)" call.
                    exitStatus = 0;
                } else {
                    return s;
                }
            }
        }
    }
    return SmtSolver::getErrorMessage(exitStatus);
}

std::string
SmtlibSolver::typeName(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    ASSERT_require(expr->nBits() > 0);

    if (expr->isScalar()) {
        // as in (declare-fun v1 () (_ BitVec 32))
        //                       ^^^^^^^^^^^^^^^^
        return "() (_ BitVec " + boost::lexical_cast<std::string>(expr->nBits()) + ")";
    } else {
        // as in (declare-fun m1 () (Array (_ BitVec 32) (_ BitVec 8)))
        //                       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        ASSERT_require(expr->domainWidth() > 0);
        return "() (Array (_ BitVec " + boost::lexical_cast<std::string>(expr->domainWidth()) + ")"
            " (_ BitVec " + boost::lexical_cast<std::string>(expr->nBits()) + "))";
    }
}

void
SmtlibSolver::outputAssertion(std::ostream &o, const SymbolicExpr::Ptr &expr) {
    o <<"(assert ";
    if (expr->isIntegerConstant() && 1==expr->nBits()) {
        if (expr->toUnsigned().get()) {
            o <<"true";
        } else {
            o <<"false";
        }
    } else {
        SExpr::Ptr smtExpr = outputCast(outputExpression(expr), BOOLEAN).first;
        o <<*smtExpr;
    }
    o <<")\n";
}

void
SmtlibSolver::varForSet(const SymbolicExpr::InteriorPtr &set, const SymbolicExpr::LeafPtr &var) {
    ASSERT_not_null(set);
    ASSERT_not_null(var);
    varsForSets_.insert(set, var);
}

SymbolicExpr::LeafPtr
SmtlibSolver::varForSet(const SymbolicExpr::InteriorPtr &set) {
    ASSERT_not_null(set);
    SymbolicExpr::Ptr expr = varsForSets_.getOrDefault(set);
    ASSERT_not_null(expr);
    SymbolicExpr::LeafPtr var = expr->isLeafNode();
    ASSERT_not_null(var);
    return var;
}

// A side effect is that the varsForSets_ map is updated.
void
SmtlibSolver::findVariables(const SymbolicExpr::Ptr &expr, VariableSet &variables) {
    struct T1: SymbolicExpr::Visitor {
        SmtlibSolver *self;
        VariableSet &variables;
        std::set<const SymbolicExpr::Node*> seen;

        T1(SmtlibSolver *self, VariableSet &variables): self(self), variables(variables) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable2())
                    variables.insert(leaf);
            } else if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpr::OP_SET) {
                    // Sets are ultimately converted to ITEs and therefore each set needs a free variable.
                    SymbolicExpr::LeafPtr var = SymbolicExpr::makeIntegerVariable(32, "set")->isLeafNode();
                    variables.insert(var);
                    self->varForSet(inode, var);
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(this, variables);
    expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputVariableDeclarations(std::ostream &o, const VariableSet &variables) {
    BOOST_FOREACH (const SymbolicExpr::LeafPtr &var, variables.values()) {
        ASSERT_require(var->isVariable2());
        o <<"\n";
        if (!var->comment().empty())
            o <<StringUtility::prefixLines(var->comment(), "; ") <<"\n";
        o <<"(declare-fun " <<var->toString() <<" " <<typeName(var) <<")\n";
    }
}

void
SmtlibSolver::outputComments(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    struct T1: SymbolicExpr::Visitor {
        typedef std::set<const SymbolicExpr::Node*> SeenNodes;
        SeenNodes seen;
        std::ostream &o;
        bool commented;

        T1(std::ostream &o)
            : o(o), commented(false) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable2() && !leaf->comment().empty()) {
                    if (!commented) {
                        o <<"\n"
                          <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
                          <<"; Variable comments\n"
                          <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
                        commented = true;
                    }
                    o <<"\n; " <<leaf->toString() <<": "
                      <<StringUtility::prefixLines(leaf->comment(), ";    ", false) <<"\n";
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(o);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputBvxorFunctions(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    struct T1: SymbolicExpr::Visitor {
        std::set<size_t> widths;
        std::ostream &o;

        T1(std::ostream &o)
            : o(o) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpr::OP_XOR && widths.insert(inode->nBits()).second) {
                    size_t w = inode->nBits();
                    o <<"(define-fun bvxor" <<w
                      <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                      <<" (_ BitVec " <<w <<")"
                      <<" (bvor (bvand a (bvnot b)) (bvand (bvnot a) b)))\n";
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(o);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputComparisonFunctions(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    struct T1: SymbolicExpr::Visitor {
        Sawyer::Container::Map<SymbolicExpr::Operator, std::set<size_t> > widths;
        std::ostream &o;

        T1(std::ostream &o): o(o) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                size_t w = inode->child(0)->nBits();
                switch (inode->getOperator()) {
                    case SymbolicExpr::OP_ULT:
                        break;                          // SMT-LIB 2 defines "bvult" for this
                    case SymbolicExpr::OP_ULE:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvule" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (or (bvult a b) (= a b)))\n";
                        }
                        break;
                    case SymbolicExpr::OP_UGT:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvugt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (not (bvule" <<w <<" a b)))\n";

                        }
                        break;
                    case SymbolicExpr::OP_UGE:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvuge" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (not (bvult a b)))\n";
                        }
                        break;
                    case SymbolicExpr::OP_SLT:
                        // signed(a) < signed(b) iff signed(b) - signed(a) > 0 (i.e., sign bit is clear)
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvslt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd (bvneg a) b))))\n";
                        }
                        break;
                    case SymbolicExpr::OP_SLE:
                        // signed(a) <= signed(b) iff (signed(b) - signed(a) > 0 or a == b
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvsle" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (or (= a b) (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd (bvneg a) b)))))\n";
                        }
                        break;
                    case SymbolicExpr::OP_SGT:
                        // signed(a) > signed(b) iff (signed(a) - signed(b) > 0 (i.e., sign bit is clear)
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvsgt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd a (bvneg b)))))\n";
                        }
                        break;
                    case SymbolicExpr::OP_SGE:
                        // signed(a) >= signed(b) iff (signed(a) - signed(b) > 0 || a == b
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvsge" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (or (= a b) (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd a (bvneg b))))))\n";
                        }
                        break;
                    default:
                        // not a comparison operation
                        break;
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(o);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputCommonSubexpressions(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<SymbolicExpr::Ptr> cses = findCommonSubexpressions(exprs);
    size_t cseId = 0;
    BOOST_FOREACH (const SymbolicExpr::Ptr &cse, cses) {
        o <<"\n";
        if (!cse->comment().empty())
            o <<StringUtility::prefixLines(cse->comment(), "; ") <<"\n";
        o <<"; effective size = " <<StringUtility::plural(cse->nNodes(), "nodes")
          <<", actual size = " <<StringUtility::plural(cse->nNodesUnique(), "nodes") <<"\n";
        std::string termName = "cse_" + StringUtility::numberToString(++cseId);

        SExprTypePair et = outputCast(outputExpression(cse), BIT_VECTOR);
        ASSERT_not_null(et.first);

        o <<"(define-fun " <<termName <<" " <<typeName(cse) <<" " <<*et.first <<")\n";
        termNames_.insert(cse, StringTypePair(termName, et.second));
    }
}

SmtSolver::Type
SmtlibSolver::mostType(const std::vector<SExprTypePair> &ets) {
    typedef Sawyer::Container::Map<Type, size_t> Histogram;
    Histogram histogram;
    BOOST_FOREACH (const SExprTypePair &et, ets)
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

SmtSolver::SExprTypePair
SmtlibSolver::outputCast(const SExprTypePair &et, Type toType) {
    SExpr::Ptr retval;
    Type fromType = et.second;
    if (fromType == toType) {
        retval = et.first;
    } else if (BOOLEAN == fromType && BIT_VECTOR == toType) {
        retval = SExpr::instance(SExpr::instance("ite"), et.first, SExpr::instance("#b1"), SExpr::instance("#b0"));
    } else if (BIT_VECTOR == fromType && BOOLEAN == toType) {
        retval = SExpr::instance(SExpr::instance("="), et.first, SExpr::instance("#b1"));
    } else {
        Stringifier string(stringifyBinaryAnalysisSmtSolverType);
        ASSERT_not_reachable("invalid cast from " + string(et.second) + " to " + string(toType));
    }
    return SExprTypePair(retval, toType);
}

std::vector<SmtSolver::SExprTypePair>
SmtlibSolver::outputCast(const std::vector<SExprTypePair> &ets, Type toType) {
    std::vector<SExprTypePair> retval;
    retval.reserve(ets.size());
    BOOST_FOREACH (const SExprTypePair &et, ets)
        retval.push_back(outputCast(et, toType));
    return retval;
}

std::vector<SmtSolver::SExprTypePair>
SmtlibSolver::outputExpressions(const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<SExprTypePair> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(outputExpression(expr));
    return retval;
}

SmtSolver::SExprTypePair
SmtlibSolver::outputExpression(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    typedef std::vector<SExprTypePair> Etv;
    SExprTypePair retval;

    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    // If we previously found a common subexpression, then use its name rather than re-emitting the same expression again. This
    // can drastically reduce the size of the output file.
    StringTypePair st;
    if (termNames_.getOptional(expr).assignTo(st)) {
        return SExprTypePair(SExpr::instance(st.first), st.second);
    } else if (leaf) {
        retval = outputLeaf(leaf);
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case SymbolicExpr::OP_ADD:
                retval = outputLeftAssoc("bvadd", inode);
                break;
            case SymbolicExpr::OP_AND: {
                Etv children = outputExpressions(inode->children());
                Type type = mostType(children);
                children = outputCast(children, type);
                if (BOOLEAN == type) {
                    retval = outputLeftAssoc("and", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    retval = outputLeftAssoc("bvand", children);
                }
                break;
            }
            case SymbolicExpr::OP_ASR:
                retval = outputArithmeticShiftRight(inode);
                break;
            case SymbolicExpr::OP_XOR:
                retval = outputXor(inode);
                break;
            case SymbolicExpr::OP_EQ:
                retval = outputBinary("=", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_CONCAT: {
                Etv children = outputCast(outputExpressions(inode->children()), BIT_VECTOR);
                retval = outputLeftAssoc("concat", children);
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
                    retval = outputLeftAssoc("or", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    retval = outputLeftAssoc("bvor", children);
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
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpr::OP_SLE:
                retval = outputSignedCompare(inode);
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
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpr::OP_SGT:
                retval = outputSignedCompare(inode);
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
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpr::OP_UGT:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpr::OP_ULE:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpr::OP_ULT:
                retval = outputUnsignedCompare(inode);
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
    ASSERT_forbid(retval.second == NO_TYPE);
    return retval;
}

SmtSolver::SExprTypePair
SmtlibSolver::outputLeaf(const SymbolicExpr::LeafPtr &leaf) {
    SExprTypePair retval;
    if (leaf->isIntegerConstant()) {
        retval.second = BIT_VECTOR;
        if (leaf->nBits() % 4 == 0) {
            retval.first = SExpr::instance("#x" + leaf->bits().toHex());
        } else {
            retval.first = SExpr::instance("#b" + leaf->bits().toBinary());
        }
    } else if (leaf->isIntegerVariable()) {
        retval.second = BIT_VECTOR;
        retval.first = SExpr::instance(leaf->toString());
    } else {
        ASSERT_require(leaf->isMemoryVariable());
        retval.second = MEM_STATE;
        retval.first = SExpr::instance(leaf->toString());
    }
    return retval;
}

// ROSE (rose-operator expr) => SMT-LIB (smtlib-operator expr)
SmtSolver::SExprTypePair
SmtlibSolver::outputUnary(const std::string &name, const SExprTypePair &child) {
    ASSERT_require(!name.empty());
    SExpr::Ptr retval = SExpr::instance(SExpr::instance(name), child.first);
    return SExprTypePair(retval, child.second);
}

// ROSE (rose-operator a b) => SMT-LIB (smtlib-operator a b)
SmtSolver::SExprTypePair
SmtlibSolver::outputBinary(const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type rettype) {
    ASSERT_require(inode->nChildren() == 2);
    return outputLeftAssoc(name, inode, rettype);
}

// ROSE (rose-operator a b)     => SMT-LIB (smtlib-operator a b)
// ROSE (rose-operator a b c)   => SMT-LIB (smtlib-operator (smtlib-operator a b) c)
// ROSE (rose-operator a b c d) => SMT-LIB (smtlib-operator (smtlib-operator (smtlib-operator a b) c) d)
// etc.
// where "identity" is all zeros or all ones and the same width as "a".
SmtSolver::SExprTypePair
SmtlibSolver::outputLeftAssoc(const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type rettype) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() > 1);

    std::vector<SExprTypePair> children = outputExpressions(inode->children());
    return outputLeftAssoc(name, children, rettype);
}

SmtSolver::SExprTypePair
SmtlibSolver::outputLeftAssoc(const std::string &name, const std::vector<SExprTypePair> &children, Type rettype) {
    Type childType = mostType(children);
    SExpr::Ptr retval = outputCast(children[0], childType).first;
    for (size_t i=1; i<children.size(); ++i) {
        SExpr::Ptr child = outputCast(children[i], childType).first;
        retval = SExpr::instance(SExpr::instance(name), retval, child);
    }
    if (NO_TYPE == rettype)
        rettype = childType;
    return SExprTypePair(retval, rettype);
}

// SMT-LIB doesn't have a bit-wise XOR function, but we should have by now generated our own "bvxorN" functions where N is the
// width of the operands.
SmtSolver::SExprTypePair
SmtlibSolver::outputXor(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() > 1);

    std::vector<SExprTypePair> children = outputExpressions(inode->children());
    Type type = mostType(children);
    children = outputCast(children, type);
    if (BOOLEAN == type) {
        return outputLeftAssoc("xor", children);
    } else {
        ASSERT_require(BIT_VECTOR == type);
        return outputLeftAssoc("bvxor" + boost::lexical_cast<std::string>(inode->nBits()), children);
    }
}

// ROSE (extract lo hi expr) => SMT-LIB ((_ extract hi lo) expr); lo and hi are inclusive
SmtSolver::SExprTypePair
SmtlibSolver::outputExtract(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(1)->isIntegerConstant());
    size_t begin = inode->child(0)->toUnsigned().get();            // low, inclusive
    size_t end = inode->child(1)->toUnsigned().get();              // high, exclusive
    ASSERT_require(end > begin);
    ASSERT_require(end <= inode->child(2)->nBits());
    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(end-1),
                                        SExpr::instance(begin)),
                        outputCast(outputExpression(inode->child(2)), BIT_VECTOR).first);
    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (ite boolean-expr a b) => SMT-LIB (ite boolean-expr a b); a and b same size, condition is Boolean not bit vector
SmtSolver::SExprTypePair
SmtlibSolver::outputIte(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_ITE);
    ASSERT_require(inode->nChildren() == 3);

    SExpr::Ptr cond = outputCast(outputExpression(inode->child(0)), BOOLEAN).first;
    std::vector<SExprTypePair> alternatives;
    alternatives.push_back(outputExpression(inode->child(1)));
    alternatives.push_back(outputExpression(inode->child(2)));
    Type type = mostType(alternatives);
    alternatives = outputCast(alternatives, type);
    SExpr::Ptr retval = SExpr::instance(SExpr::instance("ite"), cond, alternatives[0].first, alternatives[1].first);
    return SExprTypePair(retval, type);
}

// ROSE (!= a b) => SMT-LIB (not (= a b))
SmtSolver::SExprTypePair
SmtlibSolver::outputNotEqual(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());
    std::vector<SExprTypePair> children = outputExpressions(inode->children());
    children = outputCast(children, mostType(children));
    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("not"),
                        SExpr::instance(SExpr::instance("="), children[0].first, children[1].first));
    return SExprTypePair(retval, BOOLEAN);
}

// ROSE (uextend size expr) =>
// SMT-LIB (concat zeros expr)
//
// Where "zeros" is a bit vector of all clear bits with width size-expr.size. Due to ROSE symbolic simplifications that have
// already occurred by this point, "size" is guaranteed to be greater than expr.size.
SmtSolver::SExprTypePair
SmtlibSolver::outputUnsignedExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    size_t newWidth = inode->child(0)->toUnsigned().get();
    size_t needBits = newWidth - inode->child(1)->nBits();

    SExpr::Ptr zeros =
        outputCast(outputExpression(SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(needBits, false))),
                   BIT_VECTOR).first;

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("concat"),
                        zeros,
                        outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first);

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (sextend size expr) =>
// SMT-LIB (concat (ite (= ((_ extract [expr.size-1] [expr.size-1]) expr) #b1)
//                      (bvnot zeros)
//                      zeros)
//                 expr)
//
//  where "zeros" is a bit vector of size size-expr.size with all bits clear.
//  The "size" > expr.size due to sextend simplifications that would have kicked in otherwise.
SmtSolver::SExprTypePair
SmtlibSolver::outputSignExtend(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    SymbolicExpr::Ptr newSize = inode->child(0);
    size_t signBitIdx = inode->child(1)->nBits() - 1;
    size_t growth = newSize->toUnsigned().get() - inode->child(1)->nBits();

    SExpr::Ptr expr = outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first;
    SExpr::Ptr zeros = outputExpression(SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(growth, false))).first;
    SExpr::Ptr ones = SExpr::instance(SExpr::instance("bvnot"), zeros);

    SExpr::Ptr isNegative =
        SExpr::instance(SExpr::instance("="),
                        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                                        SExpr::instance("extract"),
                                                        SExpr::instance(signBitIdx),
                                                        SExpr::instance(signBitIdx)),
                                        expr),
                        SExpr::instance("#b1"));

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("concat"),
                        SExpr::instance(SExpr::instance("ite"), isNegative, ones, zeros),
                        expr);

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (set a b ...) =>
// SMT-LIB (ite v1 (ite v2 ... b) a)
//
// where v1, v2, ... are new variables
SmtSolver::SExprTypePair
SmtlibSolver::outputSet(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(inode->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(inode);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(inode, SmtSolverPtr(), var);
    ite->comment(inode->comment());
    return outputExpression(ite);
}

// ROSE (ror amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvlshr (concat expr expr) extended_amount))
// where extended_amount is numerically equal to amount but extended to be 2*expr.nbits in width.
SmtSolver::SExprTypePair
SmtlibSolver::outputRotateRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*w), sa);

    SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(w-1),
                                        SExpr::instance(0)),
                        SExpr::instance(SExpr::instance("bvlshr"),
                                        SExpr::instance(SExpr::instance("concat"), shiftee, shiftee),
                                        outputCast(outputExpression(sa), BIT_VECTOR).first));
    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (rol amount expr) =>
// SMT-LIB (_ extract [2*expr.size-1] [expr.size] (bvshl (concat expr expr) extended_amount))
// where extended_amount is numerically equal to amount but extended to be 2*expr.nbits in width.
SmtSolver::SExprTypePair
SmtlibSolver::outputRotateLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, 2*w), sa);

    SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(2*w-1),
                                        SExpr::instance(w)),
                        SExpr::instance(SExpr::instance("bvshl"),
                                        SExpr::instance(SExpr::instance("concat"), shiftee, shiftee),
                                        outputCast(outputExpression(sa), BIT_VECTOR).first));
    return SExprTypePair(retval, BIT_VECTOR);
}

// For logical right shifts:
// ROSE (rose-right-shift-op amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvlshr (concat zeros_or_ones expr) extended_amount))
//
// Where extended_amount is the ROSE "amount" widened (or truncated) to the same width as "expr",
// and where "zeros_or_ones" is a constant with all bits set or clear and the same width as "expr"
SmtSolver::SExprTypePair
SmtlibSolver::outputLogicalShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHR0 || inode->getOperator() == SymbolicExpr::OP_SHR1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHR1;
    SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    SExpr::Ptr zerosOrOnes =
        outputCast(outputExpression(SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), newBits))),
                   BIT_VECTOR).first;

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(expr->nBits()-1),
                                        SExpr::instance(0)),
                        SExpr::instance(SExpr::instance("bvlshr"),
                                        SExpr::instance(SExpr::instance("concat"), zerosOrOnes, shiftee),
                                        outputCast(outputExpression(sa), BIT_VECTOR).first));

    return SExprTypePair(retval, BIT_VECTOR);
}

// For left shifts:
// ROSE (rose-left-shift-op amount expr) =>
// SMT-LIB ((_ extract [2*expr.size-1] expr.size) (bvshl (concat expr zeros_or_ones) extended_amount))
//
// Where extended_amount is the ROSE "amount" widened (or truncated) to the same width as "expr",
// and where "zeros_or_ones" is a constant with all bits set or clear and the same width as "expr"
SmtSolver::SExprTypePair
SmtlibSolver::outputShiftLeft(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHL0 || inode->getOperator() == SymbolicExpr::OP_SHL1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHL1;
    SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    SExpr::Ptr zerosOrOnes =
        outputCast(outputExpression(SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), newBits))),
                   BIT_VECTOR).first;

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(2*expr->nBits()-1),
                                        SExpr::instance(expr->nBits())),
                        SExpr::instance(SExpr::instance("bvshl"),
                                        SExpr::instance(SExpr::instance("concat"), shiftee, zerosOrOnes),
                                        outputCast(outputExpression(sa), BIT_VECTOR).first));

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (asr amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0)
//             (bvlshr
//                 (concat
//                     (ite (= ((_ extract [expr.size-1] [expr.size-1]) expr) #b1) ; is expr negative?
//                          (bvnot zeros)
//                          zeros)
//                     expr)
//                 extended_amount))
//
// where "extended_amount" is the shift amount signe extended to the same width as 2 * expr.nBits.
// where zeros is a constant containing all clear bits the same width as expr.
SmtSolver::SExprTypePair
SmtlibSolver::outputArithmeticShiftRight(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t width = expr->nBits();
    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, width), sa); //  widen same as expr

    SExprTypePair shiftee = outputExpression(expr);
    ASSERT_require(BIT_VECTOR == shiftee.second);

    SExpr::Ptr isNegative =
        SExpr::instance(SExpr::instance("="),
                        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                                        SExpr::instance("extract"),
                                                        SExpr::instance(width-1),
                                                        SExpr::instance(width-1)),
                                        shiftee.first),
                        SExpr::instance("#b1"));

    SExpr::Ptr zeros = outputExpression(SymbolicExpr::makeIntegerConstant(width, 0)).first;
    SExpr::Ptr ones = SExpr::instance(SExpr::instance("bvnot"), zeros);

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(width-1),
                                        SExpr::instance(0)),
                        SExpr::instance(SExpr::instance("bvlshr"),
                                        SExpr::instance(SExpr::instance("concat"),
                                                        SExpr::instance(SExpr::instance("ite"),
                                                                        isNegative, ones, zeros),
                                                        shiftee.first),
                                        outputCast(outputExpression(sa), BIT_VECTOR).first));

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (zerop expr) => SMT-LIB (= expr zeros)
// where "zeros" is a bit vector of all zeros the same size as "expr".
SmtSolver::SExprTypePair
SmtlibSolver::outputZerop(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);

    SymbolicExpr::Ptr zeros = SymbolicExpr::makeIntegerConstant(inode->child(0)->nBits(), 0);

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("="),
                        outputCast(outputExpression(inode->child(0)), BIT_VECTOR).first,
                        outputCast(outputExpression(zeros), BIT_VECTOR).first);

    return SExprTypePair(retval, BOOLEAN);
}

// ROSE (rose-multiply a b) =>
// SMT-LIB (smt-multiply a_extended b_extended)
//
// where "a_extended" and "b_extended" are extended (signed or unsigned) to the width a.size+b.size before being
// multiplied.
SmtSolver::SExprTypePair
SmtlibSolver::outputMultiply(const SymbolicExpr::InteriorPtr &inode) {
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

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("bvmul"),
                        outputCast(outputExpression(aExtended), BIT_VECTOR).first,
                        outputCast(outputExpression(bExtended), BIT_VECTOR).first);

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (udiv a b) =>
// SMT-LIB ((_ extract [a.size-1] 0) (bvudiv a_extended b_extended))
//
// where a_extended and b_extended are zero extended to have the width max(a.size,b.size).
SmtSolver::SExprTypePair
SmtlibSolver::outputDivide(const SymbolicExpr::InteriorPtr &inode, const std::string &operation) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(inode->child(0)->nBits()-1),
                                        SExpr::instance(0)),
                        SExpr::instance(SExpr::instance(operation),
                                        outputCast(outputExpression(aExtended), BIT_VECTOR).first,
                                        outputCast(outputExpression(bExtended), BIT_VECTOR).first));

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (umod a b) =>
// SMT-LIB ((_ extract [b.size-1] 0) (bvurem a_extended b_extended))
//
// where a_extended and b_extended are zero extended to have the width max(a.size,b.size).
SmtSolver::SExprTypePair
SmtlibSolver::outputModulo(const SymbolicExpr::InteriorPtr &inode, const std::string &operation) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, w), inode->child(1));

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                        SExpr::instance("extract"),
                                        SExpr::instance(inode->child(1)->nBits()-1),
                                        SExpr::instance(0)),
                        SExpr::instance(SExpr::instance("bvurem"),
                                        outputCast(outputExpression(aExtended), BIT_VECTOR).first,
                                        outputCast(outputExpression(bExtended), BIT_VECTOR).first));

    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (rose-sign-compare-op a b) =>
// SMT-LIB (...)
SmtSolver::SExprTypePair
SmtlibSolver::outputSignedCompare(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    std::string func;
    switch (inode->getOperator()) {
        case SymbolicExpr::OP_SLT: func = "bvslt"; break;
        case SymbolicExpr::OP_SLE: func = "bvsle"; break;
        case SymbolicExpr::OP_SGT: func = "bvsgt"; break;
        case SymbolicExpr::OP_SGE: func = "bvsge"; break;
        default:
            ASSERT_not_reachable("unhandled signed comparison operation");
    }

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(func),
                        outputCast(outputExpression(inode->child(0)), BIT_VECTOR).first,
                        outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first);

    return SExprTypePair(retval, BOOLEAN);
}

// ROSE (rose-unsigned-compare-op a b) =>
// SMT-LIB (...)
SmtSolver::SExprTypePair
SmtlibSolver::outputUnsignedCompare(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    std::string func;
    switch (inode->getOperator()) {
        case SymbolicExpr::OP_ULT:
            func = "bvult";
            break;
        case SymbolicExpr::OP_ULE:
            func = "bvule" + boost::lexical_cast<std::string>(inode->child(0)->nBits());
            break;
        case SymbolicExpr::OP_UGT:
            func = "bvugt" + boost::lexical_cast<std::string>(inode->child(0)->nBits());
            break;
        case SymbolicExpr::OP_UGE:
            func = "bvuge" + boost::lexical_cast<std::string>(inode->child(0)->nBits());
            break;
        default:
            ASSERT_not_reachable("unhandled unsigned comparison operation");
    }

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance(func),
                        outputCast(outputExpression(inode->child(0)), BIT_VECTOR).first,
                        outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first);

    return SExprTypePair(retval, BOOLEAN);
}

// ROSE (read mem addr) =>
// SMT-LIB (select mem addr)
SmtSolver::SExprTypePair
SmtlibSolver::outputRead(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("select"),
                        outputCast(outputExpression(inode->child(0)), MEM_STATE).first,
                        outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first);
    return SExprTypePair(retval, BIT_VECTOR);
}

// ROSE (write mem addr value) =>
// SMT-LIB (store mem addr value)
SmtSolver::SExprTypePair
SmtlibSolver::outputWrite(const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);

    SExpr::Ptr retval =
        SExpr::instance(SExpr::instance("store"),
                        outputCast(outputExpression(inode->child(0)), MEM_STATE).first,
                        outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first,
                        outputCast(outputExpression(inode->child(2)), BIT_VECTOR).first);

    return SExprTypePair(retval, MEM_STATE);
}

void
SmtlibSolver::parseEvidence() {
    requireLinkage(LM_EXECUTABLE);
    Sawyer::Stopwatch evidenceTimer;
    boost::regex varNameRe("v\\d+");

    // If memoization is being used and we have a previous result, then use the previous result. However, we need to undo the
    // variable renaming. That is, the memoized result is in terms of renumbered variables, so we need to use the
    // latestMemoizationRewrite_ to rename the memoized variables back to the variable names used in the actual query from the
    // caller.
    SymbolicExpr::Hash memoId = latestMemoizationId();
    if (memoId > 0) {
        MemoizedEvidence::iterator found = memoizedEvidence.find(memoId);
        if (found != memoizedEvidence.end()) {
            SymbolicExpr::ExprExprHashMap denorm = latestMemoizationRewrite_.invert();
            evidence.clear();
            BOOST_FOREACH (const ExprExprMap::Node &node, found->second.nodes()) {
                SymbolicExpr::Ptr var = node.key()->substituteMultiple(denorm);
                SymbolicExpr::Ptr val = node.value()->substituteMultiple(denorm);
                evidence.insert(var, val);
                SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<*var <<" == " <<*val <<"\n";
            }
            stats.evidenceTime += evidenceTimer.stop();
            return;
        }
    }

    // Parse the evidence
    BOOST_FOREACH (const SExpr::Ptr &sexpr, parsedOutput_) {
        if (sexpr->children().size() > 0 && sexpr->children()[0]->name() == "model") {
            for (size_t i = 1; i < sexpr->children().size(); ++i) {
                const SExpr::Ptr &elmt = sexpr->children()[i];

                // e.g., (define-fun v7 () (_ BitVec 32) #xdeadbeef)
                if (elmt->children().size() == 5 &&
                    elmt->children()[0]->name() == "define-fun" &&
                    elmt->children()[1]->name() != "" &&
                    elmt->children()[2]->name() == "" && elmt->children()[2]->children().size() == 0 &&
                    elmt->children()[3]->children().size() == 3 &&
                    elmt->children()[3]->children()[0]->name() == "_" &&
                    elmt->children()[3]->children()[1]->name() == "BitVec" &&
                    elmt->children()[4]->name().substr(0, 1) == "#") {

                    size_t nBits = boost::lexical_cast<size_t>(elmt->children()[3]->children()[2]->name());

                    // Variable
                    std::string varName = elmt->children()[1]->name();
                    if (!boost::regex_match(varName, varNameRe)) {
                        mlog[ERROR] <<"malformed variable name \"" <<StringUtility::cEscape(varName) <<"\" in evidence: ";
                        printSExpression(mlog[ERROR], elmt);
                        continue;
                    }
                    size_t varId = boost::lexical_cast<size_t>(varName.substr(1));
                    SymbolicExpr::Ptr var = SymbolicExpr::makeIntegerVariable(nBits, varId);

                    // Value
                    std::string valStr = elmt->children()[4]->name();
                    ASSERT_require(nBits > 0);
                    Sawyer::Container::BitVector bits(nBits);
                    if (boost::starts_with(valStr, "#x")) {
                        bits.fromHex(valStr.substr(2));
                    } else if (boost::starts_with(valStr, "#b")) {
                        bits.fromBinary(valStr.substr(2));
                    } else {
                        ASSERT_not_reachable("unknown bit vector literal \"" + StringUtility::cEscape(valStr) + "\"");
                    }
                    SymbolicExpr::Ptr val = SymbolicExpr::makeIntegerConstant(bits);

                    SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<*var <<" == " <<*val <<"\n";
                    evidence.insert(var, val);

                } else if (mlog[WARN]) {
                    mlog[WARN] <<"malformed model element ignored: ";
                    printSExpression(mlog[WARN], elmt);
                    mlog[WARN] <<"\n";
                }
            }
        }
    }

    // Cache the evidence. We must cache using the normalized form of expressions.
    if (memoId > 0) {
        ExprExprMap &me = memoizedEvidence[memoId];
        BOOST_FOREACH (const ExprExprMap::Node &node, evidence.nodes()) {
            me.insert(node.key()->substituteMultiple(latestMemoizationRewrite_),
                      node.value()->substituteMultiple(latestMemoizationRewrite_));
        }
    }

    stats.evidenceTime += evidenceTimer.stop();
}

SymbolicExpr::Ptr
SmtlibSolver::evidenceForName(const std::string &varName) {
    BOOST_FOREACH (const ExprExprMap::Node &node, evidence.nodes()) {
        ASSERT_not_null(node.key()->isLeafNode());
        ASSERT_require(node.key()->isLeafNode()->isVariable2());
        if (node.key()->isLeafNode()->toString() == varName)
            return node.value();
    }
    return SymbolicExpr::Ptr();
}

std::vector<std::string>
SmtlibSolver::evidenceNames() {
    std::vector<std::string> retval;
    BOOST_FOREACH (const SymbolicExpr::Ptr &varExpr, evidence.keys()) {
        SymbolicExpr::LeafPtr leaf = varExpr->isLeafNode();
        ASSERT_not_null(leaf);
        ASSERT_require(leaf->isVariable2());
        retval.push_back(leaf->toString());
    }
    return retval;
}


} // namespace
} // namespace

#endif
