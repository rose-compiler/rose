/** List of pointers to other AST nodes.
 *
 *  The only purpose of this node is to hold a list of pointers to other nodes. We cannot store the list directly in the nodes that
 *  need it due to ROSETTA limitations. */
class SgAsmPEExportEntryList: public SgAsmExecutableFileFormat {
    /** Property: List of pointers to nodes. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmPEExportEntryPtrList exports ;
};
