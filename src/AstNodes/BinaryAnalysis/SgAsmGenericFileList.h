#include <sageContainer.h>

/** List of AST file node pointers.
 *
 *  The only reason this node type exists is because ROSETTA limitations prevent the list from being contained directly in the nodes
 *  that need it. */
class SgAsmGenericFileList: public SgAsmNode {
public:
    /** Property: List of AST node pointers. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmGenericFilePtrList files;
};
