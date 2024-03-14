#include <sageContainer.h>

/** List of ELF relocation entries.
 *
 *  The only purpose of this node is to hold a list of the actual relocation entry nodes since ROSETTA limitations prevent
 *  that list from being contained in the nodes where it's needed. */
class SgAsmElfRelocEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of relocation entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfRelocEntryPtrList entries;
};
