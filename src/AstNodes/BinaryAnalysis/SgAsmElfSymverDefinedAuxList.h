#include <sageContainer.h>

/** List of symbol version aux entries.
 *
 *  The only purpose of this node is to hold the list of pointers, which can't be contained in the classes that need the
 *  list due to limitations of ROSETTA. */
class SgAsmElfSymverDefinedAuxList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymverDefinedAuxPtrList entries;
};
