#include <sageContainer.h>

/** List of pointers to other nodes.
 *
 *  The only reason this node type exists is because ROSETTA limitations prevent the list from being included directly in the nodes
 *  that need the list. */
class SgAsmGenericDLLList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of AST node pointers. */
    [[using Rosebud: rosetta, large]]
    SgAsmGenericDLLPtrList dlls;
};
