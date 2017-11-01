#include <sage3basic.h>

#include <rosePublicConfig.h>
#include <BinarySmtlibSolver.h>
#include <boost/algorithm/string/predicate.hpp>
#include <Diagnostics.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

std::string
SmtlibSolver::getCommand(const std::string &configName) {
    return executable_.string() + " " + shellArgs_ + " " + configName;
}

void
SmtlibSolver::generateFile(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) {
    requireLinkage(LM_EXECUTABLE);

    o <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Uninterpreted variables\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    VariableSet vars = findVariables(exprs);
    outputDefinitions(o, vars);

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
    if (expr->isNumber() && 1==expr->nBits()) {
        if (expr->toInt()) {
            o <<"true";
        } else {
            o <<"false";
        }
    } else {
        outputExpression(o, expr, BOOLEAN);
    }
    o <<")\n";
}

void
SmtlibSolver::varForSet(const SymbolicExpr::InteriorPtr &set, const SymbolicExpr::LeafPtr &var) {
    ASSERT_not_null(set);
    ASSERT_not_null(var);
    ASSERT_forbid(varsForSets_.exists(set));
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
SmtlibSolver::VariableSet
SmtlibSolver::findVariables(const std::vector<SymbolicExpr::Ptr> &exprs) {
    struct T1: SymbolicExpr::Visitor {
        SmtlibSolver *self;
        VariableSet variables;
        std::set<const SymbolicExpr::Node*> seen;

        explicit T1(SmtlibSolver *self): self(self) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable() || leaf->isMemory())
                    variables.insert(leaf);
            } else if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpr::OP_SET) {
                    // Sets are ultimately converted to ITEs and therefore each set needs a free variable.
                    SymbolicExpr::LeafPtr var = SymbolicExpr::makeVariable(32, "set")->isLeafNode();
                    variables.insert(var);
                    self->varForSet(inode, var);
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(this);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
    return t1.variables;
}

