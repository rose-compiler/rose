
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

std::string unparse_register_name (SgInitializedName::asm_register_name_enum register_name)
   {
  // DQ (7/22/2006): filescope array of char
     static const char* asm_register_names[(int)SgInitializedName::e_last_register + 1] =
   {
  /* e_invalid_register */ "invalid",
  /* e_memory_register */  "memory",
  /* register_a */       "ax",
  /* register_b */       "bx",
  /* register_c */       "cx",
  /* register_d */       "dx",
  /* register_si */      "si",
  /* register_di */      "di",
  /* register_bp */      "bp",
  /* register_sp */      "sp",
  /* register_r8 */      "r8",
  /* register_r9 */      "r9",
  /* register_r10 */     "r10",
  /* register_r11 */     "r11",
  /* register_r12 */     "r12",
  /* register_r13 */     "r13",
  /* register_r14 */     "r14",
  /* register_r15 */     "r15",
  /* register_st0 */     "st(0)",
  /* register_st1 */     "st(1)",
  /* register_st2 */     "st(2)",
  /* register_st3 */     "st(3)",
  /* register_st4 */     "st(4)",
  /* register_st5 */     "st(5)",
  /* register_st6 */     "st(6)",
  /* register_st7 */     "st(7)",
  /* register_mm0 */     "mm0",
  /* register_mm1 */     "mm1",
  /* register_mm2 */     "mm2",
  /* register_mm3 */     "mm3",
  /* register_mm4 */     "mm4",
  /* register_mm5 */     "mm5",
  /* register_mm6 */     "mm6",
  /* register_mm7 */     "mm7",
  /* register_f0 */      "xmm0",
  /* register_f1 */      "xmm1",
  /* register_f2 */      "xmm2",
  /* register_f3 */      "xmm3",
  /* register_f4 */      "xmm4",
  /* register_f5 */      "xmm5",
  /* register_f6 */      "xmm6",
  /* register_f7 */      "xmm7",
  /* register_f8 */      "xmm8",
  /* register_f9 */      "xmm9",
  /* register_f10 */     "xmm10",
  /* register_f11 */     "xmm11",
  /* register_f12 */     "xmm12",
  /* register_f13 */     "xmm13",
  /* register_f14 */     "xmm14",
  /* register_f15 */     "xmm15",
  /* register_flags */   "flags",
  /* register_fpsr */    "fpsr",
  /* register_dirflag */ "dirflag",
  /* register_f16 */     "xmm16",
  /* register_f17 */     "xmm17",
  /* register_f18 */     "xmm18",
  /* register_f19 */     "xmm19",
  /* register_f20 */     "xmm20",
  /* e_unrecognized_register */ "unrecognized",
  /* e_last_register */    "last"
   };

     std::string returnString = asm_register_names[register_name];

#if (__x86_64 == 1 || __x86_64__ == 1 || __x86_32 == 1 || __x86_32__ == 1)
  // DQ (12/12/2012): Fixup the name of the register so that we are refering to EAX instead of AX (on both 32 bit and 64 bit systems).
     if (register_name >= SgInitializedName::e_register_a && register_name <= SgInitializedName::e_register_sp)
        {
       // Use the extended register name.
          returnString = "e" + returnString;
        }
#endif

  // DQ (12/12/2012): If this is an unrecognized register at least specify a simple register name.
     if (register_name == SgInitializedName::e_unrecognized_register)
        {
          printf ("Error: register names not recognized on non-x86 architectures (putting out reference name: 'ax' \n");
          returnString = "ax";
        }

     return returnString;
   }
