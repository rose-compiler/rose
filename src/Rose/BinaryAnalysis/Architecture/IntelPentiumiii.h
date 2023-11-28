#ifndef ROSE_BinaryAnalysis_Architecture_IntelPentiumiii_H
#define ROSE_BinaryAnalysis_Architecture_IntelPentiumiii_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel Pentium III.
 *
 *  The Pentium III (marketed as Intel Pentium III Processor, informally PIII or P3) brand refers to Intel's 32-bit x86 desktop and
 *  mobile CPUs based on the sixth-generation P6 microarchitecture introduced on February 28, 1999. The brand's initial processors
 *  were very similar to the earlier Pentium II-branded processors. The most notable differences were the addition of the Streaming
 *  SIMD Extensions (SSE) instruction set (to accelerate floating point and parallel calculations), and the introduction of a
 *  controversial serial number embedded in the chip during manufacturing. The Pentium III is also a single-core processor.
 *
 *  Even after the release of the Pentium 4 in late 2000, the Pentium III continued to be produced with new models introduced up
 *  until early 2003. They were then discontinued in April 2004 for desktop units and May 2007 for mobile units. */
class IntelPentiumiii: public Base {
public:
    using Ptr = IntelPentiumiiiPtr;

protected:
    IntelPentiumiii();                                  // use `instance` instead
public:
    ~IntelPentiumiii();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
