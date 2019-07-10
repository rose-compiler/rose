#include "sage3basic.h"                                 // every librose .C file must start with this

namespace Rose {
namespace BinaryAnalysis {

bool x86InstructionIsConditionalFlagControlTransfer(SgAsmX86Instruction* inst) {
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

bool x86InstructionIsConditionalControlTransfer(SgAsmX86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_loop:
      return true;
    default: return false;
  }
}

bool x86InstructionIsConditionalFlagBitAndByte(SgAsmX86Instruction* inst) {
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


bool x86InstructionIsConditionalFlagDataTransfer(SgAsmX86Instruction* inst) {
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

bool x86InstructionIsConditionalDataTransfer(SgAsmX86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_cmpxchg:
    case x86_cmpxchg8b:
      return true;

    default: return false;
  }
}


bool x86InstructionIsControlTransfer(SgAsmX86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_call:
    case x86_ret:
    case x86_iret:
    case x86_farcall:
    case x86_farjmp:
    case x86_hlt:
    case x86_jmp:
    case x86_int:
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
    case x86_syscall:
    case x86_sysret:
    case x86_ud2:
      return true;
    default: return false;
  }
}

bool x86InstructionIsConditionalBranch(SgAsmX86Instruction* inst) {
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

bool x86InstructionIsDataTransfer(SgAsmX86Instruction* inst) {
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

bool x86InstructionIsUnconditionalBranch(SgAsmX86Instruction* inst) {
  switch (inst->get_kind()) {
    case x86_call:
    case x86_ret:
    case x86_int:
    case x86_int1:
    case x86_int3:
    case x86_into:
    case x86_iret:
    case x86_farcall:
    case x86_farjmp:
    case x86_hlt:
    case x86_jmp:
    case x86_retf:
    case x86_rsm:
    case x86_syscall:
    case x86_sysret:
      return true;
    default: return false;
  }
}

bool x86InstructionIsPrivileged(SgAsmX86Instruction *insn)
{
    switch (insn->get_kind()) {
        case x86_cli:           // clear interrupt flag
        case x86_clts:          // clear task switched flag in CR0
        case x86_hlt:           // halt
        case x86_in:            // input from port
        case x86_insb:          // input from port to string
        case x86_insw:          // input from port to string
        case x86_insd:          // input from port to string
        case x86_lgdt:          // load global descriptor table register
        case x86_lidt:          // load interrupt descriptor table register
        case x86_lldt:          // load local descriptor table register
        case x86_lmsw:          // load machine status word
        case x86_ltr:           // load task register
        case x86_out:           // output to port
        case x86_outs:          // output string to port
        case x86_outsb:         // output string to port
        case x86_outsw:         // output string to port
        case x86_outsd:         // output string to port
        case x86_sti:           // set interrupt flag
        case x86_sysexit:       // fast return from fast system call
            return true;

        case x86_mov: {         // mov (but only to/from control and debug registers)
            const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            if (2!=operands.size())
                return false;
            SgAsmRegisterReferenceExpression *rre0 = isSgAsmRegisterReferenceExpression(operands[0]);
            SgAsmRegisterReferenceExpression *rre1 = isSgAsmRegisterReferenceExpression(operands[1]);
            if ((rre0!=NULL && (rre0->get_descriptor().majorNumber()==x86_regclass_cr ||
                                rre0->get_descriptor().majorNumber()==x86_regclass_dr)) ||
                (rre1!=NULL && (rre1->get_descriptor().majorNumber()==x86_regclass_cr ||
                                rre1->get_descriptor().majorNumber()==x86_regclass_dr)))
                return true;
            return false;
        }

        default:                // to shut up warnings from compiler
            break;
    }
    return false;               // outside of switch to shut up warnings
}

bool x86InstructionIsFloatingPoint(SgAsmX86Instruction *insn)
{
    switch (insn->get_kind()) {
        case x86_addpd:         // add packed double precision
        case x86_addps:         // add packed single precision
        case x86_addsd:         // add scalar double precision
        case x86_addss:         // add scalar single precision
        case x86_addsubpd:      // packed double floating ponit add/subtract
        case x86_addsubps:      // packed single floating point add/subtract
        case x86_andpd:         // bitwise logical AND of packed double precision
        case x86_andps:         // bitwise logical AND of packed single precision
        case x86_andnpd:        // bitwise logical AND NOT of packed double precision
        case x86_andnps:        // bitwise logical AND NOT of packed single precision
        case x86_blendpd:       // blend packed double precision
        case x86_blendps:       // blend packed single precision
        case x86_blendvpd:      // variable blend packed double precision
        case x86_blendvps:      // variable blend packed single precision
        case x86_cmppd:         // compare packed double precision
        case x86_cmpps:         // compare packed single precision
        case x86_cmpsd:         // compare scalar double precision
        case x86_cmpss:         // compare scalar single precision
        case x86_comisd:        // compare scalar ordered double precision and set EFLAGS
        case x86_comiss:        // compare scalar ordered single precision and set EFLAGS
        case x86_cvtdq2pd:      // convert packed dword integers to packed double precision
        case x86_cvtdq2ps:      // convert packed dword integers to packed single precision
        case x86_cvtpd2dq:      // convert packed double precision to packed dword integers
        case x86_cvtpd2pi:      // convert packed double precision to packed dword integers
        case x86_cvtpd2ps:      // convert packed double precision to packed single precision
        case x86_cvtpi2pd:      // convert packed dword integers to packed double precision
        case x86_cvtpi2ps:      // convert packed dword integers to packed single precision
        case x86_cvtps2dq:      // convert packed single precision to packed dword integers
        case x86_cvtps2pd:      // convert packed single precision to packed double precision
        case x86_cvtps2pi:      // convert packed single precision to packed dword integers
        case x86_cvtsd2si:      // convert scalar double precision to integer
        case x86_cvtsd2ss:      // convert scalar double precision to scalar single precision
        case x86_cvtsi2sd:      // convert dword integer to scalar double precision
        case x86_cvtsi2ss:      // convert dword integer to scalar single precision
        case x86_cvtss2sd:      // convert scalar single precision to scalar double precision
        case x86_cvtss2si:      // convert scalar single precision to dword integer
        case x86_cvttpd2dq:     // convert with truncation packed double precision to packed dword integers
        case x86_cvttpd2pi:     // convert with truncation packed double precision to packed dword integers
        case x86_cvttps2dq:     // convert with truncation packed single precision to packed dword integers
        case x86_cvttps2pi:     // convert with truncation packed single precision to packed dword integers
        case x86_cvttsd2si:     // convert with truncation scalar double precision to signed integer
        case x86_cvttss2si:     // convert with truncation scalar single precision to dword integer
        case x86_divpd:         // divide packed double precision
        case x86_divps:         // divide packed single precision
        case x86_divsd:         // divide scalar double precision
        case x86_divss:         // divide scalar single precision
        case x86_dppd:          // dot product of packed double precision
        case x86_dpps:          // dot product of packed single precision
        case x86_emms:          // empty MMX technology state
        case x86_extractps:     // extract packed single precision
        case x86_f2xm1:         // compute 2^x-1
        case x86_fabs:          // absolute value
        case x86_fadd:          // add
        case x86_faddp:         // add
        case x86_fiadd:         // add
        case x86_fbld:          // load binary coded decimal
        case x86_fbstp:         // store BCD integer and pop
        case x86_fchs:          // change sign
        case x86_fnclex:        // clear exceptions
        case x86_fcmovb:        // conditional move
        case x86_fcmove:        // conditional move
        case x86_fcmovbe:       // conditional move
        case x86_fcmovu:        // conditional move
        case x86_fcmovnb:       // conditional move
        case x86_fcmovne:       // conditional move
        case x86_fcmovnbe:      // conditional move
        case x86_fcmovnu:       // conditional move
        case x86_fcom:          // compare
        case x86_fcomp:         // compare
        case x86_fcompp:        // compare
        case x86_fcomi:         // compare and set EFLAGS
        case x86_fcomip:        // compare and set EFLAGS
        case x86_fucomi:        // compare and set EFLAGS
        case x86_fucomip:       // compare and set EFLAGS
        case x86_fcos:          // cosine
        case x86_fdecstp:       // decrement stack top pointer
        case x86_fdiv:          // divide
        case x86_fdivp:         // divide
        case x86_fidiv:         // divide
        case x86_fdivr:         // reverse divide
        case x86_fdivrp:        // reverse divide
        case x86_fidivr:        // reverse divide
        case x86_ffree:         // free floating point register
        case x86_ficom:         // compare integer
        case x86_ficomp:        // compare integer
        case x86_fild:          // load integer
        case x86_fincstp:       // increment stack top pointer
        case x86_fninit:        // initialize floating point unit
        case x86_fist:          // store integer
        case x86_fistp:         // store integer
        case x86_fisttp:        // store integer with truncation
        case x86_fld:           // load
        case x86_fld1:          // load constant
        case x86_fldl2t:        // load constant
        case x86_fldl2e:        // load constant
        case x86_fldpi:         // load constant
        case x86_fldlg2:        // load constant
        case x86_fldln2:        // load constant
        case x86_fldz:          // load constant
        case x86_fldcw:         // load x87 fpu control word
        case x86_fldenv:        // load x87 fpu environment
        case x86_fmul:          // multiply
        case x86_fmulp:         // multiply
        case x86_fimul:         // multiply
        case x86_fnop:          // no operation
        case x86_fpatan:        // partial arctangent
        case x86_fprem:         // partial remainder
        case x86_fprem1:        // partial remainder
        case x86_fptan:         // partial tangent
        case x86_frndint:       // round to integer
        case x86_frstor:        // restore x87 fpu state
        case x86_fnsave:        // store x87 fpu state
        case x86_fscale:        // scale
        case x86_fsin:          // sine
        case x86_fsincos:       // sine and cosine
        case x86_fsqrt:         // square root
        case x86_fst:           // store
        case x86_fstp:          // store
        case x86_fnstcw:        // store x87 fpu control word
        case x86_fnstenv:       // store x87 fpu environment
        case x86_fnstsw:        // store x87 fpu status word
        case x86_fsub:          // subtract
        case x86_fsubp:         // subtract
        case x86_fisub:         // subtract
        case x86_fsubr:         // reverse subtract
        case x86_fsubrp:        // reverse subtract
        case x86_fisubr:        // reverse subtract
        case x86_ftst:          // test
        case x86_fucom:         // unordered compare
        case x86_fucomp:        // unordered compare
        case x86_fucompp:       // unordered compare
        case x86_fxam:          // examine ModR/M
        case x86_fxch:          // exchange register contents
        case x86_fxrstor:       // restore x87 fpu, mmx, xmm, and mxcsr state
        case x86_fxsave:        // save x87 fpu, mmx technology, and sse state
        case x86_fxtract:       // extract exponent and significand
        case x86_fyl2x:         // compute y*ln(x)
        case x86_fyl2xp1:       // compute y*ln(x+1)
        case x86_haddpd:        // packed double horizontal add
        case x86_haddps:        // packed single horizontal add
        case x86_hsubpd:        // packed double horizontal subtract
        case x86_hsubps:        // packed single horizontal subtract
        case x86_insertps:      // insert packed single precision
        case x86_maxpd:         // return maximum packed double precision
        case x86_maxps:         // return maximum packed single precision
        case x86_maxsd:         // return maximum scalar double precision
        case x86_maxss:         // return maximum scalar single precision
        case x86_minpd:         // return minimum packed double precision
        case x86_minps:         // return minimum packed single precision
        case x86_minsd:         // return minimum scalar double precision
        case x86_minss:         // return minimum scalar single precision
        case x86_movapd:        // move aligned packed double precision
        case x86_movaps:        // move aligned packed single precision
        case x86_movddup:       // move one double and duplicate
        case x86_movdq2q:       // move quadword from xmm to mmx technology register
        case x86_movhlps:       // move packed single precision high to low
        case x86_movhpd:        // move high packed double precision
        case x86_movhps:        // move high packed single precision
        case x86_movlhps:       // move packed single precision low to high
        case x86_movlpd:        // move low packed double precision
        case x86_movlps:        // move low packed single precision
        case x86_movmskpd:      // extract packed double precision sign mask
        case x86_movmskps:      // extract packed single precision sign mask
        case x86_movntpd:       // store packed double precision using non-temporal hint
        case x86_movntps:       // store packed single precision using non-temporal hint
        case x86_movq2dq:       // move quadword from mmx technology to xmm register
        case x86_movsd:         // move scalar double precision
        case x86_movshdup:      // move packed single high and duplicate
        case x86_movsldup:      // move packed single low and duplicate
        case x86_movss:         // move scalar single precision
        case x86_movupd:        // move unaligned packed double precision
        case x86_movups:        // move unaligned packed single precision
        case x86_mulpd:         // multiply packed double precision
        case x86_mulps:         // multiply packed single precision
        case x86_mulsd:         // multiply scalar double precision
        case x86_mulss:         // multiply scalar single precision
        case x86_orps:          // bitwise logical OR of single precision
        case x86_rcpps:         // compute reciprocals of packed single precision
        case x86_rcpss:         // compute reciprocals of scalar single precision
        case x86_roundpd:       // round packed double precision
        case x86_roundps:       // round packed single precision
        case x86_roundsd:       // round scalar double precision
        case x86_roundss:       // round scalar single precision
        case x86_rsqrtps:       // compute reciprocals of square roots of packed single precision
        case x86_rsqrtss:       // compute reciprocals of square roots of scalar single precision
        case x86_shufpd:        // shuffle packed double precision
        case x86_shufps:        // shuffle packed single precision
        case x86_sqrtpd:        // compute square roots of packed double precision
        case x86_sqrtps:        // compute square roots of packed single precision
        case x86_sqrtsd:        // compute square root of scalar double precision
        case x86_sqrtss:        // compute square root of scalar single precision
        case x86_subpd:         // subtract packed double precision
        case x86_subps:         // subtract packed single precision
        case x86_subsd:         // subtract scalar double precision
        case x86_subss:         // subtract scalar single precision
        case x86_ucomisd:       // unordered compare scalar double precision and set EFLAGS
        case x86_ucomiss:       // unordered compare scalar single precision and set EFLAGS
        case x86_unpckhpd:      // unpack and interleave high packed double precision
        case x86_unpckhps:      // unpack and interleave high packed single precision
        case x86_unpcklpd:      // unpack and interleave low packed double precision
        case x86_unpcklps:      // unpack and interleave low packed single precision
        case x86_xorpd:         // bitwise logical XOR for double precision
        case x86_xorps:         // bitwise logical XOR for single precision
            return true;
        default:                // to shut up warnings from compiler
            break;
    }
    return false;               // outside of switch to shut up warnings
}

// DEPRECATED. Use stringifyX86RegisterClass(n, "x86_regclass_").c_str() instead. [RPM 2010-10-13]
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
      /*21*/ "id",
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
  ROSE_ASSERT ((int)n >= 0 && (int)n < 32);
  return names[(int)n];
}

} // namespace
} // namespace
