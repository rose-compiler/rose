#include "rose.h"
#include <cassert>
#include <cstdio>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>
#include "satProblem.h"
#include <boost/smart_ptr.hpp>
#include "x86AssemblyToNetlist.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <boost/logic/tribool.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

using namespace std;
using namespace boost;

void printRegisterInfo(FILE* f, const RegisterInfo& ri) {
  for (size_t i = 0; i < 8; ++i) {
    uint32_t val = fromNumber(ri.gprs[i]);
    fprintf(f, "e%s = 0x%08"PRIX32" = %"PRIu32"\n", gprToString((X86GeneralPurposeRegister)i), val, val);
  }
  uint32_t val = fromNumber(ri.ip);
  fprintf(f, "eip = 0x%08"PRIX32" = %"PRIu32"\n", val, val);
  for (size_t i = 0; i < 16; ++i) {
    int n = ri.flags[i] == TRUE ? 1 : 0;
    const char* name = flagToString((X86Flag)i);
    if (name[0] == '?') continue; // Undefined flags
    fprintf(f, "%s=%d ", flagToString((X86Flag)i), n);
  }
  fprintf(f, "\n");
}

void printMemoryAccess(FILE* f, const MemoryAccess& ma) {
  if (ma.cond == FALSE) {
    fprintf(f, "Disabled ");
  }
  fprintf(f, "addr = 0x%08llX, data = 0x%02llX", fromNumber(ma.addr), fromNumber(ma.data));
}

RegisterInfo readRegInfo(map<string, vector<Lit> >& interfaceVarMap, const string& prefix) {
  RegisterInfo inRegs;
  for (size_t i = 0; i < 8; ++i) {
    inRegs.gprs[i] = fromVector<32>(interfaceVarMap[prefix + "_gpr" + lexical_cast<string>(i)]);
  }
  inRegs.ip = fromVector<32>(interfaceVarMap[prefix + "_ip"]);
  inRegs.flags = fromVector<16>(interfaceVarMap[prefix + "_flags"]);
  inRegs.errorFlag = fromVector<2>(interfaceVarMap[prefix + "_error"]);
  return inRegs;
}

vector<MemoryAccess> readMemoryInfo(const map<string, vector<Lit> >& interfaceVarMap, const string& prefix) {
  map<size_t, MemoryAccess> maList; // This must be sorted
  for (map<string, vector<Lit> >::const_iterator i = interfaceVarMap.begin();
       i != interfaceVarMap.end(); ++i) {
    if (i->first.size() <= prefix.size() + 1 ||
        i->first.substr(0, prefix.size() + 1) != prefix + "_") continue;
    size_t idx = lexical_cast<size_t>(i->first.substr(prefix.size() + 1));
    maList.insert(make_pair(idx, MemoryAccess(i->second)));
  }
  vector<MemoryAccess> result;
  for (map<size_t, MemoryAccess>::const_iterator i = maList.begin();
       i != maList.end(); ++i) {
    result.push_back(i->second);
  }
  return result;
}

map<uint32_t, uint8_t> groupMemoryAccessesByAddr(const vector<MemoryAccess>& a, const map<uint32_t, uint8_t>& initialMap) {
  map<uint32_t, uint8_t> result = initialMap;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i].cond == FALSE) continue;
    result.insert(make_pair(fromNumber(a[i].addr), fromNumber(a[i].data)));
  }
  return result;
}

void printOutMemoryData(FILE* f, const map<uint32_t, uint8_t>& addrMap, const char* symbol) {
  map<uint32_t, array<uint8_t, 16> > groupedMap;
  map<uint32_t, bitset<16> > presentMap;
  for (map<uint32_t, uint8_t>::const_iterator i = addrMap.begin();
       i != addrMap.end(); ++i) {
    groupedMap[i->first & ~15ULL][i->first & 15] = i->second;
    presentMap[i->first & ~15ULL][i->first & 15] = true;
  }
  for (map<uint32_t, array<uint8_t, 16> >::const_iterator i = groupedMap.begin();
       i != groupedMap.end(); ++i) {
    const array<uint8_t, 16>& data = i->second;
    const bitset<16>& present = presentMap[i->first];
    fprintf(f, "0x%08"PRIX32" %s", i->first, symbol);
    for (size_t j = 0; j < 16; ++j) {
      if (present[j]) {
        fprintf(f, " %02"PRIX8, data[j]);
      } else {
        fprintf(f, "   ");
      }
    }
    fprintf(f, "\n");
  }
}