void
SmtlibSolver::outputDefinitions(std::ostream &o, const VariableSet &variables) {
    BOOST_FOREACH (const SymbolicExpr::LeafPtr &leaf, variables.values()) {
        ASSERT_require(leaf->isVariable() || leaf->isMemory());
        o <<"\n";
        if (!leaf->comment().empty())
            o <<StringUtility::prefixLines(leaf->comment(), "; ") <<"\n";
        o <<"(declare-fun " <<leaf->toString() <<" " <<typeName(leaf) <<")\n";
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
                if ((leaf->isVariable() || leaf->isMemory()) && !leaf->comment().empty()) {
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
                if (inode->getOperator() == SymbolicExpr::OP_BV_XOR && widths.insert(inode->nBits()).second) {
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
        o <<"(define-fun " <<termName <<" " <<typeName(cse) <<" ";
        if (cse->isLeafNode() && cse->isLeafNode()->isMemory()) {
            outputExpression(o, cse, MEM_STATE);
        } else {
            outputExpression(o, cse, BIT_VECTOR);
        }
        o <<")\n";
        termNames_.insert(cse, termName);
    }
}

void
SmtlibSolver::outputExpression(std::ostream &o, const SymbolicExpr::Ptr &expr, Type needType) {
    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();

    std::string subExprName;
    if (termNames_.getOptional(expr).assignTo(subExprName)) {
        if (BOOLEAN == needType) {
            o <<"(= " <<subExprName <<" #b1)";
        } else {
            o <<subExprName; // bit vector or memory state
        }
    } else if (leaf) {
        outputLeaf(o, leaf, needType);
    } else {
        ASSERT_not_null(inode);
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_ADD:
                ASSERT_require(BIT_VECTOR == needType);
                outputLeftAssoc(o, "bvadd", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_AND:
                ASSERT_require(BOOLEAN == needType);
                outputLeftAssoc(o, "and", inode, BOOLEAN);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(BIT_VECTOR == needType);
                outputArithmeticShiftRight(o, inode);
                break;
            case SymbolicExpr::OP_BV_AND:
                ASSERT_require(BIT_VECTOR == needType);
                outputLeftAssoc(o, "bvand", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_OR:
                ASSERT_require(BIT_VECTOR == needType);
                outputLeftAssoc(o, "bvor",  inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_XOR:
                ASSERT_require(BIT_VECTOR == needType);
                outputXor(o, inode);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(BOOLEAN == needType);
                outputBinary(o, "=", inode, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_CONCAT:
                ASSERT_require(BIT_VECTOR == needType);
                outputLeftAssoc(o, "concat", inode, BIT_VECTOR);
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
                outputLeftAssoc(o, "or", inode, BOOLEAN);
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
                outputSignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(BOOLEAN == needType);
                outputSignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(BOOLEAN == needType);
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
                outputSignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(BOOLEAN == needType);
                outputSignedCompare(o, inode);
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
                outputUnsignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(BOOLEAN == needType);
                outputUnsignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(BOOLEAN == needType);
                outputUnsignedCompare(o, inode);
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(BOOLEAN == needType);
                outputUnsignedCompare(o, inode);
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

void
SmtlibSolver::outputLeaf(std::ostream &o, const SymbolicExpr::LeafPtr &leaf, Type needType) {
    if (leaf->isNumber()) {
        if (BOOLEAN == needType) {
            ASSERT_require(leaf->nBits() == 1);
            o <<(leaf->toInt() ? "#b1" : "#b0");
        } else if (leaf->nBits() % 4 == 0) {
            ASSERT_require(BIT_VECTOR == needType);
            o <<"#x" <<leaf->bits().toHex();
        } else {
            ASSERT_require(BIT_VECTOR == needType);
            o <<"#b" <<leaf->bits().toBinary();
        }
    } else if (leaf->isVariable()) {
        if (BOOLEAN == needType) {
            ASSERT_require(leaf->nBits() == 1);
            o <<"(= " <<leaf->toString() <<" #b1)";
        } else {
            o <<leaf->toString();
        }
    } else {
        ASSERT_require(leaf->isMemory());
        ASSERT_require(MEM_STATE == needType);
        o <<leaf->toString();
    }
}

// ROSE (rose-operator exprs...) => SMT-LIB (smtlib-operator exprs...); one or more expression
void
SmtlibSolver::outputList(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type type) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() >= 1);
    o <<"(" <<name;
    BOOST_FOREACH (const SymbolicExpr::Ptr &child, inode->children()) {
        o <<" ";
        outputExpression(o, child, type);
    }
    o <<")";
}

// ROSE (rose-operator expr) => SMT-LIB (smtlib-operator expr)
void
SmtlibSolver::outputUnary(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type type) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);
    outputList(o, name, inode, type);
}

// ROSE (rose-operator a b) => SMT-LIB (smtlib-operator a b)
void
SmtlibSolver::outputBinary(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type type) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    outputList(o, name, inode, type);
}

// ROSE (rose-operator a b)     => SMT-LIB (smtlib-operator a b)
// ROSE (rose-operator a b c)   => SMT-LIB (smtlib-operator (smtlib-operator a b) c)
// ROSE (rose-operator a b c d) => SMT-LIB (smtlib-operator (smtlib-operator (smtlib-operator a b) c) d)
// etc.
// where "identity" is all zeros or all ones and the same width as "a".
void
SmtlibSolver::outputLeftAssoc(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode, Type type) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() > 1);

    for (size_t i = 1; i < inode->nChildren(); ++i)
        o <<"(" <<name <<" ";
    outputExpression(o, inode->child(0), type);

    for (size_t i = 1; i < inode->nChildren(); ++i) {
        o <<" ";
        outputExpression(o, inode->child(i), type);
        o <<")";
    }
}

// SMT-LIB doesn't have a bit-wise XOR function, but we should have by now generated our own "bvxorN" functions where N is the
// width of the operands.
void
SmtlibSolver::outputXor(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() > 1);

    std::string name = "bvxor" + boost::lexical_cast<std::string>(inode->nBits());
    outputLeftAssoc(o, name, inode, BIT_VECTOR);
}

// ROSE (extract lo hi expr) => SMT-LIB ((_ extract hi lo) expr); lo and hi are inclusive
void
SmtlibSolver::outputExtract(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(1)->isNumber());
    size_t begin = inode->child(0)->toInt();            // low, inclusive
    size_t end = inode->child(1)->toInt();              // high, exclusive
    ASSERT_require(end > begin);
    ASSERT_require(end <= inode->child(2)->nBits());
    o <<"((_ extract " <<(end-1) <<" " <<begin <<") ";
    outputExpression(o, inode->child(2), BIT_VECTOR);
    o <<")";
}

