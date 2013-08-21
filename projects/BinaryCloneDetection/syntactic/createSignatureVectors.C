#include "rose.h"
#include "SqlDatabase.h"

#include "createSignatureVectors.h"
#include "vectorCompression.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <ext/hash_map>
#include <openssl/md5.h>          //FIXME: use libgcrypt instead [Robb P. Matzke 2013-08-12]
#include <stdio.h>
#include <string.h>


using namespace std;
using namespace sqlite3x;
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
}

#define NORMALIZED_UNPARSED_INSTRUCTIONS

enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};

#if 1
static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}
#else
static const size_t numberOfInstructionKinds = V_SgNumVariants;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->variantT();}
#endif

size_t numVectorsGenerated = 0;

static map<string, void*> internTable;

inline void*
intern(const std::string& s)
{
    map<string, void*>::iterator i = internTable.find(s);
    if  (i == internTable.end()) {
        void* sCopy = new string(s);
        internTable.insert(std::make_pair(s, sCopy));
        return sCopy;
    } else {
        return i->second;
    }
}

inline size_t
kindToInteger(size_t kind){
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



static hash_map<SgAsmExpression*, void*> unparseAndInternTable;

inline void*
unparseAndIntern(SgAsmExpression* e)
{
    hash_map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
    if (i == unparseAndInternTable.end()) {
        void* sPtr = intern(unparseX86Expression(e, NULL, NULL));
        unparseAndInternTable.insert(std::make_pair(e, sPtr));
        return sPtr;
    } else {
        return i->second;
    }
}
 
class SignatureVector {
public:
    static const size_t Size = numberOfInstructionKinds * 4 + 300 + 9 + 3;
    typedef uint16_t ElementType;

private:
    ElementType values[Size];

public:
    SignatureVector() {
        clear();
    }

    void clear() {
        for (size_t i = 0; i < Size; ++i)
            values[i] = 0;
    }

    ElementType operator[](size_t i) const {
        assert(i < Size);
        return values[i];
    }

    ElementType& totalForVariant(size_t var) {
        assert(var < numberOfInstructionKinds);
        return values[var * 4];
    }

    ElementType& opsForVariant(ExpressionCategory cat, size_t var) {
        assert(var < numberOfInstructionKinds);
        return values[var * 4 + (int)cat + 1];
    }

    ElementType& specificOp(ExpressionCategory cat, size_t num) {
	static ElementType dummyVariable = 0;
	if (num < 100) {
            return values[numberOfInstructionKinds * 4 + 100 * (int)cat + num];
	} else {
            return dummyVariable;
        }
    }

    ElementType& operandPair(ExpressionCategory a, ExpressionCategory b) {
        return values[numberOfInstructionKinds * 4 + 300 + (int)a * 3 + (int)b];
    }

    ElementType& operandTotal(ExpressionCategory a) {
        return values[numberOfInstructionKinds * 4 + 300 + 9 + (int)a];
    }

    const ElementType* getBase() const {
        return values;
    }
};

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
numberOperands(SgAsmx86Instruction* firstInsn[], size_t insnCount, hash_map<SgAsmExpression*, size_t> numbers[3])
{
    hash_map<void*, size_t> stringNumbers[3];
    for (size_t i = 0; i < insnCount; ++i) {
        SgAsmx86Instruction* insn = firstInsn[i];
        const SgAsmExpressionPtrList& operands = getOperands(insn);
        //size_t operandCount = operands.size();
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

void addVectorToDatabase(const SqlDatabase::TransactionPtr&, const SignatureVector& vec, const std::string& functionName,
                         size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions,
                         SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride);

void addFunctionStatistics(const SqlDatabase::TransactionPtr&, const std::string& filename, const std::string& functionName,
                           size_t functionId, size_t numInstructions);

// Ignores function boundaries
bool
createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId,
                                size_t windowSize, size_t stride, const SqlDatabase::TransactionPtr &tx)
{
    bool retVal = false;
    vector<SgAsmx86Instruction*> insns;
    FindInstructionsVisitor vis;
    AstQueryNamespace::querySubTree(top, std::bind2nd( vis, &insns ));
    size_t insnCount = insns.size();

    if(insnCount >= windowSize ){
        static SignatureVector vec;
        vec.clear();
        hash_map<SgAsmExpression*, size_t> valueNumbers[3];
        numberOperands(&insns[0], insnCount, valueNumbers);
        string normalizedUnparsedInstructions;
        // Unparse the normalized forms of the instructions
        for (size_t insnNumber = 0; insnNumber < insnCount; ++insnNumber) {
            SgAsmx86Instruction* insn = insns[insnNumber];
            size_t var = getInstructionKind(insn);
            //var = kindToInteger(var);

#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
            string mne = insn->get_mnemonic();
            boost::to_lower(mne);
            normalizedUnparsedInstructions += mne;
#endif
            const SgAsmExpressionPtrList& operands = getOperands(insn);
            size_t operandCount = operands.size();
            // Add to total for this variant
            ++vec.totalForVariant(var);
            // Add to total for each kind of operand
            for (size_t i = 0; i < operandCount; ++i) {
                SgAsmExpression* operand = operands[i];
                ExpressionCategory cat = getCategory(operand);
                ++vec.opsForVariant(cat, var);
                // Add to total for this unique operand number (for this window)
                hash_map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
                assert (numIter != valueNumbers[(int)cat].end());
                size_t num = numIter->second;
                ++vec.specificOp(cat, num);
                // Add to total for this kind of operand
                ++vec.operandTotal(cat);
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
                normalizedUnparsedInstructions += (cat == ec_reg ? "R" : cat == ec_mem ? "M" : "V") +
                                                  boost::lexical_cast<string>(num);
#endif
            }

            // Add to total for this pair of operand kinds
            if (operandCount >= 2) {
                ExpressionCategory cat1 = getCategory(operands[0]);
                ExpressionCategory cat2 = getCategory(operands[1]);
                ++vec.operandPair(cat1, cat2);
            }
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
            if (insnNumber + 1 < windowSize) {
                normalizedUnparsedInstructions += ";";
            }
#endif
        }

        // Add vector to database
        addVectorToDatabase(tx, vec, functionName, functionId, 0, normalizedUnparsedInstructions,
                            0, filename, insnCount, stride);
	retVal = true;
    }
    addFunctionStatistics(tx, filename, functionName, functionId, insnCount);
    return retVal;
}

void
createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                             const SqlDatabase::TransactionPtr &tx)
{
    int functionId = tx->statement("select coalesce(max(id),-1)+1 from functions")->execute_int(); // zero origin
    std::string functionName = filename + "-all-instructions";
    createVectorsForAllInstructions(top, filename, functionName, functionId, windowSize, stride, tx);
    tx->statement("insert into functions(file, function_name) values (?,?)")
        ->bind(0, filename)
        ->bind(1, functionName)
        ->execute();
    cout << "Total vectors generated: " << numVectorsGenerated << endl;
}

void
createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                          const SqlDatabase::TransactionPtr &tx)
{
    struct InstructionSelector: SgAsmFunction::NodeSelector {
        virtual bool operator()(SgNode *node) {
            return isSgAsmInstruction(node)!=NULL;
        }
    } iselector;

    struct DataSelector: SgAsmFunction::NodeSelector {
        virtual bool operator()(SgNode *node) {
            return isSgAsmStaticData(node)!=NULL;
        }
    } dselector;

    SqlDatabase::StatementPtr cmd1 = tx->statement("insert into functions"
                                                   // 0   1     2              3         4      5      6
                                                   " (id, file, function_name, entry_va, isize, dsize, size)"
                                                   " values(?,?,?,?,?,?,?)");
    
    SqlDatabase::StatementPtr cmd2 = tx->statement("insert into instructions"
                                                   // 0        1     2            3                      4
                                                   " (address, size, function_id, index_within_function, assembly)"
                                                   " values (?,?,?,?,?)");

    vector<SgAsmFunction*> funcs = SageInterface::querySubTree<SgAsmFunction>(top);
    int functionId = tx->statement("select coalesce(max(id),-1)+1 from functions")->execute_int(); // zero origin
    
    for (vector<SgAsmFunction*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi, ++functionId) {
        ExtentMap e_insns, e_data, e_total;
        (*fi)->get_extent(&e_insns, NULL, NULL, &iselector);
        (*fi)->get_extent(&e_data,  NULL, NULL, &dselector);
        (*fi)->get_extent(&e_total);

	createVectorsForAllInstructions(*fi, filename, (*fi)->get_name(), functionId, windowSize, stride, tx);
        cmd1->bind(0, functionId);
        cmd1->bind(1, filename);
        cmd1->bind(2, (*fi)->get_name() );
        cmd1->bind(3, (*fi)->get_entry_va());
        cmd1->bind(4, e_insns.size());
        cmd1->bind(5, e_data.size());
        cmd1->bind(6, e_total.size());
        cmd1->execute();

        vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(*fi);
        for (size_t i=0; i<insns.size(); ++i) {
            cmd2->bind(0, insns[i]->get_address());
            cmd2->bind(1, insns[i]->get_size());
            cmd2->bind(2, functionId);
            cmd2->bind(3, i);
            cmd2->bind(4, unparseInstructionWithAddress(insns[i]));
            cmd2->execute();
        }
    }
    cerr << "Total vectors generated: " << numVectorsGenerated << endl;
}

