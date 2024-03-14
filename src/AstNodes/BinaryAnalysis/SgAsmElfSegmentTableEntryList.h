#include <sageContainer.h>

class SgAsmElfSegmentTableEntryList: public SgAsmExecutableFileFormat {
public:
    /** Property: Segment table entries.
     *
     *  List of entries in this segment table.  The reason we have a whole AST node dedicated to holding this list rather
     *  than just storing the list directly in the nodes that need it is due to limitations with ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSegmentTableEntryPtrList entries;
};