// ROSE (ite boolean-expr a b) => SMT-LIB (ite boolean-expr a b); a and b same size, condition is Boolean not bit vector
void
SmtlibSolver::outputIte(std::ostream &o, const SymbolicExpr::InteriorPtr &inode, Type type) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_ITE);
    ASSERT_require(inode->nChildren() == 3);
    o <<"(ite ";
    outputExpression(o, inode->child(0), BOOLEAN);
    o <<" ";
    outputExpression(o, inode->child(1), type);
    o <<" ";
    outputExpression(o, inode->child(2), type);
    o <<")";
}

// ROSE (!= a b) => SMT-LIB (not (= a b))
void
SmtlibSolver::outputNotEqual(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());
    o <<"(not (= ";
    outputExpression(o, inode->child(0), BIT_VECTOR);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<"))";
}

// ROSE (uextend size expr) =>
// SMT-LIB (concat zeros expr)
//
// Where "zeros" is a bit vector of all clear bits with width size-expr.size. Due to ROSE symbolic simplifications that have
// already occurred by this point, "size" is guaranteed to be greater than expr.size.
void
SmtlibSolver::outputUnsignedExtend(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(0)->toInt() > inode->child(1)->nBits());
    size_t newWidth = inode->child(0)->toInt();
    size_t needBits = newWidth - inode->child(1)->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(needBits, false));

    o <<"(concat ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<")";
}

// ROSE (sextend size expr) =>
// SMT-LIB (concat (ite (= ((_ extract [expr.size-1] [expr.size-1]) expr) #b1)
//                      (bvnot zeros)
//                      zeros)
//                 expr)
//
//  where "zeros" is a bit vector of size size-expr.size with all bits clear.
//  The "size" > expr.size due to sextend simplifications that would have kicked in otherwise.
void
SmtlibSolver::outputSignExtend(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(0)->toInt() > inode->child(1)->nBits());
    SymbolicExpr::Ptr newSize = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    size_t signBitIdx = expr->nBits() - 1;
    size_t growth = newSize->toInt() - expr->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(growth, false));

    o <<"(concat (ite (= ((_ extract " <<signBitIdx <<" " <<signBitIdx <<") ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<") #b1) (bvnot ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<") ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<") ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<")";
}

// ROSE (set a b ...) =>
// SMT-LIB (ite v1 (ite v2 ... b) a)
//
// where v1, v2, ... are new variables
void
SmtlibSolver::outputSet(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(inode->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(inode);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(inode, var);
    ite->comment(inode->comment());
    outputExpression(o, ite, BIT_VECTOR);
}

// ROSE (ror amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvlshr (concat expr expr) extended_amount))
// where extended_amount is numerically equal to amount but extended to be 2*expr.nbits in width.
void
SmtlibSolver::outputRotateRight(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, 2*w), sa);
    o <<"((_ extract " <<(w-1) <<" 0) (bvlshr (concat ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<" ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<") ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<"))";
}

// ROSE (rol amount expr) =>
// SMT-LIB (_ extract [2*expr.size-1] [expr.size] (bvshl (concat expr expr) extended_amount))
// where extended_amount is numerically equal to amount but extended to be 2*expr.nbits in width.
void
SmtlibSolver::outputRotateLeft(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t w = expr->nBits();

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, 2*w), sa);
    o <<"((_ extract " <<(2*w-1) <<" " <<w <<") (bvshl (concat ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<" ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<") ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<"))";
}

// For logical right shifts:
// ROSE (rose-right-shift-op amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvlshr (concat zeros_or_ones expr) extended_amount))
//
// Where extended_amount is the ROSE "amount" widened (or truncated) to the same width as "expr",
// and where "zeros_or_ones" is a constant with all bits set or clear and the same width as "expr"
void
SmtlibSolver::outputLogicalShiftRight(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHR0 || inode->getOperator() == SymbolicExpr::OP_SHR1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHR1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    o <<"((_ extract " <<(expr->nBits()-1) <<" 0) (bvlshr (concat ";
    outputExpression(o, zerosOrOnes, BIT_VECTOR);
    o <<" ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<") ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<"))";
}

