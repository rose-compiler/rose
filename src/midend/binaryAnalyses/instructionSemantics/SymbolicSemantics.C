#include "rose.h"
#include "SymbolicSemantics.h"

namespace SymbolicSemantics {

uint64_t name_counter;

const char *
to_str(Operator o)
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
        case OP_SHL0: return "shl0";
        case OP_SHL1: return "shl1";
        case OP_SHR0: return "shr0";
        case OP_SHR1: return "shr1";
        case OP_SMOD: return "smod";
        case OP_SMUL: return "smul";
        case OP_UDIV: return "udiv";
        case OP_UEXTEND: return "uextend";
        case OP_UMOD: return "umod";
        case OP_UMUL: return "umul";
        case OP_ZEROP: return "zerop";
    }
    ROSE_ASSERT(!"list is not complete"); /*do not add as default since that would turn off compiler warnings*/
}

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
    
void
InternalNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, rmap);
    }
    o <<")";
}

bool
InternalNode::equal_to(const TreeNode *other_) const
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
LeafNode *
LeafNode::create_variable(size_t nbits)
{
    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = false;
    retval->name = name_counter++;
    return retval;
}

/* class method */
LeafNode *
LeafNode::create_integer(size_t nbits, uint64_t n)
{
    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = true;
    retval->ival = n & (((uint64_t)1<<nbits)-1);
    return retval;
}
    
bool
LeafNode::is_known() const
{
    return known;
}

uint64_t
LeafNode::get_value() const
{
    assert(known);
    return ival;
}

uint64_t
LeafNode::get_name() const
{
    assert(!known);
    return name;
}
    
void
LeafNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
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
LeafNode::equal_to(const TreeNode *other_) const
{
    if (!TreeNode::equal_to(other_)) return false;
    const LeafNode *other = dynamic_cast<const LeafNode*>(other_);
    if (!other) return false;
    if (known!=other->known) return false;
    if (known)
        return ival == other->ival;
    return name == other->name;
}

bool
MemoryCell::may_alias(const MemoryCell &other) const
{
    return must_alias(other); /*FIXME: need to tighten this up some. [RPM 2010-05-24]*/
}

bool
MemoryCell::must_alias(const MemoryCell &other) const
{
    return address.equal_to(other.address);
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
