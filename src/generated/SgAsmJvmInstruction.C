#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::JvmInstructionKind const&
SgAsmJvmInstruction::get_kind() const {
    return p_kind;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInstruction::set_kind(Rose::BinaryAnalysis::JvmInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::~SgAsmJvmInstruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::SgAsmJvmInstruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_kind(Rose::BinaryAnalysis::JvmInstructionKind::unknown) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmJvmInstruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::SgAsmJvmInstruction(rose_addr_t const& address,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::JvmInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::JvmInstructionKind::unknown;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
