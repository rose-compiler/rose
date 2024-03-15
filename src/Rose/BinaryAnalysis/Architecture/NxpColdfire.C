#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

NxpColdfire::NxpColdfire()
    : Motorola("nxp-coldfire") {}

NxpColdfire::~NxpColdfire() {}

NxpColdfire::Ptr
NxpColdfire::instance() {
    return Ptr(new NxpColdfire);
}

RegisterDictionary::Ptr
NxpColdfire::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("motorola-68040").orThrow()->registerDictionary());

        // Floating point data registers.
        // The ColdFire processors do not support extended precision real ("X") format values, and therefore don't need
        // full 80-bit floating point data registers.  Their FP data registers are only 64 bits.
        for (int i=0; i<8; ++i)
            regs->resize("fp"+StringUtility::numberToString(i), 64);

        // MAC unit
        regs->insert("macsr",     m68k_regclass_mac, m68k_mac_macsr, 0, 32);    // MAC status register
        regs->insert("macsr_c",   m68k_regclass_mac, m68k_mac_macsr, 0,  1);    //   Carry flag; this field is always zero.
        regs->insert("macsr_v",   m68k_regclass_mac, m68k_mac_macsr, 1,  1);    //   Overflow flag
        regs->insert("macsr_z",   m68k_regclass_mac, m68k_mac_macsr, 2,  1);    //   Zero flag
        regs->insert("macsr_n",   m68k_regclass_mac, m68k_mac_macsr, 3,  1);    //   Negative flag
        regs->insert("macsr_rt",  m68k_regclass_mac, m68k_mac_macsr, 4,  1);    //   Round/truncate mode
        regs->insert("macsr_fi",  m68k_regclass_mac, m68k_mac_macsr, 5,  1);    //   Fraction/integer mode
        regs->insert("macsr_su",  m68k_regclass_mac, m68k_mac_macsr, 6,  1);    //   Signed/unsigned operations mode
        regs->insert("macsr_omc", m68k_regclass_mac, m68k_mac_macsr, 7,  1);    //   Overflow/saturation mode
        regs->insert("acc",       m68k_regclass_mac, m68k_mac_acc0,  0, 32);    // MAC accumulator
        regs->insert("mask",      m68k_regclass_mac, m68k_mac_mask,  0, 32);    // MAC mask register (upper 16 bits are set)

        // Supervisor registers
        regs->insert("cacr",     m68k_regclass_sup, m68k_sup_cacr,      0, 32); // cache control register
        regs->insert("asid",     m68k_regclass_sup, m68k_sup_asid,      0, 32); // address space ID register
        regs->insert("acr0",     m68k_regclass_sup, m68k_sup_acr0,      0, 32); // access control register 0 (data)
        regs->insert("acr1",     m68k_regclass_sup, m68k_sup_acr1,      0, 32); // access control register 1 (data)
        regs->insert("acr2",     m68k_regclass_sup, m68k_sup_acr2,      0, 32); // access control register 2 (instruction)
        regs->insert("acr3",     m68k_regclass_sup, m68k_sup_acr3,      0, 32); // access control register 3 (instruction)
        regs->insert("mmubar",   m68k_regclass_sup, m68k_sup_mmubar,    0, 32); // MMU base address register
        regs->insert("rombar0",  m68k_regclass_sup, m68k_sup_rombar0,   0, 32); // ROM base address register 0
        regs->insert("rombar1",  m68k_regclass_sup, m68k_sup_rombar1,   0, 32); // ROM base address register 1
        regs->insert("rambar0",  m68k_regclass_sup, m68k_sup_rambar0,   0, 32); // RAM base address register 0
        regs->insert("rambar1",  m68k_regclass_sup, m68k_sup_rambar1,   0, 32); // RAM base address register 1
        regs->insert("mbar",     m68k_regclass_sup, m68k_sup_mbar,      0, 32); // module base address register
        regs->insert("mpcr",     m68k_regclass_sup, m68k_sup_mpcr,      0, 32); // multiprocessor control register
        regs->insert("edrambar", m68k_regclass_sup, m68k_sup_edrambar,  0, 32); // embedded DRAM base address register
        regs->insert("secmbar",  m68k_regclass_sup, m68k_sup_secmbar,   0, 32); // secondary module base address register
        regs->insert("pcr1u0",   m68k_regclass_sup, m68k_sup_0_pcr1,   32, 32); // 32 msbs of RAM 0 permutation control reg 1
        regs->insert("pcr1l0",   m68k_regclass_sup, m68k_sup_0_pcr1,    0, 32); // 32 lsbs of RAM 0 permutation control reg 1
        regs->insert("pcr2u0",   m68k_regclass_sup, m68k_sup_0_pcr2,   32, 32); // 32 msbs of RAM 0 permutation control reg 2
        regs->insert("pcr2l0",   m68k_regclass_sup, m68k_sup_0_pcr2,    0, 32); // 32 lsbs of RAM 0 permutation control reg 2
        regs->insert("pcr3u0",   m68k_regclass_sup, m68k_sup_0_pcr3,   32, 32); // 32 msbs of RAM 0 permutation control reg 3
        regs->insert("pcr3l0",   m68k_regclass_sup, m68k_sup_0_pcr3,    0, 32); // 32 lsbs of RAM 0 permutation control reg 3
        regs->insert("pcr1u1",   m68k_regclass_sup, m68k_sup_1_pcr1,   32, 32); // 32 msbs of RAM 1 permutation control reg 1
        regs->insert("pcr1l1",   m68k_regclass_sup, m68k_sup_1_pcr1,    0, 32); // 32 lsbs of RAM 1 permutation control reg 1
        regs->insert("pcr2u1",   m68k_regclass_sup, m68k_sup_1_pcr2,   32, 32); // 32 msbs of RAM 1 permutation control reg 2
        regs->insert("pcr2l1",   m68k_regclass_sup, m68k_sup_1_pcr2,    0, 32); // 32 lsbs of RAM 1 permutation control reg 2
        regs->insert("pcr3u1",   m68k_regclass_sup, m68k_sup_1_pcr3,   32, 32); // 32 msbs of RAM 1 permutation control reg 3
        regs->insert("pcr3l1",   m68k_regclass_sup, m68k_sup_1_pcr3,    0, 32); // 32 lsbs of RAM 1 permutation control reg 3

        // FreeScale ColdFire CPUs with EMAC (extended multiply-accumulate) unit.
        regs->insert("macsr_pav0", m68k_regclass_mac, m68k_mac_macsr,  8,  1);  // overflow flag for accumulator 0
        regs->insert("macsr_pav1", m68k_regclass_mac, m68k_mac_macsr,  9,  1);  // overflow flag for accumulator 1
        regs->insert("macsr_pav2", m68k_regclass_mac, m68k_mac_macsr, 10,  1);  // overflow flag for accumulator 2
        regs->insert("macsr_pav3", m68k_regclass_mac, m68k_mac_macsr, 11,  1);  // overflow flag for accumulator 3

        regs->insert("acc0",       m68k_regclass_mac, m68k_mac_acc0,   0, 32);  // accumulator #0
        regs->insert("acc1",       m68k_regclass_mac, m68k_mac_acc1,   0, 32);  // accumulator #1
        regs->insert("acc2",       m68k_regclass_mac, m68k_mac_acc2,   0, 32);  // accumulator #2
        regs->insert("acc3",       m68k_regclass_mac, m68k_mac_acc3,   0, 32);  // accumulator #3

        regs->insert("accext01",   m68k_regclass_mac, m68k_mac_ext01,  0, 32);  // extensions for acc0 and acc1
        regs->insert("accext0",    m68k_regclass_mac, m68k_mac_ext0,   0, 16);
        regs->insert("accext1",    m68k_regclass_mac, m68k_mac_ext1,  16, 16);
        regs->insert("accext23",   m68k_regclass_mac, m68k_mac_ext23,  0, 32);  // extensions for acc2 and acc3
        regs->insert("accext2",    m68k_regclass_mac, m68k_mac_ext2,   0, 16);
        regs->insert("accext3",    m68k_regclass_mac, m68k_mac_ext3,  16, 16);

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
NxpColdfire::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

Disassembler::Base::Ptr
NxpColdfire::newInstructionDecoder() const {
    return Disassembler::M68k::instance(shared_from_this(), m68k_freescale_cpu32);
}

} // namespace
} // namespace
} // namespace

#endif
