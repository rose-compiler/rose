#include "clang-to-rose-support.hpp" 

SgAsmOp::asm_operand_modifier_enum get_sgAsmOperandModifier(std::string modifier)
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


SgAsmOp::asm_operand_constraint_enum get_sgAsmOperandConstraint(std::string constraint)
{
   char c = constraint.front();
   SgAsmOp::asm_operand_constraint_enum result = SgAsmOp::e_invalid;
   switch (c)
   {
     case '@':  /* e_invalid */             
       result = SgAsmOp::e_invalid;
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
     case 'E':  /* e_imm_symbol */          
       result = SgAsmOp::e_imm_float;
       break;
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


