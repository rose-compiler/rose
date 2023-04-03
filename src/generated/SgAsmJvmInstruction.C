#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::JvmInstructionKind const&
SgAsmJvmInstruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInstruction::set_kind(Rose::BinaryAnalysis::JvmInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::~SgAsmJvmInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::SgAsmJvmInstruction()
    : p_kind(Rose::BinaryAnalysis::JvmInstructionKind::unknown) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmJvmInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInstruction::SgAsmJvmInstruction(rose_addr_t const& address,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::JvmInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::JvmInstructionKind::unknown;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
