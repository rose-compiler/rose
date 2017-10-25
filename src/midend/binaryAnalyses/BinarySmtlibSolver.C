#include <sage3basic.h>

#include <rosePublicConfig.h>
#include <BinarySmtlibSolver.h>

namespace Rose {
namespace BinaryAnalysis {

std::string
SmtlibSolver::getCommand(const std::string &configName) {
    return executable_.string() + " " + configName;
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
        outputExpression(o, expr);
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
        outputExpression(o, cse);
        o <<")\n";
        termNames_.insert(cse, termName);
    }
}

void
SmtlibSolver::outputExpression(std::ostream &o, const SymbolicExpr::Ptr &expr) {
    SymbolicExpr::LeafPtr leaf = expr->isLeafNode();
    SymbolicExpr::InteriorPtr operation = expr->isInteriorNode();

    std::string subExprName;
    if (termNames_.getOptional(expr).assignTo(subExprName)) {
        o <<subExprName;
    } else if (leaf) {
        if (leaf->isNumber()) {
            if (leaf->nBits() % 4 == 0) {
                o <<"#x" <<leaf->bits().toHex();
            } else {
                o <<"#b" <<leaf->bits().toBinary();
            }
        } else {
            ASSERT_require(leaf->isVariable() || leaf->isMemory());
            o <<leaf->toString();
        }
    } else {
        ASSERT_not_null(operation);
        switch (operation->getOperator()) {
            case SymbolicExpr::OP_ADD:     outputLeftAssoc(o, "bvadd", operation);   break;
            case SymbolicExpr::OP_AND:     outputLeftAssoc(o, "and", operation);     break;
            case SymbolicExpr::OP_ASR:     outputArithmeticShiftRight(o, operation); break;
            case SymbolicExpr::OP_BV_AND:  outputLeftAssoc(o, "bvand", operation);   break;
            case SymbolicExpr::OP_BV_OR:   outputLeftAssoc(o, "bvor",  operation);   break;
            case SymbolicExpr::OP_BV_XOR:  outputXor(o, operation); break;
            case SymbolicExpr::OP_EQ:      outputBinary(o, "=", operation);          break;
            case SymbolicExpr::OP_CONCAT:  outputLeftAssoc(o, "concat", operation);  break;
            case SymbolicExpr::OP_EXTRACT: outputExtract(o, operation);              break;
            case SymbolicExpr::OP_INVERT:  outputUnary(o, "bvnot", operation);       break;
            case SymbolicExpr::OP_ITE:     outputIte(o, operation);                  break;
            case SymbolicExpr::OP_LSSB:    throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:    throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:      outputNotEqual(o, operation);             break;
            case SymbolicExpr::OP_NEGATE:  outputUnary(o, "bvneg", operation);       break;
            case SymbolicExpr::OP_NOOP:    o <<"#b1";                                break;
            case SymbolicExpr::OP_OR:      outputLeftAssoc(o, "or", operation);      break;
            case SymbolicExpr::OP_READ:    outputRead(o, operation);                 break;
            case SymbolicExpr::OP_ROL:     outputRotateLeft(o, operation);           break;
            case SymbolicExpr::OP_ROR:     outputRotateRight(o, operation);          break;
            case SymbolicExpr::OP_SDIV:    throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:     outputSet(o, operation);                  break;
            case SymbolicExpr::OP_SEXTEND: outputSignExtend(o, operation);           break;
            case SymbolicExpr::OP_SLT:     outputSignedCompare(o, operation);        break;
            case SymbolicExpr::OP_SLE:     outputSignedCompare(o, operation);        break;
            case SymbolicExpr::OP_SHL0:    outputShiftLeft(o, operation);            break;
            case SymbolicExpr::OP_SHL1:    outputShiftLeft(o, operation);            break;
            case SymbolicExpr::OP_SHR0:    outputLogicalShiftRight(o, operation);    break;
            case SymbolicExpr::OP_SHR1:    outputLogicalShiftRight(o, operation);    break;
            case SymbolicExpr::OP_SGE:     outputSignedCompare(o, operation);        break;
            case SymbolicExpr::OP_SGT:     outputSignedCompare(o, operation);        break;
            case SymbolicExpr::OP_SMOD:    throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:    outputMultiply(o, operation);             break;
            case SymbolicExpr::OP_UDIV:    outputUnsignedDivide(o, operation);       break;
            case SymbolicExpr::OP_UEXTEND: outputUnsignedExtend(o, operation);       break;
            case SymbolicExpr::OP_UGE:     outputUnsignedCompare(o, operation);      break;
            case SymbolicExpr::OP_UGT:     outputUnsignedCompare(o, operation);      break;
            case SymbolicExpr::OP_ULE:     outputUnsignedCompare(o, operation);      break;
            case SymbolicExpr::OP_ULT:     outputUnsignedCompare(o, operation);      break;
            case SymbolicExpr::OP_UMOD:    outputUnsignedModulo(o, operation);       break;
            case SymbolicExpr::OP_UMUL:    outputMultiply(o, operation);             break;
            case SymbolicExpr::OP_WRITE:   outputWrite(o, operation);                break;
            case SymbolicExpr::OP_ZEROP:   outputZerop(o, operation);                break;
        }
    }
}

// ROSE (rose-operator exprs...) => SMT-LIB (smtlib-operator exprs...); one or more expression
void
SmtlibSolver::outputList(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() >= 1);
    o <<"(" <<name;
    BOOST_FOREACH (const SymbolicExpr::Ptr &child, inode->children()) {
        o <<" ";
        outputExpression(o, child);
    }
    o <<")";
}

