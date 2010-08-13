/* Documentation is in AssemblerX86.h */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "rose_getline.h"

#define __STDC_FORMAT_MACROS
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>

AssemblerX86::InsnDictionary AssemblerX86::defns;

static void
printExpr(FILE *f, SgAsmExpression *e, const std::string &prefix, unsigned variant=V_SgNode)
{
    if (!e) {
        fprintf(f, "null");
        return;
    }
    if (variant==V_SgNode)
        variant = e->variantT();
    switch (variant) {
        case V_SgAsmExpression: {
            fprintf(f, ",\n%sreplacement=\"%s\"", prefix.c_str(), e->get_replacement().c_str());
            fprintf(f, ",\n%scomment=\"%s\"", prefix.c_str(), e->get_comment().c_str());
            break;
        }

        /*=== Value Expressions ===*/

        case V_SgAsmValueExpression: {
            SgAsmValueExpression *ee = isSgAsmValueExpression(e);
            fprintf(f, ",\n%sbit_offset=%u, bit_size=%u", prefix.c_str(), ee->get_bit_offset(), ee->get_bit_size());
            fprintf(f, ", unfolded=");
            printExpr(f, ee->get_unfolded_expression_tree(), prefix+"  ");
            printExpr(f, e, prefix, V_SgAsmExpression);
            break;
        }
        case V_SgAsmByteValueExpression: {
            SgAsmByteValueExpression *ee = isSgAsmByteValueExpression(e);
            fprintf(f, "ByteValue {value=0x%02x", ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmWordValueExpression: {
            SgAsmWordValueExpression *ee = isSgAsmWordValueExpression(e);
            fprintf(f, "WordValue {value=0x%04x", ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmDoubleWordValueExpression: {
            SgAsmDoubleWordValueExpression *ee = isSgAsmDoubleWordValueExpression(e);
            fprintf(f, "DoubleWordValue {value=0x%08x", ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmQuadWordValueExpression: {
            SgAsmQuadWordValueExpression *ee = isSgAsmQuadWordValueExpression(e);
            fprintf(f, "QuadWordValue {value=0x%016"PRIx64, ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmSingleFloatValueExpression: {
            SgAsmSingleFloatValueExpression *ee = isSgAsmSingleFloatValueExpression(e);
            fprintf(f, "SingleFloatValue {value=%g", ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmDoubleFloatValueExpression: {
            SgAsmDoubleFloatValueExpression *ee = isSgAsmDoubleFloatValueExpression(e);
            fprintf(f, "DoubleFloatValue {value=%g", ee->get_value());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmVectorValueExpression: {
            SgAsmVectorValueExpression *ee = isSgAsmVectorValueExpression(e);
            fprintf(f, "VectorValue {size=%u, type=?", ee->get_size());
            printExpr(f, e, prefix, V_SgAsmValueExpression);
            fprintf(f, "}");
            break;
        }

        /*=== Binary Expressions ===*/
            
        case V_SgAsmBinaryExpression: {
            SgAsmBinaryExpression *ee = isSgAsmBinaryExpression(e);
            fprintf(f, "\n%slhs=", prefix.c_str());
            printExpr(f, ee->get_lhs(), prefix+"  ");
            fprintf(f, "\n%srhs=", prefix.c_str());
            printExpr(f, ee->get_rhs(), prefix+"  ");
            printExpr(f, e, prefix, V_SgAsmExpression);
            break;
        }
        case V_SgAsmBinaryAdd: {
            fprintf(f, "Add {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinarySubtract: {
            fprintf(f, "Subtract {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryMultiply: {
            fprintf(f, "Multiply {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryDivide: {
            fprintf(f, "Divide {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryMod: {
            fprintf(f, "Mod {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryAddPreupdate: {
            fprintf(f, "AddPreupdate {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinarySubtractPreupdate: {
            fprintf(f, "SubtractPreupdate {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryAddPostupdate: {
            fprintf(f, "AddPostupdate {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinarySubtractPostupdate: {
            fprintf(f, "SubtractPostupdate {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryLsl: {
            fprintf(f, "Lsl {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryLsr: {
            fprintf(f, "Lsr {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryAsr: {
            fprintf(f, "Asr {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmBinaryRor: {
            fprintf(f, "Ror {");
            printExpr(f, e, prefix, V_SgAsmBinaryExpression);
            fprintf(f, "}");
            break;
        }

        /*=== Unary Expressions ===*/

        case V_SgAsmUnaryExpression: {
            SgAsmUnaryExpression *ee = isSgAsmUnaryExpression(e);
            fprintf(f, "operand=");
            printExpr(f, ee->get_operand(), prefix+"  ");
            printExpr(f, e, prefix, V_SgAsmExpression);
            break;
        }
        case V_SgAsmUnaryPlus: {
            fprintf(f, "Plus {");
            printExpr(f, e, prefix, V_SgAsmUnaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmUnaryMinus: {
            fprintf(f, "Minus {");
            printExpr(f, e, prefix, V_SgAsmUnaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmUnaryRrx: {
            fprintf(f, "Rrx {");
            printExpr(f, e, prefix, V_SgAsmUnaryExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmUnaryArmSpecialRegisterList: {
            fprintf(f, "ArmSpecialRegisterList {");
            printExpr(f, e, prefix, V_SgAsmUnaryExpression);
            fprintf(f, "}");
            break;
        }

        /*=== Register Reference Expressions ===*/

        case V_SgAsmRegisterReferenceExpression: {
            fprintf(f, ", type=?");
            printExpr(f, e, prefix, V_SgAsmExpression);
            break;
        }
        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression *ee = isSgAsmx86RegisterReferenceExpression(e);
            fprintf(f, "x86RegisterReference {register_class=%u, register_number=%u, position_in_register=%u", 
                    ee->get_register_class(), ee->get_register_number(), ee->get_position_in_register());
            printExpr(f, e, prefix, V_SgAsmRegisterReferenceExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmArmRegisterReferenceExpression: {
            fprintf(f, "ArmRegisterReference {");
            printExpr(f, e, prefix, V_SgAsmRegisterReferenceExpression);
            fprintf(f, "}");
            break;
        }
        case V_SgAsmPowerpcRegisterReferenceExpression: {
            fprintf(f, "PowerpcRegisterReference {");
            printExpr(f, e, prefix, V_SgAsmRegisterReferenceExpression);
            fprintf(f, "}");
            break;
        }

        /*=== Miscellaneous ===*/
            
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression *ee = isSgAsmMemoryReferenceExpression(e);
            fprintf(f, "MemoryReference {\n%saddress=", prefix.c_str());
            printExpr(f, ee->get_address(), prefix+"  ");
            fprintf(f, "\n%ssegment=", prefix.c_str());
            printExpr(f, ee->get_segment(), prefix+"  ");
            fprintf(f, "\n%stype=?", prefix.c_str());
            printExpr(f, e, prefix, V_SgAsmExpression);
            fprintf(f, "}");
            break;
        }
            
        case V_SgAsmControlFlagsExpression: {
            SgAsmControlFlagsExpression *ee = isSgAsmControlFlagsExpression(e);
            fprintf(f, "ControlFlags {bit_flags=0x%08lx", ee->get_bit_flags());
            printExpr(f, e, prefix, V_SgAsmExpression);
            fprintf(f, "}");
            break;
        }
            
        case V_SgAsmCommonSubExpression: {
            fprintf(f, "CommonSub {");
            printExpr(f, e, prefix, V_SgAsmExpression);
            fprintf(f, "}");
            break;
        }

        case V_SgAsmExprListExp: {
            SgAsmExprListExp *ee = isSgAsmExprListExp(e);
            fprintf(f, "ExprList {");
            for (size_t i=0; i<ee->get_expressions().size(); i++) {
                SgAsmExpression *operand = ee->get_expressions()[i];
                fprintf(f, "\n%soperand[%zu]=", prefix.c_str(), i);
                printExpr(f, operand, prefix+"  ");
            }
            printExpr(f, e, prefix, V_SgAsmExpression);
            fprintf(f, "}");
            break;
        }
    }
}

std::string
AssemblerX86::InsnDefn::to_str() const
{
    char buf[1024];
    buf[0] = '\0';

    sprintf(buf, "%s(", mnemonic.c_str());
    if (mnemonic!=AssemblerX86::to_str(kind))
        sprintf(buf+strlen(buf),  "[x86_%s] ", AssemblerX86::to_str(kind).c_str());

    if (opcode_modifiers & od_rex_pres) {
        uint8_t rex = od_rex_byte(opcode_modifiers);
        strcat(buf, "REX");
        if (rex & 0xf) {
            strcat(buf, ".");
            if (rex & 0x8)
                strcat(buf, "W");
            if (rex & 0x4)
                strcat(buf, "R");
            if (rex & 0x2)
                strcat(buf, "X");
            if (rex & 0x1)
                strcat(buf, "B");
        }
        strcat(buf, " + ");
    }

//#ifdef _MSC_VER
//#pragma message ("WARNING: MSVC does not allow specification of contant 0xffffffffffLLU")
//	printf ("ERROR: MSVC does not allow specification of contant 0xffffffffffLLU");
//	ROSE_ASSERT(false);
//#else
    ROSE_ASSERT(opcode <= 0xffffffffffLLU);
//#endif
    if (opcode > 0xffffffff)
        sprintf(buf+strlen(buf), "%02X", (unsigned)((opcode>>32) & 0xff));
    if (opcode > 0xffffff)
        sprintf(buf+strlen(buf), "%02X", (unsigned)((opcode>>24) & 0xff));
    if (opcode > 0xffff)
        sprintf(buf+strlen(buf), "%02X ", (unsigned)((opcode>>16) & 0xff));
    if (opcode > 0xff)
        sprintf(buf+strlen(buf), "%02X ", (unsigned)((opcode>>8) & 0xff));
    sprintf(buf+strlen(buf), "%02X", (unsigned)(opcode & 0xff));

    if (opcode_modifiers & od_e_mask)
        sprintf(buf+strlen(buf), "/%zu", od_e_val(opcode_modifiers));
    if (opcode_modifiers & od_modrm)
        strcat(buf, "/r");

    switch (opcode_modifiers & od_c_mask) {
        case od_cb: strcat(buf, " cb"); break;
        case od_cw: strcat(buf, " cw"); break;
        case od_cd: strcat(buf, " cd"); break;
        case od_cp: strcat(buf, " cp"); break;
        case od_co: strcat(buf, " co"); break;
        case od_ct: strcat(buf, " ct"); break;
    }
    
    switch (opcode_modifiers & od_i_mask) {
        case od_ib: strcat(buf, " ib"); break;
        case od_iw: strcat(buf, " iw"); break;
        case od_id: strcat(buf, " id"); break;
        case od_io: strcat(buf, " io"); break;
    }
    
    switch (opcode_modifiers & od_r_mask) {
        case od_rb: strcat(buf, "+rb"); break;
        case od_rw: strcat(buf, "+rw"); break;
        case od_rd: strcat(buf, "+rd"); break;
        case od_ro: strcat(buf, "+ro"); break;
    }

    if (opcode_modifiers & od_i)
        strcat(buf, "+i");

    strcat(buf, ") ");
    
    for (size_t i=0; i<operands.size(); i++) {
        if (i>0) strcat(buf, ", ");
        switch (operands[i]) {
            case od_none:                                       break;
            case od_AL:         strcat(buf, "AL");              break;
            case od_AX:         strcat(buf, "AX");              break;
            case od_EAX:        strcat(buf, "EAX");             break;
            case od_RAX:        strcat(buf, "RAX");             break;
            case od_CS:         strcat(buf, "CS");              break;
            case od_CL:         strcat(buf, "CL");              break;
            case od_DS:         strcat(buf, "DS");              break;
            case od_DX:         strcat(buf, "DX");              break;
            case od_ES:         strcat(buf, "ES");              break;
            case od_FS:         strcat(buf, "FS");              break;
            case od_GS:         strcat(buf, "GS");              break;
            case od_SS:         strcat(buf, "SS");              break;
            case od_CR0:        strcat(buf, "CR0");             break;
            case od_CR7:        strcat(buf, "CR7");             break;
            case od_CR8:        strcat(buf, "CR8");             break;
            case od_CR0CR7:     strcat(buf, "CR0CR7");          break;
            case od_DR0DR7:     strcat(buf, "DR0DR7");          break;
            case od_rel8:       strcat(buf, "rel8");            break;
            case od_rel16:      strcat(buf, "rel16");           break;
            case od_rel32:      strcat(buf, "rel32");           break;
            case od_rel64:      strcat(buf, "rel64");           break;
            case od_ptr16_16:   strcat(buf, "ptr16:16");        break;
            case od_ptr16_32:   strcat(buf, "ptr16:32");        break;
            case od_ptr16_64:   strcat(buf, "ptr16:64");        break;
            case od_reg:        strcat(buf, "reg");             break;
            case od_r8:         strcat(buf, "r8");              break;
            case od_r16:        strcat(buf, "r16");             break;
            case od_r32:        strcat(buf, "r32");             break;
            case od_r64:        strcat(buf, "r64");             break;
            case od_imm8:       strcat(buf, "imm8");            break;
            case od_imm16:      strcat(buf, "imm16");           break;
            case od_imm32:      strcat(buf, "imm32");           break;
            case od_imm64:      strcat(buf, "imm64");           break;
            case od_r_m8:       strcat(buf, "r/m8");            break;
            case od_r_m16:      strcat(buf, "r/m16");           break;
            case od_r_m32:      strcat(buf, "r/m32");           break;
            case od_r_m64:      strcat(buf, "r/m64");           break;
            case od_r16_m16:    strcat(buf, "r16/m16");         break;
            case od_r32_m8:     strcat(buf, "r32/m8");          break;
            case od_r32_m16:    strcat(buf, "r32/m16");         break;
            case od_r64_m16:    strcat(buf, "r64/m16");         break;
            case od_m:          strcat(buf, "m");               break;
            case od_m8:         strcat(buf, "m8");              break;
            case od_m16:        strcat(buf, "m16");             break;
            case od_m32:        strcat(buf, "m32");             break;
            case od_m64:        strcat(buf, "m64");             break;
            case od_m128:       strcat(buf, "m128");            break;
            case od_m16_16:     strcat(buf, "m16:16");          break;
            case od_m16_32:     strcat(buf, "m16:32");          break;
            case od_m16_64:     strcat(buf, "m16:64");          break;
            case od_m16a16:     strcat(buf, "m16&16");          break;
            case od_m16a32:     strcat(buf, "m16&32");          break;
            case od_m32a32:     strcat(buf, "m32&32");          break;
            case od_m16a64:     strcat(buf, "m16&64");          break;
            case od_moffs8:     strcat(buf, "moffs8");          break;
            case od_moffs16:    strcat(buf, "moffs16");         break;
            case od_moffs32:    strcat(buf, "moffs32");         break;
            case od_moffs64:    strcat(buf, "moffs64");         break;
            case od_sreg:       strcat(buf, "sreg");            break;
            case od_m32fp:      strcat(buf, "m32fp");           break;
            case od_m64fp:      strcat(buf, "m64fp");           break;
            case od_m80fp:      strcat(buf, "m80fp");           break;
            case od_st0:        strcat(buf, "ST(0)");           break;
            case od_st1:        strcat(buf, "ST(1)");           break;
            case od_st2:        strcat(buf, "ST(2)");           break;
            case od_st3:        strcat(buf, "ST(3)");           break;
            case od_st4:        strcat(buf, "ST(4)");           break;
            case od_st5:        strcat(buf, "ST(5)");           break;
            case od_st6:        strcat(buf, "ST(6)");           break;
            case od_st7:        strcat(buf, "ST(7)");           break;
            case od_sti:        strcat(buf, "ST(i)");           break;
            case od_mm:         strcat(buf, "mm");              break;
            case od_mm_m32:     strcat(buf, "mm/m32");          break;
            case od_mm_m64:     strcat(buf, "mm/m64");          break;
            case od_xmm:        strcat(buf, "xmm");             break;
            case od_xmm_m16:    strcat(buf, "xmm/m16");         break;
            case od_xmm_m32:    strcat(buf, "xmm/m32");         break;
            case od_xmm_m64:    strcat(buf, "xmm/m64");         break;
            case od_xmm_m128:   strcat(buf, "xmm/m128");        break;
            case od_XMM0:       strcat(buf, "<XMM0>");          break;
            case od_0:          strcat(buf, "0");               break;
            case od_1:          strcat(buf, "1");               break;
            case od_m80:        strcat(buf, "m80");             break;
            case od_dec:        strcat(buf, "dec");             break;
            case od_m80bcd:     strcat(buf, "m80bcd");          break;
            case od_m2byte:     strcat(buf, "m2byte");          break;
            case od_m14_28byte: strcat(buf, "m14_28byte");      break;
            case od_m94_108byte:strcat(buf, "m94_108byte");     break;
            case od_m512byte:   strcat(buf, "m512byte");        break;
        }
    }
    return std::string(buf);
}

bool
AssemblerX86::matches_rel(SgAsmInstruction *insn, int64_t val, size_t nbytes)
{
    if (nbytes>=8) return true;
    int64_t minval = (int64_t)-1 << (8*nbytes-1);
    int64_t maxval = (int64_t)~(uint64_t)minval;
    const int64_t leeway = 16; /*we don't know the assembled size yet, so allow some leeway.*/
    val -= (int64_t)insn->get_address();
    return val>= minval && val<=maxval+leeway;
}

bool
AssemblerX86::matches(OperandDefn od, SgAsmExpression *expr, SgAsmInstruction *insn,
                      int64_t *disp_p/*out*/, int64_t *imm_p/*out*/) const
{
    SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(expr);
    SgAsmMemoryReferenceExpression      *mre = isSgAsmMemoryReferenceExpression(expr);
    SgAsmValueExpression                *ve  = isSgAsmValueExpression(expr);
    switch (od) {
        case od_none:
            assert(false);
            abort();

        case od_0:
            return ve && SageInterface::getAsmConstant(ve)==0;
        case od_1:
            return ve && SageInterface::getAsmConstant(ve)==1;

        case od_AL:
            return matches(od_r8, expr, insn, disp_p, imm_p) &&
                rre->get_register_number()==x86_gpr_ax && rre->get_position_in_register()==x86_regpos_low_byte;
        case od_AX:
            return matches(od_r16, expr, insn, disp_p, imm_p) && rre->get_register_number()==x86_gpr_ax;
        case od_EAX:
            return matches(od_r32, expr, insn, disp_p, imm_p) && rre->get_register_number()==x86_gpr_ax;
        case od_RAX:
            return matches(od_r64, expr, insn, disp_p, imm_p) && rre->get_register_number()==x86_gpr_ax;

        case od_CL:
            return matches(od_r8, expr, insn, disp_p, imm_p) &&
                rre->get_register_number()==x86_gpr_cx && rre->get_position_in_register()==x86_regpos_low_byte;

        case od_DX:
            return matches(od_r16, expr, insn, disp_p, imm_p) && rre->get_register_number()==x86_gpr_dx;

        case od_CS:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_cs;
        case od_DS:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_ds;
        case od_ES:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_es;
        case od_FS:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_fs;
        case od_GS:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_gs;
        case od_SS:
            return rre && rre->get_register_class()==x86_regclass_segment && rre->get_register_number()==x86_segreg_ss;

        case od_CR0:
        case od_CR7:
        case od_CR8:
        case od_CR0CR7:
        case od_DR0DR7:
            throw Exception("operand type not implemented", insn);

        case od_rel8:
            if (ve) {
                *disp_p = SageInterface::getAsmSignedConstant(ve);
                return matches_rel(insn, *disp_p, 1);
            }
            return false;
        case od_rel16:
            if (ve) {
                *disp_p = SageInterface::getAsmSignedConstant(ve);
                return matches_rel(insn, *disp_p, 2);
            }
            return false;
        case od_rel32:
            if (ve) {
                *disp_p = SageInterface::getAsmSignedConstant(ve);
                return matches_rel(insn, *disp_p, 4);
            }
            return false;
        case od_rel64:
            if (ve) {
                *disp_p = SageInterface::getAsmSignedConstant(ve);
                return matches_rel(insn, *disp_p, 8);
            }
            return false;

        case od_ptr16_16:
        case od_ptr16_32:
        case od_ptr16_64:
            throw Exception("operand types ptr16:16, ptr16:32, ptr16:64 not implemented", insn);

        case od_m16_16:
        case od_m16_32:
        case od_m16_64:
            throw Exception("operand types m16:16, m16:32, m16:64 not implemented", insn);

        case od_reg:
            throw Exception("operand type reg not implemented", insn);

        case od_r8:
            return (rre && rre->get_register_class()==x86_regclass_gpr &&
                    (rre->get_position_in_register()==x86_regpos_low_byte ||
                     rre->get_position_in_register()==x86_regpos_high_byte));
        case od_r16:
            return rre && rre->get_register_class()==x86_regclass_gpr && rre->get_position_in_register()==x86_regpos_word;
        case od_r32:
            return rre && rre->get_register_class()==x86_regclass_gpr && rre->get_position_in_register()==x86_regpos_dword;
        case od_r64:
            return rre && rre->get_register_class()==x86_regclass_gpr && rre->get_position_in_register()==x86_regpos_qword;

        case od_imm8:
            if (ve) {
                *imm_p = SageInterface::getAsmSignedConstant(ve);
                if (honor_operand_types) {
                    return NULL!=isSgAsmByteValueExpression(ve);
                } else {
                    return *imm_p>=-128 && *imm_p<=127;
                }
            }
            return false;
        case od_imm16:
            if (ve) {
                *imm_p = SageInterface::getAsmSignedConstant(ve);
                if (honor_operand_types) {
                    return NULL!=isSgAsmWordValueExpression(ve);
                } else {
                    return *imm_p>=-32768 && *imm_p<=32767;
                }
            }
            return false;
        case od_imm32:
            if (ve) {
                *imm_p = SageInterface::getAsmSignedConstant(ve);
                if (honor_operand_types) {
                    return NULL!=isSgAsmDoubleWordValueExpression(ve);
                } else {
                    return *imm_p>=-2147483648LL && *imm_p<=2147483647LL;
                }
            }
            return false;
        case od_imm64:
            if (ve) {
                *imm_p = SageInterface::getAsmSignedConstant(ve);
                if (honor_operand_types) {
                    return NULL!=isSgAsmQuadWordValueExpression(ve);
                } else {
                    return true;
                }
            }
            return false;

        case od_r_m8:
            return matches(od_r8, expr, insn, disp_p, imm_p) || matches(od_m8, expr, insn, disp_p, imm_p);
        case od_r_m16:
            return matches(od_r16, expr, insn, disp_p, imm_p) || matches(od_m16, expr, insn, disp_p, imm_p);
        case od_r_m32:
            return matches(od_r32, expr, insn, disp_p, imm_p) || matches(od_m32, expr, insn, disp_p, imm_p);
        case od_r_m64:
            return matches(od_r64, expr, insn, disp_p, imm_p) || matches(od_m64, expr, insn, disp_p, imm_p);

        case od_r16_m16:
        case od_r32_m8:
        case od_r32_m16:
        case od_r64_m16:
            throw Exception("operand type not implemented", insn);

        case od_m:
            return (matches(od_m8, expr, insn, disp_p, imm_p) || matches(od_m16, expr, insn, disp_p, imm_p) ||
                    matches(od_m32, expr, insn, disp_p, imm_p) || matches(od_m64, expr, insn, disp_p, imm_p));
        case od_m8:
            return mre && isSgAsmTypeByte(mre->get_type());
        case od_m16:
            return mre && isSgAsmTypeWord(mre->get_type());
        case od_m32:
            return mre && (isSgAsmTypeDoubleWord(mre->get_type()) || isSgAsmTypeSingleFloat(mre->get_type()));
        case od_m64:
            return mre && (isSgAsmTypeQuadWord(mre->get_type()) || isSgAsmTypeDoubleFloat(mre->get_type()));
        case od_m128:
            throw Exception("m128 not implemented", insn);

        case od_m16a16:
        case od_m16a32:
        case od_m32a32:
        case od_m16a64:
            throw Exception("m16&16, m16&32, m32&32, m16&64 not implemented", insn);

        case od_moffs8:
            if (mre && isSgAsmTypeByte(mre->get_type()) && isSgAsmValueExpression(mre->get_address())) {
                *imm_p = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(mre->get_address()));
                return true;
            }
            return false;
        case od_moffs16:
            if (mre && isSgAsmTypeWord(mre->get_type()) && isSgAsmValueExpression(mre->get_address())) {
                *imm_p = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(mre->get_address()));
                return true;
            }
            return false;
        case od_moffs32:
            if (mre && isSgAsmTypeDoubleWord(mre->get_type()) && isSgAsmValueExpression(mre->get_address())) {
                *imm_p = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(mre->get_address()));
                return true;
            }
            return false;
        case od_moffs64:
            if (mre && isSgAsmTypeQuadWord(mre->get_type()) && isSgAsmValueExpression(mre->get_address())) {
                *imm_p = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(mre->get_address()));
                return true;
            }
            return false;

        case od_sreg:
            throw Exception("sreg not implemented", insn);

        case od_m32fp:
            return mre && isSgAsmTypeSingleFloat(mre->get_type());
        case od_m64fp:
            return mre && isSgAsmTypeDoubleFloat(mre->get_type());
        case od_m80fp:
            return mre && NULL!=isSgAsmType80bitFloat(mre->get_type());

        case od_st0:
            return rre && x86_regclass_st==rre->get_register_class() && 0==rre->get_register_number();
        case od_st1:
            return rre && x86_regclass_st==rre->get_register_class() && 1==rre->get_register_number();
        case od_st2:
            return rre && x86_regclass_st==rre->get_register_class() && 2==rre->get_register_number();
        case od_st3:
            return rre && x86_regclass_st==rre->get_register_class() && 3==rre->get_register_number();
        case od_st4:
            return rre && x86_regclass_st==rre->get_register_class() && 4==rre->get_register_number();
        case od_st5:
            return rre && x86_regclass_st==rre->get_register_class() && 5==rre->get_register_number();
        case od_st6:
            return rre && x86_regclass_st==rre->get_register_class() && 6==rre->get_register_number();
        case od_st7:
            return rre && x86_regclass_st==rre->get_register_class() && 7==rre->get_register_number();
        case od_sti:
            return rre && x86_regclass_st==rre->get_register_class();

        case od_mm:
            return rre && x86_regclass_mm==rre->get_register_class();

        case od_mm_m32:
            return matches(od_mm, expr, insn, disp_p, imm_p) || matches(od_m32, expr, insn, disp_p, imm_p);
        case od_mm_m64:
            return matches(od_mm, expr, insn, disp_p, imm_p) || matches(od_m64, expr, insn, disp_p, imm_p);

        case od_xmm:
            return rre && x86_regclass_xmm==rre->get_register_class();

        case od_xmm_m32:
            return matches(od_xmm, expr, insn, disp_p, imm_p) || matches(od_m32, expr, insn, disp_p, imm_p);
        case od_xmm_m64:
            return matches(od_xmm, expr, insn, disp_p, imm_p) || matches(od_m64, expr, insn, disp_p, imm_p);
        case od_xmm_m128: {
            /* An XMM register or memory with one of the following data types:
             *   - a vector of four single-precision floating-point values
             *   - a vector of two double-precision floating-point values
             *   - a double quad word */
            if (rre)
                return matches(od_xmm, expr, insn, disp_p, imm_p);
            if (!mre)
                return false;
            SgAsmTypeVector *tv = isSgAsmTypeVector(mre->get_type());
            if (tv)
                return ((tv->get_elementCount()==4 && isSgAsmTypeSingleFloat(tv->get_elementType())) ||
                        (tv->get_elementCount()==2 && isSgAsmTypeDoubleFloat(tv->get_elementType())));
            return NULL!=isSgAsmTypeDoubleQuadWord(mre->get_type());
        }

        case od_XMM0:   /*implicit register "<XMM0>" */
        case od_xmm_m16:
            throw Exception("xmm operand types are not implemented", insn);

        case od_m2byte:
            return matches(od_m16, expr, insn, disp_p, imm_p);

        case od_dec:
        case od_m80:
        case od_m80bcd:
        case od_m14_28byte:
        case od_m94_108byte:
        case od_m512byte:
            throw Exception("operand type not implemented", insn);
    }
    abort();
    /* avoid MSCV warning by adding return stmt */
    return false;
}

void
AssemblerX86::matches(const InsnDefn *defn, SgAsmx86Instruction *insn, int64_t *disp_p, int64_t *imm_p) const
{
    if (insn->get_kind()!=defn->kind)
        throw Exception("insn kind doesn't match definition", insn);
    if (insn->get_operandList()->get_operands().size()!=defn->operands.size())
        throw Exception("wrong number of operands", insn);
    if (insn->get_baseSize()==x86_insnsize_64) {
        if (0==(defn->compatibility & COMPAT_64))
            throw Exception("invalid instruction for 64-bit assembly", insn);
    } else if (0==(defn->compatibility & COMPAT_LEGACY)) {
        throw Exception("invalid instruction for legacy assembly", insn);
    }

    if (disp_p || imm_p) {
        for (size_t i=0; i<defn->operands.size(); i++) {
            if (!matches(defn->operands[i], insn->get_operandList()->get_operands()[i], insn, disp_p, imm_p)) {
                throw Exception("operand " + StringUtility::numberToString(i) + " is incorrect type", insn);
            }
        }
    }
}

AssemblerX86::MemoryReferencePattern
AssemblerX86::parse_memref(SgAsmInstruction *insn, SgAsmMemoryReferenceExpression *expr,
                           SgAsmx86RegisterReferenceExpression **base_reg/*out*/,
                           SgAsmx86RegisterReferenceExpression **index_reg/*out*/, SgAsmValueExpression **scale_ve/*out*/, 
                           SgAsmValueExpression **disp_ve/*out*/)
{
    if (!expr)
        return mrp_unknown;

    *base_reg = *index_reg = NULL;
    *scale_ve = *disp_ve = NULL;

    std::vector<SgAsmExpression*> addends;
    if (isSgAsmBinaryAdd(expr->get_address())) {
        std::vector<SgAsmBinaryAdd*> adds;
        adds.push_back(isSgAsmBinaryAdd(expr->get_address()));
        while (adds.size()>0) {
            SgAsmBinaryAdd *add = adds.back();
            adds.pop_back();
            SgAsmBinaryAdd *add2 = isSgAsmBinaryAdd(add->get_lhs());
            if (!add2) {
                addends.push_back(add->get_lhs());
            } else {
                adds.push_back(add2);
            }
            add2 = isSgAsmBinaryAdd(add->get_rhs());
            if (!add2) {
                addends.push_back(add->get_rhs());
            } else {
                adds.push_back(add2);
            }
        }
    } else {
        addends.push_back(expr->get_address());
    }
    
    SgAsmx86RegisterReferenceExpression *add_rre=NULL, *mult_rre=NULL;
    SgAsmValueExpression *add_ve=NULL, *mult_ve=NULL;
    for (std::vector<SgAsmExpression*>::iterator i=addends.begin(); i!=addends.end(); ++i) {
        if (isSgAsmx86RegisterReferenceExpression(*i)) {
            if (add_rre) {
                /* treat as Multiply(expr,1) */
                if (mult_rre)
                    throw Exception("unable to encode memory reference expression (multiple register reference addends)", insn);
                mult_rre = isSgAsmx86RegisterReferenceExpression(*i);
            } else {
                add_rre = isSgAsmx86RegisterReferenceExpression(*i);
            }
        } else if (isSgAsmValueExpression(*i)) {
            if (add_ve)
                throw Exception("unable to encode memory reference expression (multiple value addends)", insn);
            add_ve = isSgAsmValueExpression(*i);
        } else if (isSgAsmBinaryMultiply(*i)) {
            if (mult_rre)
                throw Exception("unable to encode memory reference expression (multiple multiplies)", insn);
            SgAsmBinaryMultiply *mult = isSgAsmBinaryMultiply(*i);
            mult_rre = isSgAsmx86RegisterReferenceExpression(mult->get_lhs());
            if (!mult_rre) mult_rre = isSgAsmx86RegisterReferenceExpression(mult->get_rhs());
            mult_ve = isSgAsmValueExpression(mult->get_lhs());
            if (!mult_ve) mult_ve = isSgAsmValueExpression(mult->get_rhs());
            if (!mult_rre || !mult_ve)
                throw Exception("unable to encode memory reference expression (multiply is not register * value)", insn);
        } else {
            throw Exception("unable to encode memory reference expression (unsupported addend)", insn);
        }
    }

    *base_reg = add_rre;
    *disp_ve = add_ve;
    *index_reg = mult_rre;
    *scale_ve = mult_ve;
        
    switch ((add_rre ? 0x4 : 0) | (mult_rre ? 0x2 : 0) | (add_ve ? 0x1 : 0) ) {
        case 0: return mrp_unknown;
        case 1: return mrp_disp;
        case 2: return mrp_index;
        case 3: return mrp_index_disp;
        case 4: return mrp_base;
        case 5: return mrp_base_disp;
        case 6: return mrp_base_index;
        case 7: return mrp_base_index_disp;
    }

    return mrp_unknown;
}

void
AssemblerX86::build_modreg(const InsnDefn *defn, SgAsmx86Instruction *insn, size_t argno,
                           uint8_t *modrm/*in,out*/, uint8_t *rex/*in,out*/) const
{
    ROSE_ASSERT(insn!=NULL);
    ROSE_ASSERT(argno<insn->get_operandList()->get_operands().size());
    ROSE_ASSERT(argno<defn->operands.size());
    ROSE_ASSERT(modrm && 0==(*modrm & 070)); /*reg field must be zero*/

    switch (defn->operands[argno]) {
        case od_r8:
        case od_r16:
        case od_r32:
        case od_r64:
        case od_mm:
        case od_xmm:
            break;
        default:
            throw Exception("operand does not affect reg field of ModR/M byte", insn);
    }
    
    uint8_t local_rex=0, local_sib=0;
    uint8_t local_modrm = build_modrm(defn, insn, argno, &local_sib, NULL, &local_rex);
    ROSE_ASSERT(3==modrm_mod(local_modrm)); /*mod field indicates register*/
    ROSE_ASSERT(local_sib==0);
    
    /* Return reg and REX based on build_modrm */
    *modrm |= (local_modrm & 7) << 3;                   /*set "reg" field from "r/m" field*/
    if (local_rex & 0x1) *rex |= od_rex_byte(od_rexr);  /*set REX.R if REX.B was set*/
}

uint8_t
AssemblerX86::build_modrm(const InsnDefn *defn, SgAsmx86Instruction *insn, size_t argno, 
                          uint8_t *sib/*out*/, int64_t *displacement/*out*/, uint8_t *rex/*in,out*/) const
{
    ROSE_ASSERT(insn!=NULL);
    ROSE_ASSERT(argno<insn->get_operandList()->get_operands().size());
    ROSE_ASSERT(argno<defn->operands.size());

    SgAsmExpression *expr = insn->get_operandList()->get_operands()[argno];
    uint8_t mod=0, reg=0, rm=0;     /*parts of modR/M byte */
    uint8_t ss=0, index=0, base=0;  /*parts of SIB byte */

    /* These are the operand types that need a ModR/M byte */
    switch (defn->operands[argno]) {
        case od_r8:
        case od_r16:
        case od_r32:
        case od_r64:
        case od_r_m8:
        case od_r_m16:
        case od_r_m32:
        case od_r_m64:
        case od_m8:
        case od_m16:
        case od_m32:
        case od_m64:
        case od_m:
        case od_mm:
        case od_mm_m32:
        case od_mm_m64:
        case od_xmm:
        case od_xmm_m32:
        case od_xmm_m64:
        case od_xmm_m128:
        case od_m32fp:
        case od_m64fp:
        case od_m80fp:
        case od_m2byte:
            break;
        default:
            throw Exception("operand does not affect ModR/M byte", insn);
    }
    
    SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(expr);
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr);

    if (rre) {
        mod = 3;
        if (rre->get_register_class()==x86_regclass_gpr) {
            switch (rre->get_position_in_register()) {
                case x86_regpos_unknown: {
                    ROSE_ASSERT(!"unknown register position");
                }
                case x86_regpos_low_byte: {
                    rm = rre->get_register_number() % 8;
                    if (rre->get_register_number()>=8)
                        *rex |= od_rex_byte(od_rexb);
                    if (rm>4)
                        *rex |= od_rex_byte(od_rex); /*low byte of SP BP SI DI*/
                    break;
                }
                case x86_regpos_high_byte: {
                    switch (rre->get_register_number()) {
                        case x86_gpr_ax: rm = 4; break;
                        case x86_gpr_cx: rm = 5; break;
                        case x86_gpr_dx: rm = 6; break;
                        case x86_gpr_bx: rm = 7; break;
                        default: ROSE_ASSERT(!"invalid register number for high byte ModR/M");
                    }
                    break;
                }
                case x86_regpos_word: {
                    rm = rre->get_register_number() % 8;
                    if (rre->get_register_number() >= 8)
                        *rex |= od_rex_byte(od_rexb);
                    break;
                }
                case x86_regpos_dword: {
                    rm = rre->get_register_number() % 8;
                    if (rre->get_register_number()>=8)
                        *rex |= od_rex_byte(od_rexb);
                    break;
                }
                case x86_regpos_qword: {
                    rm = rre->get_register_number() % 8;
                    if (rre->get_register_number()>=8)
                        *rex |= od_rex_byte(od_rexb);
                    break;
                }
                case x86_regpos_all: {
                    ROSE_ASSERT(!"not implemented");
                }
            }
        } else if (rre->get_register_class()==x86_regclass_mm) {
            rm = rre->get_register_number() % 8;
        } else if (rre->get_register_class()==x86_regclass_xmm) {
            rm = rre->get_register_number() % 8;
            if (rre->get_register_number() >= 8)
                *rex |= od_rex_byte(od_rexb);
        } else {
            ROSE_ASSERT(!"not implemented");
        }
    } else if (mre) {
        SgAsmx86RegisterReferenceExpression *base_reg=NULL, *index_reg=NULL;
        SgAsmValueExpression *disp_ve=NULL, *scale_ve=NULL;
        MemoryReferencePattern mrp = parse_memref(insn, mre, &base_reg, &index_reg, &scale_ve, &disp_ve);

        /* Scale for SIB byte */
        if (scale_ve) {
            switch (SageInterface::getAsmSignedConstant(scale_ve)) {
                case 1: ss = 0; break;
                case 2: ss = 1; break;
                case 4: ss = 2; break;
                case 8: ss = 3; break;
                default: throw Exception("cannot encode scale factor for index", insn);
            }
        }

        /* Displacement from base address */
        if (disp_ve) {
            ROSE_ASSERT(displacement);
            *displacement = SageInterface::getAsmSignedConstant(disp_ve);
        }

        switch (mrp) {
            case mrp_disp: {
                ROSE_ASSERT(!base_reg && !index_reg && !scale_ve && disp_ve);
                if (insn->get_baseSize()==x86_insnsize_32) {
                    /* No need for SIB byte; use "disp32" addressing mode */
                    mod = 0;
                    rm = 5;
                    ss = index = base = 0; /*not used*/
                } else {
                    mod = 0; /*indicates disp32 when combined with SIB*/
                    rm = 4; /*implies SIB*/
                    ss = 0;
                    index = 4; /*none*/
                    base = 5; /*none*/
                }
                break;
            }
                
            case mrp_index:
                ROSE_ASSERT(!"not implemented");

            case mrp_base: {
                ROSE_ASSERT(base_reg && !index_reg && !scale_ve && !disp_ve);
                ROSE_ASSERT(base_reg->get_register_class()==x86_regclass_gpr);
                mod = 0;
                rm = base_reg->get_register_number() % 8;
                if (base_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexb);
                if (rm==4) {
                    ss = 0;
                    index = 4;
                    base = 4;
                }
                break;
            }

            case mrp_base_disp: {
                ROSE_ASSERT(base_reg && !index_reg && !scale_ve && disp_ve);
                if (base_reg->get_register_class()==x86_regclass_ip) {
                    /* disp32 */
                    mod = 0;
                    rm = 5;
                } else if (base_reg->get_register_class()==x86_regclass_gpr) {
                    /* [register]+disp8  or  [register]+disp32 */
                    if (honor_operand_types) {
                        mod = isSgAsmByteValueExpression(disp_ve) ? 1 : 2;
                    } else {
                        mod = (*displacement>=-128 && *displacement<=127) ? 1 : 2;
                    }
                    rm = base_reg->get_register_number() % 8;
                    if (base_reg->get_register_number()>=8)
                        *rex |= od_rex_byte(od_rexb);
                    if (rm==4) {
                        ss = 0;
                        index = 4;
                        base = 4;
                    }
                } else {
                    throw Exception("bad register class for [register+disp8] or [register+disp32]", insn);
                }
                break;
            }

            case mrp_base_index: {
                ROSE_ASSERT(base_reg && index_reg && !disp_ve);
                mod = 0;
                rm = 4; /*implies SIB*/
                if (base_reg->get_register_class()!=x86_regclass_gpr)
                    throw Exception("cannot encode register class for base register", insn);
                if (index_reg->get_register_class()!=x86_regclass_gpr)
                    throw Exception("cannot encode register class for index register", insn);
                base = base_reg->get_register_number() % 8;
                if (base_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexb);
                index = index_reg->get_register_number() % 8;
                if (index_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexx);
                break;
            }

            case mrp_index_disp: {
                ROSE_ASSERT(!base_reg && index_reg && disp_ve);
                /* mod=0 and base=5 implies "[scaled index] + disp32". Other values of mod are possible for 8- and 32-bit
                 * displacements from EBP, but parse_memref() doesn't handle them yet. FIXME [RPM 2009-07-09] */  
                mod = 0;
                rm = 4; /*implies SIB*/
                base = 5;
                index = index_reg->get_register_number() % 8;
                if (index_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexx);
                break;
            }

            case mrp_base_index_disp: {
                ROSE_ASSERT(base_reg && index_reg && disp_ve);
                if (honor_operand_types) {
                    mod = isSgAsmByteValueExpression(disp_ve) ? 1 : 2;
                } else {
                    mod = (*displacement>=-128 && *displacement<=127) ? 1 : 2;
                }
                rm = 4; /*implies SIB*/
                if (base_reg->get_register_class()!=x86_regclass_gpr)
                    throw Exception("cannot encode register class for base register", insn);
                if (index_reg->get_register_class()!=x86_regclass_gpr)
                    throw Exception("cannot encode register class for index register", insn);
                base = base_reg->get_register_number() % 8;
                if (base_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexb);
                index = index_reg->get_register_number() % 8;
                if (index_reg->get_register_number()>=8)
                    *rex |= od_rex_byte(od_rexx);
                break;
            }

            case mrp_unknown:
                throw Exception("cannot encode memory reference expression", insn);
        }

    } else {
        return false;
    }
    
    *sib = build_sib(ss, index, base);
    return build_modrm(mod, reg, rm);
}

uint8_t
AssemblerX86::segment_override(SgAsmx86Instruction *insn)
{
    const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
    for (size_t i=0; i<operands.size(); i++) {
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(operands[i]);
        if (mre) {
            /* Find general purpose register in memory reference expression. */
            struct T1: public SgSimpleProcessing {
                bool is_found;
                int gpr;
                T1(): is_found(false) {}
                void visit(SgNode *node) {
                    SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(node);
                    if (rre && x86_regclass_gpr==rre->get_register_class() && !is_found) {
                        is_found = true;
                        gpr = rre->get_register_number();
                    }
                }
            } reg;
            reg.traverse(mre->get_address(), preorder);

            SgAsmx86RegisterReferenceExpression *seg_reg = isSgAsmx86RegisterReferenceExpression(mre->get_segment());
            ROSE_ASSERT(seg_reg!=NULL);
            ROSE_ASSERT(seg_reg->get_register_class()==x86_regclass_segment);
            switch (seg_reg->get_register_number()) {
                case x86_segreg_es: return 0x26;
                case x86_segreg_cs: return 0x2e;
                case x86_segreg_ss: 
                    if (!reg.is_found || (reg.gpr!=x86_gpr_sp && reg.gpr!=x86_gpr_bp))
                        return 0x36;
                    return 0;
                case x86_segreg_ds: return 0;
                    if (reg.is_found && (reg.gpr==x86_gpr_sp || reg.gpr==x86_gpr_bp))
                        return 0x3e;
                    return 0;
                case x86_segreg_fs: return 0x64;
                case x86_segreg_gs: return 0x65;
            }
        }
    }
    return 0;
}

SgUnsignedCharList
AssemblerX86::fixup_prefix_bytes(SgAsmx86Instruction *insn, SgUnsignedCharList source)
{
    /* What prefixes are present in the source encoding? */
    std::set<uint8_t> present;
    for (size_t i=0; i<source.size(); i++) {
        switch (source[i]) {
            case 0xf0: /* Group 1: LOCK */
            case 0xf2: /*          REPNE/REPNZ */
            case 0xf3: /*          REPE/REPZ */
            case 0x2e: /* Group 2: CS segment override or Branch not taken */
            case 0x36: /*          SS segment override */
            case 0x3e: /*          DS segment override or Branch taken */
            case 0x26: /*          ES segment override */
            case 0x64: /*          FS segment override */
            case 0x65: /*          GS segment override */
            case 0x66: /* Group 3: Operand size override */
            case 0x67: /* Group 4: Address size override */
                present.insert(source[i]);
                break;
            default:
                break;
        }
    }

    /* Copy target prefixes into return value provided they're present in the source. */
    std::set<uint8_t> pushed;
    SgUnsignedCharList reordered;
    const SgUnsignedCharList &target = insn->get_raw_bytes();
    for (size_t i=0; i<target.size(); i++) {
        switch (target[i]) {
            case 0x2e:
            case 0x3e:
                /* Group 2 branch hints. These are apparently(?) ignored in 64-bit unless the instruction is a conditional
                 * branch (Jcc), in which case they are branch taken/not-taken hints. Therefore we'll allow them as prefixes
                 * for non-jump instructions even if they aren't present in the source. See
                 * DisassemblerX86::makeSegmentRegister(). */
                if (insn->get_baseSize()!=x86_insnsize_64 ||
                    (insn->get_baseSize()==x86_insnsize_64 && x86InstructionIsConditionalBranch(insn))) {
                    if (present.find(target[i])!=present.end()) {
                        reordered.push_back(target[i]);
                        pushed.insert(target[i]);
                    }
                } else {
                    reordered.push_back(target[i]);
                    pushed.insert(target[i]);
                }
                break;

            case 0xf0:
            case 0xf2:
            case 0xf3:
            case 0x36:
            case 0x26:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
                /* Group 1, Group 2 segment overrides, Group 3, Group 4 */
                if (present.find(target[i])!=present.end()) {
                    reordered.push_back(target[i]);
                    pushed.insert(target[i]);
                }
                break;

            default:
                /* Not a known prefix. Therefore this must the the end of the prefixes. */
                goto done;
        }
    }
done:

    /* Append source prefixes to the return value if they're not present in the target. */
    for (size_t i=0; i<source.size(); i++) {
        if (pushed.find(source[i])==pushed.end())
            reordered.push_back(source[i]);
    }

    return reordered;
}

SgUnsignedCharList
AssemblerX86::assemble(SgAsmx86Instruction *insn, const InsnDefn *defn) 
{
    SgUnsignedCharList retval;
    uint8_t modrm = 0;                          /* The ModR/M byte */
    bool modrm_defined = false;                 /* True if "modrm" is defined */
    bool reg_defined = false;                   /* True if the "reg" field of the ModR/M byte is defined */
    uint8_t sib;                                /* SIB byte */
    int64_t displacement = 0;                   /* Displacement for ModR/M, as in "DWORD PTR ds:[rip+0x216417]" */
    uint8_t rex_byte = 0;                       /* REX byte; valid only if non-zero */
    uint64_t opcode = defn->opcode;
    int64_t iprel=0;                            /* IP-relative operand */
    int64_t immediate=0;                        /* Immediate-valued operand */

    matches(defn, insn, &iprel, &immediate);    /* throws exception when match fails */

    /* Lock prefix */
    if (insn->get_lockPrefix())
        retval.push_back(0xf0);

    /* Legacy prefixes that are included in the opcode definition. */
    bool emitted_repeat_prefix=false;
    bool emitted_osize_prefix=false;
    for (int i=7; i>=0; --i) {
        uint8_t opcode_byte = (opcode >> (8*i)) & 0xff;
        switch (opcode_byte) {
            case 0xf2:
            case 0xf3:
                retval.push_back(opcode_byte);
                opcode &= ~(0xff << (8*i));
                opcode_byte = 0;
                emitted_repeat_prefix = true;
                break;
            case 0x66:
                retval.push_back(opcode_byte);
                opcode &= ~(0xff << (8*i));
                opcode_byte = 0;
                emitted_osize_prefix = true;
                break;
        }
        if (opcode_byte)
            break;
    }

    /* Legacy Repeat prefixes if necessary according to instruction. */
    if (!emitted_repeat_prefix) {
        switch (insn->get_repeatPrefix()) {
            case x86_repeat_none:                          break;
            case x86_repeat_repne: retval.push_back(0xf2); break;
            case x86_repeat_repe:  retval.push_back(0xf3); break;
        }
    }

    /* Segment Override legacy prefix. */
    if (x86InstructionIsConditionalBranch(insn)) {
        switch (insn->get_branchPrediction()) {
            case x86_branch_prediction_none: break;
            case x86_branch_prediction_taken: retval.push_back(0x3e); break;
            case x86_branch_prediction_not_taken: retval.push_back(0x2e); break;
        }
    } else {
        uint8_t so_byte = segment_override(insn);
        if (so_byte!=0)
            retval.push_back(so_byte);
    }

    /* Operand size override legacy prefix. */
    if (!emitted_osize_prefix) {
        switch (insn->get_baseSize()) {
            case x86_insnsize_none:
                abort();
            case x86_insnsize_64:
                if (insn->get_operandSize()==x86_insnsize_16)
                    retval.push_back(0x66);
                break;
            case x86_insnsize_32:
                if (insn->get_operandSize()==x86_insnsize_16)
                    retval.push_back(0x66);
                break;
            case x86_insnsize_16:
                if (insn->get_operandSize()==x86_insnsize_32)
                    retval.push_back(0x66);
                break;
        }
    }

    /* Reorder the legacy prefixes */
    if (get_encoding_type()==ET_MATCHES)
        retval = fixup_prefix_bytes(insn, retval);

    /* REX byte if necessary (64-bit only), output delayed */
    if (defn->opcode_modifiers & od_rex_pres)
        rex_byte = od_rex_byte(defn->opcode_modifiers);
        
    /* Adjust opcode according to register number. */
    if (defn->opcode_modifiers & od_r_mask) {
        ROSE_ASSERT(insn->get_operandList()->get_operands().size()>=1);
        SgAsmExpression *expr = insn->get_operandList()->get_operands()[0];
        SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(expr);
        ROSE_ASSERT(rre!=NULL);
        ROSE_ASSERT(rre->get_register_class()==x86_regclass_gpr);
        if (rre->get_register_number()>=8)
            rex_byte |= od_rex_byte(od_rexb);
        opcode += rre->get_register_number() % 8;
        if (rre->get_position_in_register()==x86_regpos_high_byte)
            opcode += 4;
    }
    if (defn->opcode_modifiers & od_i) {
        for (size_t i=0; i<defn->operands.size(); i++) {
            if (defn->operands[i]==od_sti) {
                SgAsmExpression *expr = insn->get_operandList()->get_operands()[i];
                SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(expr);
                ROSE_ASSERT(rre && x86_regclass_st==rre->get_register_class());
                opcode += rre->get_register_number();
                break;
            }
        }
    }
    
    /* Calculate the mod and r/m fields (bits 0307) of the ModR/M byte for any operand that's an appropriate type. There can
     * be only one such operand. If we find that the operand refers to a 64-bit register then we also adjust the REX byte. Any
     * displacement value is also returned and displacement_defined is set accordingly. */
    for (size_t i=0; i<defn->operands.size(); i++) {
        switch (defn->operands[i]) {
            case od_r_m8:
            case od_r_m16:
            case od_r_m32:
            case od_r_m64:
            case od_m8:
            case od_m16:
            case od_m32:
            case od_m64:
            case od_m:
            case od_mm_m32:
            case od_mm_m64:
            case od_xmm_m32:
            case od_xmm_m64:
            case od_xmm_m128:
            case od_m32fp:
            case od_m64fp:
            case od_m80fp:
            case od_m2byte:
                if (modrm_defined)
                    throw Exception("multiple ModR/M-affecting operands", insn);
                modrm = build_modrm(defn, insn, i, &sib, &displacement, &rex_byte);
                modrm_defined = true;
                break;
            default:
                break;
        }
    }

    /* If a full ModR/M byte is indicated then look for a register operand. If we didn't see a previous register-or-memory
     * operand then the first register operand is placed in the "mod" and "rm" fields of the ModR/M byte. Otherwise the
     * register is placed in the "reg" field. */
    if (defn->opcode_modifiers & od_modrm) {
        for (size_t i=0; i<defn->operands.size(); i++) {
            switch (defn->operands[i]) {
                case od_r8:
                case od_r16:
                case od_r32:
                case od_r64:
                case od_mm:
                case od_xmm:
                    if (modrm_defined) {
                        if (reg_defined)
                            throw Exception("too many ModR/M-affecting operands", insn);
                        if (defn->opcode_modifiers & od_e_mask)
                            throw Exception("register and opcode extension are mutually exclusive", insn);
                        build_modreg(defn, insn, i, &modrm, &rex_byte);
                        reg_defined = true;
                    } else {
                        modrm = build_modrm(defn, insn, i, &sib, &displacement, &rex_byte);
                        modrm_defined = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    /* Opcode extension in the "reg" field of the ModR/M byte. */
    if (defn->opcode_modifiers & od_e_mask) {
        ROSE_ASSERT(modrm_defined);
        ROSE_ASSERT(!reg_defined);
        ROSE_ASSERT(0==modrm_reg(modrm));
        modrm = build_modrm(modrm_mod(modrm), od_e_val(defn->opcode_modifiers), modrm_rm(modrm));
    }

    /* Output REX byte */
    if (rex_byte)
        retval.push_back(rex_byte);
    
    /* Output opcode */
    ROSE_ASSERT(0==(opcode>>40));
    if (opcode > 0xffffffff)
        retval.push_back((opcode>>32) & 0xff);
    if (opcode > 0xffffff)
        retval.push_back((opcode>>24) & 0xff);
    if (opcode > 0xffff)
        retval.push_back((opcode>>16) & 0xff);
    if (opcode > 0xff)
        retval.push_back((opcode>>8) & 0xff);
    retval.push_back(opcode & 0xff);

    /* Output ModR/M and SIB bytes */
    if (modrm_defined)
        retval.push_back(modrm);
    if (modrm_defined && 4==modrm_rm(modrm) && 3!=modrm_mod(modrm))
        retval.push_back(sib);

    /* Output displacement for ModR/M */
    if (modrm_defined) {
        if (0==modrm_mod(modrm) && 4==modrm_rm(modrm) && 5==sib_base(sib)) {
            /* [disp32 + reg*scale] */
            retval.push_back(displacement & 0xff);
            retval.push_back((displacement>>8) & 0xff);
            retval.push_back((displacement>>16) & 0xff);
            retval.push_back((displacement>>24) & 0xff);
        } else if (0==modrm_mod(modrm) && 5==modrm_rm(modrm)) {
            /* disp32 (i.e, rip-relative) */
            retval.push_back(displacement & 0xff);
            retval.push_back((displacement>>8) & 0xff);
            retval.push_back((displacement>>16) & 0xff);
            retval.push_back((displacement>>24) & 0xff);
        } else if (1==modrm_mod(modrm)) {
            /* [register]+disp8 */
            ROSE_ASSERT(displacement <= 0xff);
            retval.push_back(displacement);
        } else if (2==modrm_mod(modrm)) {
            /* [register]+disp32 */
            retval.push_back(displacement & 0xff);
            retval.push_back((displacement>>8) & 0xff);
            retval.push_back((displacement>>16) & 0xff);
            retval.push_back((displacement>>24) & 0xff);
        }
    }

    /* Output IP displacement for cb, cw, cd, cp, co, and ct opcode modifiers. */
    if (defn->opcode_modifiers & od_c_mask) {
        iprel -= insn->get_address() + retval.size(); /*adjustment for what we've already encoded*/
        switch (defn->opcode_modifiers & od_c_mask) {
            case od_cb:
                iprel -= 1;
                if (iprel<-128 || iprel>127)
                    throw Exception("displacement out of range", insn);
                retval.push_back(iprel & 0xff);
                break;
            case od_cw:
                /* The following 'if' was added for 32-bit assembly to prevent JMP instructions from being encoded with a two
                 * byte displacement when there was no operand override prefix.  The relevant lines from the Intel manual are
                 * on page 3-105:
                 *
                 *   E8 cw                CALL rel16    N.S.      Valid      Call near, relative, displacement
                 *                                                           relative to next instruction.
                 *   E8 cd                CALL rel32    Valid     Valid      Call near, relative, displacement
                 *                                                           relative to next instruction. 32-bit
                 *                                                           displacement sign extended to 64-bits
                 *                                                           in 64-bit mode.
                 * [RPM 2010-05-03] */
                if (x86_insnsize_16!=insn->get_operandSize())
                    throw Exception("operand size is not 16", insn);
                iprel -= 2;
                if (iprel<-32768 || iprel>32767)
                    throw Exception("displacement out of range", insn);
                retval.push_back(iprel & 0xff);
                retval.push_back((iprel>>8) & 0xff);
                break;
            case od_cd:
                iprel -= 4;
                if (iprel<-2147483648LL || iprel>2147483647)
                    throw Exception("displacement out of range", insn);
                retval.push_back(iprel & 0xff);
                retval.push_back((iprel>>8) & 0xff);
                retval.push_back((iprel>>16) & 0xff);
                retval.push_back((iprel>>24) & 0xff);
                break;
            case od_cp:
                iprel -= 6;
                if (iprel<-140737488355328LL || iprel>140737488355327LL)
                    throw Exception("displacement out of range", insn);
                retval.push_back(iprel & 0xff);
                retval.push_back((iprel>>8) & 0xff);
                retval.push_back((iprel>>16) & 0xff);
                retval.push_back((iprel>>24) & 0xff);
                retval.push_back((iprel>>32) & 0xff);
                retval.push_back((iprel>>40) & 0xff);
                break;
            case od_co:
                if (iprel<-9223372036854775800LL) /* -(2^63)+8 */
                    throw Exception("displacement out of range", insn);
                iprel -= 8;
                retval.push_back(iprel & 0xff);
                retval.push_back((iprel>>8) & 0xff);
                retval.push_back((iprel>>16) & 0xff);
                retval.push_back((iprel>>24) & 0xff);
                retval.push_back((iprel>>32) & 0xff);
                retval.push_back((iprel>>40) & 0xff);
                retval.push_back((iprel>>48) & 0xff);
                retval.push_back((iprel>>56) & 0xff);
                break;
            case od_ct:
                if (iprel<-9223372036854775798LL) /* -(2^63)+10 */
                    throw Exception("displacement out of range", insn);
                iprel -= 10;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);  iprel >>= 8;
                retval.push_back(iprel & 0xff);
                break;
            default:
                abort();
        }
    }

    /* Output moffs8, moffs16, moffs32, or moffs64 operand for MOV instruction. The value was loaded into "immediate" when we
     * matched the instruction. */
    if (defn->kind==x86_mov) {
        for (size_t i=0; i<defn->operands.size(); i++) {
            switch (defn->operands[i]) {
                case od_moffs8:
                case od_moffs16:
                case od_moffs32:
                case od_moffs64:
                    retval.push_back(immediate & 0xff);
                    retval.push_back((immediate>>8) & 0xff);
                    retval.push_back((immediate>>16) & 0xff);
                    retval.push_back((immediate>>24) & 0xff);
                    if (x86_insnsize_64==insn->get_baseSize()) {
                        retval.push_back((immediate>>32) & 0xff);
                        retval.push_back((immediate>>40) & 0xff);
                        retval.push_back((immediate>>48) & 0xff);
                        retval.push_back((immediate>>56) & 0xff);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    /* Output immediate */
    if (defn->opcode_modifiers & od_i_mask) {
        switch (defn->opcode_modifiers & od_i_mask) {
            case od_ib:
                retval.push_back(immediate & 0xff);
                break;
            case od_iw:
                retval.push_back(immediate & 0xff);
                retval.push_back((immediate>>8) & 0xff);
                break;
            case od_id:
                retval.push_back(immediate & 0xff);
                retval.push_back((immediate>>8) & 0xff);
                retval.push_back((immediate>>16) & 0xff);
                retval.push_back((immediate>>24) & 0xff);
                break;
            case od_io:
                retval.push_back(immediate & 0xff);
                retval.push_back((immediate>>8) & 0xff);
                retval.push_back((immediate>>16) & 0xff);
                retval.push_back((immediate>>24) & 0xff);
                retval.push_back((immediate>>32) & 0xff);
                retval.push_back((immediate>>40) & 0xff);
                retval.push_back((immediate>>48) & 0xff);
                retval.push_back((immediate>>56) & 0xff);
                break;
            default:
                abort();
        }
    }

    return retval;
}

/* See Assembler::assembleOne for documentation. */
SgUnsignedCharList
AssemblerX86::assembleOne(SgAsmInstruction *_insn) 
{
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(_insn);
    ROSE_ASSERT(insn);

    if (get_encoding_type()==ET_MATCHES)
        set_honor_operand_types(true);

    SgUnsignedCharList best;

    static size_t nassembled=0;
    if (0==++nassembled % 10000)
        fprintf(stderr, "AssemblerX86[va 0x%08"PRIx64"]: assembled %zu instructions\n", insn->get_address(), nassembled);

    /* Instruction */
    if (p_debug) {
        HexdumpFormat hf;
        hf.prefix = "            ";
        hf.width = 16;
        hf.pad_numeric = hf.show_chars = false;
        SgAsmExecutableFileFormat::hexdump(p_debug, insn->get_address(),  &(insn->get_raw_bytes()[0]),
                                           insn->get_raw_bytes().size(), hf);
        fprintf(p_debug, " | %s\n", unparseInstruction(insn).c_str());
#if 0 /*DEBUGGING*/
        fprintf(p_debug, "  baseSize=%d, operandSize=%d\n", 
               (x86_insnsize_16==insn->get_baseSize()?16:(x86_insnsize_32==insn->get_baseSize()?32:64)),
               (x86_insnsize_16==insn->get_operandSize()?16:(x86_insnsize_32==insn->get_operandSize()?32:64)));
        for (size_t i=0; i<insn->get_operandList()->get_operands().size(); i++) {
            SgAsmExpression *operand = insn->get_operandList()->get_operands()[i];
            fprintf(p_debug, "  operand[%zu]=", i);
            printExpr(p_debug, operand, "    ");
            fprintf(p_debug, "\n");
        }
#endif
    }

    InsnDictionary::const_iterator dict_i = defns.find(insn->get_kind());
    if (dict_i==defns.end())
        throw Exception("no assembly definition", insn);
    
    const DictionaryPage &dict_page = dict_i->second;
    for (size_t i=0; i<dict_page.size(); i++) {
        /* Definition */
        const InsnDefn *defn = dict_page[i];
        if (defn->kind != insn->get_kind())
            continue;
        if (p_debug)
            fprintf(p_debug, "  #%03zu: %s", i, defn->to_str().c_str());

        /* Assemble */
        SgUnsignedCharList s;
        try {
            s = assemble(insn, defn);
        } catch(const Exception &e) {
            if (p_debug)
                fprintf(p_debug, ": %s\n", e.mesg.c_str());
            continue;
        }
        if (p_debug) {
            fprintf(p_debug, ": encoding is");
            HexdumpFormat hf;
            hf.prefix = "    ";
            hf.addr_fmt = "";
            hf.width = 16;
            hf.pad_numeric = hf.show_chars = false;
            SgAsmExecutableFileFormat::hexdump(p_debug, 0, &(s[0]), s.size(), hf);
            fprintf(p_debug, "\n");
        }

        /* Choose best encoding */
        switch (get_encoding_type()) {
            case ET_SHORTEST:
                if (0==best.size() || s.size()<best.size())
                    best = s;
                break;
            case ET_LONGEST:
                if (0==best.size() || s.size()>best.size())
                    best = s;
                break;
            case ET_MATCHES:
                if (s==insn->get_raw_bytes()) {
                    if (p_debug)
                        fprintf(p_debug, "  MATCHES!\n");
                    return s; /*no need to continue searching*/
                    break;
                }
        }
    }

    if (best.size()==0)
        throw Exception("no matching assembly definition", insn);
    return best;
}

SgUnsignedCharList
AssemblerX86::assembleProgram(const std::string &_source)
{
    SgUnsignedCharList retval;
    char src_file_name[L_tmpnam];
    src_file_name[0] = '\0';
    char dst_file_name[L_tmpnam+4];
    dst_file_name[0] = '\0';
    int fd = -1;

    /* Boiler plate */
    std::string source = std::string("BITS 32\n") + _source;

    try {
        /* Write source code to a temporary file */
        for (int i=0; i<10 && !src_file_name[0]; i++) {
            if (!tmpnam_r(src_file_name))
                throw Exception("tmpnam failed");
            fd = open(src_file_name, O_CREAT|O_EXCL|O_RDWR, 0666);
            if (fd<0) {
                src_file_name[0] = '\0';
            } else {
                size_t offset = 0;
                size_t to_write = source.size();
                while (to_write>0) {
                    ssize_t nwritten = TEMP_FAILURE_RETRY(write(fd, source.c_str()+offset, to_write));
                    if (nwritten<0) {
                        close(fd);
                        unlink(src_file_name);
                        throw Exception(std::string("failed to write assembly source to temporary file: ")+ strerror(errno));
                    }
                    to_write -= nwritten;
                    offset += nwritten;
                }
                close(fd);
                fd = -1;
            }
        }
        if (!src_file_name[0])
            throw Exception("could not create temporary file for assembly source code");
        strcpy(dst_file_name, src_file_name);
        strcat(dst_file_name, ".bin");
        unlink(dst_file_name);

        /* Run the assembler, capturing its stdout (and combined stderr) */
        char nasm_cmd[256 + L_tmpnam + L_tmpnam];
        sprintf(nasm_cmd, "nasm -s -f bin -o %s %s", dst_file_name, src_file_name);
        FILE *nasm_output = popen(nasm_cmd, "r");
        if (!nasm_output)
            throw Exception(std::string("could not execute command: ") + nasm_cmd);
        std::string output;
        char *line = NULL;
        size_t line_nalloc = 0;
        while (0<rose_getline(&line, &line_nalloc, nasm_output))
            output += line;
        if (line) free(line);
        if (!output.empty() && '\n'==output[output.size()-1])
            output = output.substr(0, output.size()-1);
        int status = pclose(nasm_output);
        if (status!=0)
            throw Exception("nasm exited with status " + StringUtility::numberToString(status) + ": " + output);

        /* Read the raw assembly */
        fd = open(dst_file_name, O_RDONLY);
        if (fd<0)
            throw Exception(std::string("nasm didn't produce output in ") + dst_file_name);
        struct stat sb;
        if (fstat(fd, &sb)<0)
            throw Exception(std::string("fstat failed on ") + dst_file_name);
        retval.resize(sb.st_size, '\0');
        ssize_t pos = 0;
        while (pos<sb.st_size) {
            ssize_t nread = TEMP_FAILURE_RETRY(read(fd, &retval[pos], sb.st_size-pos));
            if (nread<0)
                throw Exception(std::string("failed to read nasm output: ") + strerror(errno));
            if (0==nread)
                throw Exception("possible short read of nasm output");
            pos += nread;
        }

    } catch (...) {
        /* Make sure temp files are deleted */
        if (src_file_name[0])
            unlink(src_file_name);
        if (dst_file_name[0])
            unlink(dst_file_name);
        if (fd>=0)
            close(fd);
        throw;
    }

    /* Cleanup */
    close(fd);
    unlink(src_file_name);
    unlink(dst_file_name);
    return retval;
}
