/** List of SgAsmPERVASizePair AST nodes.
 *
 *  The only reason this AST node exists rather than storing the list directly in the nodes that need it is due to ROSETTA
 *  limitations. */
class SgAsmPERVASizePairList: public SgAsmExecutableFileFormat {
    /** Property: List of pointers to other nodes. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmPERVASizePairPtrList pairs;

public:
    explicit SgAsmPERVASizePairList(SgAsmPEFileHeader *parent);
};
