#include "sage3basic.h"
#include "MemoryCellList.h"
#include "SymbolicSemantics2.h"
#include "integerOps.h"

namespace Rose {
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SValue::isBottom() const {
    return 0 != (get_expression()->flags() & SymbolicExpr::Node::BOTTOM);
}

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValuePtr &other_, const BaseSemantics::MergerPtr &merger_,
                            const SmtSolverPtr &solver) const {
    SValuePtr other = SValue::promote(other_);
    ASSERT_require(get_width() == other->get_width());
    MergerPtr merger = merger_.dynamicCast<Merger>();
    bool changed = false;
    unsigned mergedFlags = get_expression()->flags() | other->get_expression()->flags();
    SValuePtr retval = SValue::promote(copy());
    retval->set_expression(retval->get_expression()->newFlags(mergedFlags));

    // If one or the other is BOTTOM then the return value should be BOTTOM.  Some simplifications (like "xor x x => 0") might
    // cause the BOTTOM flag to be removed, and we can't have that, so make sure BOTTOM stays set!
    if (isBottom())
        return Sawyer::Nothing(); // no change
    if (other->isBottom())
        return bottom_(get_width());

    // If the values are different types then return bottom.
    if (get_expression()->type() != other->get_expression()->type())
        return bottom_(get_width());

    // Merge symbolic expressions. The merge of x and y is the set {x, y}. If the size of this set is greater than the set size
    // limit (or 1 if merger is null) then the result is bottom.  Normal set simplifcations happen first (e.g., {x, x} => {x}
    // => x).
    if (!get_expression()->mustEqual(other->get_expression(), solver)) {
        ExprPtr expr = SymbolicExpr::makeSet(get_expression(), other->get_expression(), solver, "", mergedFlags);
        size_t exprSetSize = expr->isInteriorNode() && expr->isInteriorNode()->getOperator()==SymbolicExpr::OP_SET ?
                             expr->isInteriorNode()->nChildren() : (size_t)1;
        size_t setSizeLimit = merger ? merger->setSizeLimit() : (size_t)1;
        if (exprSetSize > setSizeLimit) {
            expr = SymbolicExpr::makeVariable(retval->get_expression()->type());
            mergedFlags |= SymbolicExpr::Node::BOTTOM;
        }
        retval->set_expression(expr);
        changed = true;
    }

    // Merge flags.
    if (get_expression()->flags() != mergedFlags) {
        retval->set_expression(retval->get_expression()->newFlags(mergedFlags));
        changed = true;
    }

    // Merge definers
    InsnSet mergedDefiners = retval->get_defining_instructions();
    InsnSet otherDefiners = other->get_defining_instructions();
    BOOST_FOREACH (SgAsmInstruction *definer, otherDefiners) {
        if (mergedDefiners.insert(definer).second)
            changed = true;
    }
    retval->add_defining_instructions(mergedDefiners);

    return changed ? Sawyer::Optional<BaseSemantics::SValuePtr>(retval) : Sawyer::Nothing();
}

uint64_t
SValue::get_number() const
{
    LeafPtr leaf = expr->isLeafNode();
    ASSERT_not_null(leaf);
    return leaf->toUnsigned().get();
}

