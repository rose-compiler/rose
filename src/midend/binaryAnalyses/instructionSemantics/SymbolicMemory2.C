#include "sage3basic.h"
#include "SymbolicMemory2.h"
#include "SymbolicSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

void
SymbolicMemory::expression(const SymbolicExpr::TreeNodePtr &expr) {
    ASSERT_not_null(expr);
    ASSERT_require2(!expr->isScalar(), "expression must be a memory state");
    mem_ = expr;
}

void
SymbolicMemory::clear() {
    if (mem_) {
        mem_ = SymbolicExpr::LeafNode::create_memory(mem_->domainWidth(), mem_->get_nbits());
    } else {
        mem_ = SymbolicExpr::LeafNode::create_memory(32, 8); // can be adjusted later
    }
}

SValuePtr
SymbolicMemory::readMemory(const SValuePtr &address_, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    using namespace SymbolicExpr;
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    if (address->get_width() != mem_->domainWidth() || dflt->get_width() != mem_->get_nbits()) {
        ASSERT_require2(mem_->isLeafNode() && mem_->isLeafNode()->is_memory(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->get_nbits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = LeafNode::create_memory(address->get_width(), dflt->get_width());
    }
    TreeNodePtr resultExpr = InternalNode::create(8, OP_READ, mem_, address->get_expression());
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(dflt->copy());
    retval->set_expression(resultExpr);
    return retval;
}

void
SymbolicMemory::writeMemory(const SValuePtr &address_, const SValuePtr &value_, RiscOperators *addrOps, RiscOperators *valOps) {
    using namespace SymbolicExpr;
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    SymbolicSemantics::SValuePtr value = SymbolicSemantics::SValue::promote(value_);
    if (address->get_width() != mem_->domainWidth() || value->get_width() != mem_->get_nbits()) {
        ASSERT_require2(mem_->isLeafNode() && mem_->isLeafNode()->is_memory(),
                        "invalid address and/or value size for memory; expecting " +
                        StringUtility::numberToString(mem_->domainWidth()) + "-bit addresses and " +
                        StringUtility::numberToString(mem_->get_nbits()) + "-bit values");

        // We can finalize the domain and range widths for the memory now that they've been given.
        mem_ = LeafNode::create_memory(address->get_width(), value->get_width());
    }

    mem_ = InternalNode::create(mem_->get_nbits(), OP_WRITE, mem_, address->get_expression(), value->get_expression());
}

bool
SymbolicMemory::merge(const BaseSemantics::MemoryStatePtr &other_, BaseSemantics::RiscOperators *addrOps,
                      BaseSemantics::RiscOperators *valOps) {
    SymbolicMemoryPtr other = SymbolicMemory::promote(other_);
    TODO("[Robb P. Matzke 2015-08-10]");
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
