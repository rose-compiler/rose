#include "rose.h"
#include "SymbolicSemantics.h"

namespace SymbolicSemantics {

std::ostream &
operator<<(std::ostream &o, const State &state)
{
    state.print(o);
    return o;
}

std::ostream &
operator<<(std::ostream &o, const MemoryCell &mc)
{
    mc.print(o, NULL);
    return o;
}

/* Address X and Y may alias each other if X+datasize(X)>=Y or X<Y+datasize(Y) where datasize(A) is the number of bytes stored
 * at address A. In other words, if the following expression is satisfiable, then the memory cells might alias one another.
 *
 * \code
 *     ((X.addr + X.nbytes > Y.addr) && (X.addr < Y.addr + Y.nbytes)) ||
 *     ((Y.addr + Y.nbytes > X.addr) && (Y.addr < X.addr + X.nbytes))
 * \code
 *
 * Or, equivalently written in LISP style
 *
 * \code
 *     (and (or (> (+ X.addr X.nbytes) Y.addr) (< X.addr (+ Y.addr Y.nbytes)))
 *          (or (> (+ Y.addr Y.nbytes) X.addr) (< Y.addr (+ X.addr X.nbytes))))
 * \endcode
 */
bool
MemoryCell::may_alias(const MemoryCell &other, SMTSolver *solver/*NULL*/) const
{
    bool retval = must_alias(other, solver); /*this might be faster to solve*/
    if (retval)
        return retval;

    if (solver) {
        TreeNode *x_addr   = this->address.expr;
        TreeNode *x_nbytes = LeafNode::create_integer(32, this->nbytes);
        TreeNode *y_addr   = other.address.expr;
        TreeNode *y_nbytes = LeafNode::create_integer(32, other.nbytes);

        TreeNode *x_end = new InternalNode(32, InsnSemanticsExpr::OP_ADD, x_addr, x_nbytes);
        TreeNode *y_end = new InternalNode(32, InsnSemanticsExpr::OP_ADD, y_addr, y_nbytes);

        TreeNode *and1 = new InternalNode(1, InsnSemanticsExpr::OP_AND,
                                          new InternalNode(1, InsnSemanticsExpr::OP_UGT, x_end, y_addr),
                                          new InternalNode(1, InsnSemanticsExpr::OP_ULT, x_addr, y_end));
        TreeNode *and2 = new InternalNode(1, InsnSemanticsExpr::OP_AND,
                                          new InternalNode(1, InsnSemanticsExpr::OP_UGT, y_end, x_addr),
                                          new InternalNode(1, InsnSemanticsExpr::OP_ULT, y_addr, x_end));

        TreeNode *assertion = new InternalNode(1, InsnSemanticsExpr::OP_OR, and1, and2);
        retval = solver->satisfiable(assertion);
        assertion->deleteDeeply();
    }

    return retval;
}

bool
MemoryCell::must_alias(const MemoryCell &other, SMTSolver *solver/*NULL*/) const
{
    return address.expr->equal_to(other.address.expr, solver);
}

void
MemoryCell::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    address.print(o, rmap);
    o <<": ";
    data.print(o, rmap);
    o <<" " <<nbytes <<" byte" <<(1==nbytes?"":"s");
    if (!written) o <<" read-only";
    if (clobbered) o <<" clobbered";
}

void
State::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    std::string prefix = "    ";

    for (size_t i=0; i<n_gprs; ++i) {
        o <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<"=";
        gpr[i].print(o, rmap);
        o <<"\n";
    }
    for (size_t i=0; i<n_segregs; ++i) {
        o <<prefix <<segregToString((X86SegmentRegister)i) <<"=";
        segreg[i].print(o, rmap);
        o <<"\n";
    }
    for (size_t i=0; i<n_flags; ++i) {
        o <<prefix <<flagToString((X86Flag)i) <<"=";
        flag[i].print(o, rmap);
        o <<"\n";
    }
    o <<prefix <<"ip=";
    ip.print(o, rmap);
    o <<"\n";

    /* Print memory contents. */
    o <<prefix << "memory:\n";
    for (Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi) {
        o <<prefix <<"    ";
        (*mi).print(o, rmap);
        o <<"\n";
    }
}

}