void
addFunctionStatistics(const SqlDatabase::TransactionPtr &tx, const std::string& filename, const std::string& functionName,
                      size_t functionId, size_t numInstructions)
{
    tx->statement("insert into function_statistics (function_id, num_instructions) values (?,?)")
        ->bind(0, functionId)
        ->bind(1, numInstructions)
        ->execute();
}

void
addVectorToDatabase(const SqlDatabase::TransactionPtr &tx, const SignatureVector& vec, const std::string& functionName,
                    size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions,
                    SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride)
{
    ++numVectorsGenerated;

    vector<uint8_t> compressedCounts = compressVector(vec.getBase(), SignatureVector::Size);
    size_t vectorSum = 0;
    for (size_t i=0; i<SignatureVector::Size; ++i)
        vectorSum += vec[i];

    ExtentMap extent;
    for (size_t i=0; i<windowSize; ++i)
        extent.insert(Extent(firstInsn[i]->get_address(), firstInsn[i]->get_size()));

    unsigned char md[16];
    MD5((const unsigned char*)normalizedUnparsedInstructions.data(), normalizedUnparsedInstructions.size(), md);

    SqlDatabase::StatementPtr cmd = tx->statement("insert into vectors"
                                                  // 0   1            2                      3     4             5
                                                  " (id, function_id, index_within_function, line, last_insn_va, size,"
                                                  // 6            7           8
                                                  "sum_of_counts, counts_b64, instr_seq_b64)"
                                                  " values (?,?,?,?,?,?,?,?,?)");
    int vector_id = tx->statement("select coalesce(max(id),0)+1 from vectors")->execute_int(); // 1-origin
    cmd->bind(0, vector_id);
    cmd->bind(1, functionId);
    cmd->bind(2, indexWithinFunction);
    cmd->bind(3, firstInsn[0]->get_address());
    cmd->bind(4, firstInsn[windowSize-1]->get_address());
    cmd->bind(5, extent.size());
    cmd->bind(6, vectorSum);
    cmd->bind(7, StringUtility::encode_base64(&compressedCounts[0], compressedCounts.size()));
    cmd->bind(8, StringUtility::encode_base64(md, 16));
    cmd->execute();
}

void
dropDatabases(const SqlDatabase::TransactionPtr &tx)
{
    extern const char *syntactic_schema_drop; // defined in machine-generated SyntacticSchema.C
    tx->execute(syntactic_schema_drop);
}

void
createDatabases(const SqlDatabase::TransactionPtr &tx)
{
    extern const char *syntactic_schema_create; // defined in machine-generated SyntacticSchema.C
    tx->execute(syntactic_schema_create);
}
