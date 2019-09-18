#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <SymbolicSemantics2.h>
#include <Registers.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

int
main() {
    const RegisterDictionary *regdict = RegisterDictionary::dictionary_i386();
    SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(regdict);
    RegisterStateGenericPtr rstate = RegisterStateGeneric::promote(ops->currentState()->registerState());
    SValuePtr x;
    Formatter fmt;
    fmt.set_line_prefix("    ");

    // Write all 32 bits of a register
    std::cout <<"write reg@0+32\n";
    RegisterDescriptor r_0_32(x86_regclass_gpr, 0, 0, 32);
    rstate->writeRegister(r_0_32, ops->undefined_(32), ops.get());
    rstate->print(std::cout, fmt);

    // Write low-order 16 bits of a register
    std::cout <<"\nwrite reg@0+16\n";
    RegisterDescriptor r_0_16(x86_regclass_gpr, 0, 0, 16);
    rstate->writeRegister(r_0_16, ops->undefined_(16), ops.get());
    rstate->print(std::cout, fmt);

    // Read bits 8-31. Bits [8-15] are from one register and [16-31] are from another
    std::cout <<"\nread reg@8+24\n";
    RegisterDescriptor r_8_24(x86_regclass_gpr, 0, 8, 24);
    x = rstate->readRegister(r_8_24, ops->undefined_(r_8_24.nBits()), ops.get());
    std::cout <<"  got " <<*x <<"\n";
    rstate->print(std::cout, fmt);
}

#endif
