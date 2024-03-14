#include <sageContainer.h>

/** Node to hold list of ELF note entries.
 *
 *  This node's only purpose is to hold the list of pointers to note entries, which must be done like this because of
 *  limitations of ROSETTA. */
class SgAsmElfNoteEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of ELF not entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmElfNoteEntryPtrList entries;
};
