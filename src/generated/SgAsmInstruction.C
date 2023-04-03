#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmInstruction::get_mnemonic() const {
    return p_mnemonic;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_mnemonic(std::string const& x) {
    this->p_mnemonic = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmInstruction::get_raw_bytes() const {
    return p_raw_bytes;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_raw_bytes(SgUnsignedCharList const& x) {
    this->p_raw_bytes = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList* const&
SgAsmInstruction::get_operandList() const {
    return p_operandList;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_operandList(SgAsmOperandList* const& x) {
    this->p_operandList = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int64_t const&
SgAsmInstruction::get_stackDeltaIn() const {
    return p_stackDeltaIn;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_stackDeltaIn(int64_t const& x) {
    this->p_stackDeltaIn = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp* const&
SgAsmInstruction::get_semantics() const {
    return p_semantics;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::set_semantics(SgAsmExprListExp* const& x) {
    this->p_semantics = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::~SgAsmInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::SgAsmInstruction()
    : p_operandList(nullptr)
    , p_cacheLockCount(0)
    , p_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA)
    , p_semantics(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmInstruction::SgAsmInstruction(rose_addr_t const& address,
                                   std::string const& mnemonic)
    : SgAsmStatement(address)
    , p_mnemonic(mnemonic) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstruction::initializeProperties() {
    p_operandList = nullptr;
    p_cacheLockCount = 0;
    p_stackDeltaIn = SgAsmInstruction::INVALID_STACK_DELTA;
    p_semantics = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
