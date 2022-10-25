#include "sage3basic.h"
#include "clang-to-rose-support.hpp" 

SgAsmOp::asm_operand_modifier_enum ClangToSageTranslator::get_sgAsmOperandModifier(std::string modifier)
{
   char c = modifier.front();
   SgAsmOp::asm_operand_modifier_enum result;
   
   switch(c)
   {
      case '=':
        result = SgAsmOp::e_output;
        break;
      case '+':
        result = SgAsmOp::e_modify;
        break;
      case '&':
        result = SgAsmOp::e_earlyclobber;
        break;
      case '%':
        result = SgAsmOp::e_commutative;
        break;
      case '*':
        result = SgAsmOp::e_ignore_next;
        break;
      case '#':
        result = SgAsmOp::e_ignore_till_comma;
        break;
      case '?':
        result = SgAsmOp::e_poor_choice;
        break;
      case '!':
        result = SgAsmOp::e_bad_choice;
        break;
      default:
        result = SgAsmOp::e_unknown; 
   }
   return result;
}


SgAsmOp::asm_operand_constraint_enum ClangToSageTranslator::get_sgAsmOperandConstraint(std::string constraint)
{
   char c = constraint.front();
   SgAsmOp::asm_operand_constraint_enum result = SgAsmOp::e_invalid;
   switch (c)
   {
     case '@':  /* e_invalid */             
       result = SgAsmOp::e_invalid;
       break;
     case ',':  /* e_end_of_constraint */                 
       result = SgAsmOp::e_end_of_constraint;
       break;
     case '&':  /* e_mod_earlyclobber */                 
       result = SgAsmOp::e_mod_earlyclobber;
       break;
     case '%':  /* e_mod_commutative_ops */                 
       result = SgAsmOp::e_mod_commutative_ops;
       break;
     case '#':  /* e_mod_commutative_ops */                 
       result = SgAsmOp::e_mod_commutative_ops;
       break;
     case '*':  /* e_mod_ignore_char */                 
       result = SgAsmOp::e_mod_ignore_char;
       break;
     case '?':  /* e_mod_disparage_slightly */                 
       result = SgAsmOp::e_mod_disparage_slightly;
       break;
     case '!':  /* e_mod_disparage_severely */                 
       result = SgAsmOp::e_mod_disparage_severely;
       break;
     case 'X':  /* e_any */                 
       result = SgAsmOp::e_any;
       break;
     case 'g':  /* e_general */             
       result = SgAsmOp::e_general;
       break;
     case '0':  /* e_match_0 */             
       result = SgAsmOp::e_match_0;
       break;
     case '1':  /* e_match_1 */             
       result = SgAsmOp::e_match_1;
       break;
     case '2':  /* e_match_2 */             
       result = SgAsmOp::e_match_2;
       break;
     case '3':  /* e_match_3 */             
       result = SgAsmOp::e_match_3;
       break;
     case '4':  /* e_match_4 */             
       result = SgAsmOp::e_match_4;
       break;
     case '5':  /* e_match_5 */             
       result = SgAsmOp::e_match_5;
       break;
     case '6':  /* e_match_6 */             
       result = SgAsmOp::e_match_6;
       break;
     case '7':  /* e_match_7 */             
       result = SgAsmOp::e_match_7;
       break;
     case '8':  /* e_match_8 */             
       result = SgAsmOp::e_match_8;
       break;
     case '9':  /* e_match_9 */             
       result = SgAsmOp::e_match_9;
       break;
     case 'r':  /* e_reg_integer */         
       result = SgAsmOp::e_reg_integer;
       break;
     case 'f':  /* e_reg_float */           
       result = SgAsmOp::e_reg_float;
       break;
     case 'm':  /* e_mem_any */             
       result = SgAsmOp::e_mem_any;
       break;
     case 'p':  /* e_mem_load */          
       result = SgAsmOp::e_mem_load;
       break;
     case 'o':  /* e_mem_offset */          
       result = SgAsmOp::e_mem_offset;
       break;
     case 'V':  /* e_mem_nonoffset */       
       result = SgAsmOp::e_mem_nonoffset;
       break;
     case '>':  /* e_mem_autoinc */         
       result = SgAsmOp::e_mem_autoinc;
       break;
     case '<':  /* e_mem_autodec */         
       result = SgAsmOp::e_mem_autodec;
       break;
     case 'i':  /* e_imm_int */             
       result = SgAsmOp::e_imm_int;
       break;
     case 'n':  // e_imm_number             
       result = SgAsmOp::e_imm_number;
       break;
     case 's':  /* e_imm_symbol */          
       result = SgAsmOp::e_imm_symbol;
       break;
     case 'E':  /* e_imm_float */          
     case 'F':  /* e_imm_float */           
       result = SgAsmOp::e_imm_float;
       break;
     case 'a':  /* e_reg_a */               
       result = SgAsmOp::e_reg_a;
       break;
     case 'b':  /* e_reg_b */               
       result = SgAsmOp::e_reg_b;
       break;
     case 'c':  /* e_reg_c */               
       result = SgAsmOp::e_reg_c;
       break;
     case 'd':  /* e_reg_d */               
       result = SgAsmOp::e_reg_d;
       break;
     case 'S':  /* e_reg_si */              
       result = SgAsmOp::e_reg_si;
       break;
     case 'D':  /* e_reg_di */              
       result = SgAsmOp::e_reg_di;
       break;
     case 'R':  /* e_reg_legacy */          
       result = SgAsmOp::e_reg_legacy;
       break;
     case 'q':  /* e_reg_q */               
       result = SgAsmOp::e_reg_q;
       break;
     case 'Q':  /* e_reg_Q */               
       result = SgAsmOp::e_reg_Q;
       break;
     case 'A':  /* e_reg_ad */              
       result = SgAsmOp::e_reg_ad;
       break;
     case 't':  /* e_reg_float_tos */       
       result = SgAsmOp::e_reg_float_tos;
       break;
     case 'u':  /* e_reg_float_second */    
       result = SgAsmOp::e_reg_float_second;
       break;
     case 'x':  /* e_reg_sse */             
       result = SgAsmOp::e_reg_sse;
       break;
     case 'Y':  /* e_reg_sse2 */            
       result = SgAsmOp::e_reg_sse2;
       break;
     case 'y':  /* e_reg_mmx */             
       result = SgAsmOp::e_reg_mmx;
       break;
     case 'I':  /* e_imm_short_shift */     
       result = SgAsmOp::e_imm_short_shift;
       break;
     case 'J':  /* e_imm_long_shift */      
       result = SgAsmOp::e_imm_long_shift;
       break;
     case 'M':  /* e_imm_lea_shift */       
       result = SgAsmOp::e_imm_lea_shift;
       break;
     case 'K':  /* e_imm_signed8 */         
       result = SgAsmOp::e_imm_signed8;
       break;
     case 'N':  /* e_imm_unsigned8 */       
       result = SgAsmOp::e_imm_unsigned8;
       break;
     case 'L':  /* e_imm_and_zext */        
       result = SgAsmOp::e_imm_and_zext;
       break;
     case 'G':  /* e_imm_80387 */           
       result = SgAsmOp::e_imm_80387;
       break;
     case 'H':  /* e_imm_sse */             
       result = SgAsmOp::e_imm_sse;
       break;
     case 'e':  /* e_imm_sext32 */          
       result = SgAsmOp::e_imm_sext32;
       break;
     case 'z':  /* e_imm_zext32 */          
       result = SgAsmOp::e_imm_zext32;
       break;
     case '~':  /* e_last */                
       result = SgAsmOp::e_last;
       break;
     default:
       result = SgAsmOp::e_invalid;
  }
  return result;
}

