#ifndef ROSE_BinaryAnalysis_Architecture_Mips32_H
#define ROSE_BinaryAnalysis_Architecture_Mips32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for MIPS with 32-bit word size.'
 *
 *  MIPS (Microprocessor without Interlocked Pipelined Stages)[1] is a family of reduced instruction set computer (RISC) instruction
 *  set architectures (ISA) developed by MIPS Computer Systems, now MIPS Technologies, based in the United States.
 *
 *  There are multiple versions of MIPS: including MIPS I, II, III, IV, and V; as well as five releases of MIPS32/64 (for 32- and
 *  64-bit implementations, respectively). The early MIPS architectures were 32-bit; 64-bit versions were developed later. As of
 *  April 2017, the current version of MIPS is MIPS32/64 Release 6. MIPS32/64 primarily differs from MIPS I–V by defining the
 *  privileged kernel mode System Control Coprocessor in addition to the user mode architecture.
 *
 *  The MIPS architecture has several optional extensions. MIPS-3D which is a simple set of floating-point SIMD instructions
 *  dedicated to common 3D tasks, MDMX (MaDMaX) which is a more extensive integer SIMD instruction set using the 64-bit
 *  floating-point registers, MIPS16e which adds compression to the instruction stream to make programs take up less room, and
 *  MIPS MT, which adds multithreading capability. */
class Mips32: public Base {
public:
    using Ptr = Mips32Ptr;

protected:
    explicit Mips32(ByteOrder::Endianness);             // use `instance` instead
public:
    ~Mips32();

public:
    /** Allocating constructor. */
    static Ptr instance(ByteOrder::Endianness);

public:
    RegisterDictionaryPtr registerDictionary() const override;
    RegisterDictionaryPtr interruptDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;

    std::string instructionDescription(const SgAsmInstruction*) const override;
    bool isUnknown(const SgAsmInstruction*) const override;
    bool terminatesBasicBlock(SgAsmInstruction*) const override;
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const override;
    Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const override;
    AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const override;

    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;

    InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;

    std::vector<Partitioner2::FunctionPrologueMatcherPtr>
    functionPrologueMatchers(const Partitioner2::EnginePtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
