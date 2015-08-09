#include "sage3basic.h"
#include "SymbolicSemantics2.h"
#include "integerOps.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace SymbolicSemantics {

// temporarily disables usedef analysis; restores original value on destruction
class PartialDisableUsedef {
private:
    bool saved_value;
    RiscOperators *ops;
public:
    PartialDisableUsedef(RiscOperators *ops): saved_value(false), ops(ops) {
        saved_value = ops->getset_omit_cur_insn(true);
    }
    ~PartialDisableUsedef() {
        ops->getset_omit_cur_insn(saved_value);
    }
};

/*******************************************************************************************************************************
 *                                      SValue
 *******************************************************************************************************************************/

uint64_t
SValue::get_number() const
{
    LeafNodePtr leaf = expr->isLeafNode();
    ASSERT_not_null(leaf);
    return leaf->get_value();
}

SValuePtr
SValue::substitute(const SValuePtr &from, const SValuePtr &to) const
{
    SValuePtr retval = SValue::promote(copy());
    retval->set_expression(retval->get_expression()->substitute(from->get_expression(), to->get_expression()));
    return retval;
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
    ASSERT_require(get_width()==other->get_width());
    return get_expression()->may_equal(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, SMTSolver *solver) const
{
    SValuePtr other = SValue::promote(other_);
    ASSERT_require(get_width()==other->get_width());
    return get_expression()->must_equal(other->get_expression(), solver);
}

std::string
SValue::get_comment() const
{
    return get_expression()->get_comment();
}

void
SValue::set_comment(const std::string &s) const
{
    get_expression()->set_comment(s);
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter_) const
{
    Formatter *formatter = dynamic_cast<Formatter*>(&formatter_);
    InsnSemanticsExpr::Formatter dflt_expr_formatter;
    InsnSemanticsExpr::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;

    if (defs.empty()) {
        stream <<(*expr + expr_formatter);
    } else {
        stream <<"{defs={";
        size_t ndefs=0;
        for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
            SgAsmInstruction *insn = *di;
            if (insn!=NULL)
                stream <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
        }
        stream <<"}, expr=" <<(*expr+expr_formatter) <<"}";
    }
}
    

/*******************************************************************************************************************************
 *                                      Memory state
 *******************************************************************************************************************************/

MemoryState::CellCompressorChoice MemoryState::cc_choice;

SValuePtr
MemoryState::CellCompressorMcCarthy::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                const CellList &cells)
{
    bool compute_usedef = true;
    if (RiscOperators *addrOpsSymbolic = dynamic_cast<RiscOperators*>(addrOps)) {
        compute_usedef = addrOpsSymbolic->get_compute_usedef();
    } else if (RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps)) {
        compute_usedef = valOpsSymbolic->get_compute_usedef();
    }

    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());
    // FIXME: This makes no attempt to remove duplicate values [Robb Matzke 2013-03-01]
    TreeNodePtr expr = LeafNode::create_memory(address->get_width(), dflt->get_width());
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
    SValuePtr retval = SValue::promote(address->undefined_(8)); // "address" used only to lookup virtual function
    retval->set_expression(InternalNode::create(8, InsnSemanticsExpr::OP_READ, expr, address->get_expression()));
    if (compute_usedef)
        retval->add_defining_instructions(definers);
    return retval;
}

SValuePtr
MemoryState::CellCompressorSimple::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                              BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                              const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());
    return SValue::promote(dflt);
}

SValuePtr
MemoryState::CellCompressorChoice::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                              BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                              const CellList &cells)
{
    if (addrOps->get_solver() || valOps->get_solver())
        return cc_mccarthy(address, dflt, addrOps, valOps, cells);
    return cc_simple(address, dflt, addrOps, valOps, cells);
}

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &address_, const BaseSemantics::SValuePtr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    size_t nbits = dflt->get_width();
    SValuePtr address = SValue::promote(address_);
    ASSERT_require(8==nbits); // SymbolicSemantics::MemoryState assumes that memory cells contain only 8-bit data
    bool short_circuited;
    CellList matches = scan(address, nbits, addrOps, valOps, short_circuited/*out*/);

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists.
    if (!short_circuited) {
        BaseSemantics::MemoryCellPtr tmpcell = protocell->create(address, dflt);
        cells.push_front(tmpcell);
        matches.push_back(tmpcell);
    }

    ASSERT_require(dflt->get_width()==nbits);
    SValuePtr retval = get_cell_compressor()->operator()(address, dflt, addrOps, valOps, matches);
    ASSERT_require(retval->get_width()==8);
    return retval;
}

void
MemoryState::writeMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &value,
                         BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_require(8==value->get_width());
    BaseSemantics::MemoryCellList::writeMemory(address, value, addrOps, valOps);
}

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

