/* SgAsmInterpretation member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstructions.code (or any *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"

/** Finish constructing. */
void SgAsmInterpretation::ctor() {
    ROSE_ASSERT(p_headers==NULL);
    p_headers = new SgAsmGenericHeaderList();
    p_headers->set_parent(this);
}

/** Returns a list of all files referenced by an interpretation.  It does this by looking at the file headers referenced by
 *  the interpretation, following their parent pointers up to an SgAsmGenericFile node, and returning a vector of those nodes
 *  with duplicate files removed. */
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

/** Get the register dictionary. */
const RegisterDictionary *
SgAsmInterpretation::get_registers() const
{
    return p_registers;
}

/** Set the register dictionary.  The register dictionary is used for disassembling, unparsing, instruction semantics, etc. */
void
SgAsmInterpretation::set_registers(const RegisterDictionary *regs)
{
    p_registers = regs;
}

/** Populate a map of instructions indexed by their virtual addresses. */
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

/** Remove some instructions from an instruction map. */
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

/** Returns the InstructionMap associated with an interpretation. The instruction map is recomputed if the currently cached map
 * is empty or if the @p recompute argument is true. Otherwise this just returns the existing map. No attempt is made to make
 * sure that the map is up-to-date with respect to the current state of the AST. */
InstructionMap &
SgAsmInterpretation::get_instruction_map(bool recompute)
{
    if (recompute || instruction_map.empty()) {
        instruction_map.clear();
        insert_instructions(instruction_map);
    }
    return instruction_map;
}

/** Caches the specified instruction map in this interpretation.  The contents of the supplied map are copied into the
 *  interpretation. */
void
SgAsmInterpretation::set_instruction_map(const InstructionMap &imap)
{
    instruction_map = imap;
}
