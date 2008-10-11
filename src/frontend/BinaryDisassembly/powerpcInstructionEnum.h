#ifndef ROSE_POWERPC_INSTRUCTION_ENUM_H
#define ROSE_POWERPC_INSTRUCTION_ENUM_H

#include <string>

// DQ (10/11/2008): Started work to support PowerPC Instruction Set for BGL Performance Tool
enum PowerpcInstructionKind
   {
     powerpc_unknown_instruction = 0,

  // powerpc_add[o][.],  // Add 
     powerpc_add,  // Add 
     powerpc_add_record,  // Add 
     powerpc_addo,  // Add 
     powerpc_addo_record,  // Add 

  // powerpc_addc[o][.], // Add Carrying 
     powerpc_addc, // Add Carrying 
     powerpc_addc_record, // Add Carrying 
     powerpc_addco, // Add Carrying 
     powerpc_addco_record, // Add Carrying 

  // powerpc_adde[o][.], // Add Extended 
     powerpc_adde,         // Add Extended 
     powerpc_adde_record,  // Add Extended 
     powerpc_addeo,        // Add Extended 
     powerpc_addeo_record, // Add Extended 

     powerpc_addi,       // Add Immediate 
     powerpc_addic,      // Add Immediate Carrying 
     powerpc_addic_record, // Add Immediate Carrying and Record 
     powerpc_addis, // Add Immediate Shifted 

#if 0
  // DQ (10/11/2008): Comment out instruction mnemonic enum values that not compileable yet!

     powerpc_addme[o][.], // Add to Minus One Extended 
     powerpc_addze[o][.], // Add to Zero Extended 
     powerpc_and[.], // AND 
     powerpc_andc[.], // AND with Complement 
     powerpc_andi., // AND Immediate 
     powerpc_andis., // AND Immediate Shifted 
     powerpc_b[l][a], // Branch 
     powerpc_bc[l][a], // Branch Conditional 
     powerpc_bcctr[l], // Branch Conditional to Count Register 
     powerpc_bclr[l], // Branch Conditional Link Register 
     powerpc_cmp, // Compare 
     powerpc_cmpi, // Compare Immediate 
     powerpc_cmpl, // Compare Logical 
     powerpc_cmpli, // Compare Logical Immediate 
     powerpc_cntlzd, // Count Leading Zeros Doubleword 
     powerpc_cntlzw[.], // Count Leading Zeros Word 
     powerpc_crand, // Condition Register AND 
     powerpc_crandc, // Condition Register AND with Complement 
     powerpc_creqv, // Condition Register Equivalent 
     powerpc_crnand, // Condition Register NAND 
     powerpc_crnor, // Condition Register NOR 
     powerpc_cror, // Condition Register OR 
     powerpc_crorc, // Condition Register OR with Complement 
     powerpc_crxor, // Condition Register XOR 
     powerpc_dcbf, // Data Cache Block Flush 
     powerpc_dcbi, // Data Cache Block Invalidate 
     powerpc_dcbst, // Data Cache Block Store 
     powerpc_dcbt, // Data Cache Block Touch 
     powerpc_dcbtst, // Data Cache Block Touch for Store 
     powerpc_dcbz, // Data Cache Block Set to Zero 
     powerpc_divd, // Divide Doubleword 
     powerpc_divdu, // Divide Doubleword Unsigned 
     powerpc_divw[o][.], // Divide Word 
     powerpc_divwu[o][.], // Divide Word Unsigned 
     powerpc_eciwx, // External Control in Word Indexed (opt.) 
     powerpc_ecowx, // External Control out Word Indexed (opt.) 
     powerpc_eieio, // Enforce In-order Execution of I/O 
     powerpc_eqv[.], // Equivalent 
     powerpc_extsb[.], // Extend Sign Byte 
     powerpc_extsh[.], // Extend Sign Halfword 
     powerpc_extsw, // Extend Sign Word 
     powerpc_fabs[.], // Floating Absolute Value 
     powerpc_fadd[.], // Floating Add 
     powerpc_fadds[.], // Floating Add Single 
     powerpc_fcfid, // Floating Convert from Integer Doubleword 
     powerpc_fcmpo, // Floating Compare Ordered 
     powerpc_fcmpu, // Floating Compare Unordered 
     powerpc_fctid, // Floating Convert to Integer Doubleword 
     powerpc_fctidz, // Floating Convert to Integer Doubleword with Round Toward Zero 
     powerpc_fctiw[.], // Floating Convert to Integer Word 
     powerpc_fctiwz[.], // Floating Convert to Integer Word with Round to Zero 
     powerpc_fdiv[.], // Floating Divide 
     powerpc_fdivs[.], // Floating Divide Single 
     powerpc_fmadd[.], // Floating Multiply-Add 
     powerpc_fmadds[.], // Floating Multiply-Add Single 
     powerpc_fmr[.], // Floating Move Register 
     powerpc_fmsub[.], // Floating Multiply-Subtract 
     powerpc_fmsubs[.], // Floating Multiply-Subtract Single 
     powerpc_fmul[.], // Floating Multiply 
     powerpc_fmuls[.], // Floating Multiply Single 
     powerpc_fnabs[.], // Floating Negative Absolute Value 
     powerpc_fneg[.], // Floating Negate 
     powerpc_fnmadd[.], // Floating Negative Multiply-Add 
     powerpc_fnmadds[.], // Floating Negative Multiply-Add Single 
     powerpc_fnmsub[.], // Floating Negative Multiply-Subtract 
     powerpc_fnmsubs[.], // Floating Negative Multiply-Subtract Single 
     powerpc_fres[.], // Floating Reciprocal Estimate Single (optional) 
     powerpc_frsp[.], // Floating Round to Single Precision 
     powerpc_frsqrte[.], // Floating Reciprocal Square Root Estimate (optional) 
     powerpc_fsel[.], // Floating-Point Select (optional) 
     powerpc_fsub[.], // Floating Subtract 
     powerpc_fsubs[.], // Floating Subtract Single 
     powerpc_icbi, // Instruction Cache Block Invalidate 
     powerpc_isync, // Instruction Synchronize 
     powerpc_lbz, // Load Byte and Zero 
     powerpc_lbzu, // Load Byte and Zero with Update 
     powerpc_lbzux, // Load Byte and Zero with Update Indexed 
     powerpc_lbzx, // Load Byte and Zero Indexed 
     powerpc_ld, // Load Doubleword 
     powerpc_ldarx, // Load Doubleword and Reserve Indexed 
     powerpc_ldu, // Load Doubleword with Update 
     powerpc_ldux, // Load Doubleword with Update Indexed 
     powerpc_ldx, // Load Doubleword Indexed 
     powerpc_lfd, // Load Floating-Point Double 
     powerpc_lfdu, // Load Floating-Point Double with Update 
     powerpc_lfdux, // Load Floating-Point Double with Update Indexed 
     powerpc_lfdx, // Load Floating-Point Double Indexed 
     powerpc_lfs, // Load Floating-Point Single 
     powerpc_lfsu, // Load Floating-Point Single with Update 
     powerpc_lfsux, // Load Floating-Point Single with Update Indexed 
     powerpc_lfsx, // Load Floating-Point Single Indexed 
     powerpc_lha, // Load Half Algebraic 
     powerpc_lhau, // Load Half Algebraic with Update 
     powerpc_lhaux, // Load Half Algebraic with Update Indexed 
     powerpc_lhax, // Load Half Algebraic Indexed 
     powerpc_lhbrx, // Load Half Byte-Reversed Indexed 
     powerpc_lhz, // Load Half and Zero 
     powerpc_lhzu, // Load Half and Zero with Update 
     powerpc_lhzux, // Load Half and Zero with Update Indexed 
     powerpc_lhzx, // Load Half and Zero Indexed 
     powerpc_lmw, // Load Multiple Word 
     powerpc_lswi, // Load String Word Immediate 
     powerpc_lswx, // Load String Word Indexed 
     powerpc_lwa, // Load Word Algebraic 
     powerpc_lwarx, // Load Word and Reserve Indexed 
     powerpc_lwaux, // Load Word Algebraic with Update Indexed 
     powerpc_lwax, // Load Word Algebraic Indexed 
     powerpc_lwbrx, // Load Word Byte-Reversed Indexed 
     powerpc_lwz, // Load Word and Zero 
     powerpc_lwzu, // Load Word with Zero Update 
     powerpc_lwzux, // Load Word and Zero with Update Indexed 
     powerpc_lwzx, // Load Word and Zero Indexed 
     powerpc_mcrf, // Move Condition Register Field 
     powerpc_mcrfs, // Move to Condition Register from FPSCR 
     powerpc_mcrxr, // Move to Condition Register from XER 
     powerpc_mfcr, // Move from Condition Register 
     powerpc_mffs[.], // Move from FPSCR 
     powerpc_mfmsr, // Move from Machine State Register 
     powerpc_mfspr, // Move from Special-Purpose Register 
     powerpc_mfsr, // Move from Segment Register 
     powerpc_mfsrin, // Move from Segment Register Indirect 
     powerpc_mtcrf, // Move to Condition Register Fields 
     powerpc_mtfsb0[.], // Move to FPSCR Bit 0 
     powerpc_mtfsb1[.], // Move to FPSCR Bit 1 
     powerpc_mtfsf[.], // Move to FPSCR Fields 
     powerpc_mtfsfi[.], // Move to FPSCR Field Immediate 
     powerpc_mtmsr, // Move to Machine State Register 
     powerpc_mtspr, // Move to Special-Purpose Register 
     powerpc_mtsr, // Move to Segment Register 
     powerpc_mtsrin, // Move to Segment Register Indirect 
     powerpc_mulhd, // Multiply High Doubleword 
     powerpc_mulhdu, // Multiply High Doubleword Unsigned 
     powerpc_mulhw[.], // Multiply High Word 
     powerpc_mulhwu[.], // Multiply High Word Unsigned 
     powerpc_mulld, // Multiply Low Doubleword 
     powerpc_mulli, // Multiply Low Immediate 
     powerpc_mullw[o][.], // Multiply Low Word 
     powerpc_nand[.], // NAND 
     powerpc_neg[o][.], // Negate 
     powerpc_nor[.], // NOR 
     powerpc_or[.], // OR 
     powerpc_orc[.], // OR with Complement 
     powerpc_ori, // OR Immediate 
     powerpc_oris, // OR Immediate Shifted 
     powerpc_rfi, // Return from Interrupt 
     powerpc_rldcl, // Rotate Left Doubleword then Clear Left 
     powerpc_rldcr, // Rotate Left Doubleword then Clear Right 
     powerpc_rldic, // Rotate Left Doubleword Immediate then Clear 
     powerpc_rldicl, // Rotate Left Doubleword Immediate then Clear Left 
     powerpc_rldicr, // Rotate Left Doubleword Immediate then Clear Right 
     powerpc_rldimi, // Rotate Left Doubleword Immediate then Mask Insert 
     powerpc_rlwimi[.], // Rotate Left Word Immediate then Mask Insert 
     powerpc_rlwinm[.], // Rotate Left Word Immediate then AND with Mask 
     powerpc_rlwnm[.], // Rotate Left Word then AND with Mask 
     powerpc_sc, // System Call 
     powerpc_si, // Subtract Immediate 
     powerpc_si., // Subtract Immediate and Record 
     powerpc_slbia, // SLB Invalidate All 
     powerpc_slbie, // SLB Invalidate Entry 
     powerpc_sld, // Shift Left Doubleword 
     powerpc_slw[.], // Shift Left Word 
     powerpc_srad, // Shift Right Algebraic Doubleword 
     powerpc_sradi, // Shift Right Algebraic Doubleword Immediate 
     powerpc_srd, // Shift Right Doubleword 
     powerpc_sraw[.], // Shift Right Algebraic Word 
     powerpc_srawi[.], // Shift Right Algebraic Word Immediate 
     powerpc_srw[.], // Shift Right Word 
     powerpc_stb, // Store Byte 
     powerpc_stbu, // Store Byte with Update 
     powerpc_stbux, // Store Byte with Update Indexed 
     powerpc_stbx, // Store Byte Indexed 
     powerpc_std, // Store Doubleword 
     powerpc_stdcx, // Store Doubleword Conditional Indexed 
     powerpc_stdu, // Store Doubleword with Update 
     powerpc_stdux, // Store Doubleword with Update Indexed 
     powerpc_stdx, // Store Doubleword Indexed 
     powerpc_stfd, // Store Floating-Point Double 
     powerpc_stfdu, // Store Floating-Point Double with Update 
     powerpc_stfdux, // Store Floating-Point Double with Update Indexed 
     powerpc_stfdx, // Store Floating-Point Double Indexed 
     powerpc_stfiwx, // Store Floating-Point as Integer Word Indexed (optional) 
     powerpc_stfs, // Store Floating-Point Single 
     powerpc_stfsu, // Store Floating-Point Single with Update 
     powerpc_stfsux, // Store Floating-Point Single with Update Indexed 
     powerpc_stfsx, // Store Floating-Point Single Indexed 
     powerpc_sth, // Store Half 
     powerpc_sthbrx, // Store Half Byte-Reverse Indexed 
     powerpc_sthu, // Store Half with Update 
     powerpc_sthux, // Store Half with Update Indexed 
     powerpc_sthx, // Store Half Indexed 
     powerpc_stmw, // Store Multiple Word 
     powerpc_stswi, // Store String Word Immediate 
     powerpc_stswx, // Store String Word Indexed 
     powerpc_stw, // Store 
     powerpc_stwbrx, // Store Word Byte-Reversed Indexed 
     powerpc_stwcx., // Store Word Conditional Indexed 
     powerpc_stwu, // Store Word with Update 
     powerpc_stwux, // Store Word with Update Indexed 
     powerpc_stwx, // Store Word Indexed 
     powerpc_subf[o][.], // Subtract from 
     powerpc_subfc[o][.], // Subtract from Carrying 
     powerpc_subfe[o][.], // Subtract from Extended 
     powerpc_subfic, // Subtract from Immediate Carrying 
     powerpc_subfme[o][.], // Subtract from Minus One Extended 
     powerpc_subfze[o][.], // Subtract from Zero Extended 
     powerpc_sync, // Synchronize 
     powerpc_td, // Trap Doubleword 
     powerpc_tdi, // Trap Doubleword Immediate 
     powerpc_tlbie, // Translation Look-aside Buffer Invalidate Entry (optional) 
     powerpc_tlbsync, // Translation Look-aside Buffer Synchronize (optional) 
     powerpc_tw, // Trap Word 
     powerpc_twi, // Trap Word Immediate 
     powerpc_xor[.], // XOR 
#endif
     powerpc_xori, // XOR Immediate 
     powerpc_xoris, // XOR Immediate Shift 
     powerpc_last_instruction
   };

enum PowerpcRegisterClass
   {
     powerpc_regclass_unknown,
     powerpc_regclass_gpr, // General Purpose Register
     powerpc_regclass_cr,  // Condition Register Numbers are cr0..cr7
     powerpc_regclass_xer, // Integer Exception Register
     powerpc_regclass_lr,  // Link Register
     powerpc_regclass_ctr, // Count Register
     powerpc_regclass_sprg, // Special Purpose Registers General (numbers sprg4 - sprg7)
     powerpc_regclass_usprg0, // User Special Purpose Register 0
     powerpc_regclass_tbu, // Time Base - Upper 32-bits
     powerpc_regclass_tbl, // Time Base - Lower 32-bits

  // There should be flags, but I haven't come to that part of the documentation yet.
     powerpc_regclass_flags, // Are there flags to represent?
     powerpc_last_register_class // last enum value
};


std::string toString(PowerpcInstructionKind k);

#endif /* ROSE_POWERPC_INSTRUCTION_ENUM_H */
