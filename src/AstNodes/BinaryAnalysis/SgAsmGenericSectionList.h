#include <sageContainer.h>

/** List of pointers to file sections.
 *
 *  The only reason this node type exists is because ROSETTA limitations prevent the list from being stored directly in the
 *  nodes that need it. */
class SgAsmGenericSectionList: public SgAsmExecutableFileFormat {
public:
    /** Property: List of section pointers. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmGenericSectionPtrList sections;
};
