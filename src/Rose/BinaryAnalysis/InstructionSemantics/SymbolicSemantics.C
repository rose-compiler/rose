#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>
#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>

#include <Cxx_GrammarDowncast.h>
#include <integerOps.h>                                 // rose
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
//                                      Merger
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Merger::Merger() {}

Merger::Ptr
Merger::instance() {
    return Ptr(new Merger);
}

Merger::Ptr
Merger::instance(size_t n) {
    Ptr retval = Ptr(new Merger);
    retval->setSizeLimit(n);
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::SValue() {}

SValue::SValue(size_t nbits)
    : BaseSemantics::SValue(nbits) {
    expr = SymbolicExpression::makeIntegerVariable(nbits);
}

SValue::SValue(size_t nbits, uint64_t number)
    : BaseSemantics::SValue(nbits) {
    expr = SymbolicExpression::makeIntegerConstant(nbits, number);
}

SValue::SValue(ExprPtr expr)
    : BaseSemantics::SValue(expr->nBits()) {
    this->expr = expr;
}

SValue::Ptr
SValue::instance() {
    return SValue::Ptr(new SValue(SymbolicExpression::makeIntegerVariable(1)));
}

SValue::Ptr
SValue::instance_bottom(size_t nbits) {
    return SValue::Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits, "", ExprNode::BOTTOM)));
}

SValue::Ptr
SValue::instance_undefined(size_t nbits) {
    return SValue::Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits)));
}

SValue::Ptr
SValue::instance_unspecified(size_t nbits) {
    return SValue::Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits, "", ExprNode::UNSPECIFIED)));
}

SValue::Ptr
SValue::instance_integer(size_t nbits, uint64_t value) {
    return SValue::Ptr(new SValue(SymbolicExpression::makeIntegerConstant(nbits, value)));
}

