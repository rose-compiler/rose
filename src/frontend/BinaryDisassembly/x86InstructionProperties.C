// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

bool x86InstructionIsConditionalFlagControlTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_ja:
    case x86_jae:
    case x86_jb:
    case x86_jbe:
    case x86_jcxz:
    case x86_je:
    case x86_jecxz:
    case x86_jg:
    case x86_jge:
    case x86_jl:
    case x86_jle:
    case x86_jne:
    case x86_jno:
    case x86_jns:
    case x86_jo:
    case x86_jpe:
    case x86_jpo:
    case x86_jrcxz:
    case x86_loopnz:
    case x86_loopz:
      return true;
    default: return false;
  }
}

bool x86InstructionIsConditionalControlTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_loop:
      return true;
    default: return false;
  }
}

bool x86InstructionIsConditionalFlagBitAndByte(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_seta:
    case x86_setae:
    case x86_setb:
    case x86_setbe:
    case x86_sete:
    case x86_setg:
    case x86_setge:
    case x86_setl:
    case x86_setle:
    case x86_setne:
    case x86_setno:
    case x86_setns:
    case x86_seto:
    case x86_setpe:
    case x86_setpo:
    case x86_sets:
      return true;
    default: return false;
  }
}


bool x86InstructionIsConditionalFlagDataTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_cbw:
    case x86_cmova:
    case x86_cmovae:
    case x86_cmovb:
    case x86_cmovbe:
    case x86_cmove:
    case x86_cmovg:
    case x86_cmovge:
    case x86_cmovl:
    case x86_cmovle:
    case x86_cmovne:
    case x86_cmovno:
    case x86_cmovns:
    case x86_cmovo:
    case x86_cmovpe:
    case x86_cmovpo:
    case x86_cmovs:
    case x86_cmpxchg:
    case x86_cmpxchg8b:
    case x86_cwd:
    case x86_fcmovb:
    case x86_fcmovbe:
    case x86_fcmove:
    case x86_fcmovnb:
    case x86_fcmovnbe:
    case x86_fcmovne:
    case x86_fcmovnu:
    case x86_fcmovu:
    case x86_fcom:
    case x86_fcomi:
    case x86_fcomip:
    case x86_fcomp:
    case x86_fcompp:
    case x86_fcos:
      return true;

    default: return false;
  }
}

bool x86InstructionIsConditionalDataTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_cmpxchg:
    case x86_cmpxchg8b:
      return true;

    default: return false;
  }
}


bool x86InstructionIsControlTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_call:
    case x86_ret:
    case x86_iret:
    case x86_farcall:
    case x86_farjmp:
    case x86_hlt:
    case x86_jmp:
    case x86_int1:
    case x86_int3:
    case x86_into:
    case x86_ja:
    case x86_jae:
    case x86_jb:
    case x86_jbe:
    case x86_jcxz:
    case x86_je:
    case x86_jecxz:
    case x86_jg:
    case x86_jge:
    case x86_jl:
    case x86_jle:
    case x86_jne:
    case x86_jno:
    case x86_jns:
    case x86_jo:
    case x86_jpe:
    case x86_jpo:
    case x86_jrcxz:
    case x86_js:
    case x86_loop:
    case x86_loopnz:
    case x86_loopz:
    case x86_retf:
    case x86_rsm:
    case x86_ud2:
      return true;
    default: return false;
  }
}

bool x86InstructionIsConditionalBranch(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_ja:
    case x86_jae:
    case x86_jb:
    case x86_jbe:
    case x86_jcxz:
    case x86_jecxz:
    case x86_jrcxz:
    case x86_je:
    case x86_jg:
    case x86_jge:
    case x86_jl:
    case x86_jle:
    case x86_jne:
    case x86_jno:
    case x86_jns:
    case x86_jo:
    case x86_jpe:
    case x86_jpo:
    case x86_js:
      return true;
    default: return false;
  }
}