SgInitializedName::asm_register_name_enum ClangToSageTranslator::get_sgAsmRegister(std::string reg)
{
   SgInitializedName::asm_register_name_enum result = SgInitializedName::e_invalid_register;
   if(reg.compare(0, sizeof(reg), "memory") == 0)
     result = SgInitializedName::e_memory_register;
   else if(reg.compare(0, sizeof(reg), "ax") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "al") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "ah") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "eax") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "rax") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "0") == 0)
     result = SgInitializedName::e_register_a;
   else if(reg.compare(0, sizeof(reg), "bx") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "bl") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "bh") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "ebx") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "rbx") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "3") == 0)
     result = SgInitializedName::e_register_b;
   else if(reg.compare(0, sizeof(reg), "cx") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "cl") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "ch") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "ecx") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "rcx") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "2") == 0)
     result = SgInitializedName::e_register_c;
   else if(reg.compare(0, sizeof(reg), "dx") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "dl") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "dh") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "edx") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "rdx") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "1") == 0)
     result = SgInitializedName::e_register_d;
   else if(reg.compare(0, sizeof(reg), "si") == 0)
     result = SgInitializedName::e_register_si;
   else if(reg.compare(0, sizeof(reg), "esi") == 0)
     result = SgInitializedName::e_register_si;
   else if(reg.compare(0, sizeof(reg), "rsi") == 0)
     result = SgInitializedName::e_register_si;
   else if(reg.compare(0, sizeof(reg), "sil") == 0)
     result = SgInitializedName::e_register_si;
   else if(reg.compare(0, sizeof(reg), "4") == 0)
     result = SgInitializedName::e_register_si;
   else if(reg.compare(0, sizeof(reg), "di") == 0)
     result = SgInitializedName::e_register_di;
   else if(reg.compare(0, sizeof(reg), "edi") == 0)
     result = SgInitializedName::e_register_di;
   else if(reg.compare(0, sizeof(reg), "rdi") == 0)
     result = SgInitializedName::e_register_di;
   else if(reg.compare(0, sizeof(reg), "dil") == 0)
     result = SgInitializedName::e_register_di;
   else if(reg.compare(0, sizeof(reg), "5") == 0)
     result = SgInitializedName::e_register_di;
   else if(reg.compare(0, sizeof(reg), "bp") == 0)
     result = SgInitializedName::e_register_bp;
   else if(reg.compare(0, sizeof(reg), "ebp") == 0)
     result = SgInitializedName::e_register_bp;
   else if(reg.compare(0, sizeof(reg), "rbp") == 0)
     result = SgInitializedName::e_register_bp;
   else if(reg.compare(0, sizeof(reg), "bpl") == 0)
     result = SgInitializedName::e_register_bp;
   else if(reg.compare(0, sizeof(reg), "6") == 0)
     result = SgInitializedName::e_register_bp;
   else if(reg.compare(0, sizeof(reg), "sp") == 0)
     result = SgInitializedName::e_register_sp;
   else if(reg.compare(0, sizeof(reg), "esp") == 0)
     result = SgInitializedName::e_register_sp;
   else if(reg.compare(0, sizeof(reg), "rsp") == 0)
     result = SgInitializedName::e_register_sp;
   else if(reg.compare(0, sizeof(reg), "spl") == 0)
     result = SgInitializedName::e_register_sp;
   else if(reg.compare(0, sizeof(reg), "7") == 0)
     result = SgInitializedName::e_register_sp;
   else if(reg.compare(0, sizeof(reg), "r8") == 0)
     result = SgInitializedName::e_register_r8;
   else if(reg.compare(0, sizeof(reg), "r8b") == 0)
     result = SgInitializedName::e_register_r8;
   else if(reg.compare(0, sizeof(reg), "r8w") == 0)
     result = SgInitializedName::e_register_r8;
   else if(reg.compare(0, sizeof(reg), "r8d") == 0)
     result = SgInitializedName::e_register_r8;
   else if(reg.compare(0, sizeof(reg), "37") == 0)
     result = SgInitializedName::e_register_r8;
   else if(reg.compare(0, sizeof(reg), "r9") == 0)
     result = SgInitializedName::e_register_r9;
   else if(reg.compare(0, sizeof(reg), "r9b") == 0)
     result = SgInitializedName::e_register_r9;
   else if(reg.compare(0, sizeof(reg), "r9w") == 0)
     result = SgInitializedName::e_register_r9;
   else if(reg.compare(0, sizeof(reg), "r9d") == 0)
     result = SgInitializedName::e_register_r9;
   else if(reg.compare(0, sizeof(reg), "38") == 0)
     result = SgInitializedName::e_register_r9;
   else if(reg.compare(0, sizeof(reg), "r10") == 0)
     result = SgInitializedName::e_register_r10;
   else if(reg.compare(0, sizeof(reg), "r10b") == 0)
     result = SgInitializedName::e_register_r10;
   else if(reg.compare(0, sizeof(reg), "r10w") == 0)
     result = SgInitializedName::e_register_r10;
   else if(reg.compare(0, sizeof(reg), "r10d") == 0)
     result = SgInitializedName::e_register_r10;
   else if(reg.compare(0, sizeof(reg), "39") == 0)
     result = SgInitializedName::e_register_r10;
   else if(reg.compare(0, sizeof(reg), "r11") == 0)
     result = SgInitializedName::e_register_r11;
   else if(reg.compare(0, sizeof(reg), "r11b") == 0)
     result = SgInitializedName::e_register_r11;
   else if(reg.compare(0, sizeof(reg), "r11w") == 0)
     result = SgInitializedName::e_register_r11;
   else if(reg.compare(0, sizeof(reg), "r11d") == 0)
     result = SgInitializedName::e_register_r11;
   else if(reg.compare(0, sizeof(reg), "40") == 0)
     result = SgInitializedName::e_register_r11;
   else if(reg.compare(0, sizeof(reg), "r12") == 0)
     result = SgInitializedName::e_register_r12;
   else if(reg.compare(0, sizeof(reg), "r12b") == 0)
     result = SgInitializedName::e_register_r12;
   else if(reg.compare(0, sizeof(reg), "r12w") == 0)
     result = SgInitializedName::e_register_r12;
   else if(reg.compare(0, sizeof(reg), "r12d") == 0)
     result = SgInitializedName::e_register_r12;
   else if(reg.compare(0, sizeof(reg), "41") == 0)
     result = SgInitializedName::e_register_r12;
   else if(reg.compare(0, sizeof(reg), "r13") == 0)
     result = SgInitializedName::e_register_r13;
   else if(reg.compare(0, sizeof(reg), "r13b") == 0)
     result = SgInitializedName::e_register_r13;
   else if(reg.compare(0, sizeof(reg), "r13d") == 0)
     result = SgInitializedName::e_register_r13;
   else if(reg.compare(0, sizeof(reg), "r13w") == 0)
     result = SgInitializedName::e_register_r13;
   else if(reg.compare(0, sizeof(reg), "42") == 0)
     result = SgInitializedName::e_register_r13;
   else if(reg.compare(0, sizeof(reg), "r14") == 0)
     result = SgInitializedName::e_register_r14;
   else if(reg.compare(0, sizeof(reg), "r14b") == 0)
     result = SgInitializedName::e_register_r14;
   else if(reg.compare(0, sizeof(reg), "r14d") == 0)
     result = SgInitializedName::e_register_r14;
   else if(reg.compare(0, sizeof(reg), "r14w") == 0)
     result = SgInitializedName::e_register_r14;
   else if(reg.compare(0, sizeof(reg), "43") == 0)
     result = SgInitializedName::e_register_r14;
   else if(reg.compare(0, sizeof(reg), "r15") == 0)
     result = SgInitializedName::e_register_r15;
   else if(reg.compare(0, sizeof(reg), "r15b") == 0)
     result = SgInitializedName::e_register_r15;
   else if(reg.compare(0, sizeof(reg), "r15d") == 0)
     result = SgInitializedName::e_register_r15;
   else if(reg.compare(0, sizeof(reg), "r15w") == 0)
     result = SgInitializedName::e_register_r15;
   else if(reg.compare(0, sizeof(reg), "44") == 0)
     result = SgInitializedName::e_register_r15;
   else if(reg.compare(0, sizeof(reg), "st") == 0)
     result = SgInitializedName::e_register_st0;
   else if(reg.compare(0, sizeof(reg), "st(1)") == 0)
     result = SgInitializedName::e_register_st1;
   else if(reg.compare(0, sizeof(reg), "st1") == 0)
     result = SgInitializedName::e_register_st1;
   else if(reg.compare(0, sizeof(reg), "st(2)") == 0)
     result = SgInitializedName::e_register_st2;
   else if(reg.compare(0, sizeof(reg), "st2") == 0)
     result = SgInitializedName::e_register_st2;
   else if(reg.compare(0, sizeof(reg), "st(3)") == 0)
     result = SgInitializedName::e_register_st3;
   else if(reg.compare(0, sizeof(reg), "st3") == 0)
     result = SgInitializedName::e_register_st3;
   else if(reg.compare(0, sizeof(reg), "st(4)") == 0)
     result = SgInitializedName::e_register_st4;
   else if(reg.compare(0, sizeof(reg), "st4") == 0)
     result = SgInitializedName::e_register_st4;
   else if(reg.compare(0, sizeof(reg), "st(5)") == 0)
     result = SgInitializedName::e_register_st5;
   else if(reg.compare(0, sizeof(reg), "st5") == 0)
     result = SgInitializedName::e_register_st5;
   else if(reg.compare(0, sizeof(reg), "st(6)") == 0)
     result = SgInitializedName::e_register_st6;
   else if(reg.compare(0, sizeof(reg), "st6") == 0)
     result = SgInitializedName::e_register_st6;
   else if(reg.compare(0, sizeof(reg), "st(7)") == 0)
     result = SgInitializedName::e_register_st7;
   else if(reg.compare(0, sizeof(reg), "st7") == 0)
     result = SgInitializedName::e_register_st7;
   else if(reg.compare(0, sizeof(reg), "mm0") == 0)
     result = SgInitializedName::e_register_mm0;
   else if(reg.compare(0, sizeof(reg), "29") == 0)
     result = SgInitializedName::e_register_mm0;
   else if(reg.compare(0, sizeof(reg), "mm1") == 0)
     result = SgInitializedName::e_register_mm1;
   else if(reg.compare(0, sizeof(reg), "30") == 0)
     result = SgInitializedName::e_register_mm1;
   else if(reg.compare(0, sizeof(reg), "mm2") == 0)
     result = SgInitializedName::e_register_mm2;
   else if(reg.compare(0, sizeof(reg), "31") == 0)
     result = SgInitializedName::e_register_mm2;
   else if(reg.compare(0, sizeof(reg), "mm3") == 0)
     result = SgInitializedName::e_register_mm3;
   else if(reg.compare(0, sizeof(reg), "32") == 0)
     result = SgInitializedName::e_register_mm3;
   else if(reg.compare(0, sizeof(reg), "mm4") == 0)
     result = SgInitializedName::e_register_mm4;
   else if(reg.compare(0, sizeof(reg), "33") == 0)
     result = SgInitializedName::e_register_mm4;
   else if(reg.compare(0, sizeof(reg), "mm5") == 0)
     result = SgInitializedName::e_register_mm5;
   else if(reg.compare(0, sizeof(reg), "34") == 0)
     result = SgInitializedName::e_register_mm5;
   else if(reg.compare(0, sizeof(reg), "mm6") == 0)
     result = SgInitializedName::e_register_mm6;
   else if(reg.compare(0, sizeof(reg), "35") == 0)
     result = SgInitializedName::e_register_mm6;
   else if(reg.compare(0, sizeof(reg), "mm7") == 0)
     result = SgInitializedName::e_register_mm7;
   else if(reg.compare(0, sizeof(reg), "36") == 0)
     result = SgInitializedName::e_register_mm7;
   else if(reg.compare(0, sizeof(reg), "xmm0") == 0)
     result = SgInitializedName::e_register_f0;
   else if(reg.compare(0, sizeof(reg), "21") == 0)
     result = SgInitializedName::e_register_f0;
   else if(reg.compare(0, sizeof(reg), "xmm1") == 0)
     result = SgInitializedName::e_register_f1;
   else if(reg.compare(0, sizeof(reg), "22") == 0)
     result = SgInitializedName::e_register_f2;
   else if(reg.compare(0, sizeof(reg), "xmm2") == 0)
     result = SgInitializedName::e_register_f2;
   else if(reg.compare(0, sizeof(reg), "23") == 0)
     result = SgInitializedName::e_register_f2;
   else if(reg.compare(0, sizeof(reg), "xmm3") == 0)
     result = SgInitializedName::e_register_f3;
   else if(reg.compare(0, sizeof(reg), "24") == 0)
     result = SgInitializedName::e_register_f3;
   else if(reg.compare(0, sizeof(reg), "xmm4") == 0)
     result = SgInitializedName::e_register_f4;
   else if(reg.compare(0, sizeof(reg), "25") == 0)
     result = SgInitializedName::e_register_f4;
   else if(reg.compare(0, sizeof(reg), "xmm5") == 0)
     result = SgInitializedName::e_register_f5;
   else if(reg.compare(0, sizeof(reg), "26") == 0)
     result = SgInitializedName::e_register_f5;
   else if(reg.compare(0, sizeof(reg), "xmm6") == 0)
     result = SgInitializedName::e_register_f6;
   else if(reg.compare(0, sizeof(reg), "27") == 0)
     result = SgInitializedName::e_register_f6;
   else if(reg.compare(0, sizeof(reg), "xmm7") == 0)
     result = SgInitializedName::e_register_f7;
   else if(reg.compare(0, sizeof(reg), "28") == 0)
     result = SgInitializedName::e_register_f7;
   else if(reg.compare(0, sizeof(reg), "xmm8") == 0)
     result = SgInitializedName::e_register_f8;
   else if(reg.compare(0, sizeof(reg), "45") == 0)
     result = SgInitializedName::e_register_f8;
   else if(reg.compare(0, sizeof(reg), "xmm9") == 0)
     result = SgInitializedName::e_register_f9;
   else if(reg.compare(0, sizeof(reg), "46") == 0)
     result = SgInitializedName::e_register_f9;
   else if(reg.compare(0, sizeof(reg), "xmm10") == 0)
     result = SgInitializedName::e_register_f10;
   else if(reg.compare(0, sizeof(reg), "47") == 0)
     result = SgInitializedName::e_register_f10;
   else if(reg.compare(0, sizeof(reg), "xmm11") == 0)
     result = SgInitializedName::e_register_f11;
   else if(reg.compare(0, sizeof(reg), "48") == 0)
     result = SgInitializedName::e_register_f11;
   else if(reg.compare(0, sizeof(reg), "xmm12") == 0)
     result = SgInitializedName::e_register_f12;
   else if(reg.compare(0, sizeof(reg), "49") == 0)
     result = SgInitializedName::e_register_f12;
   else if(reg.compare(0, sizeof(reg), "xmm13") == 0)
     result = SgInitializedName::e_register_f13;
   else if(reg.compare(0, sizeof(reg), "50") == 0)
     result = SgInitializedName::e_register_f13;
   else if(reg.compare(0, sizeof(reg), "xmm14") == 0)
     result = SgInitializedName::e_register_f14;
   else if(reg.compare(0, sizeof(reg), "51") == 0)
     result = SgInitializedName::e_register_f14;
   else if(reg.compare(0, sizeof(reg), "xmm15") == 0)
     result = SgInitializedName::e_register_f15;
   else if(reg.compare(0, sizeof(reg), "52") == 0)
     result = SgInitializedName::e_register_f15;
   else if(reg.compare(0, sizeof(reg), "flags") == 0)
     result = SgInitializedName::e_register_flags;
   else if(reg.compare(0, sizeof(reg), "cc") == 0)
     result = SgInitializedName::e_register_flags;
   else if(reg.compare(0, sizeof(reg), "fpsr") == 0)
     result = SgInitializedName::e_register_fpsr;
   else if(reg.compare(0, sizeof(reg), "dirflag") == 0)
     result = SgInitializedName::e_register_dirflag;
   else if(reg.compare(0, sizeof(reg), "16") == 0)
     result = SgInitializedName::e_register_f16;
   else if(reg.compare(0, sizeof(reg), "17") == 0)
     result = SgInitializedName::e_register_f17;
   else if(reg.compare(0, sizeof(reg), "18") == 0)
     result = SgInitializedName::e_register_f18;
   else if(reg.compare(0, sizeof(reg), "19") == 0)
     result = SgInitializedName::e_register_f19;
   else if(reg.compare(0, sizeof(reg), "20") == 0)
     result = SgInitializedName::e_register_f20;
   else if(reg.compare(0, sizeof(reg), "unrecognized") == 0)
     result = SgInitializedName::e_unrecognized_register;
   else if(reg.compare(0, sizeof(reg), "last") == 0)
     result = SgInitializedName::e_last_register;

   return result;
}

