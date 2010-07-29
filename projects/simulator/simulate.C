#include "rose.h"
#include "simulate.h"

#ifdef ROSE_ENABLE_SIMULATOR /*protects this whole file*/

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

using namespace std;

LinuxMachineState* targetForSignals = 0;

MachineState::MachineState() {
  ip = 0;
  for (size_t i = 0; i < 8; ++i) gprs[i] = 0;
  for (size_t i = 0; i < 16; ++i) flags[i] = 0;

  // Initialize gdt using memset because some fields are not defined
  // when compiling on linux i686.
  memset(gdt, 0, sizeof gdt);
  gdt[0x23 >> 3].entry_number = (0x23 >> 3);
  gdt[0x23 >> 3].base_addr = 0;
  gdt[0x23 >> 3].limit = 0xFFFFF;
  gdt[0x23 >> 3].seg_32bit = 1;
  gdt[0x23 >> 3].contents = 0; // Don't know what this is
  gdt[0x23 >> 3].read_exec_only = 1;
  gdt[0x23 >> 3].limit_in_pages = 1;
  gdt[0x23 >> 3].seg_not_present = 0;
  gdt[0x23 >> 3].useable = 1;
  gdt[0x2B >> 3].entry_number = (0x2B >> 3);
  gdt[0x2B >> 3].base_addr = 0;
  gdt[0x2B >> 3].limit = 0xFFFFF;
  gdt[0x2B >> 3].seg_32bit = 1;
  gdt[0x2B >> 3].contents = 0; // Don't know what this is
  gdt[0x2B >> 3].read_exec_only = 0;
  gdt[0x2B >> 3].limit_in_pages = 1;
  gdt[0x2B >> 3].seg_not_present = 0;
  gdt[0x2B >> 3].useable = 1;

  writeSegreg(x86_segreg_cs, 0x23);
  writeSegreg(x86_segreg_ds, 0x2B);
  writeSegreg(x86_segreg_es, 0x2B);
  writeSegreg(x86_segreg_ss, 0x2B);
  writeSegreg(x86_segreg_fs, 0x2B);
  writeSegreg(x86_segreg_gs, 0x2B);
}

MachineState::~MachineState() {
}

void MachineState::dumpRegs() const {
    fprintf(stderr, "  Machine state:\n");
    fprintf(stderr, "    eax=0x%08x ebx=0x%08x ecx=0x%08x edx=0x%08x\n",
            gprs[x86_gpr_ax], gprs[x86_gpr_bx], gprs[x86_gpr_cx], gprs[x86_gpr_dx]);
    fprintf(stderr, "    esi=0x%08x edi=0x%08x ebp=0x%08x esp=0x%08x eip=0x%08x\n",
            gprs[x86_gpr_si], gprs[x86_gpr_di], gprs[x86_gpr_bp], gprs[x86_gpr_sp], ip);
    for (int i=0; i<6; i++) {
        X86SegmentRegister sr = (X86SegmentRegister)i;
        fprintf(stderr, "    %s=0x%04x base=0x%08x limit=0x%08x present=%s\n",
                segregToString(sr), segregs[sr], segregsShadow[sr].base, segregsShadow[sr].limit,
                segregsShadow[sr].present?"yes":"no");
    }
    fprintf(stderr, "    flags: %s %s %s %s %s %s %s\n", 
            flags[x86_flag_of]?"ov":"nv", flags[x86_flag_df]?"dn":"up", flags[x86_flag_sf]?"ng":"pl", 
            flags[x86_flag_zf]?"zr":"nz", flags[x86_flag_af]?"ac":"na", flags[x86_flag_pf]?"pe":"po", 
            flags[x86_flag_cf]?"cy":"nc");
}

void simulate_signal(int sig) {
  ROSE_ASSERT (targetForSignals);
  LinuxMachineState& ms = *targetForSignals;
  // write(2, "Got signal: ", 12);
  // write(2, sys_siglist[sig], strlen(sys_siglist[sig]));
  // write(2, "\n", 1);
  ms.signalQueue.addSignal(sig);
}

