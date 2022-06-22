#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>

#include <integerOps.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <SageBuilderAsm.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
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
    ASSERT_require(nBits() == other->nBits());
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
        return bottom_(nBits());

    // If the values are different types then return bottom.
    if (get_expression()->type() != other->get_expression()->type())
        return bottom_(nBits());

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
    for (SgAsmInstruction *definer: otherDefiners) {
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
    if (nBits() != other->nBits())
        return false;
    if (isBottom() || other->isBottom())
        return true;
    return get_expression()->mayEqual(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const
{
    SValuePtr other = SValue::promote(other_);
    if (nBits() != other->nBits())
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
SValue::hash(Combinatorics::Hasher &hasher) const {
    if (expr) {
        // SymbolicExpr doesn't allow a hasher to be specified because of the way it caches hashes for performance for very
        // large expressions with a high degree of sharing and parallelism.
        hasher.insert(expr->hash());
    }
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

// class method
MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorMcCarthy::instance() {
    return Ptr(new CellCompressorMcCarthy);
}

SValuePtr
MemoryListState::CellCompressorMcCarthy::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                    BaseSemantics::RiscOperators *addrOps,
                                                    BaseSemantics::RiscOperators *valOps, const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->value()->copy());

    RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps);
    ASSERT_not_null(valOpsSymbolic);
    DefinersMode valDefinersMode = valOpsSymbolic->computingDefiners();

    // FIXME: This makes no attempt to remove duplicate values [Robb Matzke 2013-03-01]
    ExprPtr expr = SymbolicExpr::makeMemoryVariable(address->nBits(), dflt->nBits());
    InsnSet addrDefiners, valDefiners;
    for (CellList::const_reverse_iterator ci=cells.rbegin(); ci!=cells.rend(); ++ci) {
        SValuePtr cell_addr = SValue::promote((*ci)->address());
        SValuePtr cell_value  = SValue::promote((*ci)->value());
        expr = SymbolicExpr::makeWrite(expr, cell_addr->get_expression(), cell_value->get_expression(), valOps->solver());
        if (valDefinersMode != TRACK_NO_DEFINERS) {
            const InsnSet &definers = cell_value->get_defining_instructions();
            valDefiners.insert(definers.begin(), definers.end());
        }
    }
    SValuePtr retval = SValue::promote(valOps->undefined_(dflt->nBits()));
    retval->set_expression(SymbolicExpr::makeRead(expr, address->get_expression(), valOps->solver()));
    retval->set_defining_instructions(valDefiners);
    return retval;
}

// class method
MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorSimple::instance() {
    return Ptr(new CellCompressorSimple);
}

SValuePtr
MemoryListState::CellCompressorSimple::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                  const CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->value()->copy());
    return SValue::promote(dflt);
}

MemoryListState::CellCompressorChoice::CellCompressorChoice()
    : mccarthy_(CellCompressorMcCarthy::instance()), simple_(CellCompressorSimple::instance()) {}

MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorChoice::instance() {
    return Ptr(new CellCompressorChoice);
}

SValuePtr
MemoryListState::CellCompressorChoice::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                  const CellList &cells)
{
    if (addrOps->solver() || valOps->solver()) {
        ASSERT_not_null(mccarthy_);
        return (*mccarthy_)(address, dflt, addrOps, valOps, cells);
    } else {
        ASSERT_not_null(simple_);
        return (*simple_)(address, dflt, addrOps, valOps, cells);
    }
}

MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorSet::instance() {
    return Ptr(new CellCompressorSet);
}

