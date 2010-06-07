#include "rose.h"
#include "YicesSolver.h"

/* See SMTSolver::get_command() */
std::string
YicesSolver::get_command(const std::string &config_name) 
{
#ifdef YICES
    return std::string(YICES) + " -tc " + config_name;
#else
    return "false YICES not defined"
#endif
}

/* See SMTSolver::generate_file() */
void
YicesSolver::generate_file(std::ostream &o, const SymbolicExpr::TreeNode *tn, Definitions *defns)
{
    Definitions *allocated = NULL;
    if (!defns)
        defns = allocated = new Definitions;

    out_define(o, tn, defns);
    out_assert(o, tn);
    o <<"\n(check)\n";

    delete allocated;
}

/** Traverse an expression and produce Yices "define" statements for variables. */
void
YicesSolver::out_define(std::ostream &o, const SymbolicExpr::TreeNode *tn, Definitions *defns)
{
    ROSE_ASSERT(defns!=NULL);
    const SymbolicExpr::LeafNode *ln = dynamic_cast<const SymbolicExpr::LeafNode*>(tn);
    const SymbolicExpr::InternalNode *in = dynamic_cast<const SymbolicExpr::InternalNode*>(tn);

    if (ln) {
        if (!ln->is_known() && defns->find(ln->get_name())==defns->end()) {
            defns->insert(ln->get_name());
            o <<"(define v" <<ln->get_name() <<"::(bitvector " <<ln->get_nbits() <<"))\n";
        }
    } else {
        ROSE_ASSERT(in!=NULL);
        for (size_t i=0; i<in->size(); i++)
            out_define(o, in->child(i), defns);
    }
}

/** Generate a Yices "assert" statement for an expression. */
void
YicesSolver::out_assert(std::ostream &o, const SymbolicExpr::TreeNode *tn)
{
    o <<"(assert ";
    out_expr(o, tn);
    o <<")";
}

/** Output a decimal number. */
void
YicesSolver::out_number(std::ostream &o, const SymbolicExpr::TreeNode *tn)
{
    const SymbolicExpr::LeafNode *ln = dynamic_cast<const SymbolicExpr::LeafNode*>(tn);
    ROSE_ASSERT(ln!=NULL);
    ROSE_ASSERT(ln->is_known());
    o <<ln->get_value();
}

/** Output for one expression. */
void
YicesSolver::out_expr(std::ostream &o, const SymbolicExpr::TreeNode *tn)
{
    using namespace SymbolicExpr;
    const LeafNode *ln = dynamic_cast<const LeafNode*>(tn);
    const InternalNode *in = dynamic_cast<const InternalNode*>(tn);
    if (ln) {
        if (ln->is_known()) {
            o <<"(mk-bv " <<ln->get_nbits() <<" " <<ln->get_value() <<")";
        } else {
            o <<"v" <<ln->get_name();
        }
    } else {
        ROSE_ASSERT(in!=NULL);
        switch (in->get_operator()) {
            case OP_ADD:        out_la(o, "bv-add", in, false);                 break;
            case OP_AND:        out_la(o, "and", in, true);                     break;
            case OP_ASR:        out_asr(o, in);                                 break;
            case OP_BV_AND:     out_la(o, "bv-and", in, true);                  break;
            case OP_BV_OR:      out_la(o, "bv-or", in, false);                  break;
            case OP_BV_XOR:     out_la(o, "bv-xor", in, false);                 break;
            case OP_EQ:         out_binary(o, "=", in);                         break;
            case OP_CONCAT:     out_la(o, "bv-concat", in);                     break;
            case OP_EXTRACT:    out_extract(o, in);                             break;
            case OP_INVERT:     out_unary(o, "bv-not", in);                     break;
            case OP_ITE:        out_ite(o, in);                                 break;
            case OP_LSSB:       ROSE_ASSERT(!"not implemented");
            case OP_MSSB:       ROSE_ASSERT(!"not implemented");
            case OP_NE:         out_binary(o, "/=", in);                        break;
            case OP_NEGATE:     out_unary(o, "bv-neg", in);                     break;
            case OP_NOOP:       o<<"0b1";                                       break;
            case OP_OR:         out_la(o, "or", in, false);                     break;
            case OP_ROL:        ROSE_ASSERT(!"not implemented");
            case OP_ROR:        ROSE_ASSERT(!"not implemented");
            case OP_SDIV:       ROSE_ASSERT(!"not implemented");
            case OP_SEXTEND:    out_sext(o, in);                                break;
            case OP_SLT:        out_binary(o, "bv-slt", in);                    break;
            case OP_SLE:        out_binary(o, "bv-sle", in);                    break;
            case OP_SHL0:       out_shift(o, "bv-shift-left", in, false);       break;
            case OP_SHL1:       out_shift(o, "bv-shift-left", in, true);        break;
            case OP_SHR0:       out_shift(o, "bv-shift-right", in, false);      break;
            case OP_SHR1:       out_shift(o, "bv-shift-right", in, true);       break;
            case OP_SGE:        out_binary(o, "bv-sge", in);                    break;
            case OP_SGT:        out_binary(o, "bv-sgt", in);                    break;
            case OP_SMOD:       ROSE_ASSERT(!"not implemented");
            case OP_SMUL:       out_mult(o, in);                                break;
            case OP_UDIV:       ROSE_ASSERT(!"not implemented");
            case OP_UEXTEND:    out_uext(o, in);                                break;
            case OP_UGE:        out_binary(o, "bv-ge", in);                     break;
            case OP_UGT:        out_binary(o, "bv-gt", in);                     break;
            case OP_ULE:        out_binary(o, "bv-le", in);                     break;
            case OP_ULT:        out_binary(o, "bv-lt", in);                     break;
            case OP_UMOD:       ROSE_ASSERT(!"not implemented");
            case OP_UMUL:       out_mult(o, in);                                break;
            case OP_ZEROP:      out_zerop(o, in);                               break;
        }
    }
}

