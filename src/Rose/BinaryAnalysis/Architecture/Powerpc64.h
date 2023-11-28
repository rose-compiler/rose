#ifndef ROSE_BinaryAnalysis_Architecture_Powerpc64_H
#define ROSE_BinaryAnalysis_Architecture_Powerpc64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for PowerPC with 64-bit word size.'
 *
 *  PowerPC (with the backronym Performance Optimization With Enhanced RISC – Performance Computing, sometimes abbreviated as PPC)
 *  is a reduced instruction set computer (RISC) instruction set architecture (ISA) created by the 1991 Apple–IBM–Motorola alliance,
 *  known as AIM. PowerPC, as an evolving instruction set, has been named Power ISA since 2006, while the old name lives on as a
 *  trademark for some implementations of Power Architecture–based processors.
 *
 *  PowerPC was the cornerstone of AIM's PReP and Common Hardware Reference Platform (CHRP) initiatives in the 1990s. Originally
 *  intended for personal computers, the architecture is well known for being used by Apple's Power Macintosh, PowerBook, iMac,
 *  iBook, eMac, Mac Mini, and Xserve lines from 1994 until 2006, when Apple migrated to Intel's x86. It has since become a niche in
 *  personal computers, but remains popular for embedded and high-performance processors. Its use in the 7th generation of video
 *  game consoles and embedded applications provide an array of uses, including satellites, and the Curiosity and Perseverance
 *  rovers on Mars. In addition, PowerPC CPUs are still used in AmigaOne and third party AmigaOS 4 personal computers.
 *
 *  PowerPC is largely based on the earlier IBM POWER architecture, and retains a high level of compatibility with it; the
 *  architectures have remained close enough that the same programs and operating systems will run on both if some care is taken in
 *  preparation; newer chips in the Power series use the Power ISA. */
class Powerpc64: public Base {
public:
    using Ptr = Powerpc64Ptr;

protected:
    explicit Powerpc64(ByteOrder::Endianness);          // use `instance` instead
public:
    ~Powerpc64();

public:
    /** Allocating constructor. */
    static Ptr instance(ByteOrder::Endianness);

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