// For left shifts:
// ROSE (rose-left-shift-op amount expr) =>
// SMT-LIB ((_ extract [2*expr.size-1] expr.size) (bvshl (concat expr zeros_or_ones) extended_amount))
// 
// Where extended_amount is the ROSE "amount" widened (or truncated) to the same width as "expr",
// and where "zeros_or_ones" is a constant with all bits set or clear and the same width as "expr"
void
SmtlibSolver::outputShiftLeft(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_SHL0 || inode->getOperator() == SymbolicExpr::OP_SHL1);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, expr->nBits()), sa); // widen sa same as expr
    bool newBits = inode->getOperator() == SymbolicExpr::OP_SHL1;
    SymbolicExpr::Ptr zerosOrOnes = SymbolicExpr::makeConstant(Sawyer::Container::BitVector(expr->nBits(), newBits));

    o <<"((_ extract " <<(2*expr->nBits()-1) <<" " <<expr->nBits() <<") (bvshl (concat ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<" ";
    outputExpression(o, zerosOrOnes, BIT_VECTOR);
    o <<") ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<"))";
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
void
SmtlibSolver::outputArithmeticShiftRight(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    SymbolicExpr::Ptr sa = inode->child(0);
    SymbolicExpr::Ptr expr = inode->child(1);
    size_t width = expr->nBits();
    SymbolicExpr::Ptr zeros = SymbolicExpr::makeInteger(width, 0);

    sa = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, 2*width), sa);
    o <<"((_ extract " <<(width-1) <<" 0) (bvlshr (concat (ite (= ((_ extract " <<(width-1) <<" " <<(width-1) <<") ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<") #b1) (bvnot ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<") ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<") ";
    outputExpression(o, expr, BIT_VECTOR);
    o <<") ";
    outputExpression(o, sa, BIT_VECTOR);
    o <<"))";
}

// ROSE (zerop expr) => SMT-LIB (= expr zeros)
// where "zeros" is a bit vector of all zeros the same size as "expr".
void
SmtlibSolver::outputZerop(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);

    SymbolicExpr::Ptr zeros = SymbolicExpr::makeInteger(inode->child(0)->nBits(), 0);
    o <<"(= ";
    outputExpression(o, inode->child(0), BIT_VECTOR);
    o <<" ";
    outputExpression(o, zeros, BIT_VECTOR);
    o <<")";
}

// ROSE (rose-multiply a b) =>
// SMT-LIB (smt-multiply a_extended b_extended)
//
// where "a_extended" and "b_extended" are extended (signed or unsigned) to the width a.size+b.size before being
// multiplied.
void
SmtlibSolver::outputMultiply(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
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

    o <<"(bvmul ";
    outputExpression(o, aExtended, BIT_VECTOR);
    o <<" ";
    outputExpression(o, bExtended, BIT_VECTOR);
    o <<")";
}

// ROSE (udiv a b) =>
// SMT-LIB ((_ extract [a.size-1] 0) (bvudiv a_extended b_extended))
//
// where a_extended and b_extended are zero extended to have the width max(a.size,b.size).
void
SmtlibSolver::outputUnsignedDivide(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(1));
    o <<"((_ extract " <<(inode->child(0)->nBits()-1) <<" 0) (bvudiv ";
    outputExpression(o, aExtended, BIT_VECTOR);
    o <<" ";
    outputExpression(o, bExtended, BIT_VECTOR);
    o <<"))";
}

// ROSE (umod a b) =>
// SMT-LIB ((_ extract [b.size-1] 0) (bvurem a_extended b_extended))
//
// where a_extended and b_extended are zero extended to have the width max(a.size,b.size).
void
SmtlibSolver::outputUnsignedModulo(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    size_t w = std::max(inode->child(0)->nBits(), inode->child(1)->nBits());
    SymbolicExpr::Ptr aExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(0));
    SymbolicExpr::Ptr bExtended = SymbolicExpr::makeExtend(SymbolicExpr::makeInteger(32, w), inode->child(1));
    o <<"((_ extract " <<(inode->child(1)->nBits()-1) <<" 0) (bvurem ";
    outputExpression(o, aExtended, BIT_VECTOR);
    o <<" ";
    outputExpression(o, bExtended, BIT_VECTOR);
    o <<"))";
}

