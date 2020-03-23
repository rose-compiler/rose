// tps (01/14/2010) : Switching from rose.h to sage3.
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "Assembler.h"

#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"

namespace Rose {
namespace BinaryAnalysis {

std::ostream&
operator<<(std::ostream &o, const Assembler::Exception &e)
{
    e.print(o);
    return o;
}

void
Assembler::Exception::print(std::ostream &o) const
{
    o <<what();
    if (insn)
        o <<" while assembling [" <<unparseInstruction(insn) <<"]";
}

/* class method */
Assembler *
Assembler::create(SgAsmGenericHeader *hdr)
{
    /* FIXME: This should eventually use a process similar to Disassembler [RPM 2009-07-23] */
    return new AssemblerX86();
}

Assembler *
Assembler::create(SgAsmInterpretation*)
{
    /* FIXME: This should eventually use a process similar to Disassembler [RPM 2009-07-23] */
    return new AssemblerX86();
}

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
Assembler::assembleBlock(const std::vector<SgAsmInstruction*> &insns, rose_addr_t va)
{
    SgUnsignedCharList retval;
    for (std::vector<SgAsmInstruction*>::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = *ii;
        insn->set_address(va);
        SgUnsignedCharList ucl = assembleOne(insn);
        retval.insert(retval.end(), ucl.begin(), ucl.end());
        va += ucl.size();
    }
    return retval;
}

} // namespace
} // namespace

#endif