SValue::Ptr
SValue::instance_symbolic(const SymbolicExpression::Ptr &value) {
    ASSERT_not_null(value);
    return SValuePtr(new SValue(value));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    return instance_bottom(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const {
    return instance_unspecified(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t value) const {
    return instance_integer(nbits, value);
}

BaseSemantics::SValue::Ptr
SValue::boolean_(bool value) const {
    return instance_integer(1, value?1:0);
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t new_width) const {
    SValue::Ptr retval(new SValue(*this));
    if (new_width!=0 && new_width!=retval->nBits())
        retval->set_width(new_width);
    return retval;
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValuePtr &v) {
    SValuePtr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

bool
SValue::isBottom() const {
    return 0 != (get_expression()->flags() & SymbolicExpression::Node::BOTTOM);
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &other_, const BaseSemantics::Merger::Ptr &merger_,
                            const SmtSolverPtr &solver) const {
    SValue::Ptr other = SValue::promote(other_);
    ASSERT_require(nBits() == other->nBits());
    Merger::Ptr merger = as<Merger>(merger_);
    bool changed = false;
    unsigned mergedFlags = get_expression()->flags() | other->get_expression()->flags();
    SValue::Ptr retval = SValue::promote(copy());
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
        ExprPtr expr = SymbolicExpression::makeSet(get_expression(), other->get_expression(), solver, "", mergedFlags);
        size_t exprSetSize = expr->isInteriorNode() && expr->isInteriorNode()->getOperator()==SymbolicExpression::OP_SET ?
                             expr->isInteriorNode()->nChildren() : (size_t)1;
        size_t setSizeLimit = merger ? merger->setSizeLimit() : (size_t)1;
        if (exprSetSize > setSizeLimit) {
            expr = SymbolicExpression::makeVariable(retval->get_expression()->type());
            mergedFlags |= SymbolicExpression::Node::BOTTOM;
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

    return changed ? Sawyer::Optional<BaseSemantics::SValue::Ptr>(retval) : Sawyer::Nothing();
}

void
SValue::set_width(size_t nbits) {
    ASSERT_always_require(nbits==nBits());
}

bool
SValue::is_number() const {
    return expr->isIntegerConstant();
}

uint64_t
SValue::get_number() const
{
    LeafPtr leaf = expr->isLeafNode();
    ASSERT_not_null(leaf);
    return leaf->toUnsigned().get();
}

SValue::Ptr
SValue::substitute(const SValue::Ptr &from, const SValue::Ptr &to, const SmtSolverPtr &solver) const
{
    SValue::Ptr retval = SValue::promote(copy());
    retval->set_expression(retval->get_expression()->substitute(from->get_expression(), to->get_expression(), solver));
    return retval;
}

void
SValue::defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2, const InsnSet &set3) {
    add_defining_instructions(set3);
    defined_by(insn, set1, set2);
}

void
SValue::defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2) {
    add_defining_instructions(set2);
    defined_by(insn, set1);
}

void
SValue::defined_by(SgAsmInstruction *insn, const InsnSet &set1) {
    add_defining_instructions(set1);
    defined_by(insn);
}

void
SValue::defined_by(SgAsmInstruction *insn) {
    add_defining_instructions(insn);
}

const ExprPtr&
SValue::get_expression() const {
    return expr;
}

void
SValue::set_expression(const ExprPtr &new_expr) {
    ASSERT_not_null(new_expr);
    expr = new_expr;
    width = new_expr->nBits();
}

void
SValue::set_expression(const SValue::Ptr &source) {
    set_expression(source->get_expression());
}

const InsnSet&
SValue::get_defining_instructions() const {
    return defs;
}

size_t
SValue::add_defining_instructions(const SValue::Ptr &source) {
    return add_defining_instructions(source->get_defining_instructions());
}

void
SValue::set_defining_instructions(const InsnSet &new_defs) {
    defs = new_defs;
}

void
SValue::set_defining_instructions(const SValue::Ptr &source) {
    set_defining_instructions(source->get_defining_instructions());
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
SValue::may_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const
{
    SValue::Ptr other = SValue::promote(other_);
    if (nBits() != other->nBits())
        return false;
    if (isBottom() || other->isBottom())
        return true;
    return get_expression()->mayEqual(other->get_expression(), solver);
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const
{
    SValue::Ptr other = SValue::promote(other_);
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
        // SymbolicExpression doesn't allow a hasher to be specified because of the way it caches hashes for performance for very
        // large expressions with a high degree of sharing and parallelism.
        hasher.insert(expr->hash());
    }
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter_) const
{
    Formatter *formatter = dynamic_cast<Formatter*>(&formatter_);
    SymbolicExpression::Formatter dflt_expr_formatter;
    SymbolicExpression::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;
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

MemoryListState::MemoryListState()
    : cellCompressor_(CellCompressorChoice::instance()) {}

MemoryListState::MemoryListState(const BaseSemantics::MemoryCell::Ptr &protocell)
    : BaseSemantics::MemoryCellList(protocell), cellCompressor_(CellCompressorChoice::instance()) {}

MemoryListState::MemoryListState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval)
    : BaseSemantics::MemoryCellList(addrProtoval, valProtoval), cellCompressor_(CellCompressorChoice::instance()) {}

MemoryListState::MemoryListState(const MemoryListState &other)
    : BaseSemantics::MemoryCellList(other), cellCompressor_(other.cellCompressor_) {}

// class method
MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorMcCarthy::instance() {
    return Ptr(new CellCompressorMcCarthy);
}

MemoryListState::Ptr
MemoryListState::instance(const BaseSemantics::MemoryCell::Ptr &protocell) {
    return MemoryListState::Ptr(new MemoryListState(protocell));
}

MemoryListState::Ptr
MemoryListState::instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) {
    return MemoryListState::Ptr(new MemoryListState(addrProtoval, valProtoval));
}

MemoryListState::Ptr
MemoryListState::instance(const MemoryListState::Ptr &other) {
    return MemoryListState::Ptr(new MemoryListState(*other));
}

BaseSemantics::MemoryState::Ptr
MemoryListState::create(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) const {
    return instance(addrProtoval, valProtoval);
}

BaseSemantics::MemoryState::Ptr
MemoryListState::create(const BaseSemantics::MemoryCell::Ptr &protocell) const {
    return instance(protocell);
}

BaseSemantics::AddressSpace::Ptr
MemoryListState::clone() const {
    return BaseSemantics::AddressSpace::Ptr(new MemoryListState(*this));
}

MemoryListState::Ptr
MemoryListState::promote(const BaseSemantics::AddressSpace::Ptr &x) {
    MemoryListState::Ptr retval = as<MemoryListState>(x);
    ASSERT_not_null(retval);
    return retval;
}

SValue::Ptr
MemoryListState::CellCompressorMcCarthy::operator()(const SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                                                    BaseSemantics::RiscOperators */*addrOps*/,
                                                    BaseSemantics::RiscOperators *valOps, const BaseSemantics::CellList &cells)
{
    if (1==cells.size())
        return SValue::promote(cells.front()->value()->copy());

    RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps);
    ASSERT_not_null(valOpsSymbolic);
    DefinersMode valDefinersMode = valOpsSymbolic->computingDefiners();

    // FIXME: This makes no attempt to remove duplicate values [Robb Matzke 2013-03-01]
    ExprPtr expr = SymbolicExpression::makeMemoryVariable(address->nBits(), dflt->nBits());
    InsnSet addrDefiners, valDefiners;
    for (BaseSemantics::CellList::const_reverse_iterator ci = cells.rbegin(); ci != cells.rend(); ++ci) {
        SValue::Ptr cell_addr = SValue::promote((*ci)->address());
        SValue::Ptr cell_value  = SValue::promote((*ci)->value());
        expr = SymbolicExpression::makeWrite(expr, cell_addr->get_expression(), cell_value->get_expression(), valOps->solver());
        if (valDefinersMode != TRACK_NO_DEFINERS) {
            const InsnSet &definers = cell_value->get_defining_instructions();
            valDefiners.insert(definers.begin(), definers.end());
        }
    }
    SValue::Ptr retval = SValue::promote(valOps->undefined_(dflt->nBits()));
    retval->set_expression(SymbolicExpression::makeRead(expr, address->get_expression(), valOps->solver()));
    retval->set_defining_instructions(valDefiners);
    return retval;
}

// class method
MemoryListState::CellCompressor::Ptr
MemoryListState::CellCompressorSimple::instance() {
    return Ptr(new CellCompressorSimple);
}

SValue::Ptr
MemoryListState::CellCompressorSimple::operator()(const SValue::Ptr &/*address*/, const BaseSemantics::SValue::Ptr &dflt,
                                                  BaseSemantics::RiscOperators */*addrOps*/,
                                                  BaseSemantics::RiscOperators */*valOps*/, const BaseSemantics::CellList &cells)
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

SValue::Ptr
MemoryListState::CellCompressorChoice::operator()(const SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                                  const BaseSemantics::CellList &cells)
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

SValue::Ptr
MemoryListState::CellCompressorSet::operator()(const SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                                               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                               const BaseSemantics::CellList &cells) {
    ASSERT_always_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_always_not_null(addrOps);
    ASSERT_not_null(valOps);

    RiscOperators *valOpsSymbolic = dynamic_cast<RiscOperators*>(valOps);
    const DefinersMode valDefinersMode = valOpsSymbolic->computingDefiners();
    InsnSet valDefiners;
    SymbolicExpression::Ptr first, set;
    std::cerr <<"ROBB: CellCompressorSet for " <<StringUtility::plural(cells.size(), "cells") <<"\n";
    for (const BaseSemantics::MemoryCell::Ptr &cell: cells) {
        SValue::Ptr cellValue = SValue::promote(cell->value());
        SymbolicExpression::Ptr cur = cellValue->get_expression();
        if (!first) {
            first = cur;
        } else if (!set) {
            ASSERT_not_null(first);
            set = SymbolicExpression::makeSet(first, cur);
        } else {
            set = SymbolicExpression::makeSet(set, cur);
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
        SValue::Ptr retval = SValue::promote(valOps->undefined_(dflt->nBits()));
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

BaseSemantics::SValue::Ptr
MemoryListState::readOrPeekMemory(const BaseSemantics::SValue::Ptr &address_, const BaseSemantics::SValue::Ptr &dflt,
                                  BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                  AllowSideEffects::Flag allowSideEffects) {
    size_t nBits = dflt->nBits();
    SValue::Ptr address = SValue::promote(address_);
    ASSERT_require(8==nBits); // SymbolicSemantics::MemoryListState assumes that memory cells contain only 8-bit data

    BaseSemantics::CellList::iterator cursor = get_cells().begin();
    BaseSemantics::CellList cells = scan(cursor /*in,out*/, address, nBits, addrOps, valOps);

    // If we fell off the end of the list then the read could be reading from a memory location for which no cell exists. If
    // side effects are allowed, we should add a new cell to the return value.
    if (cursor == get_cells().end()) {
        if (AllowSideEffects::YES == allowSideEffects) {
            BaseSemantics::MemoryCell::Ptr newCell = insertReadCell(address, dflt);
            cells.push_back(newCell);
        } else {
            BaseSemantics::MemoryCell::Ptr newCell = protocell->create(address, dflt);
            cells.push_back(newCell);
        }
    }

    // If we're doing an actual read (rather than just a peek), then update the returned cells to indicate that they've been
    // read. Since the "cells" vector is pointers that haven't been deep-copied, this is a side effect on the memory
    // state. But even if it weren't a side effect, we don't want the returned value to be marked as having been actually
    // read when we're only peeking.
    if (AllowSideEffects::YES == allowSideEffects)
        updateReadProperties(cells);

    SValue::Ptr retval = cellCompressor()->operator()(address, dflt, addrOps, valOps, cells);
    ASSERT_require(retval->nBits()==8);
    return retval;
}

BaseSemantics::SValue::Ptr
MemoryListState::readMemory(const BaseSemantics::SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, AllowSideEffects::YES);
}

BaseSemantics::SValue::Ptr
MemoryListState::peekMemory(const BaseSemantics::SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                            BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(address, dflt, addrOps, valOps, AllowSideEffects::NO);
}

void
MemoryListState::writeMemory(const BaseSemantics::SValue::Ptr &address, const BaseSemantics::SValue::Ptr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_require(8==value->nBits());
    BaseSemantics::MemoryCellList::writeMemory(address, value, addrOps, valOps);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Map-based Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryMapState::MemoryMapState() {}

MemoryMapState::MemoryMapState(const BaseSemantics::MemoryCell::Ptr &protocell)
    : BaseSemantics::MemoryCellMap(protocell) {}

MemoryMapState::MemoryMapState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval)
    : BaseSemantics::MemoryCellMap(addrProtoval, valProtoval) {}

MemoryMapState::Ptr
MemoryMapState::instance(const BaseSemantics::MemoryCell::Ptr &protocell) {
    return MemoryMapState::Ptr(new MemoryMapState(protocell));
}

MemoryMapState::Ptr
MemoryMapState::instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) {
    return MemoryMapStatePtr(new MemoryMapState(addrProtoval, valProtoval));
}

MemoryMapState::Ptr
MemoryMapState::instance(const MemoryMapState::Ptr &other) {
    return MemoryMapState::Ptr(new MemoryMapState(*other));
}

BaseSemantics::MemoryState::Ptr
MemoryMapState::create(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) const {
    return instance(addrProtoval, valProtoval);
}

BaseSemantics::MemoryState::Ptr
MemoryMapState::create(const BaseSemantics::MemoryCell::Ptr &protocell) const {
    return instance(protocell);
}

BaseSemantics::AddressSpace::Ptr
MemoryMapState::clone() const {
    return Ptr(new MemoryMapState(*this));
}

MemoryMapState::Ptr
MemoryMapState::promote(const BaseSemantics::AddressSpace::Ptr &x) {
    MemoryMapState::Ptr retval = as<MemoryMapState>(x);
    ASSERT_not_null(retval);
    return retval;
}

BaseSemantics::MemoryCellMap::CellKey
MemoryMapState::generateCellKey(const BaseSemantics::SValue::Ptr &addr_) const {
    SValue::Ptr addr = SValue::promote(addr_);
    return addr->get_expression()->hash();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RiscOperators::RiscOperators()
    : omit_cur_insn(false), computingDefiners_(TRACK_NO_DEFINERS), computingMemoryWriters_(TRACK_LATEST_WRITER),
      computingRegisterWriters_(TRACK_LATEST_WRITER), trimThreshold_(0), reinterpretMemoryReads_(true),
      reinterpretRegisterReads_(true) {}

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver), omit_cur_insn(false), computingDefiners_(TRACK_NO_DEFINERS),
    computingMemoryWriters_(TRACK_LATEST_WRITER), computingRegisterWriters_(TRACK_LATEST_WRITER), trimThreshold_(0),
    reinterpretMemoryReads_(true), reinterpretRegisterReads_(true) {
    name("Symbolic");
    ASSERT_always_not_null(protoval);
    ASSERT_always_not_null2(as<SValue>(protoval), "SymbolicSemantics supports only symbolic SValue types or derivatives thereof");
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver), omit_cur_insn(false), computingDefiners_(TRACK_NO_DEFINERS),
    computingMemoryWriters_(TRACK_LATEST_WRITER), computingRegisterWriters_(TRACK_LATEST_WRITER), trimThreshold_(0),
    reinterpretMemoryReads_(true), reinterpretRegisterReads_(true) {
    name("Symbolic");
    ASSERT_always_not_null(state);
    ASSERT_always_not_null(state->registerState());
    ASSERT_always_not_null2(as<RegisterState>(state->registerState()),
                            "SymbolicSemantics supports only RegisterStateGeneric or derivatives thereof");
    ASSERT_always_not_null(state->protoval());
    ASSERT_always_not_null2(as<SValue>(state->protoval()),
                            "SymbolicSemantics supports only symbolic SValue types or derivatives thereof");
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict, const SmtSolverPtr &solver) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryListState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromProtoval(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(protoval, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromState(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(state, solver));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) const {
    return instanceFromProtoval(protoval, solver);
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) const {
    return instanceFromState(state, solver);
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::boolean_(bool b) {
    SValue::Ptr retval = SValue::promote(BaseSemantics::RiscOperators::boolean_(b));
    if (computingDefiners() != TRACK_NO_DEFINERS && !omit_cur_insn)
        retval->defined_by(currentInstruction());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::number_(size_t nbits, uint64_t value) {
    SValue::Ptr retval = SValue::promote(BaseSemantics::RiscOperators::number_(nbits, value));
    if (computingDefiners() != TRACK_NO_DEFINERS && !omit_cur_insn)
        retval->defined_by(currentInstruction());
    return retval;
}

SValue::Ptr
RiscOperators::svalueExpr(const ExprPtr &expr, const InsnSet &defs) {
    SValue::Ptr newval = SValue::promote(protoval()->undefined_(expr->nBits()));
    newval->set_expression(expr);
    newval->set_defining_instructions(defs);
    return newval;
}

SValue::Ptr
RiscOperators::svalueUndefined(size_t nbits) {
    return SValue::promote(undefined_(nbits));
}

SValue::Ptr
RiscOperators::svalueBottom(size_t nbits) {
    return SValue::promote(bottom_(nbits));
}

SValue::Ptr
RiscOperators::svalueUnspecified(size_t nbits) {
    return SValue::promote(unspecified_(nbits));
}

SValue::Ptr
RiscOperators::svalueNumber(size_t nbits, uint64_t value) {
    return SValue::promote(number_(nbits, value));
}

SValue::Ptr
RiscOperators::svalueBoolean(bool b) {
    return SValue::promote(boolean_(b));
}

void
RiscOperators::substitute(const SValue::Ptr &from, const SValue::Ptr &to)
{
    BaseSemantics::State::Ptr state = currentState();

    // Substitute in registers
    struct RegSubst: RegisterState::Visitor {
        SValue::Ptr from, to;
        SmtSolverPtr solver;
        RegSubst(const SValue::Ptr &from, const SValue::Ptr &to, const SmtSolverPtr &solver)
            : from(from), to(to), solver(solver) {}
        virtual BaseSemantics::SValue::Ptr operator()(RegisterDescriptor, const BaseSemantics::SValue::Ptr &val_) {
            SValue::Ptr val = SValue::promote(val_);
            return val->substitute(from, to, solver);
        }
    } regsubst(from, to, solver());
    RegisterState::promote(state->registerState())->traverse(regsubst);

    // Substitute in memory
    struct MemSubst: BaseSemantics::MemoryCell::Visitor {
        SValue::Ptr from, to;
        SmtSolverPtr solver;
        MemSubst(const SValue::Ptr &from, const SValue::Ptr &to, const SmtSolverPtr &solver)
            : from(from), to(to), solver(solver) {}
        virtual void operator()(BaseSemantics::MemoryCell::Ptr &cell) {
            SValue::Ptr addr = SValue::promote(cell->address());
            cell->address(addr->substitute(from, to, solver));
            SValue::Ptr val = SValue::promote(cell->value());
            cell->value(val->substitute(from, to, solver));
        }
    } memsubst(from, to, solver());
    MemoryState::promote(state->memoryState())->traverse(memsubst);
}

BaseSemantics::SValue::Ptr
RiscOperators::filterResult(const BaseSemantics::SValue::Ptr &a_) {
    if (trimThreshold_ == 0)
        return a_;
    SValue::Ptr a = SValue::promote(a_);
    if (a->get_expression()->nNodes() <= trimThreshold_)
        return a_;

    SymbolicExpression::Ptr expr = SymbolicExpression::makeVariable(a->get_expression()->type(),
                                                                    a->get_expression()->comment(),
                                                                    a->get_expression()->flags());
    a->set_expression(expr);
    ++nTrimmed_;
    return a;
}

void
RiscOperators::interrupt(int /*major*/, int /*minor*/) {
    currentState()->clear();
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeAnd(a->get_expression(), b->get_expression(), solver()));

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

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeOr(a->get_expression(), b->get_expression(), solver()));

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

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval;
    // We leave these simplifications here because SymbolicExpression doesn't yet have a way to pass an SMT solver to its
    // simplifier.
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        retval = svalueNumber(a->nBits(), *aNum ^ *bNum);
    } else if (a->get_expression()->mustEqual(b->get_expression(), solver())) {
        retval = svalueNumber(a->nBits(), 0);
    } else {
        retval = svalueExpr(SymbolicExpression::makeXor(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->nBits()));
    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeInvert(a->get_expression(), solver()));

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

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, const size_t begin_bit, const size_t end_bit)
{
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->nBits());
    ASSERT_require(begin_bit<end_bit);
    SymbolicExpression::Ptr beginExpr = SymbolicExpression::makeIntegerConstant(32, begin_bit);
    SymbolicExpression::Ptr endExpr = SymbolicExpression::makeIntegerConstant(32, end_bit);

    if (a->isBottom()) {
        return filterResult(svalueExpr(SymbolicExpression::makeExtract(beginExpr, endExpr, a->get_expression(), solver(),
                                                                       "", SymbolicExpression::Node::BOTTOM)));
    }

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeExtract(beginExpr, endExpr, a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &lo_bits_, const BaseSemantics::SValue::Ptr &hi_bits_)
{
    SValue::Ptr lo = SValue::promote(lo_bits_);
    SValue::Ptr hi = SValue::promote(hi_bits_);
    if (lo->isBottom() || hi->isBottom()) {
        return filterResult(svalueExpr(SymbolicExpression::makeConcat(hi->get_expression(), lo->get_expression(), solver(),
                                                                      "", SymbolicExpression::Node::BOTTOM)));
    }

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeConcat(hi->get_expression(), lo->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(1));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeZerop(a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel_,
                             const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                             IteStatus &status /*out*/)
{
    SValue::Ptr sel = SValue::promote(sel_);
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(1==sel->nBits());
    ASSERT_require(a->nBits()==b->nBits());

    // (ite bottom A B) should be A when A==B. However, SymbolicExpression would have already simplified that to A.
    if (sel->isBottom()) {
        status = IteStatus::NEITHER;
        return filterResult(bottom_(a->nBits()));
    }

    SValue::Ptr retval;
    if (auto selNum = sel->toUnsigned()) {
        retval = SValue::promote(*selNum ? a->copy() : b->copy());
        status = *selNum ? IteStatus::A : IteStatus::B;
        switch (computingDefiners_) {
            case TRACK_NO_DEFINERS:
                break;
            case TRACK_ALL_DEFINERS:
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
                status = IteStatus::B;
                switch (computingDefiners_) {
                    case TRACK_NO_DEFINERS:
                        break;
                    case TRACK_ALL_DEFINERS:
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
            ExprPtr inverseCondition = SymbolicExpression::makeInvert(sel->get_expression(), solver());
            solver()->insert(inverseCondition);
            bool can_be_false = SmtSolver::SAT_NO != solver()->check();
            if (!can_be_false) {
                retval = SValue::promote(a->copy());
                status = IteStatus::A;
                switch (computingDefiners_) {
                    case TRACK_NO_DEFINERS:
                        break;
                    case TRACK_ALL_DEFINERS:
                        retval->add_defining_instructions(sel); // fall through...
                    case TRACK_LATEST_DEFINER:
                        retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
                        break;
                }
                return filterResult(retval);
            }
        }
    }

    retval = svalueExpr(SymbolicExpression::makeIte(sel->get_expression(), a->get_expression(), b->get_expression(), solver()));
    status = IteStatus::BOTH;
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

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeLssb(a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeMssb(a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeRol(sa->get_expression(), a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeRor(sa->get_expression(), a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeShl0(sa->get_expression(), a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeShr0(sa->get_expression(), a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (a->isBottom() || sa->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeAsr(sa->get_expression(), a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(new_width));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeExtend(SymbolicExpression::makeIntegerConstant(32, new_width),
                                                                   a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->nBits() != a->nBits()) {
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            } else {
                retval->add_defining_instructions(a);   // preserve definers if this extend is a no-op
            }
            break;
    }
    return filterResult(retval);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeAdd(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                              const BaseSemantics::SValue::Ptr &c, BaseSemantics::SValue::Ptr &carry_out/*out*/)
{
    ASSERT_require(a->nBits()==b->nBits() && c->nBits()==1);
    if (a->isBottom() || b->isBottom() || c->isBottom()) {
        carry_out = bottom_(a->nBits());
        return filterResult(bottom_(a->nBits()));
    }
    BaseSemantics::SValue::Ptr aa = unsignedExtend(a, a->nBits()+1);
    BaseSemantics::SValue::Ptr bb = unsignedExtend(b, a->nBits()+1);
    BaseSemantics::SValue::Ptr cc = unsignedExtend(c, a->nBits()+1);
    BaseSemantics::SValue::Ptr sumco = add(aa, add(bb, cc));
    carry_out = filterResult(extract(xor_(aa, xor_(bb, sumco)), 1, a->nBits()+1));
    return filterResult(add(a, add(b, unsignedExtend(c, a->nBits()))));
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeNegate(a->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeSignedDiv(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(b->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeSignedMod(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));
    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeSignedMul(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(a->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeDiv(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(b->nBits()));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeMod(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    if (a->isBottom() || b->isBottom())
        return filterResult(bottom_(retwidth));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeMul(a->get_expression(), b->get_expression(), solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return filterResult(bottom_(new_width));

    SValue::Ptr retval = svalueExpr(SymbolicExpression::makeSignExtend(SymbolicExpression::makeIntegerConstant(32, new_width),
                                                                       a->get_expression(), solver()));
    switch (computingDefiners_) {
        case TRACK_NO_DEFINERS:
            break;
        case TRACK_ALL_DEFINERS:
            retval->add_defining_instructions(a);       // fall through...
        case TRACK_LATEST_DEFINER:
            if (retval->nBits() != a->nBits()) {
                retval->add_defining_instructions(omit_cur_insn ? NULL : currentInstruction());
            } else {
                retval->add_defining_instructions(a);   // preserve definers if this extend is a no-op
            }
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
    if (fpType->significandBits().overlaps(fpType->exponentBits()))
        return NULL;
    if (fpType->significandBits().overlaps(fpType->signBit()))
        return NULL;
    if (fpType->exponentBits().overlaps(fpType->signBit()))
        return NULL;

    return fpType;
}

SymbolicExpression::Type
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
        return SymbolicExpression::Type::floatingPoint(fpType->exponentBits().size(),
                                                       fpType->significandBits().size()+1/*implicit bit*/);
    } else if (SgAsmIntegerType *iType = isSgAsmIntegerType(sgType)) {
        return SymbolicExpression::Type::integer(iType->get_nBits());
    } else {
        throw BaseSemantics::Exception("cannot convert Sage type to symbolic type: "
                                       "not an integer or IEEE-754 type",
                                       currentInstruction());
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::fpConvert(const BaseSemantics::SValue::Ptr &a_, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_not_null(retType);

    SymbolicExpression::Type srcType = sgTypeToSymbolicType(aType);
    ASSERT_require(a->get_expression()->type() == srcType);
    SymbolicExpression::Type dstType = sgTypeToSymbolicType(retType);

    SValue::Ptr result;
    if (srcType == dstType) {
        result = SValue::promote(a->copy());
    } else {
        result = svalueExpr(SymbolicExpression::makeConvert(a->get_expression(), dstType, solver()));
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

BaseSemantics::SValue::Ptr
RiscOperators::reinterpret(const BaseSemantics::SValue::Ptr &a_, SgAsmType *retType) {
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_not_null(a);
    ASSERT_not_null(retType);
    SymbolicExpression::Type srcType = a->get_expression()->type();
    SymbolicExpression::Type dstType = sgTypeToSymbolicType(retType);
    SValue::Ptr result;
    if (srcType == dstType) {
        result = a;
    } else if (srcType.nBits() != dstType.nBits()) {
        throw Exception("reinterpret type (" + dstType.toString() + ") is not the same size as the value type (" +
                        srcType.toString() + ")");
    } else {
        result = svalueExpr(SymbolicExpression::makeReinterpret(a->get_expression(), dstType, solver()));
        result->add_defining_instructions(a);           // reinterpret should have no effect on the definers.
    }
    ASSERT_not_null(result);
    return filterResult(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    PartialDisableUsedef du(this);
    SValue::Ptr result = SValue::promote(BaseSemantics::RiscOperators::readRegister(reg, dflt));

    if (currentInstruction()) {
        RegisterState::Ptr regs = RegisterState::promote(currentState()->registerState());
        regs->updateReadProperties(reg);
    }

    if (reinterpretRegisterReads_)
        result = SValue::promote(reinterpret(result, SageBuilderAsm::buildTypeU(result->nBits())));
    return filterResult(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    PartialDisableUsedef du(this);
    BaseSemantics::SValue::Ptr result = BaseSemantics::RiscOperators::peekRegister(reg, dflt);
    ASSERT_require(result!=NULL && result->nBits() == reg.nBits());
    if (reinterpretRegisterReads_)
        result = reinterpret(result, SageBuilderAsm::buildTypeU(result->nBits()));
    return filterResult(result);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr a = SValue::promote(a_->copy());
    PartialDisableUsedef du(this);
    BaseSemantics::RiscOperators::writeRegister(reg, a);

    // Update register properties and writer info.
    RegisterState::Ptr regs = RegisterState::promote(currentState()->registerState());
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

BaseSemantics::SValue::Ptr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg,
                                const BaseSemantics::SValue::Ptr &address,
                                const BaseSemantics::SValue::Ptr &dflt,
                                AllowSideEffects::Flag allowSideEffects) {
    size_t nbits = dflt->nBits();
    ASSERT_require(0 == nbits % 8);
    SValue::Ptr retval;

    PartialDisableUsedef du(this);

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue;
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

    // Read the bytes and concatenate them together. SymbolicExpression will simplify the expression so that reading after
    // writing a multi-byte value will return the original value written rather than a concatenation of byte extractions.
    InsnSet allDefiners;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryState::Ptr currentMem = currentState()->memoryState();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==currentMem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValue::Ptr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValue::Ptr byte_addr = add(adjustedVa, number_(adjustedVa->nBits(), bytenum));

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
        SValue::Ptr byte_value;
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

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt,
                          const BaseSemantics::SValue::Ptr &condition) {
    ASSERT_require(1 == condition->nBits());
    if (condition->isFalse())
        return filterResult(dflt);
    return readOrPeekMemory(segreg, address, dflt, AllowSideEffects::YES);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt) {
    return readOrPeekMemory(segreg, address, dflt, AllowSideEffects::NO);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg,
                           const BaseSemantics::SValue::Ptr &address,
                           const BaseSemantics::SValue::Ptr &value_,
                           const BaseSemantics::SValue::Ptr &condition) {
    ASSERT_require(1 == condition->nBits());
    if (condition->isFalse())
        return;

    PartialDisableUsedef du(this);

    // Offset the address by the value of the segment register.
    SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = SValue::promote(address);
    } else {
        BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = SValue::promote(add(address, signExtend(segregValue, address->nBits())));
    }

    if (adjustedVa->isBottom())
        return;
    SValue::Ptr value = SValue::promote(value_);
    size_t nbits = value->nBits();
    ASSERT_require(0 == nbits % 8);
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();
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

        SValue::Ptr byte_value = SValue::promote(extract(value, 8*byteOffset, 8*byteOffset+8));
        byte_value->add_defining_instructions(value);
        SValue::Ptr byte_addr = SValue::promote(add(adjustedVa, number_(adjustedVa->nBits(), bytenum)));
        byte_addr->add_defining_instructions(adjustedVa);
        currentState()->writeMemory(byte_addr, byte_value, this, this);

        // Update the latest writer info if we have a current instruction and the memory state supports it.
        if (computingMemoryWriters() != TRACK_NO_WRITERS) {
            if (SgAsmInstruction *insn = currentInstruction()) {
                if (BaseSemantics::MemoryCellList::Ptr cellList = as<BaseSemantics::MemoryCellList>(mem)) {
                    if (BaseSemantics::MemoryCell::Ptr cell = cellList->latestWrittenCell()) {
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
                } else if (BaseSemantics::MemoryCellMap::Ptr cellMap = as<BaseSemantics::MemoryCellMap>(mem)) {
                    if (BaseSemantics::MemoryCell::Ptr cell = cellMap->latestWrittenCell()) {
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

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::SValue);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryListState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryMapState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::RiscOperators);
#endif

#endif
