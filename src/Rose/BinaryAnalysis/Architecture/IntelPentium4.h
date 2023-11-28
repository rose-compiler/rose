#ifndef ROSE_BinaryAnalysis_Architecture_IntelPentium4_H
#define ROSE_BinaryAnalysis_Architecture_IntelPentium4_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel Pentium4.
 *
 *  Pentium 4 is a series of single-core CPUs for desktops, laptops and entry-level servers manufactured by Intel. The processors
 *  were shipped from November 20, 2000 until August 8, 2008. It was removed from the official price lists starting in 2010, being
 *  replaced by Pentium Dual-Core.
 *
 *  All Pentium 4 CPUs are based on the NetBurst microarchitecture. The Pentium 4 Willamette (180 nm) introduced SSE2, while the
 *  Prescott (90 nm) introduced SSE3. Later versions introduced Hyper-Threading Technology (HTT).
 *
 *  The first Pentium 4-branded processor to implement 64-bit was the Prescott (90 nm) (February 2004), but this feature was not
 *  enabled. Intel subsequently began selling 64-bit Pentium 4s using the "E0" revision of the Prescotts, being sold on the OEM
 *  market as the Pentium 4, model F. The E0 revision also adds eXecute Disable (XD) (Intel's name for the NX bit) to Intel
 *  64. Intel's official launch of Intel 64 (under the name EM64T at that time) in mainstream desktop processors was the N0 stepping
 *  Prescott-2M.
 *
 *  Intel also marketed a version of their low-end Celeron processors based on the NetBurst microarchitecture (often referred to as
 *  Celeron 4), and a high-end derivative, Xeon, intended for multi-socket servers and workstations. In 2005, the Pentium 4 was
 *  complemented by the dual-core-brands Pentium D and Pentium Extreme Edition. */
class IntelPentium4: public Base {
public:
    using Ptr = IntelPentium4Ptr;

protected:
    IntelPentium4();                                    // use `instance` instead
public:
    ~IntelPentium4();

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
