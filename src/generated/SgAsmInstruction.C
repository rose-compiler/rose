#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmInstruction::get_mnemonic() const {
    return p_mnemonic;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_mnemonic(std::string const& x) {
    this->p_mnemonic = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmInstruction::get_raw_bytes() const {
    return p_raw_bytes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_raw_bytes(SgUnsignedCharList const& x) {
    this->p_raw_bytes = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList* const&
SgAsmInstruction::get_operandList() const {
    return p_operandList;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_operandList(SgAsmOperandList* const& x) {
    this->p_operandList = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int64_t const&
SgAsmInstruction::get_stackDeltaIn() const {
    return p_stackDeltaIn;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_stackDeltaIn(int64_t const& x) {
    this->p_stackDeltaIn = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp* const&
SgAsmInstruction::get_semantics() const {
    return p_semantics;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_semantics(SgAsmExprListExp* const& x) {
    this->p_semantics = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::~SgAsmInstruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::SgAsmInstruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_operandList(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_cacheLockCount(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_semantics(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::SgAsmInstruction(rose_addr_t const& address,
                                   std::string const& mnemonic)
    : SgAsmStatement(address)
    , p_mnemonic(mnemonic)
    , p_operandList(nullptr)
    , p_cacheLockCount(0)
    , p_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA)
    , p_semantics(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::initializeProperties() {
    p_operandList = nullptr;
    p_cacheLockCount = 0;
    p_stackDeltaIn = SgAsmInstruction::INVALID_STACK_DELTA;
    p_semantics = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
