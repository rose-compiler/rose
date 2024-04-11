#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/SmtlibSolver.h>

#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Convert.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>
#include <stringify.h>                                  // rose

#include <Sawyer/Stopwatch.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

SmtlibSolver::Ptr
SmtlibSolver::create() const {
    auto newSolver = new SmtlibSolver(name(), executable_, shellArgs_, linkage());
    if (timeout_)
        newSolver->timeout(*timeout_);
    newSolver->memoizer(memoizer());
    return Ptr(newSolver);
}

void
SmtlibSolver::reset() {
    SmtSolver::reset();
    varsForSets_.clear();
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
SmtlibSolver::generateFile(std::ostream &o, const std::vector<SymbolicExpression::Ptr> &exprs, Definitions*) {
    requireLinkage(LM_EXECUTABLE);

    o <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Statistics\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"\n"
      <<"; number of expressions: " <<exprs.size() <<"\n"
      <<"; size of each expression: actual / effective:\n";
    for (const SymbolicExpression::Ptr &e: exprs)
        o <<(boost::format(";   %9d / %-9d\n") % e->nNodesUnique() % e->nNodes());
    o <<"\n";

    if (timeout_) {
        // It's not well documented. Experimentally determined to be milliseconds using Z3.
        o <<"(set-option :timeout " <<(unsigned)::round(timeout_->count()*1000) <<")\n\n";
    }

    // Find all variables
    VariableSet vars;
    for (const SymbolicExpression::Ptr &expr: exprs) {
        VariableSet tmp;
        findVariables(expr, tmp);
        for (const SymbolicExpression::LeafPtr &var: tmp.values())
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

    for (const SymbolicExpression::Ptr &expr: exprs) {
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
    for (const SExpr::Ptr &sexpr: parsedOutput_) {
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
SmtlibSolver::typeName(const SymbolicExpression::Ptr &expr) {
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
SmtlibSolver::outputAssertion(std::ostream &o, const SymbolicExpression::Ptr &expr) {
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
SmtlibSolver::varForSet(const SymbolicExpression::InteriorPtr &set, const SymbolicExpression::LeafPtr &var) {
    ASSERT_not_null(set);
    ASSERT_not_null(var);
    varsForSets_.insert(set, var);
}

SymbolicExpression::LeafPtr
SmtlibSolver::varForSet(const SymbolicExpression::InteriorPtr &set) {
    ASSERT_not_null(set);
    SymbolicExpression::Ptr expr = varsForSets_.getOrDefault(set);
    ASSERT_not_null(expr);
    SymbolicExpression::LeafPtr var = expr->isLeafNode();
    ASSERT_not_null(var);
    return var;
}

// A side effect is that the varsForSets_ map is updated.
void
SmtlibSolver::findVariables(const SymbolicExpression::Ptr &expr, VariableSet &variables) {
    struct T1: SymbolicExpression::Visitor {
        SmtlibSolver *self;
        VariableSet &variables;
        std::set<const SymbolicExpression::Node*> seen;

        T1(SmtlibSolver *self, VariableSet &variables): self(self), variables(variables) {}

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Node *node) {
            if (!seen.insert(node).second)
                return SymbolicExpression::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpression::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable2())
                    variables.insert(leaf);
            } else if (auto inode = dynamic_cast<const SymbolicExpression::Interior*>(node)) {
                if (inode->getOperator() == SymbolicExpression::OP_SET) {
                    // Sets are ultimately converted to ITEs and therefore each set needs a free variable.
                    SymbolicExpression::LeafPtr var = SymbolicExpression::makeIntegerVariable(32, "set")->isLeafNode();
                    variables.insert(var);
                    self->varForSet(inode->sharedFromThis()->isInteriorNode(), var);
                }
            }
            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Node*) {
            return SymbolicExpression::CONTINUE;
        }
    } t1(this, variables);
    expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputVariableDeclarations(std::ostream &o, const VariableSet &variables) {
    for (const SymbolicExpression::LeafPtr &var: variables.values()) {
        ASSERT_require(var->isVariable2());
        o <<"\n";
        if (!var->comment().empty())
            o <<StringUtility::prefixLines(var->comment(), "; ") <<"\n";
        o <<"(declare-fun " <<var->toString() <<" " <<typeName(var) <<")\n";
    }
}

void
SmtlibSolver::outputComments(std::ostream &o, const std::vector<SymbolicExpression::Ptr> &exprs) {
    struct T1: SymbolicExpression::Visitor {
        typedef std::set<const SymbolicExpression::Node*> SeenNodes;
        SeenNodes seen;
        std::ostream &o;
        bool commented;

        T1(std::ostream &o)
            : o(o), commented(false) {}

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpression::TRUNCATE;          // already processed this subexpression
            if (const SymbolicExpression::Leaf *leaf = node->isLeafNodeRaw()) {
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
            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            return SymbolicExpression::CONTINUE;
        }
    } t1(o);

    for (const SymbolicExpression::Ptr &expr: exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputBvxorFunctions(std::ostream &o, const std::vector<SymbolicExpression::Ptr> &exprs) {
    struct T1: SymbolicExpression::Visitor {
        std::set<size_t> widths;
        std::ostream &o;

        T1(std::ostream &o)
            : o(o) {}

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            if (SymbolicExpression::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpression::OP_XOR && widths.insert(inode->nBits()).second) {
                    size_t w = inode->nBits();
                    o <<"(define-fun bvxor" <<w
                      <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                      <<" (_ BitVec " <<w <<")"
                      <<" (bvor (bvand a (bvnot b)) (bvand (bvnot a) b)))\n";
                }
            }
            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            return SymbolicExpression::CONTINUE;
        }
    } t1(o);

    for (const SymbolicExpression::Ptr &expr: exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputComparisonFunctions(std::ostream &o, const std::vector<SymbolicExpression::Ptr> &exprs) {
    struct T1: SymbolicExpression::Visitor {
        Sawyer::Container::Map<SymbolicExpression::Operator, std::set<size_t> > widths;
        std::ostream &o;

        T1(std::ostream &o): o(o) {}

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            if (SymbolicExpression::InteriorPtr inode = node->isInteriorNode()) {
                size_t w = inode->child(0)->nBits();
                switch (inode->getOperator()) {
                    case SymbolicExpression::OP_ULT:
                        break;                          // SMT-LIB 2 defines "bvult" for this
                    case SymbolicExpression::OP_ULE:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvule" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (or (bvult a b) (= a b)))\n";
                        }
                        break;
                    case SymbolicExpression::OP_UGT:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvugt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (not (bvule" <<w <<" a b)))\n";

                        }
                        break;
                    case SymbolicExpression::OP_UGE:
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvuge" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (not (bvult a b)))\n";
                        }
                        break;
                    case SymbolicExpression::OP_SLT:
                        // signed(a) < signed(b) iff signed(b) - signed(a) > 0 (i.e., sign bit is clear)
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvslt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd (bvneg a) b))))\n";
                        }
                        break;
                    case SymbolicExpression::OP_SLE:
                        // signed(a) <= signed(b) iff (signed(b) - signed(a) > 0 or a == b
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvsle" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (or (= a b) (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd (bvneg a) b)))))\n";
                        }
                        break;
                    case SymbolicExpression::OP_SGT:
                        // signed(a) > signed(b) iff (signed(a) - signed(b) > 0 (i.e., sign bit is clear)
                        if (widths.insertMaybeDefault(inode->getOperator()).insert(w).second) {
                            o <<"(define-fun bvsgt" <<w
                              <<" ((a (_ BitVec " <<w <<")) (b (_ BitVec " <<w <<")))"
                              <<" Bool"
                              <<" (= #b0 ((_ extract " <<(w-1) <<" " <<(w-1) <<") (bvadd a (bvneg b)))))\n";
                        }
                        break;
                    case SymbolicExpression::OP_SGE:
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
            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            return SymbolicExpression::CONTINUE;
        }
    } t1(o);

    for (const SymbolicExpression::Ptr &expr: exprs)
        expr->depthFirstTraversal(t1);
}

