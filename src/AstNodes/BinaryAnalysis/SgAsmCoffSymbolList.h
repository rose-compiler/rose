/** List of COFF symbols. */
class SgAsmCoffSymbolList: public SgAsmExecutableFileFormat {
    /** Property: List of symbol pointers. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmCoffSymbolPtrList symbols;
};