// Pei-Hung (05/27/2022) referece APIs from edgRose.C
std::string ClangToSageTranslator::generate_source_position_string(clang::SourceLocation srcLoc)
{
  clang::SourceManager& SM = p_compiler_instance->getSourceManager();
  clang::SourceLocation expansionLoc = SM.getExpansionLoc(srcLoc);
  clang::FileID fID = SM.getFileID(expansionLoc);
  const clang::FileEntry* fEntry = SM.getFileEntryForID(fID);
  std::string return_string;

//  return_string = string("0x") + StringUtility::numberToString(file_id) + "_" + StringUtility::numberToString(line_number) + "_" + StringUtility::numberToString(column_number);
  return_string = std::string("0x") + std::to_string(fEntry->getUID()) + "_" + std::to_string(SM.getSpellingLineNumber(srcLoc)) + "_" + std::to_string(SM.getSpellingColumnNumber(srcLoc));
  return return_string;

}


std::string ClangToSageTranslator::generate_name_for_variable(clang::Stmt* p)
   {
     return "__anonymous_" + Rose::StringUtility::numberToString(p);
   }

std::string ClangToSageTranslator::generate_name_for_type(clang::TypeSourceInfo* typeInfo)
{
   clang::SourceManager& SM = p_compiler_instance->getSourceManager();
   clang::TypeLoc typeLoc = typeInfo->getTypeLoc();
   return "__anonymous_" + generate_source_position_string(typeLoc.getBeginLoc());

}
