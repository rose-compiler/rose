[[Rosebud::abstract]]
class SgAsmDwarfConstruct: public SgAsmDwarfInformation {
public:
    // For now we will store the nesting level (just to support debugging), then it will be removed.
    [[using Rosebud: rosetta, ctor_arg]]
    int nesting_level = 0;

    [[using Rosebud: rosetta, ctor_arg]]
    uint64_t offset = 0;

    [[using Rosebud: rosetta, ctor_arg]]
    uint64_t overall_offset = 0;

    // A lot of constructs have a name so put the name into the base class
    [[using Rosebud: rosetta]]
    std::string name;

    // This is a data member that can be filled in via a separate analysis (we would have to read the line information before
    // hand).
    [[using Rosebud: rosetta]]
    SgAsmDwarfLine* source_position = nullptr;

    // [Robb Matzke 2023-03-22]: SgNode already has an attributeMechanism
    // [[using Rosebud: rosetta]]
    // AstAttributeMechanism* attributeMechanism = nullptr;

public:
    /** Factory pattern to build IR nodes based on the tag. */
    static SgAsmDwarfConstruct* createDwarfConstruct( int tag, int nesting_level, uint64_t offset, uint64_t overall_offset );

    virtual SgAsmDwarfConstructList* get_children();
};