static inline uint32_t pop(LinuxMachineState& ms) {
  return ms.readMemory<32>(x86_segreg_ss, --ms.gprs[x86_gpr_sp]);
}

static inline void push(LinuxMachineState& ms, uint32_t val) {
  ms.writeMemory<32>(x86_segreg_ss, ms.gprs[x86_gpr_sp]++, val);
}

void simulate_sigreturn(LinuxMachineState& ms) {
  uint32_t retAddr = pop(ms);
  pop(ms); // Signal number
  ms.gprs[x86_gpr_bp] = pop(ms);
  ms.gprs[x86_gpr_di] = pop(ms);
  ms.gprs[x86_gpr_si] = pop(ms);
  ms.gprs[x86_gpr_dx] = pop(ms);
  ms.gprs[x86_gpr_cx] = pop(ms);
  ms.gprs[x86_gpr_bx] = pop(ms);
  ms.gprs[x86_gpr_ax] = pop(ms);
  ms.gprs[x86_gpr_sp] = pop(ms);
  ms.ip = retAddr;
}

void simulate_signal_dispatch(LinuxMachineState& ms, int sig, uint32_t sourceAddr) {
  fprintf(stdout, "Dispatching signal '%s' at addr 0x%08X\n", strsignal(sig), sourceAddr);
  uint32_t oldSp = ms.gprs[x86_gpr_sp];
#if 0
  if (st.signalStack) {
    st.esp = st.signalStack;
  }
#endif
  push(ms, oldSp);
  push(ms, ms.gprs[x86_gpr_ax]);
  push(ms, ms.gprs[x86_gpr_bx]);
  push(ms, ms.gprs[x86_gpr_cx]);
  push(ms, ms.gprs[x86_gpr_dx]);
  push(ms, ms.gprs[x86_gpr_si]);
  push(ms, ms.gprs[x86_gpr_di]);
  push(ms, ms.gprs[x86_gpr_bp]);
  push(ms, sourceAddr);
  push(ms, sig);
  push(ms, 0x00536967);
  ms.ip = ms.signalHandlers[sig];
}

void simulate_signal_check(LinuxMachineState& ms, uint32_t sourceAddr) {
  assert (ms.signalQueue.anySignalsWaiting());
  int sig = ms.signalQueue.popSignal();
  simulate_signal_dispatch(ms, sig, sourceAddr);
}

SgAsmx86Instruction* IncrementalDisassembler::disassembleNewInstruction(uint64_t addr) {
  std::pair<std::vector<uint8_t>, SgAsmx86Instruction*>& insnData = insnMap[addr];
  std::vector<uint8_t> insnBuf = memory.readAsFarAsPossibleForExec(addr, 15);

  insnData.second = isSgAsmx86Instruction(disassembler->disassembleOne(&insnBuf[0], addr, insnBuf.size(), addr, NULL));
  ROSE_ASSERT (insnData.second);
  insnData.first = insnData.second->get_raw_bytes();
  return insnData.second;
}

SgAsmx86Instruction* IncrementalDisassembler::operator[](uint64_t addr) {
  // std::cerr << (void*)addr << " --> " << memory.pages[addr / PAGE_SIZE] << std::endl;
  if (insnMap.find(addr) == insnMap.end()) {
    return disassembleNewInstruction(addr);
  } else {
    const std::pair<std::vector<uint8_t>, SgAsmx86Instruction*>& insnData = insnMap[addr];
    // Check location
    std::vector<uint8_t> insnBuf = memory.readAsFarAsPossibleForExec(addr, insnData.first.size());
    if (insnBuf != insnData.first) {
      // Disassembly is old
      return disassembleNewInstruction(addr);
    } else {
      return insnData.second;
    }
  }
}

#endif /*ROSE_ENABLE_SIMULATOR*/