/** Output for unary operators. */
void
YicesSolver::out_unary(std::ostream &o, const char *opname, const SymbolicExpr::InternalNode *in)
{
    assert(opname && *opname);
    assert(in && 1==in->size());

    o <<"(" <<opname <<" ";
    out_expr(o, in->child(0));
    o <<")";
}

/** Output for binary operators. */
void
YicesSolver::out_binary(std::ostream &o, const char *opname, const SymbolicExpr::InternalNode *in)
{
    assert(opname && *opname);
    assert(in && 2==in->size());

    o <<"(" <<opname <<" ";
    out_expr(o, in->child(0));
    o <<" ";
    out_expr(o, in->child(1));
    o <<")";
}

/** Output for if-then-else operator.  The condition must be cast from a 1-bit vector to a number, therefore, the input
 *  \code
 *      (OP_ITE COND S1 S2)
 *  \endcode
 *
 *  will be rewritten as
 *  \code
 *      (ite (= COND 0b1) S1 S2)
 *  \code
 */
void
YicesSolver::out_ite(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    assert(in && 3==in->size());
    assert(in->child(0)->get_nbits()==1);
    o <<"(ite (=";
    out_expr(o, in->child(0));
    o <<" 0b1)";
    for (size_t i=1; i<3; i++) {
        o <<" ";
        out_expr(o, in->child(i));
    }
    o <<")";
}

/** Output for left-associative, binary operators. The identity_element is sign-extended and used as the second operand
 *  if only one operand is supplied. */
void
YicesSolver::out_la(std::ostream &o, const char *opname, const SymbolicExpr::InternalNode *in, bool identity_element)
{
    assert(opname && *opname);
    assert(in && in->size()>=1);

    for (size_t i=1; i<std::max((size_t)2, in->size()); i++)
        o <<"(" <<opname <<" ";
    out_expr(o, in->child(0));

    if (in->size()>1) {
        for (size_t i=1; i<in->size(); i++) {
            o <<" ";
            out_expr(o, in->child(i));
            o <<")";
        }
    } else {
        SymbolicExpr::LeafNode *ident = SymbolicExpr::LeafNode::create_integer(in->child(0)->get_nbits(),
                                                                               identity_element ? (uint64_t)(-1) : 0);
        out_expr(o, ident);
        o <<")";
    }
}

/** Output for left-associative operators. */
void
YicesSolver::out_la(std::ostream &o, const char *opname, const SymbolicExpr::InternalNode *in)
{
    if (in->size()==1) {
        out_unary(o, opname, in);
    } else {
        out_la(o, opname, in, false);
    }
}

/** Output for extract. Yices bv-extract first two arguments must be constants. */
void
YicesSolver::out_extract(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    using namespace SymbolicExpr;

    assert(in && 3==in->size());
    assert(in->child(0)->is_known());
    assert(in->child(1)->is_known());
    assert(in->child(0)->get_value() < in->child(1)->get_value());
    size_t lo = in->child(0)->get_value();
    size_t hi = in->child(1)->get_value() - 1;          /*inclusive*/
    o <<"(bv-extract " <<hi <<" " <<lo <<" ";
    out_expr(o, in->child(2));
    o <<")";
}

