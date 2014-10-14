#include "sage3basic.h"
#include <Partitioner2/Semantics.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    if (map_ && addr->is_number()) {
        ASSERT_require2(8==dflt->get_width(), "multi-byte reads should have been handled above this call");
        uint8_t byte;
        size_t nRead = map_->at(addr->get_number()).limit(1)
                       .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE)
                       .read(&byte).size();
        if (1==nRead)
            return valOps->number_(8, byte);
    }
    return SymbolicSemantics::MemoryState::readMemory(addr, dflt, addrOps, valOps);
}
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Risc Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::SValuePtr
RiscOperators::trim(const BaseSemantics::SValuePtr &a_) {
    if (trimThreshold_ > 0) {
        SValuePtr a = SValue::promote(a_);
        if (a->get_expression()->nnodes() > trimThreshold_)
            return get_protoval()->undefined_(a->get_width());
    }
    return a_;
}

void
RiscOperators::interrupt(int majr, int minr) {
    SymbolicSemantics::RiscOperators::interrupt(majr, minr);
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::and_(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::or_(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::xor_(a, b));
}
    
BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a) {
    return trim(SymbolicSemantics::RiscOperators::invert(a));
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a, size_t begin_bit, size_t end_bit) {
    return trim(SymbolicSemantics::RiscOperators::extract(a, begin_bit, end_bit));
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::concat(a, b));
}
    
BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    return trim(SymbolicSemantics::RiscOperators::leastSignificantSetBit(a));
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    return trim(SymbolicSemantics::RiscOperators::mostSignificantSetBit(a));
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return trim(SymbolicSemantics::RiscOperators::rotateLeft(a, sa));
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return trim(SymbolicSemantics::RiscOperators::rotateRight(a, sa));
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return trim(SymbolicSemantics::RiscOperators::shiftLeft(a, sa));
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return trim(SymbolicSemantics::RiscOperators::shiftRight(a, sa));
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return trim(SymbolicSemantics::RiscOperators::shiftRightArithmetic(a, sa));
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a) {
    return trim(SymbolicSemantics::RiscOperators::equalToZero(a));
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::ite(sel, a, b));
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a, size_t new_width) {
    return trim(SymbolicSemantics::RiscOperators::unsignedExtend(a, new_width));
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a, size_t new_width) {
    return trim(SymbolicSemantics::RiscOperators::signExtend(a, new_width));
}
    
BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::add(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/) {
    BaseSemantics::SValuePtr result = SymbolicSemantics::RiscOperators::addWithCarries(a, b, c, carry_out);
    carry_out = trim(carry_out);
    return trim(result);
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a) {
    return trim(SymbolicSemantics::RiscOperators::negate(a));
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::signedDivide(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::signedModulo(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::signedMultiply(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::unsignedDivide(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::unsignedModulo(a, b));
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return trim(SymbolicSemantics::RiscOperators::unsignedMultiply(a, b));
}
    
BaseSemantics::SValuePtr
RiscOperators::readRegister(const RegisterDescriptor &reg) {
    return trim(SymbolicSemantics::RiscOperators::readRegister(reg));
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    return trim(SymbolicSemantics::RiscOperators::readMemory(segreg, addr, dflt, cond));
}

} // namespace
} // namespace
} // namespace
} // namespace
