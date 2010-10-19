// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DEPRECATED. Use stringifyPowerpcRegisterClass(n, "powerpc_regclass_").c_str() instead.
const char* regclassToString(PowerpcRegisterClass n) {
  static const char* names[] = {"unknown", "gpr", "fpr", "cr", "fpscr", "spr", "tbr", "msr", "sr"};
  ROSE_ASSERT ((int)n >= 0 && (int)n <= 8);
  return names[(int)n];
}

const char* sprToString(PowerpcSpecialPurposeRegister n) {
  switch (n) {
    case powerpc_spr_xer: return "xer";
    case powerpc_spr_lr: return "lr";
    case powerpc_spr_ctr: return "ctr";
    default: return "unknown_spr";
  }
}

const char* tbrToString(PowerpcTimeBaseRegister n) {
  switch (n) {
    case powerpc_tbr_tbl: return "tbl";
    case powerpc_tbr_tbu: return "tbu";
    default: return "unknown_tbr";
  }
}
