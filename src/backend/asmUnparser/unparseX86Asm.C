#include "sage3basic.h"
#include "Registers.h"
#include "AsmUnparser.h"

#include <iomanip>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseX86Mnemonic(SgAsmx86Instruction *insn) {
    ROSE_ASSERT(insn!=NULL);
    std::string result = insn->get_mnemonic();
    switch (insn->get_branchPrediction()) {
        case x86_branch_prediction_none: break;
        case x86_branch_prediction_taken: result += ",pt"; break;
        case x86_branch_prediction_not_taken: result += ",pn"; break;
        default: ROSE_ASSERT (!"Bad branch prediction");
    }
    return result;
}

/** Returns the name of an X86 register.
 *
 *  We use the amd64 architecture because, since it's backward compatible with the 8086, it contains definitions for all the
 *  registers from older architectures. */
std::string unparseX86Register(const RegisterDescriptor &reg) {
    using namespace StringUtility;
    const RegisterDictionary *dict = RegisterDictionary::dictionary_amd64();
    std::string name = dict->lookup(reg);
    if (name.empty()) {
        static bool dumped_dict = false;
        std::cerr <<"unparseX86Register(" <<reg <<"): register descriptor not found in dictionary.\n";
        if (!dumped_dict) {
            std::cerr <<"  FIXME: we might be using the amd64 register dictionary. [RPM 2011-03-02]\n";
            //std::cerr <<*dict;
            dumped_dict = true;
        }
        return (std::string("BAD_REGISTER(") +
                numberToString(reg.get_major()) + "." +
                numberToString(reg.get_minor()) + "." +
                numberToString(reg.get_offset()) + "." +
                numberToString(reg.get_nbits()) + ")");
    }
    return name;
}

static std::string x86ValToLabel(uint64_t val, const AsmUnparser::LabelMap *labels)
{
    if (!val || !labels)
        return "";
    
    AsmUnparser::LabelMap::const_iterator li = labels->find(val);
    if (li==labels->end())
        return "";

    return li->second;
}

static std::string x86TypeToPtrName(SgAsmType* ty) {
    ROSE_ASSERT(ty != NULL);
    switch (ty->variantT()) {
        case V_SgAsmTypeByte: return "BYTE";
        case V_SgAsmTypeWord: return "WORD";
        case V_SgAsmTypeDoubleWord: return "DWORD";
        case V_SgAsmTypeQuadWord: return "QWORD";
        case V_SgAsmTypeDoubleQuadWord: return "DQWORD";
        case V_SgAsmTypeSingleFloat: return "FLOAT";
        case V_SgAsmTypeDoubleFloat: return "DOUBLE";
        case V_SgAsmType80bitFloat: return "LDOUBLE";
        case V_SgAsmTypeVector: {
            SgAsmTypeVector* v = isSgAsmTypeVector(ty);
            return "V" + StringUtility::numberToString(v->get_elementCount()) + x86TypeToPtrName(v->get_elementType());
        }
        default: {
            std::cerr << "x86TypeToPtrName: Bad class " << ty->class_name() << std::endl;
            ROSE_ASSERT(false);
            return "error in x86TypeToPtrName()";// DQ (11/29/2009): Avoid MSVC warning.
        }
    }
}


std::string unparseX86Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels, bool leaMode) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";

    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, false) + " + " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, false);
            break;
        case V_SgAsmBinarySubtract:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, false) + " - " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, false);
            break;
        case V_SgAsmBinaryMultiply:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, false) + "*" +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, false);
            break;
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            if (!leaMode) {
                result += x86TypeToPtrName(mr->get_type()) + " PTR " +
                          (mr->get_segment() ? unparseX86Expression(mr->get_segment(), labels, false) + ":" : "");
            }
            result += "[" + unparseX86Expression(mr->get_address(), labels, false) + "]";
            break;
        }
        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
            result = unparseX86Register(rr->get_descriptor());
            break;
        }
        case V_SgAsmByteValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%02"PRIx64, v);
            if (v & 0x80)
                sprintf(buf+strlen(buf), "<-0x%02"PRIx64">", (~v+1) & 0xff);
            result = buf;
            break;
        }
        case V_SgAsmWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%04"PRIx64, v);
            if (v & 0x8000)
                sprintf(buf+strlen(buf), "<-0x%04"PRIx64">", (~v+1) & 0xffff);
            result = buf;
            break;
        }
        case V_SgAsmDoubleWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            std::string label = x86ValToLabel(v, labels);
            sprintf(buf, "0x%08"PRIx64, v);
            if (!label.empty()) {
                sprintf(buf+strlen(buf), "<%s>", label.c_str());
            } else if (v & 0x80000000) {
                sprintf(buf+strlen(buf), "<-0x%08"PRIx64">", (~v+1) & 0xffffffff);
            }
            result = buf;
            break;
        }
        case V_SgAsmQuadWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            std::string label = x86ValToLabel(v, labels);
            sprintf(buf, "0x%016"PRIx64, v);
            if (!label.empty()) {
                sprintf(buf+strlen(buf), "<%s>", label.c_str());
            } else if (v & ((uint64_t)1<<63)) {
                sprintf(buf+strlen(buf), "<-0x%016"PRIx64">", (~v+1));
            }
            result = buf;
            break;
        }
        default: {
            std::cerr << "Unhandled expression kind " << expr->class_name() << std::endl;
            ROSE_ASSERT (false);
        }
    }

    if (expr->get_replacement() != "") {
        result += " <" + expr->get_replacement() + ">";
    }
#if 0
    if (expr->get_bit_size()>0) {
        result += " <@" + StringUtility::numberToString(expr->get_bit_offset()) +
                  "+" + StringUtility::numberToString(expr->get_bit_size()) + ">";
    }
#endif
    return result;
}

/** Returns a string containing the specified operand. */
std::string unparseX86Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels) {
    /* Find the instruction with which this expression is associated. */
    SgAsmx86Instruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmx86Instruction(node);
    }
    ROSE_ASSERT(insn!=NULL);
    return unparseX86Expression(expr, labels, insn->get_kind()==x86_lea);
}
