#include <sageContainer.h>

/** List of ELF EH frame CI entries.
 *
 *  See official ELF specification. The only reason this node exists is because of ROSETTA limitations which prevent the
 *  list from being stored directly in the class that needs it. */
class SgAsmElfEHFrameEntryCIList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of pointers to ELF EH frame CI entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmElfEHFrameEntryCIPtrList entries;
};
