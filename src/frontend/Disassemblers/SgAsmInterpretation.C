/* SgAsmInterpretation member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstructions.code (or any *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose::BinaryAnalysis;

SgAsmGenericFilePtrList
SgAsmInterpretation::get_files() const
{
    const SgAsmGenericHeaderPtrList &headers = get_headers()->get_headers();
    std::set<SgAsmGenericFile*> files;
    for (size_t i=0; i<headers.size(); i++) {
        SgAsmGenericHeader *header = headers[i];
        SgAsmGenericFile *file = SageInterface::getEnclosingNode<SgAsmGenericFile>(header);
        ROSE_ASSERT(file!=NULL);
        files.insert(file);
    }

    return SgAsmGenericFilePtrList(files.begin(), files.end());
}

void
SgAsmInterpretation::insert_instructions(InstructionMap &imap/*in,out*/)
{
    struct T: AstSimpleProcessing {
        InstructionMap &imap;
        T(InstructionMap &imap): imap(imap) {}
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                imap[insn->get_address()] = insn;
        }
    } t(imap);
    t.traverse(this, preorder);
}

void
SgAsmInterpretation::erase_instructions(InstructionMap &imap/*in,out*/)
{
    struct T: AstSimpleProcessing {
        InstructionMap &imap;
        T(InstructionMap &imap): imap(imap) {}
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                imap.erase(insn->get_address());
        }
    } t(imap);
    t.traverse(this, preorder);
}

InstructionMap &
SgAsmInterpretation::get_instruction_map(bool recompute)
{
    if (recompute || instruction_map.empty()) {
        instruction_map.clear();
        insert_instructions(instruction_map);
    }
    return instruction_map;
}

void
SgAsmInterpretation::set_instruction_map(const InstructionMap &imap)
{
    instruction_map = imap;
}

#endif
