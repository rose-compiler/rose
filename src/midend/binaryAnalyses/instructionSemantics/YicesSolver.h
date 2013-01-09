#ifndef Rose_YicesSolver_H
#define Rose_YicesSolver_H

#include "SMTSolver.h"

#ifdef ROSE_HAVE_LIBYICES
#  include <yices_c.h>
#endif

/** Interface to the Yices Satisfiability Modulo Theory (SMT) Solver.  ROSE should be configured with --with-yices in order
 *  for the satisfiable() virtual method to work (otherwise, the YicesSolver class is still available but will fail an
 *  assertion when instantiated).
 *
 *  Yices provides two interfaces: an executable named "yices", and a library. The choice of which linkage to use to answer
 *  satisfiability questions is made at runtime (see set_linkage()).
 */
class YicesSolver: public SMTSolver {
public:
    /** Bit flags to indicate what style of calls are made to Yices. */
    enum LinkMode 
        {
        LM_NONE=0x0000,                         /**< No available linkage. */
        LM_LIBRARY=0x0001,                      /**< The Yices runtime library is available. */
        LM_EXECUTABLE=0x0002                    /**< The "yices" executable is available. */
    };

    /** Constructor prefers to use the Yices executable interface. See set_linkage(). */
    YicesSolver(): linkage(LM_NONE), context(NULL) {
        init();
    }
    virtual ~YicesSolver();

    virtual void generate_file(std::ostream&, const std::vector<InsnSemanticsExpr::TreeNodePtr> &exprs, Definitions*);
    virtual std::string get_command(const std::string &config_name);

    /** Returns a bit vector indicating what calling modes are available.  The bits are defined by the LinkMode enum. */
    static unsigned available_linkage();

    /** Returns the style of linkage currently enabled. */
    LinkMode get_linkage() const {
        return linkage;
    }

    /** Sets the linkage style. */
    void set_linkage(LinkMode lm) {
        ROSE_ASSERT(lm & available_linkage());
        linkage = lm;
    }

    /** Determines if the specified expression is satisfiable.  Most solvers use the implementation in the base class, which
     *  creates a text file (usually in SMT-LIB format) and then invokes an executable with that input, looking for a line of
     *  output containing "sat" or "unsat". However, Yices provides a library that can optionally be linked into ROSE, and
     *  uses this library if the link mode is LM_LIBRARY.
     *  @{ */
    virtual Satisfiable satisfiable(const std::vector<InsnSemanticsExpr::TreeNodePtr> &exprs);
    virtual Satisfiable satisfiable(const InsnSemanticsExpr::TreeNodePtr &tn) {
        std::vector<InsnSemanticsExpr::TreeNodePtr> exprs;
        exprs.push_back(tn);
        return satisfiable(exprs);
    }
    /** @} */

    virtual InsnSemanticsExpr::TreeNodePtr evidence_for_name(const std::string&) /*overrides*/;
    virtual std::vector<std::string> evidence_names() /*overrides*/;
    virtual void clear_evidence() /*overrides*/;

protected:
    virtual uint64_t parse_variable(const char *nptr, char **endptr, char first_char);
    virtual void parse_evidence();
    typedef std::map<std::string/*name or hex-addr*/, std::pair<size_t/*nbits*/, uint64_t/*value*/> > Evidence;
    Evidence evidence;

private:
    LinkMode linkage;
    void init();

    /* These out_*() functions convert a InsnSemanticsExpr expression into text which is suitable as input to "yices"
     * executable. */
    void out_define(std::ostream&, const InsnSemanticsExpr::TreeNodePtr&, Definitions*);
    void out_assert(std::ostream&, const InsnSemanticsExpr::TreeNodePtr&);
    void out_number(std::ostream&, const InsnSemanticsExpr::TreeNodePtr&);
    void out_expr(std::ostream&, const InsnSemanticsExpr::TreeNodePtr&);
    void out_unary(std::ostream&, const char *opname, const InsnSemanticsExpr::InternalNodePtr&);
    void out_binary(std::ostream&, const char *opname, const InsnSemanticsExpr::InternalNodePtr&);
    void out_ite(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_la(std::ostream&, const char *opname, const InsnSemanticsExpr::InternalNodePtr&, bool identity_elmt);
    void out_la(std::ostream&, const char *opname, const InsnSemanticsExpr::InternalNodePtr&);
    void out_extract(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_sext(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_uext(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_shift(std::ostream&, const char *opname, const InsnSemanticsExpr::InternalNodePtr&, bool newbits);
    void out_asr(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_zerop(std::ostream&, const InsnSemanticsExpr::InternalNodePtr&);
    void out_mult(std::ostream &o, const InsnSemanticsExpr::InternalNodePtr&);
    void out_read(std::ostream &o, const InsnSemanticsExpr::InternalNodePtr&);
    void out_write(std::ostream &o, const InsnSemanticsExpr::InternalNodePtr&);

#ifdef ROSE_HAVE_LIBYICES
    /* These ctx_*() functions build a Yices context object if Yices is linked into this executable. */
    typedef yices_expr (*UnaryAPI)(yices_context, yices_expr operand);
    typedef yices_expr (*BinaryAPI)(yices_context, yices_expr operand1, yices_expr operand2);
    typedef yices_expr (*NaryAPI)(yices_context, yices_expr *operands, unsigned n_operands);
    typedef yices_expr (*ShiftAPI)(yices_context, yices_expr, unsigned amount);

    yices_context context;
    void ctx_define(const InsnSemanticsExpr::TreeNodePtr&, Definitions*);
    void ctx_assert(const InsnSemanticsExpr::TreeNodePtr&);
    yices_expr ctx_expr(const InsnSemanticsExpr::TreeNodePtr&);
    yices_expr ctx_unary(UnaryAPI, const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_binary(BinaryAPI, const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_ite(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_la(BinaryAPI, const InsnSemanticsExpr::InternalNodePtr&, bool identity_elmt);
    yices_expr ctx_la(NaryAPI, const InsnSemanticsExpr::InternalNodePtr&, bool identity_elmt);
    yices_expr ctx_la(BinaryAPI, const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_extract(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_sext(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_uext(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_shift(ShiftAPI, const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_asr(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_zerop(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_mult(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_read(const InsnSemanticsExpr::InternalNodePtr&);
    yices_expr ctx_write(const InsnSemanticsExpr::InternalNodePtr&);
    
#else
    void *context; /*unused for now*/
#endif

};

#endif