int main(int argc, char** argv) {
  SatProblem cnf;
  cnf.parse(stdin);

  map<string, vector<Lit> > interfaceVarMap(cnf.interfaceVariables.begin(), cnf.interfaceVariables.end());

  // Set the outgoing program error flag, and clear the incoming one
  cnf.identify(interfaceVarMap["out_error"][bmc_error_program_failure], TRUE);
  cnf.identify(interfaceVarMap["in_error"][bmc_error_program_failure], FALSE);

  // Clear both bogus IP flags
  cnf.identify(interfaceVarMap["out_error"][bmc_error_bogus_ip], FALSE);
  cnf.identify(interfaceVarMap["in_error"][bmc_error_bogus_ip], FALSE);

  // Set the initial conditions for testing
  cnf.condEquivalenceWords(TRUE, fromVector<32>(interfaceVarMap["in_ip"]), number<32>(0x8048130));
  cnf.condEquivalenceWords(TRUE, fromVector<32>(interfaceVarMap["in_gpr4"]), number<32>(0xBFFF0000));
  for (size_t i = 0; i < 8; ++i) {
    if (i == 4) continue;
    cnf.condEquivalenceWords(TRUE, fromVector<32>(interfaceVarMap["in_gpr" + lexical_cast<string>(i)]), number<32>(0));
  }
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_cf], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_pf], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_af], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_zf], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_sf], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_df], FALSE);
  cnf.identify(interfaceVarMap["in_flags"][x86_flag_of], FALSE);

  // Run Picosat
  FILE* f = popen("/home/willcock2/picosat-846/picosat > picosat_output", "w");
  if (!f) return 2;
  cnf.unparse(f);
  int result = pclose(f);
  if (WIFEXITED(result)) {
    result = WEXITSTATUS(result);
  } else if (WIFSIGNALED(result)) {
    fprintf(stderr, "Killed by signal %d (%s)\n", WTERMSIG(result), strsignal(WTERMSIG(result)));
    return 1;
  } else {
    fprintf(stderr, "Unknown wait4 result %d\n", result);
    return 1;
  }
  if (result == 10) { // SAT
    fprintf(stderr, "SAT\n");
  } else if (result == 20) { // UNSAT
    fprintf(stderr, "UNSAT\n");
    return 1;
  } else {
    fprintf(stderr, "UNKNOWN %d\n", result);
    return 1;
  }

  FILE* vars = fopen("picosat_output", "r");
  if (!vars) return 3;
  vector<boost::tribool> varBindings;
  while (true) {
    int c = fgetc(vars);
    switch (c) {
      case EOF: abort();
      case ' ':
      case '\n':
      case '\r':
      case '\v':
      case '\t': continue;
      case 's': while (true) {int v = fgetc(vars); if (v == '\n') break; if (v == EOF) abort();} continue;
      case 'v': continue;
      case '0' ... '9':
      case '-': {
        ungetc(c, vars);
        int v;
        int count = fscanf(vars, "%d", &v);
        if (count != 1) abort();
        int absV = abs(v);
        if ((size_t)absV >= varBindings.size()) {
          varBindings.resize((size_t)absV + 1, indeterminate);
        }
        if (v == 0) {
          goto doneWithRead;
        } else if (v < 0) {
          varBindings[absV] = false;
        } else {
          varBindings[absV] = true;
        }
        continue;
      }
      default: {
        fprintf(stderr, "Bad character %d (%c)\n", c, c);
        abort();
      }
    }
  }
  doneWithRead:
  fclose(vars);

  for (size_t i = 0; i < cnf.interfaceVariables.size(); ++i) {
    vector<Lit>& rhs = cnf.interfaceVariables[i].second;
    for (size_t j = 0; j < rhs.size(); ++j) {
      // if (j != 0) fprintf(stdout, " ");
      tribool tb = indeterminate;
      Lit l = rhs[j];
      if (l == TRUE) {
        tb = true;
      } else if (l == FALSE) {
        tb = false;
      } else if (l < 0) {
        if ((size_t)(-l) < varBindings.size()) {
          tb = !varBindings[-l];
        }
      } else {
        if ((size_t)l < varBindings.size()) {
          tb = varBindings[l];
        }
      }
      if (tb == true) {
        rhs[j] = TRUE;
      } else {
        rhs[j] = FALSE;
      }
    }
  }

  // Rebuild map with new data
  interfaceVarMap.clear();
  interfaceVarMap.insert(cnf.interfaceVariables.begin(), cnf.interfaceVariables.end());

  RegisterInfo inRegs = readRegInfo(interfaceVarMap, "in");
  RegisterInfo outRegs = readRegInfo(interfaceVarMap, "out");

  fprintf(stdout, "IN:\n");
  printRegisterInfo(stdout, inRegs);
  fprintf(stdout, "OUT:\n");
  printRegisterInfo(stdout, outRegs);
  fprintf(stdout, "INITIAL MEMORY:\n");
  vector<MemoryAccess> reads = readMemoryInfo(interfaceVarMap, "memoryRead");
  vector<MemoryAccess> writes = readMemoryInfo(interfaceVarMap, "memoryWrite");
  map<uint32_t, uint8_t> readsByAddr = groupMemoryAccessesByAddr(reads, map<uint32_t, uint8_t>()); // Must be sorted
  printOutMemoryData(stdout, readsByAddr, "->");
  fprintf(stdout, "FINAL MEMORY:\n");
  map<uint32_t, uint8_t> writesByAddr = groupMemoryAccessesByAddr(writes, readsByAddr); // Must be sorted
  printOutMemoryData(stdout, writesByAddr, "<-");

  return 0;
}
