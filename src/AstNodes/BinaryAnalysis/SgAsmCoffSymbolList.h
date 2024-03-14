#include <sageContainer.h>

/** List of COFF symbols. */
class SgAsmCoffSymbolList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of symbol pointers. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmCoffSymbolPtrList symbols;
};