SValuePtr
SValue::substitute(const SValuePtr &from, const SValuePtr &to, const SmtSolverPtr &solver) const
{
    SValuePtr retval = SValue::promote(copy());
    retval->set_expression(retval->get_expression()->substitute(from->get_expression(), to->get_expression(), solver));
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
SValue::may_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const
{
    SValuePtr other = SValue::promote(other_);
    if (get_width() != other->get_width())
        return false;
    if (isBottom() || other->isBottom())
        return true;
    return get_expression()->mayEqual(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const
{
    SValuePtr other = SValue::promote(other_);
    if (get_width() != other->get_width())
        return false;
    if (isBottom() || other->isBottom())
        return false;
    return get_expression()->mustEqual(other->get_expression(), solver);
}

std::string
SValue::get_comment() const
{
    return get_expression()->comment();
}

void
SValue::set_comment(const std::string &s) const
{
    get_expression()->comment(s);
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter_) const
{
    Formatter *formatter = dynamic_cast<Formatter*>(&formatter_);
    SymbolicExpr::Formatter dflt_expr_formatter;
    SymbolicExpr::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;
    std::string closing;

    if (!defs.empty()) {
        stream <<"{defs={";
        size_t ndefs=0;
        for (InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di, ++ndefs) {
            SgAsmInstruction *insn = *di;
            if (insn!=NULL)
                stream <<(ndefs>0?",":"") <<StringUtility::addrToString(insn->get_address());
        }
        stream <<"}, expr=";
        closing = "}";
    }

    if (isBottom()) {
        stream <<"BOTTOM (";
        closing = ")" + closing;
    }

    stream <<(*expr + expr_formatter) <<closing;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      List-base Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryListState::CellCompressorChoice MemoryListState::cc_choice;

SValuePtr
MemoryListState::CellCompressorMcCarthy::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                    BaseSemantics::RiscOperators *addrOps,
                                                    BaseSemantics::RiscOperators *valOps, const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());

    RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps);
    ASSERT_not_null(valOpsSymbolic);
    DefinersMode valDefinersMode = valOpsSymbolic->computingDefiners();

    // FIXME: This makes no attempt to remove duplicate values [Robb Matzke 2013-03-01]
    ExprPtr expr = SymbolicExpr::makeMemoryVariable(address->get_width(), dflt->get_width());
    InsnSet addrDefiners, valDefiners;
    for (CellList::const_reverse_iterator ci=cells.rbegin(); ci!=cells.rend(); ++ci) {
        SValuePtr cell_addr = SValue::promote((*ci)->get_address());
        SValuePtr cell_value  = SValue::promote((*ci)->get_value());
        expr = SymbolicExpr::makeWrite(expr, cell_addr->get_expression(), cell_value->get_expression(), valOps->solver());
        if (valDefinersMode != TRACK_NO_DEFINERS) {
            const InsnSet &definers = cell_value->get_defining_instructions();
            valDefiners.insert(definers.begin(), definers.end());
        }
    }
    SValuePtr retval = SValue::promote(valOps->undefined_(dflt->get_width()));
    retval->set_expression(SymbolicExpr::makeRead(expr, address->get_expression(), valOps->solver()));
    retval->set_defining_instructions(valDefiners);
    return retval;
}

SValuePtr
MemoryListState::CellCompressorSimple::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                  const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->get_value()->copy());
    return SValue::promote(dflt);
}

SValuePtr
MemoryListState::CellCompressorChoice::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                  const CellList &cells)
{
    if (addrOps->solver() || valOps->solver())
        return cc_mccarthy(address, dflt, addrOps, valOps, cells);
    return cc_simple(address, dflt, addrOps, valOps, cells);
}

BaseSemantics::SValuePtr
MemoryListState::readOrPeekMemory(const BaseSemantics::SValuePtr &address_, const BaseSemantics::SValuePtr &dflt,
                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                  bool allowSideEffects) {
    size_t nBits = dflt->get_width();
    SValuePtr address = SValue::promote(address_);
    ASSERT_require(8==nBits); // SymbolicSemantics::MemoryListState assumes that memory cells contain only 8-bit data

    CellList::iterator cursor = get_cells().begin();
    CellList cells = scan(cursor /*in,out*/, address, nBits, addrOps, valOps);

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists. If
    // side effects are allowed, we should add a new cell to the return value.
    if (cursor == get_cells().end()) {
        if (allowSideEffects) {
            BaseSemantics::MemoryCellPtr newCell = insertReadCell(address, dflt);
            cells.push_back(newCell);
        } else {
            BaseSemantics::MemoryCellPtr newCell = protocell->create(address, dflt);
            cells.push_back(newCell);
        }
    }

    // If we're doing an actual read (rather than just a peek), then update the returned cells to indicate that they've been
    // read. Since the "cells" vector is pointers that haven't been deep-copied, this is a side effect on the memory
    // state. But even if it weren't a side effect, we don't want the returned value to be marked as having been actually
    // read when we're only peeking.
    if (allowSideEffects)
        updateReadProperties(cells);

    SValuePtr retval = get_cell_compressor()->operator()(address, dflt, addrOps, valOps, cells);
    ASSERT_require(retval->get_width()==8);
    return retval;
}

