class SgAsmNEEntryPoint: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    enum NEEntryFlags {
        EF_ZERO             = 0x00,                     /* No flags set */
        EF_RESERVED         = 0xfc,                     /* Reserved bits */
        EF_EXPORTED         = 0x01,                     /* Exported */
        EF_GLOBAL           = 0x02                      /* Uses a global (shared) data section */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    SgAsmNEEntryPoint::NEEntryFlags flags = SgAsmNEEntryPoint::EF_ZERO;

    [[using Rosebud: rosetta]]
    unsigned int3f = 0;

    [[using Rosebud: rosetta]]
    unsigned sectionIndex = 0;

    [[using Rosebud: rosetta]]
    unsigned sectionOffset = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmNEEntryPoint(SgAsmNEEntryPoint::NEEntryFlags flags, unsigned int3f, unsigned s_idx, unsigned s_off);
    void dump(FILE*, const char *prefix, ssize_t idx) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    unsigned get_section_idx() const ROSE_DEPRECATED("use get_sectionIndex");
    void set_section_idx(unsigned) ROSE_DEPRECATED("use set_sectionIndex");
    unsigned get_section_offset() const ROSE_DEPRECATED("use get_sectionOffset");
    void set_section_offset(unsigned) ROSE_DEPRECATED("use set_sectionOffset");
};
