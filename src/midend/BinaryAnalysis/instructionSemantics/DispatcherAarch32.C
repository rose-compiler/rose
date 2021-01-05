#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <DispatcherAarch32.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A32/T32 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Aarch32 {
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherAarch32::initializeInsnDispatchTable() {}

void
DispatcherAarch32::initializeRegisterDescriptors() {}

void
DispatcherAarch32::initializeMemory() {}

RegisterDescriptor
DispatcherAarch32::instructionPointerRegister() const {
    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
}

RegisterDescriptor
DispatcherAarch32::stackPointerRegister() const {
    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
}

RegisterDescriptor
DispatcherAarch32::callReturnRegister() const {
    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
}

void
DispatcherAarch32::set_register_dictionary(const RegisterDictionary *regdict) {
    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
}

int
DispatcherAarch32::iproc_key(SgAsmInstruction *insn_) const {
    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherAarch32);
#endif

#endif
