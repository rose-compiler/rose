#include "rose.h"
#include "x86InstructionSemantics.h"
#include "x86AssemblyToBtor.h"
#include "integerOps.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

using namespace std;
using namespace IntegerOps;
using boost::array;

static void usage(const char* argv0) {
  fprintf(stderr, "Usage: %s (start|middle) program\n", argv0);
  exit(1);
}

struct HLTHooks: public BtorTranslationHooks {
  void hlt(BtorTranslationPolicy* policy) {
    policy->registerMap.errorFlag[bmc_error_program_failure] = policy->problem.build_op_redor(policy->readGPR(x86_gpr_bx));
  }
  void startInstruction(BtorTranslationPolicy* policy, SgAsmx86Instruction* insn) {
  }
  void finishInstruction(BtorTranslationPolicy* policy, SgAsmx86Instruction* insn) {
  }
};

int main(int argc, char** argv) {
  if (argc < 3) {
    usage(argv[0]);
  }
  string initialCondFlag = argv[1];
  argv[1] = argv[0];
  ++argv;
  --argc;
  bool initialConditionsAreUnknown;
  if (initialCondFlag == "start") { // Start of execution
    initialConditionsAreUnknown = false;
  } else if (initialCondFlag == "middle") { // Arbitrary point in execution
    initialConditionsAreUnknown = true;
  } else {
    usage(argv[0]);
  }
  SgProject* proj = frontend(argc, argv);
  FILE* f = fopen("foo.btor", "w");
  assert (f);
  HLTHooks hooks;
  BtorTranslationPolicy policy(&hooks, 1, 0, proj);
  string s = btorTranslate(policy, proj, f, initialConditionsAreUnknown, true);
  fprintf(f, "%s", s.c_str());
  fclose(f);
  return 0;
}
