#include <sageContainer.h>

/** Node to hold a list of symbol node pointers.
 *
 *  The only reason this node exists is because a ROSETTA limitation prevents us from storing the list directly in the nodes that
 *  need it. */
class SgAsmGenericSymbolList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of symbol nodes. */
    [[using Rosebud: rosetta, large]]
    SgAsmGenericSymbolPtrList symbols;
};
