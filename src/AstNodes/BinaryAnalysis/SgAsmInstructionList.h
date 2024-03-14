#include <sageContainer.h>

/** List of SgAsmInstruction nodes. */
class SgAsmInstructionList: public SgAsmNode {
public:
    /** Property: List of @ref SgAsmInstruction entries.
     *
     *  The @ref SgAsmInstruction nodes in this list have not been broken
     *  into separate instruction blocks.  Therefore, the list may contain
     *  branching instructions that do not terminate the list. */
    [[using Rosebud: rosetta, large]]
    SgAsmInstructionPtrList instructions;
};
