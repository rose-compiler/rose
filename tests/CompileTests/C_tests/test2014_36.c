#include "test2014_36.h"

// __attribute__((regparm(3))) extern UInt x86g_calculate_eflags_c (UInt cc_op, UInt cc_dep1, UInt cc_dep2, UInt cc_ndep);

#define X86G_CC_OP_LOGICL 1
#define X86G_CC_OP_LOGICW 2
#define X86G_CC_OP_LOGICB 3
#define X86G_CC_OP_SUBL   4
#define X86G_CC_OP_SUBW   5
#define X86G_CC_OP_SUBB   6
#define X86G_CC_OP_INCL   7
#define X86G_CC_OP_DECL   8

__attribute__((regparm(3)))
UInt x86g_calculate_eflags_c ( UInt cc_op,
                               UInt cc_dep1,
                               UInt cc_dep2,
                               UInt cc_ndep )
{
#if 1
# 605 "priv/guest_x86_helpers.c"
   switch (cc_op) {
      case X86G_CC_OP_LOGICL:
      case X86G_CC_OP_LOGICW:
      case X86G_CC_OP_LOGICB:
         return 0;
      case X86G_CC_OP_SUBL:
         return ((UInt)cc_dep1) < ((UInt)cc_dep2) ? (1 << 0) : 0;
      case X86G_CC_OP_SUBW:
         return ((UInt)(cc_dep1 & 0xFFFF)) < ((UInt)(cc_dep2 & 0xFFFF)) ? (1 << 0) : 0;
      case X86G_CC_OP_SUBB:
         return ((UInt)(cc_dep1 & 0xFF)) < ((UInt)(cc_dep2 & 0xFF)) ? (1 << 0) : 0;
      case X86G_CC_OP_INCL:
      case X86G_CC_OP_DECL:
         return cc_ndep & (1 << 0);
      default:
         break;
   }

// return x86g_calculate_eflags_all_WRK(cc_op,cc_dep1,cc_dep2,cc_ndep) & (1 << 0);
   return 0;
#else
   return 0;
#endif
}
