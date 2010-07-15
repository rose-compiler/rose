#ifndef SIMULATE_H
#define SIMULATE_H

#include "rose.h"

/* Define one CPP symbol to determine whether this simulator can be compiled.  The definition of this one symbol depends on
 * all the header file prerequisites. */
#if defined(HAVE_ASM_LDT_H) && defined(HAVE_ELF_H) && \
    defined(HAVE_LINUX_TYPES_H) && defined(HAVE_LINUX_DIRENT_H) && defined(HAVE_LINUX_UNISTD_H)
#  define ROSE_ENABLE_SIMULATOR
#else
#  undef ROSE_ENABLE_SIMULATOR
#endif

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole header file */

#include "mymemory.h"
#include "x86InstructionSemantics.h"
#include "integerOps.h"
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/ldt.h>

template <size_t Len>
struct Value {
    uint64_t val_;
    Value(uint64_t v): val_(v & IntegerOps::GenMask<uint64_t,Len>::value) {}
    uint64_t val() const {return val_;}
};

struct SegmentInfo {
  uint32_t base, limit;
  bool present;
  SegmentInfo(): base(0), limit(0), present(false) {}
  SegmentInfo(uint32_t base, uint32_t limit, bool present): base(base), limit(limit), present(present) {}
  SegmentInfo(const user_desc& ud):
    base(ud.base_addr),
    limit(ud.limit_in_pages ? ((ud.limit << 12) | 0xFFF) : ud.limit),
    present(!ud.seg_not_present && ud.useable) {}
};

struct MachineState {
  uint32_t gprs[8];
  uint32_t ip;
  bool flags[16];
  uint16_t segregs[6];
  SegmentInfo segregsShadow[6];
  user_desc gdt[8192];
  Memory memory;

  MachineState();
  ~MachineState();
  void dumpRegs() const;

  void writeSegreg(X86SegmentRegister sr, uint16_t val) {
#ifdef DEBUG
    fprintf(stderr, "writeSegreg(%s, %X)\n", segregToString(sr), (unsigned)val);
#endif
    ROSE_ASSERT ((val & 7) == 3); // GDT and privilege level 3
    segregs[sr] = val;
    segregsShadow[sr] = (SegmentInfo)gdt[val >> 3];
    ROSE_ASSERT (segregsShadow[sr].present);
#ifdef DEBUG
    dumpRegs();
#endif
  }

  template <size_t Len>
  uint64_t readMemory(X86SegmentRegister segreg, uint32_t address) {
#ifdef DEBUG
    fprintf(stderr, "readMemory(%s, %"PRIX32")\n", segregToString(segreg), address);
#endif
    BOOST_STATIC_ASSERT(Len % 8 == 0);
    ROSE_ASSERT (address <= segregsShadow[segreg].limit);
    ROSE_ASSERT (address + (Len / 8) - 1 <= segregsShadow[segreg].limit);
    return memory.read<Len / 8>(address + segregsShadow[segreg].base);
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, uint32_t address, uint64_t data) {
#ifdef DEBUG
    fprintf(stderr, "writeMemory(%s, %"PRIX32")\n", segregToString(segreg), address);
#endif
    BOOST_STATIC_ASSERT (Len % 8 == 0);
    ROSE_ASSERT (address <= segregsShadow[segreg].limit);
    ROSE_ASSERT (address + (Len / 8) - 1 <= segregsShadow[segreg].limit);
    memory.write<Len / 8>(address + segregsShadow[segreg].base, data);
  }
};

struct SignalQueue {
  static const int MAX_PENDING_SIGNALS = 32;
  int pendingSignals[MAX_PENDING_SIGNALS];
  int pendingSignalCount;
  int startEntry;

  SignalQueue(): pendingSignalCount(0), startEntry(0) {}
  void addSignal(int sig) volatile {
    assert (pendingSignalCount != MAX_PENDING_SIGNALS);
    pendingSignals[(startEntry + pendingSignalCount++) % MAX_PENDING_SIGNALS] = sig;
  }
  bool anySignalsWaiting() const volatile {
    return pendingSignalCount != 0;
  }
  int popSignal() volatile {
    assert (pendingSignalCount != 0);
    int sig = pendingSignals[startEntry++];
    --pendingSignalCount;
    if (startEntry == MAX_PENDING_SIGNALS) startEntry = 0;
    return sig;
  }
};

struct LinuxMachineState: public MachineState {
  uint32_t signalHandlers[_NSIG + 1];
  uint32_t signalStack;
  uint32_t brk;
  volatile SignalQueue signalQueue;
  LinuxMachineState(): MachineState(), signalStack(0), brk(0x1000000) {}
};

static inline bool simulate_signals_pending(LinuxMachineState& ms) {
  return ms.signalQueue.anySignalsWaiting();
}

extern void linuxSyscall(LinuxMachineState& ms);
extern void setup(LinuxMachineState& ms, int argc, char** argv);

class IncrementalDisassembler {
private:
    std::map<uint64_t /* address */,
             std::pair<std::vector<uint8_t> /* raw bytes */,
                       SgAsmx86Instruction* /* disassembled instruction */ >
            > insnMap;
    const Memory& memory;
    SgAsmx86Instruction* disassembleNewInstruction(uint64_t addr);
    Disassembler *disassembler;

public:
    IncrementalDisassembler(const Memory& memory): memory(memory), disassembler(NULL) {}
    void init_disassembler(SgAsmGenericHeader *hdr) {
        delete disassembler;
        disassembler = Disassembler::lookup(hdr)->clone();
        ROSE_ASSERT(disassembler!=NULL);
    }
    SgAsmx86Instruction* operator[](uint64_t addr);
};

#endif /* ROSE_ENABLE_SIMULATOR */
#endif /* SIMULATE_H */
