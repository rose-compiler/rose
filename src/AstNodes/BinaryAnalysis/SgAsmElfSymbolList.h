#include <sageContainer.h>

class SgAsmElfSymbolList: public SgAsmExecutableFileFormat {
public:
    /** Property: Symbol list.
     *
     *  This points to an AST node that contains the actual symbol list. The reason that the list is not held directly in
     *  the nodes that need it is due to ROSETTA limitations. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymbolPtrList symbols;
};
