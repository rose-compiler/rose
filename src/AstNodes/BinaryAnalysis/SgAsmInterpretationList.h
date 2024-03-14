#include <sageContainer.h>

class SgAsmInterpretationList: public SgAsmNode {
public:
    /** Property: Interpretation list.
     *
     *  The actual list of pointers to interpretations. The only reason this list is in its own node instead of being
     *  contained directly in the node that needs it is because of limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmInterpretationPtrList interpretations;
};
