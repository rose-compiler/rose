#ifndef ROSE_BinaryAnalysis_Architecture_Cil_H
#define ROSE_BinaryAnalysis_Architecture_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for the Common Intermediate Language.
 *
 *  Common Intermediate Language (CIL), formerly called Microsoft Intermediate Language (MSIL) or Intermediate Language (IL), is the
 *  intermediate language binary instruction set defined within the Common Language Infrastructure (CLI) specification. CIL
 *  instructions are executed by a CLI-compatible runtime environment such as the Common Language Runtime. Languages which target
 *  the CLI compile to CIL. CIL is object-oriented, stack-based bytecode. Runtimes typically just-in-time compile CIL instructions
 *  into native code.
 *
 *  CIL was originally known as Microsoft Intermediate Language (MSIL) during the beta releases of the .NET languages. Due to
 *  standardization of C# and the CLI, the bytecode is now officially known as CIL. Windows Defender virus definitions continue to
 *  refer to binaries compiled with it as MSIL. */
class Cil: public Base {
public:
    using Ptr = CilPtr;

protected:
    Cil();                                              // use `instance` instead
public:
    ~Cil();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;

    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override;
    Alignment instructionAlignment() const override;
    std::string instructionMnemonic(const SgAsmInstruction*) const override;
    std::string instructionDescription(const SgAsmInstruction*) const override;
    bool isUnknown(const SgAsmInstruction*) const override;
    bool terminatesBasicBlock(SgAsmInstruction*) const override;
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const override;
    Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const override;
    AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const override;

    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
