#include "sage3basic.h"

#include "rosePublicConfig.h"
#include "rose_strtoull.h"
#include "YicesSolver.h"

#include <errno.h>

#ifdef _MSC_VER
#define strtoull _strtoui64
#endif

using namespace InsnSemanticsExpr;

void
YicesSolver::init()
{
    if (available_linkage() & LM_EXECUTABLE) {
        linkage = LM_EXECUTABLE;
    } else if (available_linkage() & LM_LIBRARY) {
        linkage = LM_LIBRARY;
    } else {
        linkage = LM_NONE;
        ROSE_ASSERT(!"no available Yices linkage");
    }
}

YicesSolver::~YicesSolver() 
{
#ifdef ROSE_HAVE_LIBYICES
    if (context) {
        yices_del_context(context);
        context = NULL;
    }
#endif
}

unsigned
YicesSolver::available_linkage()
{
    unsigned retval = 0;
#ifdef ROSE_HAVE_LIBYICES
    retval |= LM_LIBRARY;
#endif
#ifdef ROSE_YICES
    retval |= LM_EXECUTABLE;
#endif
    return retval;
}

/* See YicesSolver.h */
SMTSolver::Satisfiable
YicesSolver::satisfiable(const std::vector<TreeNodePtr> &exprs)
{
#ifdef ROSE_HAVE_LIBYICES
    if (get_linkage() & LM_LIBRARY) {

        ++stats.ncalls;
        RTS_MUTEX(class_stats_mutex) {
            ++class_stats.ncalls;
        } RTS_MUTEX_END;

        if (!context) {
            context = yices_mk_context();
            assert(context);
        } else {
            yices_reset(context);
        }

#ifndef NDEBUG
        yices_enable_type_checker(true);
#endif

        Definitions defns;
        for (std::vector<TreeNodePtr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
            ctx_define(*ei, &defns);
        for (std::vector<TreeNodePtr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
            ctx_assert(*ei);
        switch (yices_check(context)) {
            case l_false: return SAT_NO;
            case l_true:  return SAT_YES;
            case l_undef: return SAT_UNKNOWN;
        }
        assert(!"switch statement is incomplete");
        abort();
    }
#endif

    ROSE_ASSERT(get_linkage() & LM_EXECUTABLE);
    return SMTSolver::satisfiable(exprs);
}


/* See SMTSolver::get_command() */
std::string
YicesSolver::get_command(const std::string &config_name)
{
#ifdef ROSE_YICES
    ROSE_ASSERT(get_linkage() & LM_EXECUTABLE);
    return std::string(ROSE_YICES) + " --evidence --type-check " + config_name;
#else
    return "false no yices command";
#endif
}

/* See SMTSolver::generate_file() */
void
YicesSolver::generate_file(std::ostream &o, const std::vector<TreeNodePtr> &exprs, Definitions *defns)
{
    ROSE_ASSERT(get_linkage() & LM_EXECUTABLE);
    Definitions *allocated = NULL;
    if (!defns)
        defns = allocated = new Definitions;

    for (std::vector<TreeNodePtr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
        out_define(o, *ei, defns);
    for (std::vector<TreeNodePtr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
        out_assert(o, *ei);
    o <<"\n(check)\n";

    delete allocated;
}

uint64_t
YicesSolver::parse_variable(const char *nptr, char **endptr, char first_char)
{
    uint64_t retval = (uint64_t)(-1);
    if (!nptr) {
        errno = EINVAL;
    } else {
        const char *s = nptr;
        while (isspace(*s)) ++s;
        if (first_char==*s && isdigit(s[1])) {
            errno = 0;
            uint64_t n = strtoull(s+1, endptr, 10);
            if (!errno)
                retval = n;
        } else {
            if (endptr) *endptr = const_cast<char*>(nptr); // following standard strtoul() interface
            errno = EINVAL;
        }
    }
    return retval;
}

/* See SMTSolver::parse_evidence() */
void
YicesSolver::parse_evidence()
{
    /* Look for text like "(= v36 0b01101111111111111101011110110100)" or
     * "(= (m95 0b01000000000000011100111111110100) 0b00000000000000000100111100000100)".  The text is free-form, with any
     * white space appearing in place of the SPC characters shown. */
    evidence.clear();
    const char *s = output_text.c_str();
    char *rest = NULL;

    struct Error {
        Error(const char *at, const std::string &mesg): at(at), mesg(mesg) {}
        const char *at;
        std::string mesg;
    };

    try {
        while (*s) {
            // "(= "
            while (isspace(*s)) ++s;
            if (!*s)
                break;
            if ('('!=*s++)
                throw Error(s-1, "left paren expected (1)");
            while (isspace(*s)) ++s;
            if ('='!=*s++)
                throw Error(s-1, "'=' operator expected (2)");
            if (!isspace(*s++))
                throw Error(s-1, "'=' operator expected (3)");
        
            // variable or function-apply
            while (isspace(*s)) ++s;
            if ('('==*s) {
                ++s;

                // memory variable, like "m95"
                errno = 0;
                (void) parse_variable(s, &rest, 'm');
                if (errno || s==rest)
                    throw Error(s, "memory variable expected (5)");
                s = rest;

                // memory address, like 0b01000000000000011100111111110100
                uint64_t addr = rose_strtoull(s, &rest, 0);
                if (errno || s==rest)
                    throw Error(s, "memory address expected (6)");
                s = rest;

                // right paren
                while (isspace(*s)) ++s;
                if (')'!=*s++)
                    throw Error(s, "')' expected after memory address (7)");

                // memory value
                while (isspace(*s)) ++s;
                if (0!=strncmp(s, "0b", 2) || !strchr("01", s[2]))
                    throw Error(s, "binary constant expected for memory value (8)");
                s += 2;
                uint64_t val = rose_strtoull(s, &rest, 2);
                if (errno || s==rest)
                    throw Error(s, "memory value expected (9)");
                size_t nbits = rest-s;
                s = rest;

                if (')'!=*s++)
                    throw Error(s-1, "')' expected at end of memory value (10)");

                std::string addr_name = StringUtility::addrToString(addr);
                assert(evidence.find(addr_name)==evidence.end());
                evidence[addr_name] = std::pair<size_t, uint64_t>(nbits, val);

            } else {
                // bitvector variable name
                errno = 0;
                uint64_t vnum = parse_variable(s, &rest, 'v');
                if (errno || s==rest)
                    throw Error(s, "bitvector variable expected (11)");
                s = rest;

                // bitvector value
                while (isspace(*s)) ++s;
                if (0!=strncmp(s, "0b", 2) || !strchr("01", s[2]))
                    throw Error(s, "binary constant expected for bitvector variable (12)");
                s += 2;
                uint64_t val = rose_strtoull(s, &rest, 2);
                if (errno || s==rest)
                    throw Error(s, "bitvector value expected (13)");
                size_t nbits = rest-s;
                s = rest;

                char vname[64];
                snprintf(vname, sizeof vname, "v%"PRIu64, vnum);
                assert(evidence.find(vname)==evidence.end());
                evidence[vname] = std::pair<size_t, uint64_t>(nbits, val);
            }

            // ')' closing the '=' operator
            if (')'!=*s++)
                throw Error(s-1, "expected ')' to close '=' operator (14)");
        }
        
    } catch (const Error &err) {
        std::cerr <<"YicesSolver::parse_evidence: " <<err.mesg <<" at char position " <<(err.at-output_text.c_str()) <<"\n"
                  <<"YicesSolver::parse_evidence: before \"" <<std::string(err.at).substr(0, 20) <<"\"...\n"
                  <<"YicesSolver::parse_evidence: entire evidence string follows...\n"
                  <<output_text.c_str();
    }
}

std::vector<std::string>
YicesSolver::evidence_names()
{
    std::vector<std::string> retval;
    for (Evidence::const_iterator ei=evidence.begin(); ei!=evidence.end(); ++ei)
        retval.push_back(ei->first);
    return retval;
}

TreeNodePtr
YicesSolver::evidence_for_name(const std::string &name)
{
    Evidence::const_iterator found = evidence.find(name);
    if (found==evidence.end())
        return TreeNodePtr(); // null
    return LeafNode::create_integer(found->second.first/*nbits*/, found->second.second/*value*/);
}

void
YicesSolver::clear_evidence()
{
    evidence.clear();
}

/** Traverse an expression and produce Yices "define" statements for variables. */
void
YicesSolver::out_define(std::ostream &o, const TreeNodePtr &tn, Definitions *defns)
{
    assert(defns);
    LeafNodePtr ln = tn->isLeafNode();
    InternalNodePtr in = tn->isInternalNode();

    if (ln && ln->is_variable()) {
        if (defns->find(ln->get_name())==defns->end()) {
            defns->insert(ln->get_name());
            o <<"(define v" <<ln->get_name() <<"::(bitvector " <<ln->get_nbits() <<"))\n";
        }
    } else if (ln && ln->is_memory()) {
        if (defns->find(ln->get_name())==defns->end()) {
            defns->insert(ln->get_name());
            o <<"(define m" <<ln->get_name() <<"::(-> (bitvector 32) (bitvector " <<ln->get_nbits() <<")))\n";
        }
    } else if (in) {
        for (size_t i=0; i<in->size(); i++)
            out_define(o, in->child(i), defns);
    }
}

/** Generate a Yices "assert" statement for an expression. */
void
YicesSolver::out_assert(std::ostream &o, const TreeNodePtr &tn)
{
    o <<"(assert ";
    out_expr(o, tn);
    o <<")\n";
}

/** Output a decimal number. */
void
YicesSolver::out_number(std::ostream &o, const TreeNodePtr &tn)
{
    LeafNodePtr ln = tn->isLeafNode();
    assert(ln && ln->is_known());
    o <<ln->get_value();
}

/** Output for one expression. */
void
YicesSolver::out_expr(std::ostream &o, const TreeNodePtr &tn)
{
    using namespace InsnSemanticsExpr;
    LeafNodePtr ln = tn->isLeafNode();
    InternalNodePtr in = tn->isInternalNode();
    if (ln) {
        if (ln->is_known()) {
            o <<"(mk-bv " <<ln->get_nbits() <<" " <<ln->get_value() <<")";
        } else if (ln->is_memory()) {
            o <<"m" <<ln->get_name();
        } else {
            assert(ln->is_variable());
            o <<"v" <<ln->get_name();
        }
    } else {
        assert(in);
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
            case OP_LSSB:       throw Exception("OP_LSSB not implemented");
            case OP_MSSB:       throw Exception("OP_MSSB not implemented");
            case OP_NE:         out_binary(o, "/=", in);                        break;
            case OP_NEGATE:     out_unary(o, "bv-neg", in);                     break;
            case OP_NOOP:       o<<"0b1";                                       break;
            case OP_OR:         out_la(o, "or", in, false);                     break;
            case OP_READ:       out_read(o, in);                                break;
            case OP_ROL:        throw Exception("OP_ROL not implemented");
            case OP_ROR:        throw Exception("OP_ROR not implemented");
            case OP_SDIV:       throw Exception("OP_SDIV not implemented");
            case OP_SEXTEND:    out_sext(o, in);                                break;
            case OP_SLT:        out_binary(o, "bv-slt", in);                    break;
            case OP_SLE:        out_binary(o, "bv-sle", in);                    break;
            case OP_SHL0:       out_shift(o, "bv-shift-left", in, false);       break;
            case OP_SHL1:       out_shift(o, "bv-shift-left", in, true);        break;
            case OP_SHR0:       out_shift(o, "bv-shift-right", in, false);      break;
            case OP_SHR1:       out_shift(o, "bv-shift-right", in, true);       break;
            case OP_SGE:        out_binary(o, "bv-sge", in);                    break;
            case OP_SGT:        out_binary(o, "bv-sgt", in);                    break;
            case OP_SMOD:       throw Exception("OP_SMOD not implemented");
            case OP_SMUL:       out_mult(o, in);                                break;
            case OP_UDIV:       throw Exception("OP_UDIV not implemented");
            case OP_UEXTEND:    out_uext(o, in);                                break;
            case OP_UGE:        out_binary(o, "bv-ge", in);                     break;
            case OP_UGT:        out_binary(o, "bv-gt", in);                     break;
            case OP_ULE:        out_binary(o, "bv-le", in);                     break;
            case OP_ULT:        out_binary(o, "bv-lt", in);                     break;
            case OP_UMOD:       throw Exception("OP_UMOD not implemented");
            case OP_UMUL:       out_mult(o, in);                                break;
            case OP_WRITE:      out_write(o, in);                               break;
            case OP_ZEROP:      out_zerop(o, in);                               break;
        }
    }
}

/** Output for unary operators. */
void
YicesSolver::out_unary(std::ostream &o, const char *opname, const InternalNodePtr &in)
{
    assert(opname && *opname);
    assert(in && 1==in->size());

    o <<"(" <<opname <<" ";
    out_expr(o, in->child(0));
    o <<")";
}

/** Output for binary operators. */
void
YicesSolver::out_binary(std::ostream &o, const char *opname, const InternalNodePtr &in)
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
YicesSolver::out_ite(std::ostream &o, const InternalNodePtr &in)
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
YicesSolver::out_la(std::ostream &o, const char *opname, const InternalNodePtr &in, bool identity_element)
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
        LeafNodePtr ident = LeafNode::create_integer(in->child(0)->get_nbits(), identity_element ? (uint64_t)(-1) : 0);
        out_expr(o, ident);
        o <<")";
    }
}

/** Output for left-associative operators. */
void
YicesSolver::out_la(std::ostream &o, const char *opname, const InternalNodePtr &in)
{
    if (in->size()==1) {
        out_unary(o, opname, in);
    } else {
        out_la(o, opname, in, false);
    }
}

/** Output for extract. Yices bv-extract first two arguments must be constants. */
void
YicesSolver::out_extract(std::ostream &o, const InternalNodePtr &in)
{
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
YicesSolver::out_sext(std::ostream &o, const InternalNodePtr &in)
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
YicesSolver::out_uext(std::ostream &o, const InternalNodePtr &in)
{
    using namespace InsnSemanticsExpr;
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
YicesSolver::out_shift(std::ostream &o, const char *opname, const InternalNodePtr &in,
                       bool newbits)
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
YicesSolver::out_asr(std::ostream &o, const InternalNodePtr &in)
{
    assert(in && 2==in->size());
    TreeNodePtr vector = in->child(1);
    uint64_t vector_size = vector->get_nbits();
    assert(in->child(0)->is_known());
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
YicesSolver::out_zerop(std::ostream &o, const InternalNodePtr &in)
{
    assert(in && 1==in->size());
    o <<"(ite (= (mk-bv " <<in->child(0)->get_nbits() <<" 0) ";
    out_expr(o, in->child(0));
    o <<") 0b1 0b0)";
}

/** Output for multiply. The OP_SMUL and OP_UMUL nodes of InsnSemanticsExpr define the result width to be the sum of the input
 *  widths. Yices' bv-mul operator requires that both operands are the same size and the result is the size of each operand.
 *  Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *  \code
 *    (bv-mul (bv-sign-extend A [|B|-1]) (bv-sign-extend B [|A|-1]))
 *  \endcode
 */
void
YicesSolver::out_mult(std::ostream &o, const InternalNodePtr &in)
{
    o <<"(bv-mul (bv-sign-extend ";
    out_expr(o, in->child(0));
    o <<" " <<(in->child(1)->get_nbits()-1) <<") (bv-sign-extend ";
    out_expr(o, in->child(1));
    o <<" " <<(in->child(0)->get_nbits()-1) <<"))";
}

/** Output for write. */
void
YicesSolver::out_write(std::ostream &o, const InternalNodePtr &in)
{
    o <<"(update ";
    out_expr(o, in->child(0));  // previous memory state
    o <<" (";
    out_expr(o, in->child(1));  // address to which we are writing
    o <<") ";
    out_expr(o, in->child(2));  // value we are writing
    o <<")";
}

/** Output for read. */
void
YicesSolver::out_read(std::ostream &o, const InternalNodePtr &in)
{
    o <<"(";
    out_expr(o, in->child(0));  // memory state from which we are reading
    o <<" ";
    out_expr(o, in->child(1));  // address from which we are reading
    o <<")";
}

#ifdef ROSE_HAVE_LIBYICES
/** Traverse an expression and define Yices variables. */
void
YicesSolver::ctx_define(const TreeNodePtr &tn, Definitions *defns)
{
    assert(defns);
    LeafNodePtr ln = tn->isLeafNode();
    InternalNodePtr in = tn->isInternalNode();

    if (ln && ln->is_variable()) {
        if (defns->find(ln->get_name())==defns->end()) {
            defns->insert(ln->get_name());
            yices_type bvtype = yices_mk_bitvector_type(context, ln->get_nbits());
            assert(bvtype);
            char name[64];
            snprintf(name, sizeof name, "v%"PRIu64, ln->get_name());
            yices_var_decl vdecl = yices_mk_var_decl(context, name, bvtype);
            assert(vdecl);
        }
    } else if (ln && ln->is_memory()) {
        if (defns->find(ln->get_name())==defns->end()) {
            defns->insert(ln->get_name());
            yices_type domain = yices_mk_bitvector_type(context, 32);
            yices_type range = yices_mk_bitvector_type(context, ln->get_nbits());
            yices_type ftype = yices_mk_function_type(context, &domain, 1, range);
            assert(ftype);
            char name[64];
            snprintf(name, sizeof name, "m%"PRIu64, ln->get_name());
            yices_var_decl vdecl = yices_mk_var_decl(context, name, ftype);
            assert(vdecl);
        }
    } else if (in) {
        for (size_t i=0; i<in->size(); i++)
            ctx_define(in->child(i), defns);
    }
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Assert a constraint in the logical context. */
void
YicesSolver::ctx_assert(const TreeNodePtr &tn)
{
    yices_expr e = ctx_expr(tn);
    assert(e);
    yices_assert(context, e);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Builds a Yices expression from a ROSE expression. */
yices_expr
YicesSolver::ctx_expr(const TreeNodePtr &tn)
{
    yices_expr retval = 0;
    using namespace InsnSemanticsExpr;
    LeafNodePtr ln = tn->isLeafNode();
    InternalNodePtr in = tn->isInternalNode();
    if (ln) {
        if (ln->is_known()) {
            retval = yices_mk_bv_constant(context, ln->get_nbits(), ln->get_value());
        } else {
            assert(ln->is_memory() || ln->is_variable());
            char name[64];
            sprintf(name, "%c%"PRIu64, ln->is_memory()?'m':'v', ln->get_name());
            yices_var_decl vdecl = yices_get_var_decl_from_name(context, name);
            assert(vdecl);
            retval = yices_mk_var_from_decl(context, vdecl);
        }
    } else {
        assert(in!=NULL);
        switch (in->get_operator()) {
            case OP_ADD:        retval = ctx_la(yices_mk_bv_add, in, false);            break;
            case OP_AND:        retval = ctx_la(yices_mk_and, in, true);                break;
            case OP_ASR:        retval = ctx_asr(in);                                   break;
            case OP_BV_AND:     retval = ctx_la(yices_mk_bv_and, in, true);             break;
            case OP_BV_OR:      retval = ctx_la(yices_mk_bv_or, in, false);             break;
            case OP_BV_XOR:     retval = ctx_la(yices_mk_bv_xor, in, false);            break;
            case OP_EQ:         retval = ctx_binary(yices_mk_eq, in);                   break;
            case OP_CONCAT:     retval = ctx_la(yices_mk_bv_concat, in);                break;
            case OP_EXTRACT:    retval = ctx_extract(in);                               break;
            case OP_INVERT:     retval = ctx_unary(yices_mk_bv_not, in);                break;
            case OP_ITE:        retval = ctx_ite(in);                                   break;
            case OP_LSSB:       throw Exception("OP_LSSB not implemented");
            case OP_MSSB:       throw Exception("OP_MSSB not implemented");
            case OP_NE:         retval = ctx_binary(yices_mk_diseq, in);                break;
            case OP_NEGATE:     retval = ctx_unary(yices_mk_bv_minus, in);              break;
            case OP_NOOP:       throw Exception("OP_NOOP not implemented");
            case OP_OR:         retval = ctx_la(yices_mk_or, in, false);                break;
            case OP_READ:       retval = ctx_read(in);                                  break;
            case OP_ROL:        throw Exception("OP_ROL not implemented");
            case OP_ROR:        throw Exception("OP_ROR not implemented");
            case OP_SDIV:       throw Exception("OP_SDIV not implemented");
            case OP_SEXTEND:    retval = ctx_sext(in);                                  break;
            case OP_SLT:        retval = ctx_binary(yices_mk_bv_slt, in);               break;
            case OP_SLE:        retval = ctx_binary(yices_mk_bv_sle, in);               break;
            case OP_SHL0:       retval = ctx_shift(yices_mk_bv_shift_left0, in);        break;
            case OP_SHL1:       retval = ctx_shift(yices_mk_bv_shift_left1, in);        break;
            case OP_SHR0:       retval = ctx_shift(yices_mk_bv_shift_right0, in);       break;
            case OP_SHR1:       retval = ctx_shift(yices_mk_bv_shift_right1, in);       break;
            case OP_SGE:        retval = ctx_binary(yices_mk_bv_sge, in);               break;
            case OP_SGT:        retval = ctx_binary(yices_mk_bv_sgt, in);               break;
            case OP_SMOD:       throw Exception("OP_SMOD not implemented");
            case OP_SMUL:       retval = ctx_mult(in);                                  break;
            case OP_UDIV:       throw Exception("OP_UDIV not implemented");
            case OP_UEXTEND:    retval = ctx_uext(in);                                  break;
            case OP_UGE:        retval = ctx_binary(yices_mk_bv_ge, in);                break;
            case OP_UGT:        retval = ctx_binary(yices_mk_bv_gt, in);                break;
            case OP_ULE:        retval = ctx_binary(yices_mk_bv_le, in);                break;
            case OP_ULT:        retval = ctx_binary(yices_mk_bv_lt, in);                break;
            case OP_UMOD:       throw Exception("OP_UMOD not implemented");
            case OP_UMUL:       retval = ctx_mult(in);                                  break;
            case OP_WRITE:      retval = ctx_write(in);                                 break;
            case OP_ZEROP:      retval = ctx_zerop(in);                                 break;
        }
    }
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression from unary ROSE expression. */
yices_expr
YicesSolver::ctx_unary(UnaryAPI f, const InternalNodePtr &in)
{
    assert(f);
    assert(in && 1==in->size());
    yices_expr retval = (f)(context, ctx_expr(in->child(0)));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices epxression from binary ROSE expression. */
yices_expr
YicesSolver::ctx_binary(BinaryAPI f, const InternalNodePtr &in)
{
    assert(f);
    assert(in && 2==in->size());
    yices_expr retval = (f)(context, ctx_expr(in->child(0)), ctx_expr(in->child(1)));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for if-then-else operator. */
yices_expr
YicesSolver::ctx_ite(const InternalNodePtr &in)
{
    assert(in && 3==in->size());
    assert(in->child(0)->get_nbits()==1);
    yices_expr cond = yices_mk_eq(context, ctx_expr(in->child(0)), yices_mk_bv_constant(context, 1, 1));
    yices_expr retval = yices_mk_ite(context, cond, ctx_expr(in->child(1)), ctx_expr(in->child(2)));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for left-associative, binary operators. The @p identity is sign-extended and used as the
 * second operand if only one operand is supplied. */
yices_expr
YicesSolver::ctx_la(BinaryAPI f, const InternalNodePtr &in, bool identity)
{
    assert(f);
    assert(in && in->size()>=1);

    yices_expr retval = ctx_expr(in->child(0));

    for (size_t i=1; i<in->size(); i++) {
        retval = (f)(context, retval, ctx_expr(in->child(i)));
        assert(retval);
    }
    
    if (1==in->size()) {
        yices_expr ident = yices_mk_bv_constant(context, 1, identity ? (uint64_t)(-1) : 0);
        retval = (f)(context, retval, ident);
        assert(retval);
    }
    
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
yices_expr
YicesSolver::ctx_la(NaryAPI f, const InternalNodePtr &in, bool identity)
{
    assert(f);
    assert(in && in->size()>=1);
    yices_expr *operands = new yices_expr[in->size()];
    for (size_t i=0; i<in->size(); i++) {
        operands[i] = ctx_expr(in->child(i));
        assert(operands[i]);
    }
    yices_expr retval = (f)(context, operands, in->size());
    assert(retval);
    delete[] operands;
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
yices_expr
YicesSolver::ctx_la(BinaryAPI f, const InternalNodePtr &in)
{
    assert(in->size()>1);
    return ctx_la(f, in, false);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generate a Yices expression for extract. The yices_mk_bv_extract() second two arguments must be constants. */
yices_expr
YicesSolver::ctx_extract(const InternalNodePtr &in)
{
    assert(in && 3==in->size());
    assert(in->child(0)->is_known());
    assert(in->child(1)->is_known());
    assert(in->child(0)->get_value() < in->child(1)->get_value());
    size_t lo = in->child(0)->get_value();
    size_t hi = in->child(1)->get_value() - 1; /*inclusive*/
    yices_expr retval = yices_mk_bv_extract(context, hi, lo, ctx_expr(in->child(2)));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generate a Yices expression for sign-extend. The third argument for yices_mk_bv_sign_extend() is the number of bits by which
 *  the second argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size)
 *  and the size of the second operand (the bit vector to be extended). */
yices_expr
YicesSolver::ctx_sext(const InternalNodePtr &in)
{
    assert(in && 2==in->size());
    assert(in->child(0)->is_known());
    assert(in->child(0)->get_value() > in->child(1)->get_nbits());
    unsigned extend_by = in->child(0)->get_value() - in->child(1)->get_nbits();
    yices_expr retval = yices_mk_bv_sign_extend(context, ctx_expr(in->child(1)), extend_by);
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generate a Yices expression for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 *  (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
yices_expr
YicesSolver::ctx_uext(const InternalNodePtr &in)
{
    assert(in && 2==in->size());
    assert(in->child(0)->is_known()); /*Yices mk-bv needs a number for the size operand*/
    assert(in->child(0)->get_value() > in->child(1)->get_nbits());
    size_t extend_by = in->child(0)->get_value() - in->child(1)->get_nbits();
    yices_expr retval = yices_mk_bv_concat(context,
                                           yices_mk_bv_constant(context, extend_by, 0),
                                           ctx_expr(in->child(1)));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generates a Yices expression for shift operators. */
yices_expr
YicesSolver::ctx_shift(ShiftAPI f, const InternalNodePtr &in)
{
    assert(in && 2==in->size());
    assert(in->child(0)->is_known()); /*Yices' bv-shift-* operators need a constant for the shift amount*/
    unsigned shift_amount = in->child(0)->get_value();
    yices_expr retval = (f)(context, ctx_expr(in->child(1)), shift_amount);
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generates a Yices expression for arithmetic right shift.  Yices doesn't have a sign-extending right shift, therefore we
 *  implement it in terms of other operations.  (OP_ASR ShiftAmount Vector) becomes
 *
 *  (ite (= (mk-bv 1 1) (bv-extract [VectorSize-1] [VectorSize-1] Vector)) ;; vector's sign bit
 *       (bv-shift-right1 Vector [ShiftAmount])
 *       (bv-shift-right0 Vector [ShiftAmount]))
 *
 * where [VectorSize], [VectorSize-1], and [ShiftAmount] are numeric constants.
 */
yices_expr
YicesSolver::ctx_asr(const InternalNodePtr &in)
{
    assert(in && 2==in->size());
    TreeNodePtr vector = in->child(1);
    unsigned vector_size = vector->get_nbits();
    assert(in->child(0)->is_known());
    unsigned shift_amount = in->child(0)->get_value();
    yices_expr retval = yices_mk_ite(context, 
                                     yices_mk_eq(context, 
                                                 yices_mk_bv_constant(context, 1, 1), 
                                                 yices_mk_bv_extract(context, vector_size-1, vector_size-1, ctx_expr(vector))),
                                     yices_mk_bv_shift_right1(context, ctx_expr(vector), shift_amount), 
                                     yices_mk_bv_shift_right0(context, ctx_expr(vector), shift_amount));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
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
yices_expr
YicesSolver::ctx_zerop(const InternalNodePtr &in)
{
    assert(in && 1==in->size());
    yices_expr retval = yices_mk_ite(context,
                                     yices_mk_eq(context, 
                                                 yices_mk_bv_constant(context, in->child(0)->get_nbits(), 0), 
                                                 ctx_expr(in->child(0))), 
                                     yices_mk_bv_constant(context, 1, 1), 
                                     yices_mk_bv_constant(context, 1, 0));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Generate a Yices expression for multiply. The OP_SMUL and OP_UMUL nodes of InsnSemanticsExpr define the result width to be
 *  the sum of the input widths. Yices' bv-mul operator requires that both operands are the same size and the result is the
 *  size of each operand. Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *  \code
 *    (bv-mul (bv-sign-extend A [|B|-1]) (bv-sign-extend B [|A|-1]))
 *  \endcode
 */
yices_expr
YicesSolver::ctx_mult(const InternalNodePtr &in)
{
    yices_expr retval = yices_mk_bv_mul(context, 
                                        yices_mk_bv_sign_extend(context, ctx_expr(in->child(0)), in->child(1)->get_nbits()-1), 
                                        yices_mk_bv_sign_extend(context, ctx_expr(in->child(1)), in->child(0)->get_nbits()-1));
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Write to memory. */
yices_expr
YicesSolver::ctx_write(const InternalNodePtr &in)
{
    yices_expr func = ctx_expr(in->child(0));
    yices_expr arg  = ctx_expr(in->child(1));
    yices_expr val  = ctx_expr(in->child(2));
    assert(func && arg && val);
    yices_expr retval = yices_mk_function_update(context, func, &arg, 1, val);
    assert(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/** Read from memory. */
yices_expr
YicesSolver::ctx_read(const InternalNodePtr &in)
{
    yices_expr func = ctx_expr(in->child(0));
    yices_expr arg  = ctx_expr(in->child(1));
    assert(func && arg);
    yices_expr retval = yices_mk_app(context, func, &arg, 1);
    assert(retval);
    return retval;
}
#endif