/** Output for sign-extend. The second argument for yices' bv-sign-extend function is the number of bits by which the first
 *  argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size) and the
 *  size of the second operand (the bit vector to be extended). */
void
YicesSolver::out_sext(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    assert(in && 2==in->size());
    assert(in->child(0)->is_known()); /*Yices bv-sign-extend needs a number for the second operand*/
    assert(in->child(0)->get_value() > in->child(1)->get_nbits());
    size_t extend_by = in->child(0)->get_value() - in->child(1)->get_nbits();
    o <<"(bv-sign-extend  ";
    out_expr(o, in->child(1)); /*vector*/
    o <<" " <<extend_by <<")";
}

/** Output for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 *  (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
void
YicesSolver::out_uext(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    using namespace SymbolicExpr;
    assert(in && 2==in->size());
    assert(in->child(0)->is_known()); /*Yices mk-bv needs a number for the size operand*/
    assert(in->child(0)->get_value() > in->child(1)->get_nbits());
    size_t extend_by = in->child(0)->get_value() - in->child(1)->get_nbits();

    o <<"(bv-concat (mk-bv " <<extend_by <<" 0) ";
    out_expr(o, in->child(1));
    o <<")";
}

/** Output for shift operators. */
void
YicesSolver::out_shift(std::ostream &o, const char *opname, const SymbolicExpr::InternalNode *in, bool newbits)
{
    assert(opname && *opname);
    assert(in && 2==in->size());
    assert(in->child(0)->is_known()); /*Yices' bv-shift-* operators need a constant for the shift amount*/

    o <<"(" <<opname <<(newbits?"1":"0") <<" ";
    out_expr(o, in->child(1));
    o <<" " <<in->child(0)->get_value() <<")";
}

/** Output for arithmetic right shift.  Yices doesn't have a sign-extending right shift, therefore we implement it in terms of
 *  other operations.  (OP_ASR ShiftAmount Vector) becomes
 *
 *  (ite (= (mk-bv 1 1) (bv-extract [VectorSize-1] [VectorSize-1] Vector)) ;; vector's sign bit
 *       (bv-shift-right1 Vector [ShiftAmount])
 *       (bv-shift-right0 Vector [ShiftAmount]))
 *
 * where [VectorSize], [VectorSize-1], and [ShiftAmount] are numeric constants.
 */
void
YicesSolver::out_asr(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    using namespace SymbolicExpr;
    assert(in && 2==in->size());
    const TreeNode *vector = in->child(1);
    uint64_t vector_size = vector->get_nbits();
    ROSE_ASSERT(in->child(0)->is_known());
    uint64_t shift_amount = in->child(0)->get_value();

    o <<"(ite (= (mk-bv 1 1) (bv-extract " <<(vector_size-1) <<" " <<(vector_size-1) <<" ";
    out_expr(o, vector);
    o <<")) (bv-shift-right1 ";
    out_expr(o, vector);
    o <<" " <<shift_amount <<") (bv-shift-right0 ";
    out_expr(o, vector);
    o <<" " <<shift_amount <<"))";
}

/** Output for zero comparison. Result should be a single bit:
 *  \code
 *      (OP_ZEROP X)
 *  \endcode
 *
 *  becomes
 *  \code
 *      (ite (= (mk-bv [sizeof(X)] 0) [X]) 0b1 0b0)
 *  \endcode
 */
void
YicesSolver::out_zerop(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    assert(in && 1==in->size());
    o <<"(ite (= (mk-bv " <<in->child(0)->get_nbits() <<" 0) ";
    out_expr(o, in->child(0));
    o <<") 0b1 0b0)";
}

/** Output for multiply. The OP_SMUL and OP_UMUL nodes of SymbolicExpr define the result width to be the sum of the input
 *  widths. Yices' bv-mul operator requires that both operands are the same size and the result is the size of each operand.
 *  Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *  \code
 *    (bv-mul (bv-sign-extend A [|B|-1]) (bv-sign-extend B [|A|-1]))
 *  \endcode
 */
void
YicesSolver::out_mult(std::ostream &o, const SymbolicExpr::InternalNode *in)
{
    o <<"(bv-mul (bv-sign-extend ";
    out_expr(o, in->child(0));
    o <<" " <<(in->child(1)->get_nbits()-1) <<") (bv-sign-extend ";
    out_expr(o, in->child(1));
    o <<" " <<(in->child(0)->get_nbits()-1) <<"))";
}
