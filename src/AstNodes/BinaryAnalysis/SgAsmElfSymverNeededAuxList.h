#include <sageContainer.h>

/** Hods a list of symbol version aux entries.
 *
 *  The only purpose of this node is to work around a limitation of ROSETTA that prevents this list from being contained
 *  directly in the class that needs it. */
class SgAsmElfSymverNeededAuxList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of entries. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmElfSymverNeededAuxPtrList entries;
};