void
SmtlibSolver::outputCommonSubexpressions(std::ostream &o, const std::vector<SymbolicExpression::Ptr> &exprs) {
    std::vector<SymbolicExpression::Ptr> cses = findCommonSubexpressions(exprs);
    size_t cseId = 0;
    for (const SymbolicExpression::Ptr &cse: cses) {
        o <<"\n";
        if (!cse->comment().empty())
            o <<StringUtility::prefixLines(cse->comment(), "; ") <<"\n";
        o <<"; effective size = " <<StringUtility::plural(cse->nNodes(), "nodes")
          <<", actual size = " <<StringUtility::plural(cse->nNodesUnique(), "nodes") <<"\n";
        o <<"; ROSE expression: " <<*cse <<"\n";

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
    for (const SExprTypePair &et: ets)
        ++histogram.insertMaybe(et.second, 0);
    Type bestType = NO_TYPE;
    size_t bestCount = 0;
    for (const Histogram::Node &node: histogram.nodes()) {
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
    for (const SExprTypePair &et: ets)
        retval.push_back(outputCast(et, toType));
    return retval;
}

std::vector<SmtSolver::SExprTypePair>
SmtlibSolver::outputExpressions(const std::vector<SymbolicExpression::Ptr> &exprs) {
    std::vector<SExprTypePair> retval;
    retval.reserve(exprs.size());
    for (const SymbolicExpression::Ptr &expr: exprs)
        retval.push_back(outputExpression(expr));
    return retval;
}

SmtSolver::SExprTypePair
SmtlibSolver::outputExpression(const SymbolicExpression::Ptr &expr) {
    ASSERT_not_null(expr);
    typedef std::vector<SExprTypePair> Etv;
    SExprTypePair retval;

    SymbolicExpression::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpression::InteriorPtr inode = expr->isInteriorNode();

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
            case SymbolicExpression::OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case SymbolicExpression::OP_ADD:
                retval = outputLeftAssoc("bvadd", inode);
                break;
            case SymbolicExpression::OP_AND: {
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
            case SymbolicExpression::OP_ASR:
                retval = outputArithmeticShiftRight(inode);
                break;
            case SymbolicExpression::OP_XOR:
                retval = outputXor(inode);
                break;
            case SymbolicExpression::OP_EQ:
                retval = outputBinary("=", inode, BOOLEAN);
                break;
            case SymbolicExpression::OP_CONCAT: {
                Etv children = outputCast(outputExpressions(inode->children()), BIT_VECTOR);
                retval = outputLeftAssoc("concat", children);
                break;
            }
            case SymbolicExpression::OP_CONVERT:
                throw Exception("OP_CONVERT is not implemented yet");
            case SymbolicExpression::OP_EXTRACT:
                retval = outputExtract(inode);
                break;
            case SymbolicExpression::OP_FP_ABS:
            case SymbolicExpression::OP_FP_NEGATE:
            case SymbolicExpression::OP_FP_ADD:
            case SymbolicExpression::OP_FP_MUL:
            case SymbolicExpression::OP_FP_DIV:
            case SymbolicExpression::OP_FP_MULADD:
            case SymbolicExpression::OP_FP_SQRT:
            case SymbolicExpression::OP_FP_MOD:
            case SymbolicExpression::OP_FP_ROUND:
            case SymbolicExpression::OP_FP_MIN:
            case SymbolicExpression::OP_FP_MAX:
            case SymbolicExpression::OP_FP_LE:
            case SymbolicExpression::OP_FP_LT:
            case SymbolicExpression::OP_FP_GE:
            case SymbolicExpression::OP_FP_GT:
            case SymbolicExpression::OP_FP_EQ:
            case SymbolicExpression::OP_FP_ISNORM:
            case SymbolicExpression::OP_FP_ISSUBNORM:
            case SymbolicExpression::OP_FP_ISZERO:
            case SymbolicExpression::OP_FP_ISINFINITE:
            case SymbolicExpression::OP_FP_ISNAN:
            case SymbolicExpression::OP_FP_ISNEG:
            case SymbolicExpression::OP_FP_ISPOS:
                throw Exception("floating point operations are not implemented yet");
            case SymbolicExpression::OP_INVERT: {
                ASSERT_require(inode->nChildren() == 1);
                SExprTypePair child = outputExpression(inode->child(0));
                retval = outputUnary((BOOLEAN==child.second?"not":"bvnot"), child);
                break;
            }
            case SymbolicExpression::OP_ITE:
                retval = outputIte(inode);
                break;
            case SymbolicExpression::OP_LET:
                throw Exception("OP_LET not implemented");
            case SymbolicExpression::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpression::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpression::OP_NE:
                retval = outputNotEqual(inode);
                break;
            case SymbolicExpression::OP_NEGATE: {
                ASSERT_require(inode->nChildren() == 1);
                SExprTypePair child = outputCast(outputExpression(inode->child(0)), BIT_VECTOR);
                retval = outputUnary("bvneg", child);
                break;
            }
            case SymbolicExpression::OP_NOOP:
                retval = outputExpression(SymbolicExpression::makeIntegerConstant(inode->nBits(), 0));
                break;
            case SymbolicExpression::OP_OR: {
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
            case SymbolicExpression::OP_READ:
                retval = outputRead(inode);
                break;
            case SymbolicExpression::OP_REINTERPRET:
                throw Exception("OP_REINTERPRET is not implemented yet");
            case SymbolicExpression::OP_ROL:
                retval = outputRotateLeft(inode);
                break;
            case SymbolicExpression::OP_ROR:
                retval = outputRotateRight(inode);
                break;
            case SymbolicExpression::OP_SDIV:
                retval = outputDivide(inode, "bvsdiv");
                break;
            case SymbolicExpression::OP_SET:
                retval = outputSet(inode);
                break;
            case SymbolicExpression::OP_SEXTEND:
                retval = outputSignExtend(inode);
                break;
            case SymbolicExpression::OP_SLT:
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpression::OP_SLE:
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpression::OP_SHL0:
                retval = outputShiftLeft0(inode);
                break;
            case SymbolicExpression::OP_SHL1:
                retval = outputShiftLeft1(inode);
                break;
            case SymbolicExpression::OP_SHR0:
                retval = outputLogicalShiftRight0(inode);
                break;
            case SymbolicExpression::OP_SHR1:
                retval = outputLogicalShiftRight1(inode);
                break;
            case SymbolicExpression::OP_SGE:
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpression::OP_SGT:
                retval = outputSignedCompare(inode);
                break;
            case SymbolicExpression::OP_SMOD:
                retval = outputModulo(inode, "bvsrem");
                break;
            case SymbolicExpression::OP_SMUL:
                retval = outputMultiply(inode);
                break;
            case SymbolicExpression::OP_UDIV:
                retval = outputDivide(inode, "bvudiv");
                break;
            case SymbolicExpression::OP_UEXTEND:
                retval = outputUnsignedExtend(inode);
                break;
            case SymbolicExpression::OP_UGE:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpression::OP_UGT:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpression::OP_ULE:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpression::OP_ULT:
                retval = outputUnsignedCompare(inode);
                break;
            case SymbolicExpression::OP_UMOD:
                retval = outputModulo(inode, "bvurem");
                break;
            case SymbolicExpression::OP_UMUL:
                retval = outputMultiply(inode);
                break;
            case SymbolicExpression::OP_WRITE:
                retval = outputWrite(inode);
                break;
            case SymbolicExpression::OP_ZEROP:
                retval = outputZerop(inode);
                break;
        }
    }
    ASSERT_not_null(retval.first);
    ASSERT_forbid(retval.second == NO_TYPE);
    return retval;
}

SmtSolver::SExprTypePair
SmtlibSolver::outputLeaf(const SymbolicExpression::LeafPtr &leaf) {
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
SmtlibSolver::outputBinary(const std::string &name, const SymbolicExpression::InteriorPtr &inode, Type rettype) {
    ASSERT_require(inode->nChildren() == 2);
    return outputLeftAssoc(name, inode, rettype);
}

// ROSE (rose-operator a b)     => SMT-LIB (smtlib-operator a b)
// ROSE (rose-operator a b c)   => SMT-LIB (smtlib-operator (smtlib-operator a b) c)
// ROSE (rose-operator a b c d) => SMT-LIB (smtlib-operator (smtlib-operator (smtlib-operator a b) c) d)
// etc.
// where "identity" is all zeros or all ones and the same width as "a".
SmtSolver::SExprTypePair
SmtlibSolver::outputLeftAssoc(const std::string &name, const SymbolicExpression::InteriorPtr &inode, Type rettype) {
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
SmtlibSolver::outputXor(const SymbolicExpression::InteriorPtr &inode) {
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
SmtlibSolver::outputExtract(const SymbolicExpression::InteriorPtr &inode) {
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
SmtlibSolver::outputIte(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_ITE);
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
SmtlibSolver::outputNotEqual(const SymbolicExpression::InteriorPtr &inode) {
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
SmtlibSolver::outputUnsignedExtend(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    size_t newWidth = inode->child(0)->toUnsigned().get();
    size_t needBits = newWidth - inode->child(1)->nBits();

    SExpr::Ptr zeros =
        outputCast(outputExpression(SymbolicExpression::makeIntegerConstant(Sawyer::Container::BitVector(needBits, false))),
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
SmtlibSolver::outputSignExtend(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isIntegerConstant());
    ASSERT_require(inode->child(0)->toUnsigned().get() > inode->child(1)->nBits());
    SymbolicExpression::Ptr newSize = inode->child(0);
    size_t signBitIdx = inode->child(1)->nBits() - 1;
    size_t growth = newSize->toUnsigned().get() - inode->child(1)->nBits();

    SExpr::Ptr expr = outputCast(outputExpression(inode->child(1)), BIT_VECTOR).first;
    SExpr::Ptr zeros = outputExpression(SymbolicExpression::makeIntegerConstant(Sawyer::Container::BitVector(growth, false))).first;
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
SmtlibSolver::outputSet(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_SET);
    ASSERT_require(inode->nChildren() >= 2);
    SymbolicExpression::LeafPtr var = varForSet(inode);
    SymbolicExpression::Ptr ite = SymbolicExpression::setToIte(inode, SmtSolverPtr(), var);
    ite->comment(inode->comment());
    return outputExpression(ite);
}

// ROSE (ror amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvlshr (concat expr expr) extended_amount))
// where extended_amount is numerically equal to amount but extended to be 2*expr.nbits in width.
SmtSolver::SExprTypePair
SmtlibSolver::outputRotateRight(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpression::Ptr sa = inode->child(0);
    SymbolicExpression::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, 2*w), sa);

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
SmtlibSolver::outputRotateLeft(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpression::Ptr sa = inode->child(0);
    SymbolicExpression::Ptr expr = inode->child(1);
    size_t w = expr->nBits();
    sa = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, 2*w), sa);

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

// Logical right shift introducing zero bits.
//
// ROSE: (shr0 AMOUNT EXPR)
// SMT-LIB: (bvlshr EXPR EXTENDED_AMOUNT)
//
// Where EXTENDED_AMOUNT is AMOUNT zero extended to be the same width as EXPR.
SmtSolver::SExprTypePair
SmtlibSolver::outputLogicalShiftRight0(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_SHR0);
    ASSERT_require(inode->nChildren() == 2);

    // Original arguments
    const SymbolicExpression::Ptr sa = inode->child(0);
    const SymbolicExpression::Ptr expr = inode->child(1);

    // The shift amount extended to the same width as the thing to be shifted
    const SExpr::Ptr extended_va =
        outputCast(outputExpression(SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, expr->nBits()), sa)),
                   BIT_VECTOR).first;

    // The thing to be shifted
    const SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    // Use SMT-LIB's bvlshr operation
    const SExpr::Ptr retval = SExpr::instance(SExpr::instance("bvlshr"), shiftee, extended_va);
    return SExprTypePair(retval, BIT_VECTOR);
}

// Logical right shift introducing one bits.
//
// ROSE: (shr1 AMOUNT EXPR)
// SMT-LIB: ((_ extract [EXPR.size-1] 0) ; the low-order half
//              (bvlshr
//                  (concat ONES EXPR)   ; thing to shift, ONES in high half, EXPR in low half
//                  EXTENDED_AMOUNT))
//
// SMT-LIB doesn't have a logical right-shift operation that introduces ones (only zeros), so we do it the hard way.  We double
// the width of the value to be shifted by concatenating one bits in the high half. Then we shift the double-width value and
// return just the low half.
//
// The EXTENDED_AMOUNT is the original AMOUNT zero extended to be twice the width of the original EXPR (and the same width as
// the value that SMT-LIB is shifting.
SmtSolver::SExprTypePair
SmtlibSolver::outputLogicalShiftRight1(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_SHR1);
    ASSERT_require(inode->nChildren() == 2);

    // Original arguments
    const SymbolicExpression::Ptr sa = inode->child(0);
    const SymbolicExpression::Ptr expr = inode->child(1);

    // Upper half of the bits to be shifted (all ones)
    const SExpr::Ptr upper =
        outputCast(outputExpression(SymbolicExpression::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), true))),
                   BIT_VECTOR).first;

    // Lower half of the bits to be shifted
    const SExpr::Ptr lower = outputCast(outputExpression(expr), BIT_VECTOR).first;

    // The thing to be shifted
    const SExpr::Ptr shiftee = SExpr::instance(SExpr::instance("concat"), upper, lower);

    // The shift amount zero extended to the same width as the shiftee (twice the width of the original expr)
    const SExpr::Ptr extended_sa =
        outputCast(outputExpression(SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, 2*expr->nBits()), sa)),
                   BIT_VECTOR).first;

    // Shift the double-wide shiftee to the right using SMT-LIB's bvlshr operator that introduces zeros at the high end.
    const SExpr::Ptr shifted = SExpr::instance(SExpr::instance("bvlshr"), shiftee, extended_sa);

    // Extract just the low half of the shifted result.
    const SExpr::Ptr retval = SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                                              SExpr::instance("extract"),
                                                              SExpr::instance(expr->nBits() - 1),
                                                              SExpr::instance(0)),
                                              shifted);
    return SExprTypePair(retval, BIT_VECTOR);
}

