#include <sageContainer.h>

/** List of symbol version needed entries.
 *
 *  The only reason this node type exists is to hold the list of pointers. The list cannot be contained in the nodes that
 *  actually need it due to limitations of ROSETTA. */
class SgAsmElfSymverNeededEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymverNeededEntryPtrList entries;
};
