#include <sageContainer.h>

/** List of ELF error handling frame descriptor entries.
 *
 *  The only purpose of this node is to hold the list of pointers to FD entries, and is necesssary due to limitations of
 *  ROSETTA. */
class SgAsmElfEHFrameEntryFDList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of pointers to error handling frame descriptor entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmElfEHFrameEntryFDPtrList entries;
};