// ROSE (rose-operator expr) => SMT-LIB (smtlib-operator expr)
void
SmtlibSolver::outputUnary(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 1);
    outputList(o, name, inode);
}

// ROSE (rose-operator a b) => SMT-LIB (smtlib-operator a b)
void
SmtlibSolver::outputBinary(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    outputList(o, name, inode);
}

// ROSE (rose-operator a b)     => SMT-LIB (smtlib-operator a b)
// ROSE (rose-operator a b c)   => SMT-LIB (smtlib-operator (smtlib-operator a b) c)
// ROSE (rose-operator a b c d) => SMT-LIB (smtlib-operator (smtlib-operator (smtlib-operator a b) c) d)
// etc.
// where "identity" is all zeros or all ones and the same width as "a".
void
SmtlibSolver::outputLeftAssoc(std::ostream &o, const std::string &name, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_require(!name.empty());
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() > 1);

    for (size_t i = 1; i < inode->nChildren(); ++i)
        o <<"(" <<name <<" ";
    outputExpression(o, inode->child(0));

    for (size_t i = 1; i < inode->nChildren(); ++i) {
        o <<" ";
        outputExpression(o, inode->child(i));
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
    outputLeftAssoc(o, name, inode);
}

// ROSE (extract lo hi expr) => SMT-LIB ((_ extract hi lo) expr); lo and hi are inclusive
void
SmtlibSolver::outputExtract(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    ASSERT_require(inode->child(0)->isNumber());
    ASSERT_require(inode->child(1)->isNumber());
    size_t begin = inode->child(0)->toInt();            // low, inclusive
    size_t end = inode->child(1)->toInt();              // hight, exclusive
    ASSERT_require(end > begin);
    ASSERT_require(end <= inode->child(2)->nBits());
    o <<"((_ extract " <<(end-1) <<" " <<begin <<") ";
    outputExpression(o, inode->child(2));
    o <<")";
}

// ROSE (ite boolean-expr a b) => SMT-LIB (ite boolean-expr a b); a and b same size, condition is Boolean not bit vector
void
SmtlibSolver::outputIte(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->getOperator() == SymbolicExpr::OP_ITE);
    ASSERT_require(inode->nChildren() == 3);
    o <<"(ite ";
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
    o <<" ";
    outputExpression(o, inode->child(2));
    o <<")";
}

// ROSE (!= a b) => SMT-LIB (not (= a b))
void
SmtlibSolver::outputNotEqual(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    ASSERT_require(inode->child(0)->nBits() == inode->child(1)->nBits());
    o <<"(not (= ";
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
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
    outputExpression(o, zeros);
    o <<" ";
    outputExpression(o, inode->child(1));
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
    outputExpression(o, inode->child(1));
    o <<") #b1) (bvnot ";
    outputExpression(o, zeros);
    o <<") ";
    outputExpression(o, zeros);
    o <<") ";
    outputExpression(o, inode->child(1));
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
    outputExpression(o, ite);
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
    outputExpression(o, expr);
    o <<" ";
    outputExpression(o, expr);
    o <<") ";
    outputExpression(o, sa);
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
    outputExpression(o, expr);
    o <<" ";
    outputExpression(o, expr);
    o <<") ";
    outputExpression(o, sa);
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
    outputExpression(o, zerosOrOnes);
    o <<" ";
    outputExpression(o, expr);
    o <<") ";
    outputExpression(o, sa);
    o <<"))";
}

// For left shifts:
// ROSE (rose-left-shift-op amount expr) =>
// SMT-LIB ((_ extract [expr.size-1] 0) (bvshl (concat expr zeros_or_ones) extended_amount))
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

    o <<"((_ extract " <<(expr->nBits()-1) <<" 0) (bvshl (concat ";
    outputExpression(o, expr);
    o <<" ";
    outputExpression(o, zerosOrOnes);
    o <<") ";
    outputExpression(o, sa);
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
    outputExpression(o, expr);
    o <<") #b1) (bvnot ";
    outputExpression(o, zeros);
    o <<") ";
    outputExpression(o, zeros);
    o <<") ";
    outputExpression(o, expr);
    o <<") ";
    outputExpression(o, sa);
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
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, zeros);
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
    outputExpression(o, aExtended);
    o <<" ";
    outputExpression(o, bExtended);
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
    outputExpression(o, aExtended);
    o <<" ";
    outputExpression(o, bExtended);
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
    outputExpression(o, aExtended);
    o <<" ";
    outputExpression(o, bExtended);
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
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
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
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
    o <<")";
}

// ROSE (read mem addr) =>
// SMT-LIB (select mem addr)
void
SmtlibSolver::outputRead(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 2);
    o <<"(select ";
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
    o <<")";
}

// ROSE (write mem addr value) =>
// SMT-LIB (store mem addr value)
void
SmtlibSolver::outputWrite(std::ostream &o, const SymbolicExpr::InteriorPtr &inode) {
    ASSERT_not_null(inode);
    ASSERT_require(inode->nChildren() == 3);
    o <<"(store ";
    outputExpression(o, inode->child(0));
    o <<" ";
    outputExpression(o, inode->child(1));
    o <<" ";
    outputExpression(o, inode->child(2));
    o <<")";
}

SymbolicExpr::Ptr
SmtlibSolver::evidenceForName(const std::string&) {
    TODO("[Robb Matzke 2017-10-19]");
}

std::vector<std::string>
SmtlibSolver::evidenceNames() {
    TODO("[Robb Matzke 2017-10-19]");
}

void
SmtlibSolver::clearEvidence() {
    TODO("[Robb Matzke 2017-10-19]");
}

void
SmtlibSolver::parseEvidence() {
    TODO("[Robb Matzke 2017-10-19]");
}

} // namespace
} // namespace