// Left shift introducing zero bits.
// ROSE: (shl0 AMOUNT EXPR)
// SMT-LIB: (bvshl EXPR EXTENDED_AMOUNT)
//
// Where EXTENDED_AMOUNT is the AMOUNT zero extended to be the same width as EXPR.
SmtSolver::SExprTypePair
SmtlibSolver::outputShiftLeft0(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_SHL0);

    // Original arguments
    const SymbolicExpression::Ptr sa = inode->child(0);
    const SymbolicExpression::Ptr expr = inode->child(1);

    // The shift amount extended to the same width as the thing to be shifted
    const SExpr::Ptr extended_sa =
        outputCast(outputExpression(SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, expr->nBits()), sa)),
                   BIT_VECTOR).first;

    // The thing to be shifted
    const SExpr::Ptr shiftee = outputCast(outputExpression(expr), BIT_VECTOR).first;

    // Use SMT-LIB's left shift operation
    const SExpr::Ptr retval = SExpr::instance(SExpr::instance("bvshl"), shiftee, extended_sa);
    return SExprTypePair(retval, BIT_VECTOR);
}

// Left shift introducing one bits.
//
// ROSE: (shl1 AMOUNT EXPR)
// SMT-LIB: ((_ extract [2*EXPR.size-1] [EXPR.size]) ; the upper half
//             (bvshl
//                 (concat EXPR ONES) ; i.e., the original value padded with zeros or ones in the low bits
//                 EXTENDED_AMOUNT))
//
// Where EXTENDED_AMOUNT is the original AMOUNT extended to be the same width as either EXPR (in the shl0 case) or twice the
// width of EXPR (in the shl1 case).
//
// What we're doing is concatenating EXPR|ONES to make a value that's twice as wide as EXPR with the EXPR part in the
// high-order half. Then we shift left, introducing zeros (it doesn't matter that they're zeros, but that's all SMT-LIB can
// do), and then we return the upper half of the bits which are the original EXPR bits and some of the ONES bits.
SmtSolver::SExprTypePair
SmtlibSolver::outputShiftLeft1(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpression::OP_SHL1);

    // Original arguments
    const SymbolicExpression::Ptr sa = inode->child(0);
    const SymbolicExpression::Ptr expr = inode->child(1);

    // Upper half of the bits to be shifted
    const SExpr::Ptr upper = outputCast(outputExpression(expr), BIT_VECTOR).first;

    // Lower half of the bits to be shifted are all set
    const SExpr::Ptr lower =
        outputCast(outputExpression(SymbolicExpression::makeIntegerConstant(Sawyer::Container::BitVector(expr->nBits(), true))),
                   BIT_VECTOR).first;

    // The thing to be shifted, twice as wide as the original ROSE argument, consisting of the original ROSE argument in the
    // high bits, and all ones in the low bits.
    const SExpr::Ptr shiftee = SExpr::instance(SExpr::instance("concat"), upper, lower);

    // The shift amount extended to the same width as the thing to be shifted since SMT-LIB requires that both arguments are
    // the same type.
    const SExpr::Ptr extended_sa =
        outputCast(outputExpression(SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, 2*expr->nBits()), sa)),
                   BIT_VECTOR).first;

    // Shift the double-wide value to the left using SMT-LIB's bvshl operator that introduces zeros at the low end.
    const SExpr::Ptr shifted = SExpr::instance(SExpr::instance("bvshl"), shiftee, extended_sa);

    // Extract just the high half of the shifted result.
    const SExpr::Ptr retval = SExpr::instance(SExpr::instance(SExpr::instance("_"),
                                                              SExpr::instance("extract"),
                                                              SExpr::instance(2*expr->nBits() - 1),
                                                              SExpr::instance(expr->nBits())),
                                              shifted);

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
SmtlibSolver::outputArithmeticShiftRight(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpression::Ptr sa = inode->child(0);
    SymbolicExpression::Ptr expr = inode->child(1);
    size_t width = expr->nBits();
    sa = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, width), sa); //  widen same as expr

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

    SExpr::Ptr zeros = outputExpression(SymbolicExpression::makeIntegerConstant(width, 0)).first;
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
SmtlibSolver::outputZerop(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);

    SymbolicExpression::Ptr zeros = SymbolicExpression::makeIntegerConstant(inode->child(0)->nBits(), 0);

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
SmtlibSolver::outputMultiply(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpression::Ptr a = inode->child(0);
    SymbolicExpression::Ptr b = inode->child(1);
    SymbolicExpression::Ptr resultSize = SymbolicExpression::makeIntegerConstant(32, a->nBits() + b->nBits());

    SymbolicExpression::Ptr aExtended, bExtended;
    if (inode->getOperator() == SymbolicExpression::OP_SMUL) {
        aExtended = SymbolicExpression::makeSignExtend(resultSize, a);
        bExtended = SymbolicExpression::makeSignExtend(resultSize, b);
    } else {
        ASSERT_require(inode->getOperator() == SymbolicExpression::OP_UMUL);
        aExtended = SymbolicExpression::makeExtend(resultSize, a);
        bExtended = SymbolicExpression::makeExtend(resultSize, b);
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
SmtlibSolver::outputDivide(const SymbolicExpression::InteriorPtr &inode, const std::string &operation) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpression::Ptr aExtended = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpression::Ptr bExtended = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, w), inode->child(1));

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
SmtlibSolver::outputModulo(const SymbolicExpression::InteriorPtr &inode, const std::string &/*operation*/) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpression::Ptr aExtended = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, w), inode->child(0));
    SymbolicExpression::Ptr bExtended = SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, w), inode->child(1));

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
SmtlibSolver::outputSignedCompare(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    std::string func;
    switch (inode->getOperator()) {
        case SymbolicExpression::OP_SLT: func = "bvslt"; break;
        case SymbolicExpression::OP_SLE: func = "bvsle"; break;
        case SymbolicExpression::OP_SGT: func = "bvsgt"; break;
        case SymbolicExpression::OP_SGE: func = "bvsge"; break;
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
SmtlibSolver::outputUnsignedCompare(const SymbolicExpression::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    std::string func;
    switch (inode->getOperator()) {
        case SymbolicExpression::OP_ULT:
            func = "bvult";
            break;
        case SymbolicExpression::OP_ULE:
            func = "bvule" + boost::lexical_cast<std::string>(inode->child(0)->nBits());
            break;
        case SymbolicExpression::OP_UGT:
            func = "bvugt" + boost::lexical_cast<std::string>(inode->child(0)->nBits());
            break;
        case SymbolicExpression::OP_UGE:
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
SmtlibSolver::outputRead(const SymbolicExpression::InteriorPtr &inode) {
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
SmtlibSolver::outputWrite(const SymbolicExpression::InteriorPtr &inode) {
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
    boost::regex cseNameRe("cse_\\d+");

    // Parse the evidence.
    //   z3 4.8.7 returns (model F1 F2 ...)
    //   z3 4.8.12 returns (F1 F2 ...)
    // where Fi are of the form:
    //   (define-fun V () (_ BitVec N) #xXXX)
    // where V is the ROSE variable name, e.g., "v100")
    // and N is the size in bits
    // and X are hexadecimal characters (lower-case x is a literal "x")
    for (const SExpr::Ptr &sexpr: parsedOutput_) {
        Sawyer::Optional<size_t> foundEvidenceStartingAt;
        if (sexpr->children().size() > 0) {
            if (sexpr->children()[0]->name() == "model") {
                foundEvidenceStartingAt = 1;
            } else {
                foundEvidenceStartingAt = 0;
            }
        }

        if (foundEvidenceStartingAt) {
            for (size_t i = *foundEvidenceStartingAt; i < sexpr->children().size(); ++i) {
                const SExpr::Ptr &elmt = sexpr->children()[i];

                if (elmt->children().size() == 5 &&
                    elmt->children()[0]->name() == "define-fun" &&
                    boost::regex_match(elmt->children()[1]->name(), varNameRe) &&
                    elmt->children()[2]->name() == "" && elmt->children()[2]->children().size() == 0 &&
                    elmt->children()[3]->children().size() == 3 &&
                    elmt->children()[3]->children()[0]->name() == "_" &&
                    elmt->children()[3]->children()[1]->name() == "BitVec" &&
                    elmt->children()[4]->name().substr(0, 1) == "#") {
                    // e.g., (define-fun v7 () (_ BitVec 32) #xdeadbeef)

                    size_t nBits = boost::lexical_cast<size_t>(elmt->children()[3]->children()[2]->name());

                    // Variable
                    std::string varName = elmt->children()[1]->name();
                    size_t varId = boost::lexical_cast<size_t>(varName.substr(1));
                    SymbolicExpression::Ptr var = SymbolicExpression::makeIntegerVariable(nBits, varId);

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
                    SymbolicExpression::Ptr val = SymbolicExpression::makeIntegerConstant(bits);

                    SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<*var <<" == " <<*val <<"\n";
                    evidence_.insert(var, val);

                } else if (elmt->children().size() == 5 &&
                           elmt->children()[0]->name() == "define-fun" &&
                           boost::regex_match(elmt->children()[1]->name(), cseNameRe)) {
                    // e.g., (define-fun cse_123 () (_ BitVec 32) (bvadd v57829 #x00002b4f))

                    // Ignored

#if 0 // [Robb Matzke 2021-09-20]: we get lots of these from apparently internal Z3 variables and functions, so silently igore
                } else if (mlog[WARN]) {
                    mlog[WARN] <<"malformed model element ignored: ";
                    printSExpression(mlog[WARN], elmt);
                    mlog[WARN] <<"\n";
#endif
                }
            }
        }
    }

    stats.evidenceTime += evidenceTimer.stop();
    stats.longestEvidenceTime = std::max(stats.longestEvidenceTime, evidenceTimer.report());
}

} // namespace
} // namespace

#endif
