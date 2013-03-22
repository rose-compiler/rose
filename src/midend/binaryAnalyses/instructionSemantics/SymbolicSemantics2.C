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

class PrintHelper: public BaseSemantics::PrintHelper, public InsnSemanticsExpr::RenameMap {};

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
    return get_expression()->equal_to(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, SMTSolver *solver) const
{
    if (must_equal(other_, solver))
        return true;
    SValuePtr other = SValue::promote(other_);
    assert(get_width()==other->get_width());
    if (!solver)
        return false;
    TreeNodePtr a = get_expression();
    TreeNodePtr b = other->get_expression();
    TreeNodePtr assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ, a, b);
    return SMTSolver::SAT_NO != solver->satisfiable(assertion);
}

void
SValue::print(std::ostream &o, BaseSemantics::PrintHelper *helper_) const
{
    PrintHelper *helper = dynamic_cast<PrintHelper*>(helper_);
    o <<"{defs={";
    size_t ndefs=0;
    for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
        SgAsmInstruction *insn = *di;
        if (insn!=NULL)
            o <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
    }
    o <<"}, expr=";
    expr->print(o, helper);
    o <<"}";
}
    

/*******************************************************************************************************************************
 *                                      Memory state
 *******************************************************************************************************************************/

SValuePtr
MemoryState::mccarthy(const CellList &cells, const SValuePtr &address)
{
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
        const InsnSet &cell_definers = cell_value->get_defining_instructions();
        definers.insert(cell_definers.begin(), cell_definers.end());
    }
    SValuePtr retval = SValue::promote(address->create(InternalNode::create(8, InsnSemanticsExpr::OP_READ,
                                                                            expr, address->get_expression())));
    retval->add_defining_instructions(definers);
    return retval;
}

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &address_, const BaseSemantics::SValuePtr &dflt,
                        size_t nbits, BaseSemantics::RiscOperators *ops)
{
    SValuePtr address = SValue::promote(address_);

    // Scan the list of cells to find those that may alias the desired cell.  Stop if we get to a cell that must
    // alias the desired cell.
    BaseSemantics::MemoryCellPtr tmpcell = protocell->create(address, dflt);
    CellList matches;
    CellList::iterator must_match = cells.end();
    for (CellList::iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (tmpcell->may_alias(*ci, ops)) {
            matches.push_back(*ci);
            if (tmpcell->must_alias(*ci, ops)) {
                must_match = ci;
                break;
            }
        }
    }

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists.
    if (must_match==cells.end())
        matches.push_back(tmpcell);

    SValuePtr retval = mccarthy(matches, address);
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
RiscOperators::interrupt(uint8_t inum)
{
    get_state()->clear();
}

void
RiscOperators::sysenter() {
    get_state()->clear();
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());

    SValuePtr retval;
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(a->get_width(), a->get_number() & b->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_AND,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());
    
    SValuePtr retval;
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(a->get_width(), a->get_number() | b->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_OR,
                                                  a->get_expression(), b->get_expression()));
    }
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
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(a->get_width(), a->get_number() ^ b->get_number());
    } else if (a->get_expression()->equal_to(b->get_expression(), solver)) {
        retval = svalue_number(a->get_width(), 0);
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_XOR,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}
    
BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval;
    if (a->is_number()) {
        retval = svalue_number(a->get_width(), ~a->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_INVERT, a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    assert(end_bit<=a->get_width());
    assert(begin_bit<end_bit);
    SValuePtr retval;
    if (0==begin_bit) {
        retval = SValue::promote(unsignedExtend(a, end_bit));
    } else if (a->is_number()) {
        retval = svalue_number(end_bit-begin_bit,
                               (IntegerOps::shiftRightLogical2(a->get_number(), begin_bit) &
                                IntegerOps::genMask<uint64_t>(end_bit-begin_bit)));
        retval->defined_by(cur_insn, a->get_defining_instructions());
    } else {
        retval = svalue_expr(InternalNode::create(end_bit-begin_bit, InsnSemanticsExpr::OP_EXTRACT,
                                                  LeafNode::create_integer(32, begin_bit),
                                                  LeafNode::create_integer(32, end_bit),
                                                  a->get_expression()));
        retval->defined_by(cur_insn, a->get_defining_instructions());
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval;
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(a->get_width()+b->get_width(),
                               a->get_number() | IntegerOps::shiftLeft2(b->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width()+b->get_width(), InsnSemanticsExpr::OP_CONCAT,
                                                  b->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval;
    if (a->is_number()) {
        retval = a->get_number() ? svalue_boolean(false) : svalue_boolean(true);
    } else {
        retval = svalue_expr(InternalNode::create(1, InsnSemanticsExpr::OP_ZEROP, a->get_expression()));
    }
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
            retval->defined_by(cur_insn, sel->get_defining_instructions());
            return retval;
        }

        // If the selection expression cannot be false, then return a
        assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                         sel->get_expression(), LeafNode::create_integer(1, 0));
        bool can_be_false = SMTSolver::SAT_NO != solver->satisfiable(assertion);
        if (!can_be_false) {
            retval = SValue::promote(a->copy());
            retval->defined_by(cur_insn, sel->get_defining_instructions());
            return retval;
        }
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ITE, sel->get_expression(),
                                              a->get_expression(), b->get_expression()));
    retval->defined_by(cur_insn, sel->get_defining_instructions(),
                       a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval;
    if (a->is_number()) {
        uint64_t n = a->get_number();
        for (size_t i=0; i<a->get_width(); ++i) {
            if (n & IntegerOps::shl1<uint64_t>(i)) {
                retval = svalue_number(a->get_width(), i);
                retval->defined_by(cur_insn, a->get_defining_instructions());
                return retval;
            }
        }
        retval = svalue_number(a->get_width(), 0);
        retval->defined_by(cur_insn, a->get_defining_instructions());
        return retval;
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_LSSB, a->get_expression()));
    retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval;
    if (a->is_number()) {
        uint64_t n = a->get_number();
        for (size_t i=a->get_width(); i>0; --i) {
            if (n & IntegerOps::shl1<uint64_t>(i-1)) {
                retval = svalue_number(a->get_width(), i-1);
                retval->defined_by(cur_insn, a->get_defining_instructions());
                return retval;
            }
        }
        retval = svalue_number(a->get_width(), 0);
        retval->defined_by(cur_insn, a->get_defining_instructions());
        return retval;
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_MSSB, a->get_expression()));
    retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval;
    if (a->is_number() && sa->is_number()) {
        retval = svalue_number(a->get_width(), IntegerOps::rotateLeft2(a->get_number(), sa->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ROL,
                                                  sa->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval;
    if (a->is_number() && sa->is_number()) {
        retval = svalue_number(a->get_width(), IntegerOps::rotateRight2(a->get_number(), sa->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ROR,
                                                  sa->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval;
    if (a->is_number() && sa->is_number()) {
        retval = svalue_number(a->get_width(), IntegerOps::shiftLeft2(a->get_number(), sa->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SHL0,
                                                  sa->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval;
    if (a->is_number() && sa->is_number()) {
        retval = svalue_number(a->get_width(), IntegerOps::shiftRightLogical2(a->get_number(), sa->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SHR0,
                                                  sa->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    SValuePtr retval;
    if (a->is_number() && sa->is_number()) {
        retval = svalue_number(a->get_width(),
                               IntegerOps::shiftRightArithmetic2(a->get_number(), sa->get_number(), a->get_width()));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ASR,
                                                  sa->get_expression(), a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    if (a->get_width()==new_width)
        return a->copy();

    SValuePtr retval;
    if (new_width < a->get_width()) {
        // Truncation.
        if (a->is_number()) {
            retval = svalue_number(new_width, a->get_number() & IntegerOps::genMask<uint64_t>(new_width));
            if (0!=IntegerOps::shiftRightLogical2(a->get_number(), a->get_width())) {
                retval->defined_by(cur_insn, a->get_defining_instructions());
            } else {
                retval->defined_by(NULL, a->get_defining_instructions());
            }
        } else {
            retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_EXTRACT,
                                                      LeafNode::create_integer(32, 0),
                                                      LeafNode::create_integer(32, new_width),
                                                      a->get_expression()));
            retval->defined_by(cur_insn, a->get_defining_instructions());
        }
    } else {
        // Extension
        if (a->is_number()) {
            retval = svalue_number(new_width, a->get_number());
            retval->defined_by(NULL, a->get_defining_instructions());
        } else {
            retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_UEXTEND,
                                                      LeafNode::create_integer(32, new_width),
                                                      a->get_expression()));
            retval->defined_by(cur_insn, a->get_defining_instructions());
        }
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    assert(a->get_width()==b->get_width());
    SValuePtr retval;
    if (a->is_number()) {
        if (b->is_number()) {
            retval = svalue_number(a->get_width(), a->get_number()+b->get_number());
            retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
            return retval;
        } else if (0==a->get_number()) {
            return b->copy();
        }
    } else if (b->is_number() && 0==b->get_number()) {
        return a->copy();
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ADD,
                                              a->get_expression(), b->get_expression()));
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
    SValuePtr retval;
    if (a->is_number()) {
        retval = svalue_number(a->get_width(), -a->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_NEGATE, a->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval;
    if (a->is_number() && b->is_number() && 0!=b->get_number()) {
        retval = svalue_number(a->get_width(),
                               IntegerOps::signExtend2(a->get_number(), a->get_width(), 64) /
                               IntegerOps::signExtend2(b->get_number(), b->get_width(), 64));
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_SDIV,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval;
    if (a->is_number() && b->is_number() && 0!=b->get_number()) {
        retval = svalue_number(b->get_width(),
                               IntegerOps::signExtend2(a->get_number(), a->get_width(), 64) %
                               IntegerOps::signExtend2(b->get_number(), b->get_width(), 64));
    } else {
        retval = svalue_expr(InternalNode::create(b->get_width(), InsnSemanticsExpr::OP_SMOD,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    SValuePtr retval;
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(retwidth,
                               IntegerOps::signExtend2(a->get_number(), a->get_width(), 64) *
                               IntegerOps::signExtend2(b->get_number(), b->get_width(), 64));
    } else {
        retval = svalue_expr(InternalNode::create(retwidth, InsnSemanticsExpr::OP_SMUL,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval;
    if (a->is_number() && b->is_number() && 0!=b->get_number()) {
        retval = svalue_number(a->get_width(), a->get_number() / b->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_UDIV,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr retval;
    if (a->is_number() && b->is_number() && 0!=b->get_number()) {
        retval = svalue_number(b->get_width(), a->get_number() % b->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(b->get_width(), InsnSemanticsExpr::OP_UMOD,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    SValuePtr retval;
    if (a->is_number() && b->is_number()) {
        retval = svalue_number(retwidth, a->get_number()*b->get_number());
    } else {
        retval = svalue_expr(InternalNode::create(retwidth, InsnSemanticsExpr::OP_UMUL,
                                                  a->get_expression(), b->get_expression()));
    }
    retval->defined_by(cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval;
    if (a->get_width() == new_width) {
        return a->copy();
    } else if (a->is_number()) {
        retval = svalue_number(new_width, IntegerOps::signExtend2(a->get_number(), a->get_width(), new_width));
        retval->defined_by(cur_insn, a->get_defining_instructions());
    } else if (a->get_width() > new_width) {
        retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_EXTRACT,
                                                  LeafNode::create_integer(32, 0),
                                                  LeafNode::create_integer(32, new_width),
                                                  a->get_expression()));
        retval->defined_by(cur_insn, a->get_defining_instructions());
    } else {
        retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_SEXTEND,
                                                  LeafNode::create_integer(32, new_width),
                                                  a->get_expression()));
        retval->defined_by(cur_insn, a->get_defining_instructions());
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(X86SegmentRegister segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &condition,
                          size_t nbits) {
    assert(1==condition->get_width()); // FIXME: condition is not used
    assert(8==nbits || 16==nbits || 32==nbits);

    // Read the bytes in little endian order
    SValuePtr dflt = svalue_undefined(nbits);
    std::vector<SValuePtr> bytes; // byte read
    InsnSet defs;
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*bytenum, 8*bytenum+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        SValuePtr byte_value = SValue::promote(state->readMemory(byte_addr, byte_dflt, 8, this));
        bytes.push_back(byte_value);
        const InsnSet &definers = byte_value->get_defining_instructions();
        defs.insert(definers.begin(), definers.end());
    }

    // Try to match the pattern of bytes
    //    (extract 0  8 EXPR_0)
    //    (extract 8 16 EXPR_1)
    //    ...
    // where EXPR_i are all structurally identical.
    bool matched = false;
    if (bytes.size()>1) {
        matched = true; // and prove otherwise
        for (size_t bytenum=0; bytenum<bytes.size() && matched; ++bytenum) {
            InternalNodePtr extract = bytes[bytenum]->get_expression()->isInternalNode();
            if (!extract || InsnSemanticsExpr::OP_EXTRACT!=extract->get_operator()) {
                matched = false;
                break;
            }
            LeafNodePtr arg0 = extract->child(0)->isLeafNode();
            LeafNodePtr arg1 = extract->child(1)->isLeafNode();
            if (!arg0 || !arg0->is_known() || arg0->get_value()!=8*bytenum ||
                !arg1 || !arg1->is_known() || arg1->get_value()!=8*(bytenum+1)) {
                matched = false;
                break;
            }
            if (bytenum>0) {
                TreeNodePtr e0 = bytes[0      ]->get_expression()->isInternalNode()->child(2);
                TreeNodePtr ei = bytes[bytenum]->get_expression()->isInternalNode()->child(2);
                matched = e0->equivalent_to(ei);
            }
        }
    }

    // If the bytes match the above pattern, then we can just return (the low order bits of) EXPR_0, otherwise
    // we have to construct a return value by extending and shifting the bytes and bitwise-OR them together.
    SValuePtr retval;
    if (matched) {
        TreeNodePtr e0 = bytes[0]->get_expression()->isInternalNode()->child(2);
        if (e0->get_nbits()==nbits) {
            retval = svalue_expr(e0);
        } else {
            assert(e0->get_nbits()>nbits);
            retval = svalue_expr(InternalNode::create(nbits, InsnSemanticsExpr::OP_EXTRACT,
                                                      LeafNode::create_integer(32, 0),
                                                      LeafNode::create_integer(32, nbits),
                                                      e0));
        }
    } else {
        for (size_t bytenum=0; bytenum<bytes.size(); ++bytenum) { // little endian
            SValuePtr &byte = bytes[bytenum];
            SValuePtr word = SValue::promote(shiftLeft(unsignedExtend(byte, nbits), number_(5, 8*bytenum)));
            retval = 0==bytenum ? word : SValue::promote(or_(retval, word));
        }
    }

    assert(retval!=NULL && retval->get_width()==nbits);
    retval->defined_by(NULL, defs);
    return retval;
}

void
RiscOperators::writeMemory(X86SegmentRegister segreg,
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
