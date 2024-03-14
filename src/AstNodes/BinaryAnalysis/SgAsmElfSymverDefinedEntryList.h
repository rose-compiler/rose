#include <sageContainer.h>

/** List of entries for the ELF symbol version definition table.
 *
 *  The only reason we have a dedicated AST node type for this information instead of storing it directly in the nodes that
 *  need it is due to limitations of ROSETTA. */
class SgAsmElfSymverDefinedEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of table entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymverDefinedEntryPtrList entries;
};
