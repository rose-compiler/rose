#include "sage3basic.h"
#include "AST_FILE_IO.h"
#include "compute_signature_vector.h"
#include "vectorCompression.h"


#include <ext/hash_map>


using namespace __gnu_cxx;

namespace __gnu_cxx {
template <>
struct hash<SgAsmExpression*> {
    size_t operator()(SgAsmExpression* expr) const {
        return size_t(expr);
    }
};

template <>
struct hash<void*> {
    size_t operator()(void* expr) const {
        return size_t(expr);
    }
};
} // namespace

namespace std {
   using namespace __gnu_cxx;
}

inline size_t
kindToInteger(size_t kind) {
    int vk = -1;
    switch (kind) {
        case x86_mov:
        case x86_movapd:
        case x86_movaps:
        case x86_movbe:
        case x86_movd:
        case x86_movddup:
        case x86_movdq2q:
        case x86_movdqa:
        case x86_movdqu:
        case x86_movhlps:
        case x86_movhpd:
        case x86_movhps:
        case x86_movlhps:
        case x86_movlpd:
        case x86_movlps:
        case x86_movmskpd:
        case x86_movmskps:
        case x86_movntdq:
        case x86_movntdqa:
        case x86_movnti:
        case x86_movntpd:
        case x86_movntps:
        case x86_movntq:
        case x86_movntsd:
        case x86_movntss:
        case x86_movq:
        case x86_movq2dq:
        case x86_movsb:
        case x86_movsd:
        case x86_movsd_sse:
        case x86_movshdup:
        case x86_movsldup:
        case x86_movsq:
        case x86_movss:
        case x86_movsw:
        case x86_movsx:
        case x86_movsxd:
        case x86_movupd:
        case x86_movups:
        case x86_movzx:
        case x86_xor:
        case x86_push:
        case x86_pop:
        case x86_lea:
            vk = x86_mov; break;
        case x86_add:
        case x86_inc:
            vk = x86_add; break;
        case x86_sub:
        case x86_dec:
            vk = x86_sub; break;
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
        case x86_jmpe:
        case x86_jne:
        case x86_jno:
        case x86_jns:
        case x86_jo:
        case x86_jpe:
        case x86_jpo:
        case x86_jrcxz:
        case x86_js:
            vk = x86_ja; break;

        case x86_ret:
        case x86_retf:
            vk = x86_ret; break;
            break;

        case x86_test:
        case x86_neg:
        case x86_cmpxchg:
        case x86_cmpsb:
        case x86_cmpsw:
        case x86_cmpsd:
        case x86_cmp:
            vk = x86_test; break;

        case x86_cbw:
        case x86_cwde:
        case x86_cwd:
        case x86_cdq:
        case x86_and:
        case x86_or:
        case x86_not:
        case x86_xadd:
        case x86_adc:
        case x86_sbb:
            vk = x86_or; break;


        case x86_shl:       // fall through
        case x86_sar:       // fall through
        case x86_shr:
            vk = x86_shr; break;
        case x86_setne:
        case x86_sete:
        case x86_setno:
        case x86_seto:
        case x86_setpo:
        case x86_setpe:
        case x86_setns:
        case x86_sets:
        case x86_setae:
        case x86_setb:
        case x86_setbe:
        case x86_seta:
        case x86_setle:
        case x86_setg:
        case x86_setge:
        case x86_setl:
            vk = x86_setb;
            break;

        case x86_cmovne:
        case x86_cmove:
        case x86_cmovno:
        case x86_cmovo:
        case x86_cmovpo:
        case x86_cmovpe:
        case x86_cmovns:
        case x86_cmovs:
        case x86_cmovae:
        case x86_cmovb:
        case x86_cmovbe:
        case x86_cmova:
        case x86_cmovle:
        case x86_cmovg:
        case x86_cmovge:
        case x86_cmovl:
            vk = x86_cmovb; break;

        case x86_repne_scasb:
        case x86_repne_scasw:
        case x86_repne_scasd:
        case x86_repe_scasb:
        case x86_repe_scasw:
        case x86_repe_scasd:
            vk = x86_repe_scasw; break;
        case x86_scasb:
        case x86_scasw:
        case x86_scasd:
            vk = x86_scasw; break;
        case x86_repne_cmpsb:
        case x86_repne_cmpsw:
        case x86_repne_cmpsd:
        case x86_repe_cmpsb:
        case x86_repe_cmpsw:
        case x86_repe_cmpsd:
            vk = x86_repe_cmpsb; break;
        case x86_rep_movsb:
        case x86_rep_movsw:
        case x86_rep_movsd:
            vk = x86_rep_movsd; break;
        case x86_stosb:
        case x86_stosw:
        case x86_stosd:
            vk = x86_stosb; break;
        case x86_rep_stosb:
        case x86_rep_stosw:
        case x86_rep_stosd:
            vk = x86_rep_stosw; break;
        case x86_lodsb:
        case x86_lodsw:
        case x86_lodsd:
            vk = x86_lodsb; break;

        default: {
            vk = kind;
        }
    }
    return vk;
}

static std::map<std::string, void*> internTable;

inline void*
intern(const std::string& s)
{
    std::map<std::string, void*>::iterator i = internTable.find(s);
    if (i == internTable.end()) {
        void* sCopy = new std::string(s);
        internTable.insert(std::make_pair(s, sCopy));
        return sCopy;
    } else {
        return i->second;
    }
}

