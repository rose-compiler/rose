#include <sageContainer.h>

/** List of dynamic linking section entries.
 *
 *  The only purpose of this node is to hold a list which, due to ROSETTA limitations, cannot be contained in the objects
 *  that actually need it. */
class SgAsmElfDynamicEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmElfDynamicEntryPtrList entries;
};
