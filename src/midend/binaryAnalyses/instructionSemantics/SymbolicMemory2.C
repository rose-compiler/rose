#include "sage3basic.h"
#include "SymbolicMemory2.h"
#include "SymbolicSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

SValuePtr
SymbolicMemory::readMemory(const SValuePtr &address_, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    using namespace InsnSemanticsExpr;
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    TreeNodePtr resultExpr = InternalNode::create(8, OP_READ, mem_, address->get_expression());
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(dflt->copy());
    retval->set_expression(resultExpr);
    return retval;
}

void
SymbolicMemory::writeMemory(const SValuePtr &address_, const SValuePtr &value_, RiscOperators *addrOps, RiscOperators *valOps) {
    using namespace InsnSemanticsExpr;
    SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(address_);
    SymbolicSemantics::SValuePtr value = SymbolicSemantics::SValue::promote(value_);
    mem_ = InternalNode::create(mem_->get_nbits(), OP_WRITE, mem_, address->get_expression(), value->get_expression());
}

void
SymbolicMemory::print(std::ostream &out, Formatter &formatter_) const {
    SymbolicSemantics::Formatter *formatter = dynamic_cast<SymbolicSemantics::Formatter*>(&formatter_);
    InsnSemanticsExpr::Formatter dflt_expr_formatter;
    InsnSemanticsExpr::Formatter &expr_formatter = formatter ? formatter->expr_formatter : dflt_expr_formatter;
    out <<(*mem_ + expr_formatter) <<"\n";
}


} // namespace
} // namespace
} // namespace
} // namespace
