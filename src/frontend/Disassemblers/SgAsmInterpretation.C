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