static std::hash_map<SgAsmExpression*, void*> unparseAndInternTable;

inline void*
unparseAndIntern(SgAsmExpression* e)
{
    std::hash_map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
    if (i == unparseAndInternTable.end()) {
        void* sPtr = intern(unparseX86Expression(e, NULL, NULL));
        unparseAndInternTable.insert(std::make_pair(e, sPtr));
        return sPtr;
    } else {
        return i->second;
    }
}


inline ExpressionCategory
getCategory(SgAsmExpression* e)
{
    if (isSgAsmValueExpression(e)) {
        return ec_val;
    } else if (isSgAsmRegisterReferenceExpression(e)) {
        return ec_reg;
    } else if (isSgAsmMemoryReferenceExpression(e)) {
        return ec_mem;
    } else {
        abort();
    }
}

SgAsmExpressionPtrList&
getOperands(SgAsmInstruction* insn)
{
    SgAsmOperandList* ol = insn->get_operandList();
    SgAsmExpressionPtrList& operands = ol->get_operands();
    return operands;
}

void
numberOperands(SgAsmInstruction* firstInsn[], size_t insnCount, hash_map<SgAsmExpression*, size_t> numbers[3])
{
    hash_map<void*, size_t> stringNumbers[3];
    for (size_t i = 0; i < insnCount; ++i) {
        SgAsmX86Instruction* insn = isSgAsmX86Instruction(firstInsn[i]);

        if (insn == NULL) {
            assert(!"only x86 instructions currently supported");
            exit(1);
        }

        const SgAsmExpressionPtrList& operands = getOperands(insn);
        for (size_t j = 0; j < operands.size(); ++j) {
            SgAsmExpression* e = operands[j];
            ExpressionCategory cat = getCategory(e);
            void* str = unparseAndIntern(e);
            hash_map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
            hash_map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
            size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
            if (stringNumIter == currentStringNums.end())
                currentStringNums.insert(std::make_pair(str, num));
            numbers[(int)cat][e] = num;
        }
    }
}

void
createVectorsForAllInstructions(SignatureVector& vec, std::vector<SgAsmInstruction*>& insns, std::vector<std::string> sc)
{
    hash_map<SgAsmExpression*, size_t> valueNumbers[3];
    size_t insnCount = insns.size();

    bool by_category       = (std::find(sc.begin(), sc.end(), "by_category") != sc.end());
    bool total_for_variant = (std::find(sc.begin(), sc.end(), "total_for_variant") != sc.end());
    bool operand_total     = (std::find(sc.begin(), sc.end(), "operant_total") != sc.end());
    bool specific_op       = (std::find(sc.begin(), sc.end(), "specific_op") != sc.end());
    bool operand_pair      = (std::find(sc.begin(), sc.end(), "operand_pair") != sc.end());
    bool apply_log         = (std::find(sc.begin(), sc.end(), "apply_log") != sc.end());

    if (!insns.empty())
        numberOperands(&insns[0], insnCount, valueNumbers);

    std::string normalizedUnparsedInstructions;

    // Unparse the normalized forms of the instructions
    for (std::vector<SgAsmInstruction*>::iterator it = insns.begin(); it != insns.end(); ++it) {
        SgAsmX86Instruction* insn = isSgAsmX86Instruction(*it);
        assert(insn != NULL);
        size_t var = insn->get_kind();
        if (by_category)
            var = kindToInteger(var);

#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
        std::string mne = insn->get_mnemonic();
        boost::to_lower(mne);
        normalizedUnparsedInstructions += mne;
#endif
        // Add to total for this variant
        if (total_for_variant)
            ++vec.totalForVariant(var);

        const SgAsmExpressionPtrList& operands = getOperands(insn);
        size_t operandCount = operands.size();

        // Add to total for each kind of operand
        for (size_t i = 0; i < operandCount; ++i) {
            SgAsmExpression* operand = operands[i];
            ExpressionCategory cat = getCategory(operand);

            // Add to total for this unique operand number (for this window)
            hash_map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
            assert (numIter != valueNumbers[(int)cat].end());
            size_t num = numIter->second;
            if (specific_op)
                ++vec.specificOp(cat, num);

            // Add to total for this kind of operand
            if (operand_total)
                ++vec.operandTotal(cat);

#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
            normalizedUnparsedInstructions += (cat == ec_reg ? "R" : cat == ec_mem ? "M" : "V") +
                                              boost::lexical_cast<std::string>(num);
#endif
        }

        // Add to total for this pair of operand kinds
        if (operandCount >= 2) {
            ExpressionCategory cat1 = getCategory(operands[0]);
            ExpressionCategory cat2 = getCategory(operands[1]);

            if (operand_pair)
                ++vec.operandPair(cat1, cat2);
        }

#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
        if (insnNumber + 1 < windowSize) {
            normalizedUnparsedInstructions += ";";
        }
#endif
    }

    if (apply_log) {
        int tmp;
        for (int i = 0; i < x86_last_instruction; i++) {
            tmp = vec.totalForVariant(i);
            if (tmp != 0) {
                vec.totalForVariant(i) = round(log10(vec.totalForVariant(i)/log(2)));
            }
        }
    }
}
