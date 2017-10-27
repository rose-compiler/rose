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

// No need to emit anything since Z3 already has a "bvxor" function.
void
Z3Solver::outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) {}

// No need to emit anything since Z3 has a full complement of signed and unsigned comparison functions.
void
Z3Solver::outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) {}

void
Z3Solver::outputExpression(std::ostream &o, const SymbolicExpr::Ptr &expr) {
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
            case SymbolicExpr::OP_ADD:     outputList(o, "bvadd", operation);        break;
            case SymbolicExpr::OP_AND:     outputList(o, "and", operation);          break;
            case SymbolicExpr::OP_ASR:     outputArithmeticShiftRight(o, operation); break;
            case SymbolicExpr::OP_BV_AND:  outputList(o, "bvand", operation);        break;
            case SymbolicExpr::OP_BV_OR:   outputList(o, "bvor",  operation);        break;
            case SymbolicExpr::OP_BV_XOR:  outputList(o, "bvxor", operation);        break;
            case SymbolicExpr::OP_EQ:      outputBinary(o, "=", operation);          break;
            case SymbolicExpr::OP_CONCAT:  outputList(o, "concat", operation);       break;
            case SymbolicExpr::OP_EXTRACT: outputExtract(o, operation);              break;
            case SymbolicExpr::OP_INVERT:  outputUnary(o, "bvnot", operation);       break;
            case SymbolicExpr::OP_ITE:     outputIte(o, operation);                  break;
            case SymbolicExpr::OP_LSSB:    throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:    throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:      outputNotEqual(o, operation);             break;
            case SymbolicExpr::OP_NEGATE:  outputUnary(o, "bvneg", operation);       break;
            case SymbolicExpr::OP_NOOP:    o <<"#b1";                                break;
            case SymbolicExpr::OP_OR:      outputList(o, "or", operation);           break;
            case SymbolicExpr::OP_READ:    outputRead(o, operation);                 break;
            case SymbolicExpr::OP_ROL:     outputRotateLeft(o, operation);           break;
            case SymbolicExpr::OP_ROR:     outputRotateRight(o, operation);          break;
            case SymbolicExpr::OP_SDIV:    throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:     outputSet(o, operation);                  break;
            case SymbolicExpr::OP_SEXTEND: outputSignExtend(o, operation);           break;
            case SymbolicExpr::OP_SLT:     outputBinary(o, "bvslt", operation);      break;
            case SymbolicExpr::OP_SLE:     outputBinary(o, "bvsle", operation);      break;
            case SymbolicExpr::OP_SHL0:    outputShiftLeft(o, operation);            break;
            case SymbolicExpr::OP_SHL1:    outputShiftLeft(o, operation);            break;
            case SymbolicExpr::OP_SHR0:    outputLogicalShiftRight(o, operation);    break;
            case SymbolicExpr::OP_SHR1:    outputLogicalShiftRight(o, operation);    break;
            case SymbolicExpr::OP_SGE:     outputBinary(o, "bvsge", operation);      break;
            case SymbolicExpr::OP_SGT:     outputBinary(o, "bvsgt", operation);      break;
            case SymbolicExpr::OP_SMOD:    throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:    outputMultiply(o, operation);             break;
            case SymbolicExpr::OP_UDIV:    outputUnsignedDivide(o, operation);       break;
            case SymbolicExpr::OP_UEXTEND: outputUnsignedExtend(o, operation);       break;
            case SymbolicExpr::OP_UGE:     outputBinary(o, "bvuge", operation);      break;
            case SymbolicExpr::OP_UGT:     outputBinary(o, "bvugt", operation);      break;
            case SymbolicExpr::OP_ULE:     outputBinary(o, "bvule", operation);      break;
            case SymbolicExpr::OP_ULT:     outputBinary(o, "bvult", operation);      break;
            case SymbolicExpr::OP_UMOD:    outputUnsignedModulo(o, operation);       break;
            case SymbolicExpr::OP_UMUL:    outputMultiply(o, operation);             break;
            case SymbolicExpr::OP_WRITE:   outputWrite(o, operation);                break;
            case SymbolicExpr::OP_ZEROP:   outputZerop(o, operation);                break;
        }
    }
}

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
    outputExpression(o, expr);
    o <<" ";
    outputExpression(o, sa);
    o <<")";
}

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
