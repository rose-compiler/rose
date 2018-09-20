#include <sage3basic.h>

#include <BinaryAstHash.h>

namespace Rose {
namespace BinaryAnalysis {
    
    /**  HashAST::visit
     *  
     * Called by traverse.  Gets the whatever data is of interest and puts
     * it in the hash.   
     *
     * @param[in] node to submit to hash
     **/
    void
    AstHash::visit(SgNode* node)
    {
        //Always include the type of each node in the hash
        VariantT vType = node->variantT();
        hasher_.insert(vType);
        
        //If it's an instruction, include the mnemonic, and maybe the address
        SgAsmInstruction* asmInstruction = isSgAsmInstruction(node);
        if(asmInstruction != NULL) {
            std::string mnemonic = asmInstruction->get_mnemonic();
            hasher_.insert(mnemonic);
            if(includeAddresses) {
                rose_addr_t addr = asmInstruction->get_address();
                hasher_.insert(addr);
            }
            return;
        }
        
        //Always include register references
        SgAsmRegisterReferenceExpression* regRef = isSgAsmRegisterReferenceExpression(node);
        if(regRef != NULL)
            {
                unsigned regHash = regRef->get_descriptor().hash();
                hasher_.insert(regHash);
                return;
            }
        
        //Maybe inlcude constants (integers, floats, pointers)
        if(includeConstants) {
            SgAsmConstantExpression* constExpr = isSgAsmConstantExpression(node);
            if(constExpr != NULL) {
                std::string mnemonic = constExpr->get_bitVector().toHex();
                hasher_.insert(mnemonic);
                return;
            }
        }    
    }


    } // namespace
} // namespace