bool x86InstructionIsDataTransfer(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_aaa:
    case x86_aad:
    case x86_aam:
    case x86_aas:
    case x86_adc:
    case x86_add:
    case x86_addsd:
    case x86_addss:
    case x86_and:
    case x86_andnpd:
    case x86_andpd:
    case x86_arpl:
    case x86_bsf:
    case x86_bsr:
    case x86_bswap:
    case x86_btc:
    case x86_btr:
    case x86_bts:
    case x86_cbw:
    case x86_cmova:
    case x86_cmovae:
    case x86_cmovb:
    case x86_cmovbe:
    case x86_cmove:
    case x86_cmovg:
    case x86_cmovge:
    case x86_cmovl:
    case x86_cmovle:
    case x86_cmovne:
    case x86_cmovno:
    case x86_cmovns:
    case x86_cmovo:
    case x86_cmovpe:
    case x86_cmovpo:
    case x86_cmovs:
    case x86_cmpxchg:
    case x86_cmpxchg8b:
    case x86_cwd:
    case x86_daa:
    case x86_das:
    case x86_dec:
    case x86_div:
    case x86_divsd:
    case x86_enter:
    case x86_f2xm1:
    case x86_fabs:
    case x86_fadd:
    case x86_faddp:
    case x86_fbld:
    case x86_fbstp:
    case x86_fchs:
    case x86_fcmovb:
    case x86_fcmovbe:
    case x86_fcmove:
    case x86_fcmovnb:
    case x86_fcmovnbe:
    case x86_fcmovne:
    case x86_fcmovnu:
    case x86_fcmovu:
    case x86_fcom:
    case x86_fcomi:
    case x86_fcomip:
    case x86_fcomp:
    case x86_fcompp:
    case x86_fcos:
    case x86_fdecstp:
    case x86_fdiv:
    case x86_fdivp:
    case x86_fdivr:
    case x86_fdivrp:
    case x86_femms:
    case x86_ffree:
    case x86_fiadd:
    case x86_ficom:
    case x86_ficomp:
    case x86_fidiv:
    case x86_fidivr:
    case x86_fild:
    case x86_fimul:
    case x86_fincstp:
    case x86_fist:
    case x86_fistp:
    case x86_fisttp:
    case x86_fisub:
    case x86_fisubr:
    case x86_fld:
    case x86_fld1:
    case x86_fldcw:
    case x86_fldenv:
    case x86_fldl2e:
    case x86_fldl2t:
    case x86_fldlg2:
    case x86_fldln2:
    case x86_fldpi:
    case x86_fldz:
    case x86_fmul:
    case x86_fmulp:
    case x86_fnclex:
    case x86_fninit:
    case x86_fnop:
    case x86_fnsave:
    case x86_fnstcw:
    case x86_fnstenv:
    case x86_fnstsw:
    case x86_fpatan:
    case x86_fprem:
    case x86_fprem1:
    case x86_fptan:
    case x86_frndint:
    case x86_frstor:
    case x86_fscale:
    case x86_fsin:
    case x86_fsincos:
    case x86_fsqrt:
    case x86_fst:
    case x86_fstp:
    case x86_fsub:
    case x86_fsubp:
    case x86_fsubr:
    case x86_fsubrp:
    case x86_ftst:
    case x86_fucom:
    case x86_fucomi:
    case x86_fucomip:
    case x86_fucomp:
    case x86_fucompp:
    case x86_fwait:
    case x86_fxam:
    case x86_fxch:
    case x86_fxsave:
    case x86_fxtract:
    case x86_fyl2x:
    case x86_fyl2xp1:
    case x86_idiv:
    case x86_imul:
    case x86_in:
    case x86_inc:
    case x86_insb:
    case x86_insw:
    case x86_insd:
    case x86_lahf:
    case x86_lar:
    case x86_ldmxcsr:
    case x86_lds:
    case x86_lea:
    case x86_les:
    case x86_lfs:
    case x86_lgs:
    case x86_lodsb:
    case x86_lodsw:
    case x86_lodsd:
    case x86_lodsq:
    case x86_lsl:
    case x86_lss:
    case x86_lzcnt:
    case x86_mov:
    case x86_movaps:
    case x86_movdqu:
    case x86_movlpd:
    case x86_movntq:
    case x86_movsb:
    case x86_movsw:
    case x86_movsd:
    case x86_movsq:
    case x86_movss:
    case x86_movsx:
    case x86_movsxd:
    case x86_movzx:
    case x86_mul:
    case x86_neg:
    case x86_not:
    case x86_or:
    case x86_paddb:
    case x86_paddw:
    case x86_paddd:
    case x86_paddq:
    case x86_pand:
    case x86_pandn:
    case x86_pop:
    case x86_popa:
    case x86_popcnt:
    case x86_popf:
    case x86_psllw:
    case x86_pslld:
    case x86_psllq:
    case x86_psrlw:
    case x86_psrld:
    case x86_psrlq:
    case x86_psubb:
    case x86_psubw:
    case x86_psubd:
    case x86_psubq:
    case x86_psubusb:
    case x86_psubusw:
    case x86_pxor:
    case x86_rcl:
    case x86_rcr:
    case x86_rdmsr:
    case x86_rdpmc:
    case x86_rdtsc:
    case x86_rol:
    case x86_ror:
    case x86_sahf:
    case x86_salc:
    case x86_sar:
    case x86_sbb:
    case x86_seta:
    case x86_setae:
    case x86_setb:
    case x86_setbe:
    case x86_sete:
    case x86_setg:
    case x86_setge:
    case x86_setl:
    case x86_setle:
    case x86_setne:
    case x86_setno:
    case x86_setns:
    case x86_seto:
    case x86_setpe:
    case x86_setpo:
    case x86_sets:
    case x86_shl:
    case x86_shld:
    case x86_shr:
    case x86_shrd:
    case x86_stmxcsr:
    case x86_stos:
    case x86_str:
    case x86_sub:
    case x86_subsd:
    case x86_subss:
    case x86_verr:
    case x86_verw:
    case x86_wrmsr:
    case x86_xadd:
    case x86_xchg:
    case x86_xlatb:
    case x86_xor:
    case x86_xorpd:
    case x86_xorps:
      return true;

    default: return false;
  }
}