void
RiscOperators::substitute(const SValuePtr &from, const SValuePtr &to)
{
    BaseSemantics::StatePtr state = get_state();

    // Substitute in registers
    struct RegSubst: RegisterState::Visitor {
        SValuePtr from, to;
        RegSubst(const SValuePtr &from, const SValuePtr &to): from(from), to(to) {}
        virtual BaseSemantics::SValuePtr operator()(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &val_) {
            SValuePtr val = SValue::promote(val_);
            return val->substitute(from, to);
        }
    } regsubst(from, to);
    RegisterState::promote(state->get_register_state())->traverse(regsubst);

    // Substitute in memory
    struct MemSubst: BaseSemantics::MemoryCellList::Visitor {
        SValuePtr from, to;
        MemSubst(const SValuePtr &from, const SValuePtr &to): from(from), to(to) {}
        virtual void operator()(BaseSemantics::MemoryCellPtr &cell) {
            SValuePtr addr = SValue::promote(cell->get_address());
            cell->set_address(addr->substitute(from, to));
            SValuePtr val = SValue::promote(cell->get_value());
            cell->set_value(val->substitute(from, to));
        }
    } memsubst(from, to);
    MemoryState::promote(state->get_memory_state())->traverse(memsubst);
}

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
    ASSERT_require(a->get_width()==b->get_width());

    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_AND,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_OR,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());

    SValuePtr retval;
    // We leave these simplifications here because InsnSemanticsExpr doesn't yet have a way to pass an SMT solver to its
    // simplifier.
    if (a->is_number() && b->is_number() && a->get_width()<=64) {
        retval = svalue_number(a->get_width(), a->get_number() ^ b->get_number());
    } else if (a->get_expression()->must_equal(b->get_expression(), solver)) {
        retval = svalue_number(a->get_width(), 0);
    } else {
        retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_BV_XOR,
                                                  a->get_expression(), b->get_expression()));
    }
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}
    
BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_INVERT, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->get_width());
    ASSERT_require(begin_bit<end_bit);
    SValuePtr retval = svalue_expr(InternalNode::create(end_bit-begin_bit, InsnSemanticsExpr::OP_EXTRACT,
                                                        LeafNode::create_integer(32, begin_bit),
                                                        LeafNode::create_integer(32, end_bit),
                                                        a->get_expression()));
    if (compute_usedef) {
        if (retval->get_width()==a->get_width()) {
            retval->defined_by(NULL, a->get_defining_instructions());
        } else {
            retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
        }
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &lo_bits_, const BaseSemantics::SValuePtr &hi_bits_)
{
    SValuePtr lo = SValue::promote(lo_bits_);
    SValuePtr hi = SValue::promote(hi_bits_);
    SValuePtr retval = svalue_expr(InternalNode::create(lo->get_width()+hi->get_width(), InsnSemanticsExpr::OP_CONCAT,
                                                        hi->get_expression(), lo->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, lo->get_defining_instructions(), hi->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(1, InsnSemanticsExpr::OP_ZEROP, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel_,
                   const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr sel = SValue::promote(sel_);
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(1==sel->get_width());
    ASSERT_require(a->get_width()==b->get_width());

    SValuePtr retval;
    if (sel->is_number()) {
        retval = SValue::promote(sel->get_number() ? a->copy() : b->copy());
        if (compute_usedef)
            retval->defined_by(omit_cur_insn ? NULL : cur_insn, sel->get_defining_instructions());
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
                retval->defined_by(omit_cur_insn ? NULL : cur_insn, sel->get_defining_instructions());
            return retval;
        }

        // If the selection expression cannot be false, then return a
        assertion = InternalNode::create(1, InsnSemanticsExpr::OP_EQ,
                                         sel->get_expression(), LeafNode::create_integer(1, 0));
        bool can_be_false = SMTSolver::SAT_NO != solver->satisfiable(assertion);
        if (!can_be_false) {
            retval = SValue::promote(a->copy());
            if (compute_usedef)
                retval->defined_by(omit_cur_insn ? NULL : cur_insn, sel->get_defining_instructions());
            return retval;
        }
    }
    retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ITE, sel->get_expression(),
                                              a->get_expression(), b->get_expression()));
    if (compute_usedef) {
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, sel->get_defining_instructions(),
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_MSSB, a->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), sa->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_UEXTEND,
                                                        LeafNode::create_integer(32, new_width),
                                                        a->get_expression()));
    if (compute_usedef) {
        if (retval->get_width()==a->get_width()) {
            retval->defined_by(NULL, a->get_defining_instructions());
        } else {
            retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
        }
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    SValuePtr retval = svalue_expr(InternalNode::create(a->get_width(), InsnSemanticsExpr::OP_ADD,
                                                        a->get_expression(), b->get_expression()));
    if (compute_usedef)
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    ASSERT_require(a->get_width()==b->get_width() && c->get_width()==1);
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
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
        retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions(), b->get_defining_instructions());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr retval = svalue_expr(InternalNode::create(new_width, InsnSemanticsExpr::OP_SEXTEND,
                                                        LeafNode::create_integer(32, new_width),
                                                        a->get_expression()));
    if (compute_usedef) {
        if (retval->get_width()==a->get_width()) {
            retval->defined_by(NULL, a->get_defining_instructions());
        } else {
            retval->defined_by(omit_cur_insn ? NULL : cur_insn, a->get_defining_instructions());
        }
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(const RegisterDescriptor &reg) 
{
    PartialDisableUsedef du(this);
    return BaseSemantics::RiscOperators::readRegister(reg);
}

void
RiscOperators::writeRegister(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_->copy());
    PartialDisableUsedef du(this);
    BaseSemantics::RiscOperators::writeRegister(reg, a);

    // Update latest writer info when appropriate and able to do so.
    if (SgAsmInstruction *insn = get_insn()) {
        RegisterStatePtr regs = boost::dynamic_pointer_cast<RegisterState>(get_state()->get_register_state());
        if (regs!=NULL) {
            switch (compute_regwriters) {
                case TRACK_NO_WRITERS:
                    break;
                case TRACK_LATEST_WRITER:
                    regs->setWriters(reg, insn->get_address());
                    break;
                case TRACK_ALL_WRITERS:
                    regs->insertWriter(reg, insn->get_address());
                    break;
            }
        }
    }
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt,
                          const BaseSemantics::SValuePtr &condition) {
    size_t nbits = dflt->get_width();
    ASSERT_require(0 == nbits % 8);
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
    if (condition->is_number() && !condition->get_number())
        return dflt;

    PartialDisableUsedef du(this);

    // Read the bytes and concatenate them together. InsnSemanticsExpr will simplify the expression so that reading after
    // writing a multi-byte value will return the original value written rather than a concatenation of byte extractions.
    SValuePtr retval;
    InsnSet defs;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr mem = get_state()->get_memory_state();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        SValuePtr byte_value = SValue::promote(state->readMemory(byte_addr, byte_dflt, this, this));
        if (0==bytenum) {
            retval = byte_value;
        } else if (ByteOrder::ORDER_MSB==mem->get_byteOrder()) {
            retval = SValue::promote(concat(byte_value, retval));
        } else if (ByteOrder::ORDER_LSB==mem->get_byteOrder()) {
            retval = SValue::promote(concat(retval, byte_value));
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte read with memory having unspecified byte order", get_insn());
        }
        if (compute_usedef) {
            const InsnSet &definers = byte_value->get_defining_instructions();
            defs.insert(definers.begin(), definers.end());
        }
    }

    ASSERT_require(retval!=NULL && retval->get_width()==nbits);
    if (compute_usedef)
        retval->defined_by(NULL, defs);
    return retval;
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg,
                           const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value_,
                           const BaseSemantics::SValuePtr &condition) {
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
    if (condition->is_number() && !condition->get_number())
        return;
    SValuePtr value = SValue::promote(value_->copy());
    PartialDisableUsedef du(this);
    size_t nbits = value->get_width();
    ASSERT_require(0 == nbits % 8);
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr mem = get_state()->get_memory_state();
    for (size_t bytenum=0; bytenum<nbytes; ++bytenum) {
        size_t byteOffset = 0;
        if (1 == nbytes) {
            // void
        } else if (ByteOrder::ORDER_MSB == mem->get_byteOrder()) {
            byteOffset = nbytes - (bytenum+1);
        } else if (ByteOrder::ORDER_LSB == mem->get_byteOrder()) {
            byteOffset = bytenum;
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte write with memory having unspecified byte order", get_insn());
        }

        BaseSemantics::SValuePtr byte_value = extract(value, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        state->writeMemory(byte_addr, byte_value, this, this);

        // Update the latest writer info if we have a current instruction and the memory state supports it.
        if (compute_memwriters) {
            if (SgAsmInstruction *insn = get_insn()) {
                if (BaseSemantics::MemoryCellListPtr cells = boost::dynamic_pointer_cast<BaseSemantics::MemoryCellList>(mem)) {
                    BaseSemantics::MemoryCellPtr cell = cells->get_latest_written_cell();
                    ASSERT_not_null(cell); // we just wrote to it!
                    cell->latestWriter(insn->get_address());
                }
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
