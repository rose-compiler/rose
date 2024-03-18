#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SymbolicMemory.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/StringUtility/NumberToString.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

void
SymbolicMemory::expression(const SymbolicExpression::Ptr &expr) {
    ASSERT_not_null(expr);
    ASSERT_require2(!expr->isScalar(), "expression must be a memory state");
    mem_ = expr;
}

void
SymbolicMemory::clear() {
    if (mem_) {
        mem_ = SymbolicExpression::makeMemoryVariable(mem_->domainWidth(), mem_->nBits());
    } else {
        mem_ = SymbolicExpression::makeMemoryVariable(32, 8); // can be adjusted later
    }
}

SValue::Ptr
SymbolicMemory::readMemory(const SValue::Ptr &address_, const SValue::Ptr &dflt,
                           RiscOperators */*addrOps*/, RiscOperators *valOps) {
    SymbolicSemantics::SValue::Ptr address = SymbolicSemantics::SValue::promote(address_);
    if (address->nBits() != mem_->domainWidth() || dflt->nBits() != mem_->nBits()) {
        ASSERT_require2(mem_->isMemoryVariable(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->nBits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = SymbolicExpression::makeMemoryVariable(address->nBits(), dflt->nBits());
    }
    SymbolicExpression::Ptr resultExpr = SymbolicExpression::makeRead(mem_, address->get_expression(), valOps->solver());
    SymbolicSemantics::SValue::Ptr retval = SymbolicSemantics::SValue::promote(dflt->copy());
    retval->set_expression(resultExpr);
    return retval;
}

SValue::Ptr
SymbolicMemory::peekMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    return readMemory(address, dflt, addrOps, valOps);  // readMemory doesn't have side effects
}

void
SymbolicMemory::writeMemory(const SValue::Ptr &address_, const SValue::Ptr &value_, RiscOperators */*addrOps*/, RiscOperators *valOps) {
    SymbolicSemantics::SValue::Ptr address = SymbolicSemantics::SValue::promote(address_);
    SymbolicSemantics::SValue::Ptr value = SymbolicSemantics::SValue::promote(value_);
    if (address->nBits() != mem_->domainWidth() || value->nBits() != mem_->nBits()) {
        ASSERT_require2(mem_->isMemoryVariable(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->nBits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = SymbolicExpression::makeMemoryVariable(address->nBits(), value->nBits());
    }

    mem_ = SymbolicExpression::makeWrite(mem_, address->get_expression(), value->get_expression(), valOps->solver());
}

bool
SymbolicMemory::merge(const MemoryState::Ptr &other_, RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    SymbolicMemory::Ptr other = SymbolicMemory::promote(other_);
    TODO("[Robb P. Matzke 2015-08-10]");
}

void
SymbolicMemory::hash(Combinatorics::Hasher &hasher, RiscOperators*, RiscOperators*) const {
    hasher.insert(mem_->hash());
}

void
SymbolicMemory::print(std::ostream &out, Formatter &formatter_) const {
    SymbolicSemantics::Formatter *formatter = dynamic_cast<SymbolicSemantics::Formatter*>(&formatter_);
    SymbolicExpression::Formatter dflt_expr_formatter;
    SymbolicExpression::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;
    out <<formatter_.get_line_prefix() <<(*mem_ + expr_formatter) <<"\n";
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
