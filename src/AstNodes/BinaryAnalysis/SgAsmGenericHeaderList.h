#include <sageContainer.h>

/** List of generic file headers.
 *
 *  The only purpose of this AST node is to hold a list of pointers which cannot be contained directly in other nodes
 *  because of ROSETTA limitations. */
class SgAsmGenericHeaderList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of file header nodes. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmGenericHeaderPtrList headers;
};
