#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmInterpretation.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>

#include <SgAsmGenericFile.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmInstruction.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

SgAsmGenericFilePtrList
SgAsmInterpretation::get_files() const
{
    const SgAsmGenericHeaderPtrList &headers = get_headers()->get_headers();
    std::set<SgAsmGenericFile*> files;
    for (size_t i=0; i<headers.size(); i++) {
        SgAsmGenericHeader *header = headers[i];
        SgAsmGenericFile *file = AST::Traversal::findParentTyped<SgAsmGenericFile>(header);
        ROSE_ASSERT(file!=NULL);
        files.insert(file);
    }

    return SgAsmGenericFilePtrList(files.begin(), files.end());
}

void
SgAsmInterpretation::insert_instructions(InstructionMap &imap/*in,out*/)
{
    return insertInstructions(imap);
}

void
SgAsmInterpretation::insertInstructions(InstructionMap &imap/*in,out*/)
{
    AST::Traversal::forwardPre<SgAsmInstruction>(this, [&imap](SgAsmInstruction *insn) {
        imap[insn->get_address()] = insn;
    });
}

void
SgAsmInterpretation::erase_instructions(InstructionMap &imap/*in,out*/)
{
    eraseInstructions(imap);
}

void
SgAsmInterpretation::eraseInstructions(InstructionMap &imap/*in,out*/)
{
    AST::Traversal::forwardPre<SgAsmInstruction>(this, [&imap](SgAsmInstruction *insn) {
        imap.erase(insn->get_address());
    });
}

InstructionMap &
SgAsmInterpretation::get_instruction_map(bool recompute)
{
    return get_instructionMap(recompute);
}

InstructionMap &
SgAsmInterpretation::get_instructionMap(bool recompute)
{
    if (recompute || instruction_map.empty()) {
        instruction_map.clear();
        insertInstructions(instruction_map);
    }
    return instruction_map;
}

void
SgAsmInterpretation::set_instruction_map(const InstructionMap &imap)
{
    set_instructionMap(imap);
}

void
SgAsmInterpretation::set_instructionMap(const InstructionMap &imap)
{
    instruction_map = imap;
}

SgAsmBlock*
SgAsmInterpretation::get_global_block() const {
    return get_globalBlock();
}

void
SgAsmInterpretation::set_global_block(SgAsmBlock *x) {
    set_globalBlock(x);
}

#endif
