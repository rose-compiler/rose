#include "rose.h"
#include "InsnSemanticsExpr.h"
#include "SMTSolver.h"

const char *
InsnSemanticsExpr::to_str(Operator o)
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

/* Shallow delete: delete this node if it has no parents, but not its children */
InsnSemanticsExpr::InternalNode::~InternalNode()
{
    ROSE_ASSERT(0==get_nrefs());
    for (std::vector<const TreeNode*>::iterator ci=children.begin(); ci!=children.end(); ++ci)
        (*ci)->dec_nrefs();
    children.clear();
}

void
InsnSemanticsExpr::TreeNode::deleteDeeply()
{
    if (0==nrefs)
        delete this;
}

/* Delete this node and its children if this node has no parents. The children of a node are pointers to const objects, but we
 * can delete them if their reference counts are zero since it is only ever the user that calls this method. The user
 * presumably knows when its safe to delete a whole expression tree. */
void
InsnSemanticsExpr::InternalNode::deleteDeeply()
{
    if (0==get_nrefs()) {
        for (std::vector<const TreeNode*>::iterator ci=children.begin(); ci!=children.end(); ++ci) {
            if (0==(*ci)->dec_nrefs()) {
                TreeNode *child = const_cast<TreeNode*>(*ci);
                child->deleteDeeply();
            }
        }
        children.clear();
        delete this;
    }
}

void
InsnSemanticsExpr::InternalNode::add_child(const TreeNode *child)
{
    ROSE_ASSERT(child!=0);
    child->inc_nrefs();
    children.push_back(child);
}

void
InsnSemanticsExpr::InternalNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, rmap);
    }
    o <<")";
}

bool
InsnSemanticsExpr::InternalNode::equal_to(const TreeNode *other_, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (solver) {
        InternalNode *assertion = new InternalNode(1, OP_NE, this, other_);
        retval = !solver->satisfiable(assertion); /*equal if we cannot find a solution for inequality*/
        assertion->deleteDeeply();
    } else {
        /* The naive approach uses structural equality */
        const InternalNode *other = dynamic_cast<const InternalNode*>(other_);
        if (this==other) {
            retval = true;
        } else if (other && op==other->op && children.size()==other->children.size()) {
            retval = true;
            for (size_t i=0; i<children.size() && retval; ++i)
                retval = children[i]->equal_to(other->children[i], NULL);
        }
    }
    return retval;
}

/* class method */
InsnSemanticsExpr::LeafNode *
InsnSemanticsExpr::LeafNode::create_variable(size_t nbits)
{
    static uint64_t name_counter = 0;

    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = false;
    retval->name = name_counter++;
    return retval;
}

/* class method */
InsnSemanticsExpr::LeafNode *
InsnSemanticsExpr::LeafNode::create_integer(size_t nbits, uint64_t n)
{
    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = true;
    retval->ival = n & (((uint64_t)1<<nbits)-1);
    return retval;
}

bool
InsnSemanticsExpr::LeafNode::is_known() const
{
    return known;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_value() const
{
    assert(known);
    return ival;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_name() const
{
    assert(!known);
    return name;
}

void
InsnSemanticsExpr::LeafNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
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
InsnSemanticsExpr::LeafNode::equal_to(const TreeNode *other_, SMTSolver *solver) const
{
    bool retval = false;
    if (solver) {
        InternalNode *assertion = new InternalNode(1, OP_NE, this, other_);
        retval = !solver->satisfiable(assertion); /*equal if we cannot find a solution for inequality*/
        assertion->deleteDeeply();
    } else {
        const LeafNode *other = dynamic_cast<const LeafNode*>(other_);
        if (this==other) {
            retval = true;
        } else if (other) {
            if (known) {
                retval = other->known && ival==other->ival;
            } else {
                retval = !other->known && name==other->name;
            }
        }
    }
    return retval;
}
