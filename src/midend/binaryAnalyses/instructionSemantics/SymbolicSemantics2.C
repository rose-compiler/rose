#include "sage3basic.h"
#include "SymbolicSemantics2.h"
#include "integerOps.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace SymbolicSemantics {

std::ostream &
operator<<(std::ostream &o, const SValue &e)
{
    e.print(o);
    return o;
}

/*******************************************************************************************************************************
 *                                      SValue
 *******************************************************************************************************************************/

class PrintHelper: public BaseSemantics::PrintHelper {
public:
        InsnSemanticsExpr::PrintHelper expr_phelp;
};

uint64_t
SValue::get_number() const
{
    LeafNodePtr leaf = expr->isLeafNode();
    assert(leaf!=NULL);
    return leaf->get_value();
}

size_t
SValue::add_defining_instructions(const InsnSet &to_add)
{
    size_t nadded = 0;
    for (InsnSet::const_iterator i=to_add.begin(); i!=to_add.end(); ++i) {
        std::pair<InsnSet::iterator, bool> inserted = defs.insert(*i);
        if (inserted.second)
            ++nadded;
    }
    return nadded;
}

size_t
SValue::add_defining_instructions(SgAsmInstruction *insn)
{
    InsnSet tmp;
    if (insn)
        tmp.insert(insn);
    return add_defining_instructions(tmp);
}

void
SValue::set_defining_instructions(SgAsmInstruction *insn)
{
    InsnSet tmp;
    if (insn)
        tmp.insert(insn);
    return set_defining_instructions(tmp);
}

bool
SValue::may_equal(const BaseSemantics::SValuePtr &other_, SMTSolver *solver) const 
{
    SValuePtr other = SValue::promote(other_);
    assert(get_width()==other->get_width());
    return get_expression()->may_equal(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, SMTSolver *solver) const
{
    SValuePtr other = SValue::promote(other_);
    assert(get_width()==other->get_width());
    return get_expression()->must_equal(other->get_expression(), solver);
}

void
SValue::print(std::ostream &o, BaseSemantics::PrintHelper *helper_) const
{
    PrintHelper *helper = dynamic_cast<PrintHelper*>(helper_);
    InsnSemanticsExpr::PrintHelper dflt_ph;
    InsnSemanticsExpr::PrintHelper &ph = helper ? helper->expr_phelp : dflt_ph;

    if (defs.empty()) {
        expr->print(o, ph);
    } else {
        o <<"{defs={";
        size_t ndefs=0;
        for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
            SgAsmInstruction *insn = *di;
            if (insn!=NULL)
                o <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
        }
        o <<"}, expr=";
        expr->print(o, ph);
        o <<"}";
    }
}
    

/*******************************************************************************************************************************
 *                                      Memory state
 *******************************************************************************************************************************/

SValuePtr
MemoryState::CellCompressorMcCarthy::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *ops, const CellList &cells)
{
    RiscOperators *ops_symbolic = dynamic_cast<RiscOperators*>(ops);
    bool compute_usedef = ops_symbolic ?  true : ops_symbolic->get_compute_usedef();

    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());
    // FIXME: This makes no attempt to remove duplicate values [Robb Matzke 2013-03-01]
    TreeNodePtr expr = LeafNode::create_memory(8);
    InsnSet definers;
    for (CellList::const_reverse_iterator ci=cells.rbegin(); ci!=cells.rend(); ++ci) {
        SValuePtr cell_addr = SValue::promote((*ci)->get_address());
        SValuePtr cell_value  = SValue::promote((*ci)->get_value());
        expr = InternalNode::create(8, InsnSemanticsExpr::OP_WRITE,
                                    expr, cell_addr->get_expression(), cell_value->get_expression());
        if (compute_usedef) {
            const InsnSet &cell_definers = cell_value->get_defining_instructions();
            definers.insert(cell_definers.begin(), cell_definers.end());
        }
    }
    SValuePtr retval = SValue::promote(address->create(InternalNode::create(8, InsnSemanticsExpr::OP_READ,
                                                                            expr, address->get_expression())));
    if (compute_usedef)
        retval->add_defining_instructions(definers);
    return retval;
}

SValuePtr
MemoryState::CellCompressorSimple::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                               BaseSemantics::RiscOperators *ops, const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());
    return SValue::promote(dflt);
}

SValuePtr
MemoryState::CellCompressorChoice::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                              BaseSemantics::RiscOperators *ops, const CellList &cells)
{
    if (ops->get_solver())
        return cc_mccarthy(address, dflt, ops, cells);
    return cc_simple(address, dflt, ops, cells);
}

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &address_, const BaseSemantics::SValuePtr &dflt,
                        size_t nbits, BaseSemantics::RiscOperators *ops)
{
    SValuePtr address = SValue::promote(address_);
    assert(8==nbits); // SymbolicSemantics::MemoryState assumes that memory cells contain only 8-bit data
    bool short_circuited;
    CellList matches = scan(address, nbits, ops, short_circuited/*out*/);

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists.
    if (!short_circuited) {
        BaseSemantics::MemoryCellPtr tmpcell = protocell->create(address, dflt);
        cells.push_front(tmpcell);
        matches.push_back(tmpcell);
    }

    assert(dflt->get_width()==nbits);
    SValuePtr retval = get_cell_compressor()->operator()(address, dflt, ops, matches);
    assert(retval->get_width()==8);
    return retval;
}

void
MemoryState::writeMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &value,
                         BaseSemantics::RiscOperators *ops)
{
    assert(32==address->get_width());
    assert(8==value->get_width());
    BaseSemantics::MemoryCellList::writeMemory(address, value, ops);
}

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

