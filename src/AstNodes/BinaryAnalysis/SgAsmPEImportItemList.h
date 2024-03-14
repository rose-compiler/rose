#include <sageContainer.h>

/** A list of imported items.
 *
 *  The only purpose of this node is to hold a list of pointers to other nodes. This list cannot be contained directly in the nodes
 *  that need it due to ROSETTA limitations. */
class SgAsmPEImportItemList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of pointers to other nodes. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmPEImportItemPtrList vector ;
};
