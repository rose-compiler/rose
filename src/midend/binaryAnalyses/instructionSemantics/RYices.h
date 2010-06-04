#ifndef Rose_RYices_H
#define Rose_RYices_H
#include "SymbolicSemantics.h"
#include <set>

namespace RYices {
    typedef std::set<uint64_t> Definitions;     /*names that have been defined to Yices*/
    
    void out(std::ostream&, const SymbolicSemantics::TreeNode*, Definitions *defns=NULL);

    void out_define(std::ostream&, const SymbolicSemantics::TreeNode*, Definitions *defns);
    void out_assert(std::ostream&, const SymbolicSemantics::TreeNode*);

    void out_number(std::ostream&, const SymbolicSemantics::TreeNode*);
    void out_expr(std::ostream&, const SymbolicSemantics::TreeNode*);
    void out_unary(std::ostream&, const char *opname, const SymbolicSemantics::InternalNode*);
    void out_binary(std::ostream&, const char *opname, const SymbolicSemantics::InternalNode*);
    void out_ite(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_la(std::ostream&, const char *opname, const SymbolicSemantics::InternalNode*, bool identity_elmt);
    void out_la(std::ostream&, const char *opname, const SymbolicSemantics::InternalNode*);
    void out_extract(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_sext(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_uext(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_shift(std::ostream&, const char *opname, const SymbolicSemantics::InternalNode*, bool newbits);
    void out_asr(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_zerop(std::ostream&, const SymbolicSemantics::InternalNode*);
    void out_mult(std::ostream &o, const SymbolicSemantics::InternalNode *in);
};

#endif