void
RiscOperators::interrupt(int majr, int minr)
{
    get_state()->clear();
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());

    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_AND,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());
    
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_OR,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());

    SValuePtr retval;
    // We leave these simplifications here because InsnSemanticsExpr doesn't yet have a way to pass an SMT solver to its
    // simplifier.
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(a->get_width(), a->get_number() ^ b->get_number());
    } else if (a->get_expression()->must_equal(b->get_expression(), solver)) {
        retval = svalue_number(a->get_width(), 0);
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_XOR,
                                                  a->get_expression(), b->get_expression()));
    }
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}
    
BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_INVERT, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    assert(end_bit<=a->get_width());
    assert(begin_bit<end_bit);
    SValuePtr retval = svalue_expr(InternalNode::create(end_bit-begin_bit, InsnSemanticsExpr::OP_EXTRACT,
                                                        LeafNode::create_integer(32, begin_bit),
                                                        LeafNode::create_integer(32, end_bit),
                                                        a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width()+b->get_width(), InsnSemanticsExpr::OP_CONCAT,
                                                        b->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(1, InsnSemanticsExpr::OP_ZEROP, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel_,
                   const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr sel = SValue::promote(sel_);
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(1==sel->get_width());
    assert(a->get_width()==b->get_width());

    SValuePtr retval;
    if (sel->is_number()) {
        retval = SValue::promote(sel->get_number() ? a->copy() : b->copy());
        if (compute_usedef)
            retval->defined_by(cur_insn, sel->get_defining_instructions());
        return retval;
    }
    if (solver) {
        // If the selection expression cannot be true, then return b
        TreeNodePtr assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                                     sel->get_expression(),
                                                     LeafNode::create_integer(1, 1));
        bool can_be_true = SMTSolver::SAT_NO != solver->satisfiable(assertion);
        if (!can_be_true) {
            retval = SValue::promote(b->copy());
            if (compute_usedef)
                retval->defined_by(cur_insn, sel->get_defining_instructions());
            return retval;
        }

        // If the selection expression cannot be false, then return a
        assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                         sel->get_expression(), LeafNode::create_integer(1, 0));
        bool can_be_false = SMTSolver::SAT_NO != solver->satisfiable(assertion);
        if (!can_be_false) {
            retval = SValue::promote(a->copy());
            if (compute_usedef)
                retval->defined_by(cur_insn, sel->get_defining_instructions());
            return retval;
        }
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ITE, sel->get_expression(),
                                              a->get_expression(), b->get_expression()));
    if (compute_usedef) {
        retval->defined_by(cur_insn, sel->get_defining_instructions(),
                           a->get_defining_instructions(), b->get_defining_instructions());
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_LSSB, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_MSSB, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ROL,
                                                        sa->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ROR,
                                                        sa->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SHL0,
                                                        sa->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SHR0,
                                                        sa->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ASR,
                                                        sa->get_expression(), a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_UEXTEND,
                                                        LeafNode::create_integer(32, new_width),
                                                        a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ADD,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    assert(a->get_width()==b->get_width() && c->get_width()==1);
    BaseSemantics::SValuePtr aa = unsignedExtend(a, a->get_width()+1);
    BaseSemantics::SValuePtr bb = unsignedExtend(b, a->get_width()+1);
    BaseSemantics::SValuePtr cc = unsignedExtend(c, a->get_width()+1);
    BaseSemantics::SValuePtr sumco = add(aa, add(bb, cc));
    carry_out = extract(xor_(aa, xor_(bb, sumco)), 1, a->get_width()+1);
    return add(a, add(b, unsignedExtend(c, a->get_width())));
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_NEGATE, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SDIV,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval = svalue_expr(InternalNode::create(b->get_width(), InsnSemanticsExpr::OP_SMOD,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    SValuePtr retval = svalue_expr(InternalNode::create(retwidth, InsnSemanticsExpr::OP_SMUL,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_UDIV,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval = svalue_expr(InternalNode::create(b->get_width(), InsnSemanticsExpr::OP_UMOD,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    SValuePtr retval = svalue_expr(InternalNode::create(retwidth, InsnSemanticsExpr::OP_UMUL,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_SEXTEND,
                                                        LeafNode::create_integer(32, new_width),
                                                        a->get_expression()));
    if (compute_usedef)
        retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &condition,
                          size_t nbits) {
    assert(1==condition->get_width()); // FIXME: condition is not used
    assert(8==nbits || 16==nbits || 32==nbits);

    // Read the bytes in little endian order and concatenate them together. InsnSemanticsExpr will simplify the expression
    // so that reading after writing a multi-byte value will return the original value written rather than a concatenation
    // of byte extractions.
    SValuePtr dflt = svalue_undefined(nbits), retval;
    InsnSet defs;
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*bytenum, 8*bytenum+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        SValuePtr byte_value = SValue::promote(state->readMemory(byte_addr, byte_dflt, 8, this));
        retval = 0==bytenum ? byte_value : SValue::promote(concat(retval, byte_value));
        if (compute_usedef) {
            const InsnSet &definers = byte_value->get_defining_instructions();
            defs.insert(definers.begin(), definers.end());
        }
    }

    assert(retval!=NULL && retval->get_width()==nbits);
    if (compute_usedef)
        retval->defined_by(NULL, defs);
    return retval;
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg,
                           const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value,
                           const BaseSemantics::SValuePtr &condition) {
    size_t nbits = value->get_width();
    assert(8==nbits || 16==nbits || 32==nbits);
    assert(1==condition->get_width()); // FIXME: condition is not used
    size_t nbytes = nbits/8;
    for (size_t bytenum=0; bytenum<nbytes; ++bytenum) {
        BaseSemantics::SValuePtr byte_value = extract(value, 8*bytenum, 8*bytenum+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        state->writeMemory(byte_addr, byte_value, this);
    }
}

} // namespace
} // namespace
} // namespace