bool x86InstructionIsUnconditionalBranch(SgAsmx86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_call:
    case x86_ret:
    case x86_iret:
    case x86_farcall:
    case x86_farjmp:
    case x86_hlt:
    case x86_jmp:
    case x86_retf:
    case x86_rsm:
      return true;
    default: return false;
  }
}

bool x86GetKnownBranchTarget(SgAsmx86Instruction* insn, uint64_t& addr) {
  // Treats far destinations as "unknown"
  switch (insn->get_kind()) {
    case x86_call:
    case x86_farcall:
    case x86_jmp:
    case x86_ja:
    case x86_jae:
    case x86_jb:
    case x86_jbe:
    case x86_jcxz:
    case x86_jecxz:
    case x86_jrcxz:
    case x86_je:
    case x86_jg:
    case x86_jge:
    case x86_jl:
    case x86_jle:
    case x86_jne:
    case x86_jno:
    case x86_jns:
    case x86_jo:
    case x86_jpe:
    case x86_jpo:
    case x86_js:
    case x86_loop:
    case x86_loopnz:
    case x86_loopz: {
      SgAsmOperandList* ls = insn->get_operandList();
      const std::vector<SgAsmExpression*>& exprs = ls->get_operands();
      if (exprs.size() != 1) return false;
      SgAsmExpression* dest = exprs[0];
      if (!isSgAsmValueExpression(dest)) return false;
      addr = SageInterface::getAsmConstant(isSgAsmValueExpression(dest));
      return true;
    }
    default: return false;
  }
}

const char* regclassToString(X86RegisterClass n) {
  static const char* names[] = {"unknown", "gpr", "segment", "cr", "dr", "st", "mm", "xmm", "ip", "st_top", "flags"};
  ROSE_ASSERT ((int)n >= 0 && (int)n <= 10);
  return names[(int)n];
}

const char* gprToString(X86GeneralPurposeRegister n) {
  static const char* names[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
                                "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
  ROSE_ASSERT ((int)n >= 0 && (int)n <= 15);
  return names[(int)n];
}

const char* segregToString(X86SegmentRegister n) {
  static const char* names[] = {"es", "cs", "ss", "ds", "fs", "gs"};
  ROSE_ASSERT ((int)n >= 0 && (int)n <= 5);
  return names[(int)n];
}

const char* flagToString(X86Flag n) {
  static const char* names[] = {
      /* 0*/ "cf",
      /* 1*/ "?1",
      /* 2*/ "pf",
      /* 3*/ "?3",
      /* 4*/ "af",
      /* 5*/ "?5",
      /* 6*/ "zf",
      /* 7*/ "sf",
      /* 8*/ "tf",
      /* 9*/ "if",
      /*10*/ "df",
      /*11*/ "of",
      /*12*/ "iopl0",
      /*13*/ "iopl1",
      /*14*/ "nt",
      /*15*/ "?15",
      /*16*/ "rf",
      /*17*/ "vm",
      /*18*/ "ac",
      /*19*/ "vif",
      /*20*/ "vip",
      /*21*/ "id"
      /*22*/ "?22",
      /*23*/ "?23",
      /*24*/ "?24",
      /*25*/ "?25",
      /*26*/ "?26",
      /*27*/ "?27",
      /*28*/ "?28",
      /*29*/ "?29",
      /*30*/ "?30",
      /*31*/ "?31"
  };
  ROSE_ASSERT ((int)n >= 0 && (int)n <= 32);
  return names[(int)n];
}
