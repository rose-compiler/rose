#ifndef ROSE_BinaryAnalysis_Architecture_Amd64_H
#define ROSE_BinaryAnalysis_Architecture_Amd64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for x86-64, x86_64, x64, amd64, Intel 64.
 *
 *  x86-64 (also known as x64, x86_64, AMD64, and Intel 64) is a 64-bit version of the x86 instruction set, announced in 1999. It
 *  introduced two new modes of operation, 64-bit mode and compatibility mode, along with a new 4-level paging mode.
 *
 *  With 64-bit mode and the new paging mode, it supports vastly larger amounts of virtual memory and physical memory than was
 *  possible on its 32-bit predecessors, allowing programs to store larger amounts of data in memory. x86-64 also expands
 *  general-purpose registers to 64-bit, and expands the number of them from 8 (some of which had limited or fixed functionality,
 *  e.g. for stack management) to 16 (fully general), and provides numerous other enhancements. Floating-point arithmetic is
 *  supported via mandatory SSE2-like instructions, and x87/MMX style registers are generally not used (but still available even in
 *  64-bit mode); instead, a set of 16 vector registers, 128 bits each, is used. (Each register can store one or two
 *  double-precision numbers or one to four single-precision numbers, or various integer formats.) In 64-bit mode, instructions are
 *  modified to support 64-bit operands and 64-bit addressing mode.
 *
 *  The compatibility mode defined in the architecture allows 16-bit and 32-bit user applications to run unmodified, coexisting with
 *  64-bit applications if the 64-bit operating system supports them. As the full x86 16-bit and 32-bit instruction sets remain
 *  implemented in hardware without any intervening emulation, these older executables can run with little or no performance
 *  penalty, while newer or modified applications can take advantage of new features of the processor design to achieve performance
 *  improvements. Also, a processor supporting x86-64 still powers on in real mode for full backward compatibility with the 8086, as
 *  x86 processors supporting protected mode have done since the 80286.
 *
 *  The original specification, created by AMD and released in 2000, has been implemented by AMD, Intel, and VIA. The AMD K8
 *  microarchitecture, in the Opteron and Athlon 64 processors, was the first to implement it. This was the first significant
 *  addition to the x86 architecture designed by a company other than Intel. Intel was forced to follow suit and introduced a
 *  modified NetBurst family which was software-compatible with AMD's specification. VIA Technologies introduced x86-64 in their VIA
 *  Isaiah architecture, with the VIA Nano.
 *
 *  The x86-64 architecture was quickly adopted for desktop and laptop personal computers and servers which were commonly configured
 *  for 16GB of memory or more. It has effectively replaced the discontinued Intel Itanium architecture (formerly IA-64), which was
 *  originally intended to replace the x86 architecture. x86-64 and Itanium are not compatible on the native instruction set level,
 *  and operating systems and applications compiled for one architecture cannot be run on the other natively. */
class Amd64: public X86 {
public:
    using Ptr = Amd64Ptr;

protected:
    Amd64();                                            // use `instance` instead
public:
    ~Amd64();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override;
    Alignment instructionAlignment() const override;
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
    const CallingConvention::Dictionary& callingConventions() const override;

protected:
    CallingConvention::DefinitionPtr cc_sysv() const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
