#include <Rose/BinaryAnalysis/Address.h>

/** Base class for statement-like subclasses.
 *
 *  This is a base class for those binary analysis entities, such as instructions and basic blocks, that have a starting
 *  address in the virtual address space. */
[[Rosebud::abstract]]
class SgAsmStatement: public SgAsmNode {
public:
    /** Property: Starting virtual address.
     *
     *  Virtual address of first byte of instruction, block, or whatever, depending on subclass. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::Address address = 0;

    /** Property: Commentary. */
    [[using Rosebud: rosetta]]
    std::string comment;
};
