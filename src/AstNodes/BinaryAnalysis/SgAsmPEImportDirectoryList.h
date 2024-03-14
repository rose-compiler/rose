#include <sageContainer.h>

/** A list of PE Import Directories.
 *
 *  The only purpose of this class is to store a list of node pointers which cannot be stored in the nodes that need it due to
 *  ROSETTA limitations. */
class SgAsmPEImportDirectoryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of nodes.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmPEImportDirectoryPtrList vector;
};
