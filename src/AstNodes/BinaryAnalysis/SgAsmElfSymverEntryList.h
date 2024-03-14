#include <sageContainer.h>

/** List of entries from a symbol version table.
 *
 *  The only reason this node exists instead of storing the entries directly in the parent node that needs them is due to
 *  ROSETTA limitations. */
class SgAsmElfSymverEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of symbol version table entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymverEntryPtrList entries;
};
