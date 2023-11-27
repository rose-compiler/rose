#ifndef ROSE_BinaryAnalysis_Architecture_IntelPentiumii_H
#define ROSE_BinaryAnalysis_Architecture_IntelPentiumii_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel Pentium II.
 *
 *  The Pentium II brand refers to Intel's sixth-generation microarchitecture ("P6") and x86-compatible microprocessors
 *  introduced on May 7, 1997. Containing 7.5 million transistors (27.4 million in the case of the mobile Dixon with 256 KB L2
 *  cache), the Pentium II featured an improved version of the first P6-generation core of the Pentium Pro, which contained 5.5
 *  million transistors. However, its L2 cache subsystem was a downgrade when compared to the Pentium Pros. It is a single-core
 *  microprocessor.
 *
 *  In 1998, Intel stratified the Pentium II family by releasing the Pentium II-based Celeron line of processors for low-end
 *  computers and the Pentium II Xeon line for servers and workstations. The Celeron was characterized by a reduced or omitted (in
 *  some cases present but disabled) on-die full-speed L2 cache and a 66 MT/s FSB. The Xeon was characterized by a range of
 *  full-speed L2 cache (from 512 KB to 2048 KB), a 100 MT/s FSB, a different physical interface (Slot 2), and support for symmetric
 *  multiprocessing. */
class IntelPentiumii: public Base {
public:
    using Ptr = IntelPentiumiiPtr;

protected:
    IntelPentiumii();                                   // use `instance` instead
public:
    ~IntelPentiumii();

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
