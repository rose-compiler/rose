#ifndef ROSE_X86INSTRUCTIONENUM_H
#define ROSE_X86INSTRUCTIONENUM_H

#include <string>

/* This is included by rose.h, but for some reason is necessary here also. [RPM 2009-07-07] */
#include "AssemblerX86Init.h"

/* The X86InstructionKind enumeration has been moved to src/frontend/Disassemblers/AssemblerX86Init.h and is now
 * machine generated. [RPM 2009-07-07] */
//enum X86InstructionKind {...};

enum X86SegmentRegister { // These must match numbering in object code
  x86_segreg_es,
  x86_segreg_cs,
  x86_segreg_ss,
  x86_segreg_ds,
  x86_segreg_fs,
  x86_segreg_gs,
  x86_segreg_none = 16 /* For unspecified segment overrides */
};

enum X86GeneralPurposeRegister { // These must match numbering in object code
  x86_gpr_ax,
  x86_gpr_cx,
  x86_gpr_dx,
  x86_gpr_bx,
  x86_gpr_sp,
  x86_gpr_bp,
  x86_gpr_si,
  x86_gpr_di // Not including r8..r15
};

enum X86RegisterClass {
  x86_regclass_unknown,
  x86_regclass_gpr, // Numbers are X86GeneralPurposeRegister (ax,cx,dx,bx,sp,bp,si,di,r8..r15)
  x86_regclass_segment, // Numbers are X86SegmentRegister (es,cs,ss,ds,fs,gs)
  x86_regclass_cr, // Numbers are cr0..cr15
  x86_regclass_dr, // Numbers are dr0..dr15
  x86_regclass_st, // Numbers are st0..st7
  x86_regclass_mm, // Numbers are mm0..mm7             Note that these should reference st(i), so don't use this constant
  x86_regclass_xmm, // Numbers are xmm0..xmm15
  x86_regclass_ip, // Only value allowed is 0
  x86_regclass_st_top, // Only value allowed is 0
  x86_regclass_flags // Only value allowed is 0
};

enum X86BranchPrediction {
  x86_branch_prediction_none,
  x86_branch_prediction_taken,
  x86_branch_prediction_not_taken
};

enum X86RepeatPrefix {
    x86_repeat_none,            /**< No repeat prefix */
    x86_repeat_repne,           /**< Repeat not equal prefix 0xf2 */
    x86_repeat_repe             /**< Repeat equal prefix 0xf3 */
};

enum X86PositionInRegister {
  x86_regpos_unknown, /*!< unknown (error or unitialized value) */
  x86_regpos_low_byte, /*!< 1st byte of register (bits 0-7), only for GPRs */
  x86_regpos_high_byte, /*!< 2nd byte of register (bits 8-15), only for ax,bx,cx,dx */
  x86_regpos_word, /*!< 16 bit part of register, only for GPRs, CR0, CR8? */
  x86_regpos_dword, /*!< lower 32 bit part of register, only for GPRs */
  x86_regpos_qword, /*!< lower 64 bit part of register, only for GPRs */
  x86_regpos_all /*!< the full register is used (default value), only value allowed for segregs and st */
};

enum X86InstructionSize {
  x86_insnsize_none,
  x86_insnsize_16,
  x86_insnsize_32,
  x86_insnsize_64
};

enum X86Flag { // These match the bit positions in rFLAGS
  x86_flag_cf = 0,
  x86_flag_1 = 1,
  x86_flag_pf = 2,
  x86_flag_3 = 3,
  x86_flag_af = 4,
  x86_flag_5 = 5,
  x86_flag_zf = 6,
  x86_flag_sf = 7,
  x86_flag_tf = 8,
  x86_flag_if = 9,
  x86_flag_df = 10,
  x86_flag_of = 11,
  x86_flag_iopl0 = 12,
  x86_flag_iopl1 = 13,
  x86_flag_nt = 14,
  x86_flag_15 = 15,
  x86_flag_rf = 16,
  x86_flag_vm = 17,
  x86_flag_ac = 18,
  x86_flag_vif = 19,
  x86_flag_vip = 20,
  x86_flag_id = 21
};

std::string toString(X86InstructionKind k);

#endif /* ROSE_X86INSTRUCTIONENUM_H */