BaseSemantics::SValuePtr
MemoryListState::readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, true /*allow side effects*/);
}

BaseSemantics::SValuePtr
MemoryListState::peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, false /*no side effects allowed*/);
}

void
MemoryListState::writeMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_require(8==value->get_width());
    BaseSemantics::MemoryCellList::writeMemory(address, value, addrOps, valOps);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Map-based Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::MemoryCellMap::CellKey
MemoryMapState::generateCellKey(const BaseSemantics::SValuePtr &addr_) const {
    SValuePtr addr = SValue::promote(addr_);
    return addr->get_expression()->hash();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::substitute(const SValuePtr &from, const SValuePtr &to)
{
    BaseSemantics::StatePtr state = currentState();

    // Substitute in registers
    struct RegSubst: RegisterState::Visitor {
        SValuePtr from, to;
        SmtSolverPtr solver;
        RegSubst(const SValuePtr &from, const SValuePtr &to, const SmtSolverPtr &solver)
            : from(from), to(to), solver(solver) {}
        virtual BaseSemantics::SValuePtr operator()(RegisterDescriptor reg, const BaseSemantics::SValuePtr &val_) {
            SValuePtr val = SValue::promote(val_);
            return val->substitute(from, to, solver);
        }
    } regsubst(from, to, solver());
    RegisterState::promote(state->registerState())->traverse(regsubst);

    // Substitute in memory
    struct MemSubst: BaseSemantics::MemoryCell::Visitor {
        SValuePtr from, to;
        SmtSolverPtr solver;
        MemSubst(const SValuePtr &from, const SValuePtr &to, const SmtSolverPtr &solver)
            : from(from), to(to), solver(solver) {}
        virtual void operator()(BaseSemantics::MemoryCellPtr &cell) {
            SValuePtr addr = SValue::promote(cell->get_address());
            cell->set_address(addr->substitute(from, to, solver));
            SValuePtr val = SValue::promote(cell->get_value());
            cell->set_value(val->substitute(from, to, solver));
        }
    } memsubst(from, to, solver());
    MemoryState::promote(state->memoryState())->traverse(memsubst);
}

BaseSemantics::SValuePtr
RiscOperators::filterResult(const BaseSemantics::SValuePtr &a_) {
    if (trimThreshold_ == 0)
        return a_;
    SValuePtr a = SValue::promote(a_);
    if (a->get_expression()->nNodes() <= trimThreshold_)
        return a_;

    SymbolicExpr::Ptr expr = SymbolicExpr::makeVariable(a->get_expression()->type(),
                                                        a->get_expression()->comment(),
                                                        a->get_expression()->flags());
    a->set_expression(expr);
    return a;
}

void
RiscOperators::interrupt(int majr, int minr)
{
    currentState()->clear();
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeAnd(a->get_expression(), b->get_expression(), solver()));

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeOr(a->get_expression(), b->get_expression(), solver()));

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval;
    // We leave these simplifications here because SymbolicExpr doesn't yet have a way to pass an SMT solver to its
    // simplifier.
    if (a->is_number() && b->is_number() && a->get_width()<=64) {
        retval = svalue_number(a->get_width(), a->get_number() ^ b->get_number());
    } else if (a->get_expression()->mustEqual(b->get_expression(), solver())) {
        retval = svalue_number(a->get_width(), 0);
    } else {
        retval = svalue_expr(SymbolicExpr::makeXor(a->get_expression(), b->get_expression(), solver()));
    }

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->get_width()));
    SValuePtr retval = svalue_expr(SymbolicExpr::makeInvert(a->get_expression(), solver()));

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->get_width());
    ASSERT_require(begin_bit<end_bit);
    if (a->isBottom())
        return filterResult(bottom_(end_bit-begin_bit));

    SymbolicExpr::Ptr beginExpr = SymbolicExpr::makeIntegerConstant(32, begin_bit);
    SymbolicExpr::Ptr endExpr = SymbolicExpr::makeIntegerConstant(32, end_bit);
    SValuePtr retval = svalue_expr(SymbolicExpr::makeExtract(beginExpr, endExpr, a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            if (retval->get_width() == a->get_width())
                retval->add_defining_instructions(a);   // preserve definers if this extract is a no-op
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // old definers and...
            if (retval->get_width() != a->get_width())  // ...new definer but only if this extract is not a no-op
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
        case TRACK_LATEST_DEFINER:
            if (retval->get_width() != a->get_width()) {
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            } else {
                retval->add_defining_instructions(a);   // preserve definers if this extract is a no-op
            }
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &lo_bits_, const BaseSemantics::SValuePtr &hi_bits_)
{
    SValuePtr lo = SValue::promote(lo_bits_);
    SValuePtr hi = SValue::promote(hi_bits_);
    if (lo->isBottom() || hi->isBottom())
        return filterResult(bottom_(lo->get_width() + hi->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeConcat(hi->get_expression(), lo->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(lo);
            retval->add_defining_instructions(hi);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(1));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeZerop(a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
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

    // (ite bottom A B) should be A when A==B. However, SymbolicExpr would have already simplified that to A.
    if (sel->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval;
    if (sel->is_number()) {
        retval = SValue::promote(sel->get_number() ? a->copy() : b->copy());
        switch (computingDefiners_) {
            case TRACK_NO_DEFINERS:
                break;
            case TRACK_ALL_DEFINERS:
#if 0 // [Robb P. Matzke 2015-09-17]: not present in original version
                retval->add_defining_instructions(sel->get_number() ? a : b);
#endif
                retval->add_defining_instructions(sel); // fall through...
            case TRACK_LATEST_DEFINER:
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
                break;
        }
        return filterResult(retval);
    }
    if (solver()) {
        // If the selection expression cannot be true, then return b
        {
            SmtSolver::Transaction transaction(solver());
            ExprPtr condition = sel->get_expression();
            solver()->insert(condition);
            bool can_be_true = SmtSolver::SAT_NO != solver()->check();
            if (!can_be_true) {
                retval = SValue::promote(b->copy());
                switch (computingDefiners_) {
                    case TRACK_NO_DEFINERS:
                        break;
                    case TRACK_ALL_DEFINERS:
#if 0 // [Robb P. Matzke 2015-09-17]: not present in original version
                        retval->add_defining_instructions(b);
#endif
                        retval->add_defining_instructions(sel); // fall through...
                    case TRACK_LATEST_DEFINER:
                        retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
                        break;
                }
                return filterResult(retval);
            }
        }

        // If the selection expression cannot be false, then return a
        {
            SmtSolver::Transaction transaction(solver());
            ExprPtr inverseCondition = SymbolicExpr::makeInvert(sel->get_expression(), solver());
            solver()->insert(inverseCondition);
            bool can_be_false = SmtSolver::SAT_NO != solver()->check();
            if (!can_be_false) {
                retval = SValue::promote(a->copy());
                switch (computingDefiners_) {
                    case TRACK_NO_DEFINERS:
                        break;
                    case TRACK_ALL_DEFINERS:
#if 0 // [Robb P. Matzke 2015-09-17]: not present in original version
                        retval->add_defining_instructions(a);
#endif
                        retval->add_defining_instructions(sel); // fall through...
                    case TRACK_LATEST_DEFINER:
                        retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
                        break;
                }
                return filterResult(retval);
            }
        }
    }

    retval = svalue_expr(SymbolicExpr::makeIte(sel->get_expression(), a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);
            retval->add_defining_instructions(sel); // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeLssb(a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeMssb(a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeRol(sa->get_expression(), a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(sa);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeRor(sa->get_expression(), a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(sa);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeShl0(sa->get_expression(), a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(sa);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeShr0(sa->get_expression(), a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(sa);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeAsr(sa->get_expression(), a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(sa);      // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(new_width));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, new_width),
                                                            a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->get_width() != a->get_width())
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeAdd(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    ASSERT_require(a->get_width()==b->get_width() && c->get_width()==1);
    if (a->isBottom() || b->isBottom() || c->isBottom()) {
        carry_out = bottom_(a->get_width());
        return filterResult(bottom_(a->get_width()));
    }
    BaseSemantics::SValuePtr aa = unsignedExtend(a, a->get_width()+1);
    BaseSemantics::SValuePtr bb = unsignedExtend(b, a->get_width()+1);
    BaseSemantics::SValuePtr cc = unsignedExtend(c, a->get_width()+1);
    BaseSemantics::SValuePtr sumco = add(aa, add(bb, cc));
    carry_out = filterResult(extract(xor_(aa, xor_(bb, sumco)), 1, a->get_width()+1));
    return filterResult(add(a, add(b, unsignedExtend(c, a->get_width()))));
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeNegate(a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeSignedDiv(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(b->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeSignedMod(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));
    SValuePtr retval = svalue_expr(SymbolicExpr::makeSignedMul(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeDiv(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(b->get_width()));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeMod(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeMul(a->get_expression(), b->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);
            retval->add_defining_instructions(b);       // fall through...
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(new_width));

    SValuePtr retval = svalue_expr(SymbolicExpr::makeSignExtend(SymbolicExpr::makeIntegerConstant(32, new_width),
                                                                a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->get_width() != a->get_width())
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(retval);
}

SgAsmFloatType*
RiscOperators::sgIsIeee754(SgAsmType *sgType) {
    ASSERT_not_null(sgType);
    SgAsmFloatType *fpType = isSgAsmFloatType(sgType);
    if (NULL == fpType)
        return NULL;
    if (fpType->significandBits().least() != 0)
        return NULL;
    if (fpType->significandBits().size() == 0)
        return NULL;
    if (fpType->exponentBits().least() != fpType->significandBits().greatest()+1)
        return NULL;
    if (fpType->exponentBits().size() < 2)
        return NULL;
    if (fpType->signBit() != fpType->exponentBits().greatest()+1)
        return NULL;
    if (fpType->signBit() != fpType->get_nBits() - 1)
        return NULL;
    if (fpType->exponentBias() != ((uint64_t)1 << (fpType->exponentBits().size() - 1)) - 1)
        return NULL;
    return fpType;
}

SymbolicExpr::Type
RiscOperators::sgTypeToSymbolicType(SgAsmType *sgType) {
    ASSERT_not_null(sgType);
    if (SgAsmFloatType *fpType = sgIsIeee754(sgType)) {
        return SymbolicExpr::Type::floatingPoint(fpType->exponentBits().size(), fpType->significandBits().size());
    } else if (SgAsmIntegerType *iType = isSgAsmIntegerType(sgType)) {
        return SymbolicExpr::Type::integer(iType->get_nBits());
    } else {
        throw Exception("cannot convert Sage type to symbolic type");
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpConvert(const BaseSemantics::SValuePtr &a_, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    SValuePtr a = SValue::promote(a_);
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_not_null(retType);

    SymbolicExpr::Type srcType = sgTypeToSymbolicType(aType);
    ASSERT_require(a->get_expression()->type() == srcType);
    SymbolicExpr::Type dstType = sgTypeToSymbolicType(retType);

    BaseSemantics::SValuePtr result;
    if (srcType == dstType) {
        result = a->copy();
    } else {
        result = svalue_expr(SymbolicExpr::makeConvert(a->get_expression(), dstType, solver()));
    }
    ASSERT_not_null(result);
    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::reinterpret(const BaseSemantics::SValuePtr &a_, SgAsmType *retType) {
    SValuePtr a = SValue::promote(a_);
    ASSERT_not_null(a);
    ASSERT_not_null(retType);
    SymbolicExpr::Type srcType = a->get_expression()->type();
    SymbolicExpr::Type dstType = sgTypeToSymbolicType(retType);
    if (srcType.nBits() != dstType.nBits()) {
        throw Exception("reinterpret type (" + dstType.toString() + ") is not the same size as the value type (" +
                        srcType.toString() + ")");
    }

    BaseSemantics::SValuePtr result = svalue_expr(SymbolicExpr::makeReinterpret(a->get_expression(), dstType, solver()));
    ASSERT_not_null(result);
    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt)
{
    PartialDisableUsedef du(this);
    SValuePtr result = SValue::promote(BaseSemantics::RiscOperators::readRegister(reg, dflt));

    if (currentInstruction()) {
        RegisterStatePtr regs = RegisterState::promote(currentState()->registerState());
        regs->updateReadProperties(reg);
    }

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
        case TRACK_LATEST_DEFINER:
            result->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }

    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    PartialDisableUsedef du(this);
    BaseSemantics::SValuePtr result = BaseSemantics::RiscOperators::peekRegister(reg, dflt);
    ASSERT_require(result!=NULL && result->get_width() == reg.nBits());
    return filterResult(result);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_->copy());
    PartialDisableUsedef du(this);
    BaseSemantics::RiscOperators::writeRegister(reg, a);

    // Update register properties and writer info.
    RegisterStatePtr regs = RegisterState::promote(currentState()->registerState());
    SgAsmInstruction *insn = currentInstruction();
    if (insn) {
        switch (computingRegisterWriters()) {
            case TRACK_NO_WRITERS:
                break;
            case TRACK_LATEST_WRITER:
                regs->setWriters(reg, insn->get_address());
                break;
            case TRACK_ALL_WRITERS:
                regs->insertWriters(reg, insn->get_address());
                break;
        }
        regs->updateWriteProperties(reg, (insn ? BaseSemantics::IO_WRITE : BaseSemantics::IO_INIT));
    }
}

BaseSemantics::SValuePtr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg,
                                const BaseSemantics::SValuePtr &address,
                                const BaseSemantics::SValuePtr &dflt,
                                bool allowSideEffects) {
    size_t nbits = dflt->get_width();
    ASSERT_require(0 == nbits % 8);
    if (address->isBottom())
        return filterResult(bottom_(dflt->get_width()));

    PartialDisableUsedef du(this);

    // Read the bytes and concatenate them together. SymbolicExpr will simplify the expression so that reading after
    // writing a multi-byte value will return the original value written rather than a concatenation of byte extractions.
    SValuePtr retval;
    InsnSet allDefiners;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr currentMem = currentState()->memoryState();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==currentMem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));

        // Read the default value from the initial memory state first. We want to use whatever value is in the initial memory
        // state if the address is not present in the current memory state. As a side effect, if this value is not in the
        // initial memory it will be added.
        if (initialState()) {
            if (allowSideEffects) {
                byte_dflt = initialState()->readMemory(byte_addr, byte_dflt, this, this);
            } else {
                byte_dflt = initialState()->peekMemory(byte_addr, byte_dflt, this, this);
            }
        }

        // Read a byte from the current memory state. Adds the new value as a side effect if necessary.
        SValuePtr byte_value;
        if (allowSideEffects) {
            byte_value = SValue::promote(currentState()->readMemory(byte_addr, byte_dflt, this, this));
        } else {
            byte_value = SValue::promote(currentState()->peekMemory(byte_addr, byte_dflt, this, this));
        }
        if (0==bytenum) {
            retval = byte_value;
        } else if (ByteOrder::ORDER_MSB==currentMem->get_byteOrder()) {
            retval = SValue::promote(concat(byte_value, retval));
        } else if (ByteOrder::ORDER_LSB==currentMem->get_byteOrder()) {
            retval = SValue::promote(concat(retval, byte_value));
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte read with memory having unspecified byte order", currentInstruction());
        }

        // Accumulating the bytes of a multibyte read is sort of like a merge operation in the way it treats definers. Since
        // we're returning a value that's composed of multiple bytes, the latest definers for the reutrn value are the union of
        // the latest definers for the individual bytes. Similarly, for TRACK_ALL_DEFINERS, the set of all definers for the
        // return value is the union of the all-definers sets of the individual bytes.
        if (computingDefiners() == TRACK_ALL_DEFINERS || computingDefiners() == TRACK_LATEST_DEFINER) {
            const InsnSet &byteDefiners = byte_value->get_defining_instructions();
            allDefiners.insert(byteDefiners.begin(), byteDefiners.end());
        }
    }

    ASSERT_require(retval!=NULL && retval->get_width()==nbits);
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            retval->add_defining_instructions(allDefiners);
            break;
    }
    return filterResult(retval);
}


BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt,
                          const BaseSemantics::SValuePtr &condition) {
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
    if (condition->is_number() && !condition->get_number())
        return filterResult(dflt);
    return readOrPeekMemory(segreg, address, dflt, true /*allow side effects*/);
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt) {
    return readOrPeekMemory(segreg, address, dflt, false /*no side effects allowed*/);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg,
                           const BaseSemantics::SValuePtr &address_,
                           const BaseSemantics::SValuePtr &value_,
                           const BaseSemantics::SValuePtr &condition) {
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
    if (condition->is_number() && !condition->get_number())
        return;
    if (address_->isBottom())
        return;
    SValuePtr address = SValue::promote(address_);
    SValuePtr value = SValue::promote(value_);
    PartialDisableUsedef du(this);
    size_t nbits = value->get_width();
    ASSERT_require(0 == nbits % 8);
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr mem = currentState()->memoryState();
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
            throw BaseSemantics::Exception("multi-byte write with memory having unspecified byte order", currentInstruction());
        }

        SValuePtr byte_value = SValue::promote(extract(value, 8*byteOffset, 8*byteOffset+8));
        byte_value->add_defining_instructions(value);
        SValuePtr byte_addr = SValue::promote(add(address, number_(address->get_width(), bytenum)));
        byte_addr->add_defining_instructions(address);
        currentState()->writeMemory(byte_addr, byte_value, this, this);

        // Update the latest writer info if we have a current instruction and the memory state supports it.
        if (computingMemoryWriters() != TRACK_NO_WRITERS) {
            if (SgAsmInstruction *insn = currentInstruction()) {
                if (BaseSemantics::MemoryCellListPtr cellList =
                    boost::dynamic_pointer_cast<BaseSemantics::MemoryCellList>(mem)) {
                    if (BaseSemantics::MemoryCellPtr cell = cellList->latestWrittenCell()) {
                        switch (computingMemoryWriters()) {
                            case TRACK_NO_WRITERS:
                                break;
                            case TRACK_LATEST_WRITER:
                                cell->setWriter(insn->get_address());
                                break;
                            case TRACK_ALL_WRITERS:
                                cell->insertWriter(insn->get_address());
                                break;
                        }
                    }
                } else if (BaseSemantics::MemoryCellMapPtr cellMap =
                           boost::dynamic_pointer_cast<BaseSemantics::MemoryCellMap>(mem)) {
                    if (BaseSemantics::MemoryCellPtr cell = cellMap->latestWrittenCell()) {
                        switch (computingMemoryWriters()) {
                            case TRACK_NO_WRITERS:
                                break;
                            case TRACK_LATEST_WRITER:
                                cell->setWriter(insn->get_address());
                                break;
                            case TRACK_ALL_WRITERS:
                                cell->insertWriter(insn->get_address());
                                break;
                        }
                    }
                }
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValue);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::MemoryListState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::MemoryMapState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators);
#endif
