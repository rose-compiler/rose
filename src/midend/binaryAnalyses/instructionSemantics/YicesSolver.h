#ifndef Rose_YicesSolver_H
#define Rose_YicesSolver_H

#include "SMTSolver.h"

/** Interface to the Yices Satisfiability Modulo Theory (SMT) Solver. */
class YicesSolver: public SMTSolver {
public:
    virtual void generate_file(std::ostream&, const SymbolicExpr::TreeNode *expr, Definitions*);
    virtual std::string get_command(const std::string &config_name);

private:
    /* These out*() functions convert a SymbolicExpr expression into text which is suitable as input to yices. */
    void out_define(std::ostream&, const SymbolicExpr::TreeNode*, Definitions *defns);
    void out_assert(std::ostream&, const SymbolicExpr::TreeNode*);
    void out_number(std::ostream&, const SymbolicExpr::TreeNode*);
    void out_expr(std::ostream&, const SymbolicExpr::TreeNode*);
    void out_unary(std::ostream&, const char *opname, const SymbolicExpr::InternalNode*);
    void out_binary(std::ostream&, const char *opname, const SymbolicExpr::InternalNode*);
    void out_ite(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_la(std::ostream&, const char *opname, const SymbolicExpr::InternalNode*, bool identity_elmt);
    void out_la(std::ostream&, const char *opname, const SymbolicExpr::InternalNode*);
    void out_extract(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_sext(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_uext(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_shift(std::ostream&, const char *opname, const SymbolicExpr::InternalNode*, bool newbits);
    void out_asr(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_zerop(std::ostream&, const SymbolicExpr::InternalNode*);
    void out_mult(std::ostream &o, const SymbolicExpr::InternalNode *in);
};

#endif