// ROSE (rose-sign-compare-op a b) =>
// SMT-LIB (...)
void
SmtlibSolver::outputSignedCompare(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    switch (inode->getOperator()) {
        case SymbolicExpr::OP_SLT: o <<"(bvslt"; break;
        case SymbolicExpr::OP_SLE: o <<"(bvsle"; break;
        case SymbolicExpr::OP_SGT: o <<"(bvsgt"; break;
        case SymbolicExpr::OP_SGE: o <<"(bvsge"; break;
        default:
            ASSERT_not_reachable("unhandled signed comparison operation");
    }
    o <<inode->child(0)->nBits() <<" ";
    outputExpression(o, inode->child(0), BIT_VECTOR);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<")";
}

// ROSE (rose-unsigned-compare-op a b) =>
// SMT-LIB (...)
void
SmtlibSolver::outputUnsignedCompare(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());

    switch (inode->getOperator()) {
        case SymbolicExpr::OP_ULT:
            o <<"(bvult";
            break;
        case SymbolicExpr::OP_ULE:
            o <<"(bvule" <<inode->child(0)->nBits();
            break;
        case SymbolicExpr::OP_UGT:
            o <<"(bvugt" <<inode->child(0)->nBits();
            break;
        case SymbolicExpr::OP_UGE:
            o <<"(bvuge" <<inode->child(0)->nBits();
            break;
        default:
            ASSERT_not_reachable("unhandled unsigned comparison operation");
    }
    o <<" ";
    outputExpression(o, inode->child(0), BIT_VECTOR);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<")";
}

// ROSE (read mem addr) =>
// SMT-LIB (select mem addr)
void
SmtlibSolver::outputRead(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    o <<"(select ";
    outputExpression(o, inode->child(0), MEM_STATE);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<")";
}

// ROSE (write mem addr value) =>
// SMT-LIB (store mem addr value)
void
SmtlibSolver::outputWrite(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    o <<"(store ";
    outputExpression(o, inode->child(0), MEM_STATE);
    o <<" ";
    outputExpression(o, inode->child(1), BIT_VECTOR);
    o <<" ";
    outputExpression(o, inode->child(2), BIT_VECTOR);
    o <<")";
}

void
SmtlibSolver::clearEvidence() {
    varsForSets_.clear();
    parsedOutput_.clear();
    evidence_.clear();
}

void
SmtlibSolver::parseEvidence() {
    boost::regex varNameRe("v\\d+");

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
                    SymbolicExpr::Ptr var = SymbolicExpr::makeExistingVariable(nBits, varId);

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
                    SymbolicExpr::Ptr val = SymbolicExpr::makeConstant(bits);

                    SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<*var <<" == " <<*val <<"\n";
                    evidence_.insert(var, val);

                } else if (mlog[ERROR]) {
                    mlog[ERROR] <<"malformed model element: ";
                    printSExpression(mlog[ERROR], elmt);
                    mlog[ERROR] <<"\n";
                }
            }
        }
    }
}

SymbolicExpr::Ptr
SmtlibSolver::evidenceForName(const std::string &varName) {
    BOOST_FOREACH (const ExprExprMap::Node &node, evidence_.nodes()) {
        ASSERT_not_null(node.key()->isLeafNode());
        ASSERT_require(node.key()->isLeafNode()->isVariable());
        if (node.key()->isLeafNode()->toString() == varName)
            return node.value();
    }
    return SymbolicExpr::Ptr();
}

std::vector<std::string>
SmtlibSolver::evidenceNames() {
    std::vector<std::string> retval;
    BOOST_FOREACH (const SymbolicExpr::Ptr &varExpr, evidence_.keys()) {
        SymbolicExpr::LeafPtr leaf = varExpr->isLeafNode();
        ASSERT_not_null(leaf);
        ASSERT_require(leaf->isVariable());
        retval.push_back(leaf->toString());
    }
    return retval;
}


} // namespace
} // namespace