SValuePtr
MemoryListState::CellCompressorSet::operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                               const CellList &cells) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);

    RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps);
    const DefinersMode valDefinersMode = valOpsSymbolic->computingDefiners();
    InsnSet valDefiners;
    SymbolicExpr::Ptr first, set;
    std::cerr <<"ROBB: CellCompressorSet for " <<StringUtility::plural(cells.size(), "cells") <<"\n";
    for (const BaseSemantics::MemoryCellPtr &cell: cells) {
        SValuePtr cellValue = SValue::promote(cell->value());
        SymbolicExpr::Ptr cur = cellValue->get_expression();
        if (!first) {
            first = cur;
        } else if (!set) {
            ASSERT_not_null(first);
            set = SymbolicExpr::makeSet(first, cur);
        } else {
            set = SymbolicExpr::makeSet(set, cur);
        }

        if (valDefinersMode != TRACK_NO_DEFINERS) {
            const InsnSet &definers = cellValue->get_defining_instructions();
            valDefiners.insert(definers.begin(), definers.end());
        }
    }
    if (first && !set)
        set = first;

    if (set) {
        std::cerr <<"        result = " <<*set <<"\n";
        ASSERT_require(dflt->nBits() == set->nBits());
        SValuePtr retval = SValue::promote(valOps->undefined_(dflt->nBits()));
        retval->set_expression(set);
        retval->set_defining_instructions(valDefiners);
        return retval;
    } else {
        std::cerr <<"        result (dflt) = " <<*dflt <<"\n";
        return SValue::promote(dflt);
    }
}

MemoryListState::CellCompressor::Ptr
MemoryListState::cellCompressor() const {
    return cellCompressor_;
}

void
MemoryListState::cellCompressor(const CellCompressor::Ptr &cc) {
    ASSERT_not_null(cc);
    cellCompressor_ = cc;
}

// deprecated [Robb Matzke 2021-12-15]
MemoryListState::CellCompressor::Ptr
MemoryListState::get_cell_compressor() const {
    return cellCompressor();
}

// deprecated [Robb Matzke 2021-12-15]
void
MemoryListState::set_cell_compressor(const CellCompressor::Ptr &cc) {
    return cellCompressor(cc);
}

BaseSemantics::SValuePtr
MemoryListState::readOrPeekMemory(const BaseSemantics::SValuePtr &address_, const BaseSemantics::SValuePtr &dflt,
                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                  AllowSideEffects::Flag allowSideEffects) {
    size_t nBits = dflt->nBits();
    SValuePtr address = SValue::promote(address_);
    ASSERT_require(8==nBits); // SymbolicSemantics::MemoryListState assumes that memory cells contain only 8-bit data

    CellList::iterator cursor = get_cells().begin();
    CellList cells = scan(cursor /*in,out*/, address, nBits, addrOps, valOps);

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists. If
    // side effects are allowed, we should add a new cell to the return value.
    if (cursor == get_cells().end()) {
        if (AllowSideEffects::YES == allowSideEffects) {
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
    if (AllowSideEffects::YES == allowSideEffects)
        updateReadProperties(cells);

    SValuePtr retval = cellCompressor()->operator()(address, dflt, addrOps, valOps, cells);
    ASSERT_require(retval->nBits()==8);
    return retval;
}

BaseSemantics::SValuePtr
MemoryListState::readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, AllowSideEffects::YES);
}

BaseSemantics::SValuePtr
MemoryListState::peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, AllowSideEffects::NO);
}

