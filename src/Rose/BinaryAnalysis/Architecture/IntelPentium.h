#ifndef ROSE_BinaryAnalysis_Architecture_IntelPentium_H
#define ROSE_BinaryAnalysis_Architecture_IntelPentium_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel Pentium.
 *
 *  The Pentium (also referred to as P5, its microarchitecture, or i586) is a fifth generation, 32-bit x86 microprocessor that was
 *  introduced by Intel on March 22, 1993, as the very first CPU in the Pentium brand. It was instruction set compatible with the
 *  80486 but was a new and very different microarchitecture design from previous iterations. The P5 Pentium was the first
 *  superscalar x86 microarchitecture and the world's first superscalar microprocessor to be in mass production—meaning it generally
 *  executes at least 2 instructions per clock mainly because of a design-first dual integer pipeline design previously thought
 *  impossible to implement on a CISC microarchitecture. Additional features include a faster floating-point unit, wider data bus,
 *  separate code and data caches, and many other techniques and features to enhance performance and support security, encryption,
 *  and multiprocessing, for workstations and servers when compared to the next best previous industry standard processor
 *  implementation before it, the Intel 80486.
 *
 *  Considered the fifth main generation in the 8086 compatible line of processors, its implementation and microarchitecture was
 *  called P5. As with all new processors from Intel since the Pentium, some new instructions were added to enhance performance for
 *  specific types of workloads.
 *
 *  The Pentium was the first Intel x86 to build in robust hardware support for multiprocessing similar to that of large IBM
 *  mainframe computers. Intel worked closely with IBM to define this ability and then Intel designed it into the P5
 *  microarchitecture. This new ability was absent in prior x86 generations and x86 copies from competitors. */
class IntelPentium: public Base {
public:
    using Ptr = IntelPentiumPtr;

protected:
    IntelPentium();                                     // use `instance` instead
public:
    ~IntelPentium();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
