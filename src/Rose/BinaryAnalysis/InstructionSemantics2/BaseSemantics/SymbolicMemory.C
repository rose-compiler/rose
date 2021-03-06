#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SymbolicMemory.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

void
SymbolicMemory::expression(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    ASSERT_require2(!expr->isScalar(), "expression must be a memory state");
    mem_ = expr;
}

void
SymbolicMemory::clear() {
    if (mem_) {
        mem_ = SymbolicExpr::makeMemoryVariable(mem_->domainWidth(), mem_->nBits());
    } else {
        mem_ = SymbolicExpr::makeMemoryVariable(32, 8); // can be adjusted later
    }
}

SValuePtr
SymbolicMemory::readMemory(const SValuePtr &address_, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    if (address->nBits() != mem_->domainWidth() || dflt->nBits() != mem_->nBits()) {
        ASSERT_require2(mem_->isMemoryVariable(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->nBits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = SymbolicExpr::makeMemoryVariable(address->nBits(), dflt->nBits());
    }
    SymbolicExpr::Ptr resultExpr = SymbolicExpr::makeRead(mem_, address->get_expression(), valOps->solver());
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(dflt->copy());
    retval->set_expression(resultExpr);
    return retval;
}

SValuePtr
SymbolicMemory::peekMemory(const SValuePtr &address, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    return readMemory(address, dflt, addrOps, valOps);  // readMemory doesn't have side effects
}

void
SymbolicMemory::writeMemory(const SValuePtr &address_, const SValuePtr &value_, RiscOperators *addrOps, RiscOperators *valOps) {
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    SymbolicSemantics::SValuePtr value = SymbolicSemantics::SValue::promote(value_);
    if (address->nBits() != mem_->domainWidth() || value->nBits() != mem_->nBits()) {
        ASSERT_require2(mem_->isMemoryVariable(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->nBits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = SymbolicExpr::makeMemoryVariable(address->nBits(), value->nBits());
    }

    mem_ = SymbolicExpr::makeWrite(mem_, address->get_expression(), value->get_expression(), valOps->solver());
}

bool
SymbolicMemory::merge(const MemoryStatePtr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    SymbolicMemoryPtr other = SymbolicMemory::promote(other_);
    TODO("[Robb P. Matzke 2015-08-10]");
}

void
SymbolicMemory::hash(Combinatorics::Hasher &hasher, RiscOperators*, RiscOperators*) const {
    hasher.insert(mem_->hash());
}

void
SymbolicMemory::print(std::ostream &out, Formatter &formatter_) const {
    SymbolicSemantics::Formatter *formatter = dynamic_cast<SymbolicSemantics::Formatter*>(&formatter_);
    SymbolicExpr::Formatter dflt_expr_formatter;
    SymbolicExpr::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;
    out <<formatter_.get_line_prefix() <<(*mem_ + expr_formatter) <<"\n";
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
