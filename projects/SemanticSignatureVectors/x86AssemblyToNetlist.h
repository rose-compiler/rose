#ifndef X86ASSEMBLYTONETLIST_H
#define X86ASSEMBLYTONETLIST_H

#include "x86InstructionSemantics.h"
#include <cassert>
#include <cstdio>
#include "satProblem.h"

struct MemoryAccess {
  LitList(32) addr;
  LitList(8) data;
  Lit cond;
  MemoryAccess(LitList(32) addr, LitList(8) data, Lit cond): addr(addr), data(data), cond(cond) {}
  MemoryAccess(const std::vector<Lit>& x) {
    assert (x.size() == 41);
    cond = x[0];
    for (size_t i = 0; i < 32; ++i) addr[i] = x[i + 1];
    for (size_t i = 0; i < 8; ++i) data[i] = x[i + 33];
  }
  std::vector<Lit> vec() const {
    std::vector<Lit> v;
    v.push_back(cond);
    v.insert(v.end(), addr.begin(), addr.end());
    v.insert(v.end(), data.begin(), data.end());
    return v;
  }
};

enum BMCError {
  bmc_error_bogus_ip,
  bmc_error_program_failure
};

static const size_t numBmcErrors = 2;

struct RegisterInfo {
  //General purpose registers for 32 bits
  LitList(32) gprs[8];
  LitList(32) ip;
  LitList(16) flags;
  LitList(numBmcErrors) errorFlag;
};

#endif // X86ASSEMBLYTONETLIST_H
