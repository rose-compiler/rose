#include "rose.h"
#include "SymbolicExpr.h"

const char *
SymbolicExpr::to_str(Operator o)
{
    switch (o) {
        case OP_ADD: return "add";
        case OP_AND: return "and";
        case OP_ASR: return "asr";
        case OP_BV_AND: return "bv-and";
        case OP_BV_OR: return "bv-or";
        case OP_BV_XOR: return "bv-xor";
        case OP_CONCAT: return "concat";
        case OP_EQ: return "eq";
        case OP_EXTRACT: return "extract";
        case OP_INVERT: return "invert";
        case OP_ITE: return "ite";
        case OP_LSSB: return "lssb";
        case OP_MSSB: return "mssb";
        case OP_NE: return "ne";
        case OP_NEGATE: return "negate";
        case OP_NOOP: return "nop";
        case OP_OR: return "or";
        case OP_ROL: return "rol";
        case OP_ROR: return "ror";
        case OP_SDIV: return "sdiv";
        case OP_SEXTEND: return "sextend";
        case OP_SGE: return "sge";
        case OP_SGT: return "sgt";
        case OP_SHL0: return "shl0";
        case OP_SHL1: return "shl1";
        case OP_SHR0: return "shr0";
        case OP_SHR1: return "shr1";
        case OP_SLE: return "sle";
        case OP_SLT: return "slt";
        case OP_SMOD: return "smod";
        case OP_SMUL: return "smul";
        case OP_UDIV: return "udiv";
        case OP_UEXTEND: return "uextend";
        case OP_UGE: return "uge";
        case OP_UGT: return "ugt";
        case OP_ULE: return "ule";
        case OP_ULT: return "ult";
        case OP_UMOD: return "umod";
        case OP_UMUL: return "umul";
        case OP_ZEROP: return "zerop";
    }
    ROSE_ASSERT(!"list is not complete"); /*do not add as default since that would turn off compiler warnings*/
}

void
SymbolicExpr::InternalNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, rmap);
    }
    o <<")";
}

bool
SymbolicExpr::InternalNode::equal_to(const TreeNode *other_) const
{
    if (!TreeNode::equal_to(other_)) return false;
    const InternalNode *other = dynamic_cast<const InternalNode*>(other_);
    if (!other) return false;
    if (op!=other->op) return false;
    if (children.size()!=other->children.size()) return false;
    for (size_t i=0; i<children.size(); ++i) {
        if (!children[i]->equal_to(other->children[i]))
            return false;
    }
    return true;
}

/* class method */
SymbolicExpr::LeafNode *
SymbolicExpr::LeafNode::create_variable(size_t nbits)
{
    static uint64_t name_counter = 0;

    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = false;
    retval->name = name_counter++;
    return retval;
}

/* class method */
SymbolicExpr::LeafNode *
SymbolicExpr::LeafNode::create_integer(size_t nbits, uint64_t n)
{
    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = true;
    retval->ival = n & (((uint64_t)1<<nbits)-1);
    return retval;
}
    
bool
SymbolicExpr::LeafNode::is_known() const
{
    return known;
}

uint64_t
SymbolicExpr::LeafNode::get_value() const
{
    assert(known);
    return ival;
}

uint64_t
SymbolicExpr::LeafNode::get_name() const
{
    assert(!known);
    return name;
}
    
void
SymbolicExpr::LeafNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    if (known) {
        if (nbits>1 && (ival & ((uint64_t)1<<(nbits-1)))) {
            uint64_t sign_extended = ival | ~(((uint64_t)1<<nbits)-1);
            o <<(int64_t)sign_extended;
        } else {
            o <<ival;
        }
    } else {
        uint64_t renamed = name;
        if (rmap) {
            RenameMap::iterator found = rmap->find(name);
            if (found==rmap->end()) {
                renamed = rmap->size();
                rmap->insert(std::make_pair(name, renamed));
            } else {
                renamed = found->second;
            }
        }
        o <<"v" <<renamed;
    }
    o <<"[" <<nbits <<"]";
}

bool
SymbolicExpr::LeafNode::equal_to(const TreeNode *other_) const
{
    if (!TreeNode::equal_to(other_)) return false;
    const LeafNode *other = dynamic_cast<const LeafNode*>(other_);
    if (!other) return false;
    if (known!=other->known) return false;
    if (known)
        return ival == other->ival;
    return name == other->name;
}