void
MemoryListState::writeMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_require(8==value->nBits());
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
            SValuePtr addr = SValue::promote(cell->address());
            cell->address(addr->substitute(from, to, solver));
            SValuePtr val = SValue::promote(cell->value());
            cell->value(val->substitute(from, to, solver));
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
    ++nTrimmed_;
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
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeAnd(a->get_expression(), b->get_expression(), solver()));

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
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeOr(a->get_expression(), b->get_expression(), solver()));

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
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval;
    // We leave these simplifications here because SymbolicExpr doesn't yet have a way to pass an SMT solver to its
    // simplifier.
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        retval = svalueNumber(a->nBits(), *aNum ^ *bNum);
    } else if (a->get_expression()->mustEqual(b->get_expression(), solver())) {
        retval = svalueNumber(a->nBits(), 0);
    } else {
        retval = svalueExpr(SymbolicExpr::makeXor(a->get_expression(), b->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));
    SValuePtr retval = svalueExpr(SymbolicExpr::makeInvert(a->get_expression(), solver()));

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
    ASSERT_require(end_bit<=a->nBits());
    ASSERT_require(begin_bit<end_bit);
    if (a->isBottom())
        return filterResult(bottom_(end_bit-begin_bit));

    SymbolicExpr::Ptr beginExpr = SymbolicExpr::makeIntegerConstant(32, begin_bit);
    SymbolicExpr::Ptr endExpr = SymbolicExpr::makeIntegerConstant(32, end_bit);
    SValuePtr retval = svalueExpr(SymbolicExpr::makeExtract(beginExpr, endExpr, a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            if (retval->nBits() == a->nBits())
                retval->add_defining_instructions(a);   // preserve definers if this extract is a no-op
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // old definers and...
            if (retval->nBits() != a->nBits())  // ...new definer but only if this extract is not a no-op
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
        case TRACK_LATEST_DEFINER:
            if (retval->nBits() != a->nBits()) {
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
        return filterResult(bottom_(lo->nBits() + hi->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeConcat(hi->get_expression(), lo->get_expression(), solver()));
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

    SValuePtr retval = svalueExpr(SymbolicExpr::makeZerop(a->get_expression(), solver()));
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
    ASSERT_require(1==sel->nBits());
    ASSERT_require(a->nBits()==b->nBits());

    // (ite bottom A B) should be A when A==B. However, SymbolicExpr would have already simplified that to A.
    if (sel->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval;
    if (auto selNum = sel->toUnsigned()) {
        retval = SValue::promote(*selNum ? a->copy() : b->copy());
        switch (computingDefiners_) {
            case TRACK_NO_DEFINERS:
                break;
            case TRACK_ALL_DEFINERS:
#if 0 // [Robb P. Matzke 2015-09-17]: not present in original version
                retval->add_defining_instructions(*selNum ? a : b);
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

    retval = svalueExpr(SymbolicExpr::makeIte(sel->get_expression(), a->get_expression(), b->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeLssb(a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeMssb(a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeRol(sa->get_expression(), a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeRor(sa->get_expression(), a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeShl0(sa->get_expression(), a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeShr0(sa->get_expression(), a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeAsr(sa->get_expression(), a->get_expression(), solver()));
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

    SValuePtr retval = svalueExpr(SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(32, new_width),
                                                           a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->nBits() != a->nBits())
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
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeAdd(a->get_expression(), b->get_expression(), solver()));
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
    ASSERT_require(a->nBits()==b->nBits() && c->nBits()==1);
    if (a->isBottom() || b->isBottom() || c->isBottom()) {
        carry_out = bottom_(a->nBits());
        return filterResult(bottom_(a->nBits()));
    }
    BaseSemantics::SValuePtr aa = unsignedExtend(a, a->nBits()+1);
    BaseSemantics::SValuePtr bb = unsignedExtend(b, a->nBits()+1);
    BaseSemantics::SValuePtr cc = unsignedExtend(c, a->nBits()+1);
    BaseSemantics::SValuePtr sumco = add(aa, add(bb, cc));
    carry_out = filterResult(extract(xor_(aa, xor_(bb, sumco)), 1, a->nBits()+1));
    return filterResult(add(a, add(b, unsignedExtend(c, a->nBits()))));
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeNegate(a->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeSignedDiv(a->get_expression(), b->get_expression(), solver()));
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
        return filterResult(bottom_(b->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeSignedMod(a->get_expression(), b->get_expression(), solver()));
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
    size_t retwidth = a->nBits() + b->nBits();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));
    SValuePtr retval = svalueExpr(SymbolicExpr::makeSignedMul(a->get_expression(), b->get_expression(), solver()));
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
        return filterResult(bottom_(a->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeDiv(a->get_expression(), b->get_expression(), solver()));
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
        return filterResult(bottom_(b->nBits()));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeMod(a->get_expression(), b->get_expression(), solver()));
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
    size_t retwidth = a->nBits() + b->nBits();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));

    SValuePtr retval = svalueExpr(SymbolicExpr::makeMul(a->get_expression(), b->get_expression(), solver()));
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

    SValuePtr retval = svalueExpr(SymbolicExpr::makeSignExtend(SymbolicExpr::makeIntegerConstant(32, new_width),
                                                               a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->nBits() != a->nBits())
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
    if (fpType->exponentBits().size() < 2)
        return NULL;
    if (fpType->exponentBias() != ((uint64_t)1 << (fpType->exponentBits().size() - 1)) - 1)
        return NULL;

    // The sign, exponent, and significand regions must not overlap, although IEEE 754 allows unused regions. For instance, the
    // Motorola 68000 family has an "extended real" type that's 96 bits but 17 bits are unused (the format is 63-bit
    // significand, a bit that's always set (i.e., the explicit leading one bit for the significand), 16 bits that are always
    // clear, a 15-bit exponent, and a sign bit).
    if (fpType->significandBits().isOverlapping(fpType->exponentBits()))
        return NULL;
    if (fpType->significandBits().isOverlapping(fpType->signBit()))
        return NULL;
    if (fpType->exponentBits().isOverlapping(fpType->signBit()))
        return NULL;

    return fpType;
}

SymbolicExpr::Type
RiscOperators::sgTypeToSymbolicType(SgAsmType *sgType) {
    ASSERT_not_null(sgType);
    if (SgAsmFloatType *fpType = sgIsIeee754(sgType)) {
        if (!fpType->implicitBitConvention())
            throw BaseSemantics::Exception("cannot convert Sage type to symbolic type: "
                                           "IEEE-754 implicit bit convention is required",
                                           currentInstruction());
        if (!fpType->gradualUnderflow())
            throw BaseSemantics::Exception("cannot convert Sage type to symbolic type: "
                                           "IEEE-754 gradual underflow capability is required",
                                           currentInstruction());
        return SymbolicExpr::Type::floatingPoint(fpType->exponentBits().size(), fpType->significandBits().size()+1/*implicit bit*/);
    } else if (SgAsmIntegerType *iType = isSgAsmIntegerType(sgType)) {
        return SymbolicExpr::Type::integer(iType->get_nBits());
    } else {
        throw BaseSemantics::Exception("cannot convert Sage type to symbolic type: "
                                       "not an integer or IEEE-754 type",
                                       currentInstruction());
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

    SValuePtr result;
    if (srcType == dstType) {
        result = SValue::promote(a->copy());
    } else {
        result = svalueExpr(SymbolicExpr::makeConvert(a->get_expression(), dstType, solver()));
    }
    ASSERT_not_null(result);

    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            result->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            result->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            break;
    }
    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::reinterpret(const BaseSemantics::SValuePtr &a_, SgAsmType *retType) {
    SValuePtr a = SValue::promote(a_);
    ASSERT_not_null(a);
    ASSERT_not_null(retType);
    SymbolicExpr::Type srcType = a->get_expression()->type();
    SymbolicExpr::Type dstType = sgTypeToSymbolicType(retType);
    SValuePtr result;
    if (srcType == dstType) {
        result = a;
    } else if (srcType.nBits() != dstType.nBits()) {
        throw Exception("reinterpret type (" + dstType.toString() + ") is not the same size as the value type (" +
                        srcType.toString() + ")");
    } else {
        result = svalueExpr(SymbolicExpr::makeReinterpret(a->get_expression(), dstType, solver()));
        result->add_defining_instructions(a);           // reinterpret should have no effect on the definers.
    }
    ASSERT_not_null(result);
    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    PartialDisableUsedef du(this);
    SValuePtr result = SValue::promote(BaseSemantics::RiscOperators::readRegister(reg, dflt));

    if (currentInstruction()) {
        RegisterStatePtr regs = RegisterState::promote(currentState()->registerState());
        regs->updateReadProperties(reg);
    }

    if (reinterpretRegisterReads_)
        result = SValue::promote(reinterpret(result, SageBuilderAsm::buildTypeU(result->nBits())));
    return filterResult(result);
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    PartialDisableUsedef du(this);
    BaseSemantics::SValuePtr result = BaseSemantics::RiscOperators::peekRegister(reg, dflt);
    ASSERT_require(result!=NULL && result->nBits() == reg.nBits());
    if (reinterpretRegisterReads_)
        result = reinterpret(result, SageBuilderAsm::buildTypeU(result->nBits()));
    return filterResult(result);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a_) {
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
                                AllowSideEffects::Flag allowSideEffects) {
    size_t nbits = dflt->nBits();
    ASSERT_require(0 == nbits % 8);
    SValuePtr retval;

    PartialDisableUsedef du(this);

    // Offset the address by the value of the segment register.
    BaseSemantics::SValuePtr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValuePtr segregValue;
        if (AllowSideEffects::YES == allowSideEffects) {
            segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        } else {
            segregValue = peekRegister(segreg, undefined_(segreg.nBits()));
        }
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }

    // Short circuit if address is Bottom.
    if (adjustedVa->isBottom()) {
        retval = SValue::promote(bottom_(dflt->nBits()));
        if (reinterpretMemoryReads_)
            retval = SValue::promote(reinterpret(retval, SageBuilderAsm::buildTypeU(retval->nBits())));
        return retval;
    }

    // Read the bytes and concatenate them together. SymbolicExpr will simplify the expression so that reading after
    // writing a multi-byte value will return the original value written rather than a concatenation of byte extractions.
    InsnSet allDefiners;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr currentMem = currentState()->memoryState();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==currentMem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(adjustedVa, number_(adjustedVa->nBits(), bytenum));

        // Read the default value from the initial memory state first. We want to use whatever value is in the initial memory
        // state if the address is not present in the current memory state. As a side effect, if this value is not in the
        // initial memory it will be added.
        if (initialState()) {
            if (AllowSideEffects::YES == allowSideEffects) {
                byte_dflt = initialState()->readMemory(byte_addr, byte_dflt, this, this);
            } else {
                byte_dflt = initialState()->peekMemory(byte_addr, byte_dflt, this, this);
            }
        }

        // Read a byte from the current memory state. Adds the new value as a side effect if necessary.
        SValuePtr byte_value;
        if (AllowSideEffects::YES == allowSideEffects) {
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

    ASSERT_require(retval!=NULL && retval->nBits()==nbits);
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
        case TRACK_LATEST_DEFINER:
            retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            retval->add_defining_instructions(allDefiners);
            break;
    }

    if (reinterpretMemoryReads_)
        retval = SValue::promote(reinterpret(retval, SageBuilderAsm::buildTypeU(retval->nBits())));

    return filterResult(retval);
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt,
                          const BaseSemantics::SValuePtr &condition) {
    ASSERT_require(1 == condition->nBits());
    if (condition->isFalse())
        return filterResult(dflt);
    return readOrPeekMemory(segreg, address, dflt, AllowSideEffects::YES);
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt) {
    return readOrPeekMemory(segreg, address, dflt, AllowSideEffects::NO);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg,
                           const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value_,
                           const BaseSemantics::SValuePtr &condition) {
    ASSERT_require(1 == condition->nBits());
    if (condition->isFalse())
        return;

    PartialDisableUsedef du(this);

    // Offset the address by the value of the segment register.
    SValuePtr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = SValue::promote(address);
    } else {
        BaseSemantics::SValuePtr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = SValue::promote(add(address, signExtend(segregValue, address->nBits())));
    }

    if (adjustedVa->isBottom())
        return;
    SValuePtr value = SValue::promote(value_);
    size_t nbits = value->nBits();
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
        SValuePtr byte_addr = SValue::promote(add(adjustedVa, number_(adjustedVa->nBits(), bytenum)));
        byte_addr->add_defining_instructions(adjustedVa);
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
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::SValue);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryListState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryMapState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::RiscOperators);
#endif

#endif
