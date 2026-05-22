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

    // This is a data member that can be filled in via a separate analysis
    // (we would have to read the line information before hand).
    [[using Rosebud: rosetta]]
    SgAsmDwarfLine* source_position = nullptr;

    // [Robb Matzke 2023-03-22]: SgNode already has an attributeMechanism
    // [[using Rosebud: rosetta]]
    // AstAttributeMechanism* attributeMechanism = nullptr;

 // DQ (11/20/2025): We want to store the source positons of all kinds of declarations,
 // and all kids of constructs that have source position in the dwarf sections, but we
 // can't use SgAsmDwarfLine because those are postions that have addresses associated
 // with instructions. so a better design is to store these explicitly, making them
 // easy to set from the print_attribute() function. The SgAsmDwarfLine* source_position
 // above is there for the purpose of being able to be filled in via a separate analysis.
    [[using Rosebud: rosetta]]  
    int decl_file_id = -1;  
      
    [[using Rosebud: rosetta]]  
    int decl_line = -1;  
      
    [[using Rosebud: rosetta]]  
    int decl_column = -1;  

public:
    /** Factory pattern to build IR nodes based on the tag. */
    static SgAsmDwarfConstruct* createDwarfConstruct( int tag, int nesting_level, uint64_t offset, uint64_t overall_offset );

    virtual SgAsmDwarfConstructList* get_children();
};
