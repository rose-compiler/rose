#include "rose.h"

SgUnsignedCharList
Assembler::assembleBlock(SgAsmBlock *bb)
{
    std::vector<SgAsmInstruction*> ivec;
    std::vector<SgNode*> nvec = NodeQuery::querySubTree(bb, V_SgAsmInstruction);
    for (std::vector<SgNode*>::iterator i=nvec.begin(); i!=nvec.end(); ++i) {
        SgAsmInstruction *insn = isSgAsmInstruction(*i);
        ROSE_ASSERT(insn!=NULL);
        ivec.push_back(insn);
    }
    if (0==ivec.size())
        return SgUnsignedCharList();
    return assembleBlock(ivec, ivec.front()->get_address());
}

SgUnsignedCharList
Assembler::assembleBlock(const std::vector<SgAsmInstruction*> &insns, rose_addr_t rva)
{
    SgUnsignedCharList retval;
    for (std::vector<SgAsmInstruction*>::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = *ii;
        insn->set_address(rva);
        SgUnsignedCharList ucl = assembleOne(insn);
        retval.insert(retval.end(), ucl.begin(), ucl.end());
    }
    return retval;
}

