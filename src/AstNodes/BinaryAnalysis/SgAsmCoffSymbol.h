#include <Rose/BinaryAnalysis/Address.h>

#include <sageContainer.h>

/** COFF symbol. */
class SgAsmCoffSymbol: public SgAsmGenericSymbol {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    struct COFFSymbol_disk {
        union {
            char            st_name[8];
            struct {
                uint32_t    st_zero;
                uint32_t    st_offset;
            };
        };
        uint32_t            st_value;
        int16_t             st_section_num;
        uint16_t            st_type;
        unsigned char       st_storage_class;
        unsigned char       st_num_aux_entries;
    }
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;
#ifdef _MSC_VER
# pragma pack ()
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties and data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Symbol name. */
    [[using Rosebud: rosetta]]
    std::string st_name;

    /** Property: Symbol name offset. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address st_name_offset = 0;

    /** Property: Section number. */
    [[using Rosebud: rosetta]]
    int st_section_num = 0;

    /** Property: Symbol type constant. */
    [[using Rosebud: rosetta]]
    unsigned st_type = 0;

    /** Property: Symbol storage class. */
    [[using Rosebud: rosetta]]
    unsigned st_storage_class = 0;

    /** Property: Number of auxilliary entries. */
    [[using Rosebud: rosetta]]
    unsigned st_num_aux_entries = 0;

    /** Property: Auxilliary data. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList auxiliaryData ;

public:
    static const unsigned int COFFSymbol_disk_size = 18;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmCoffSymbol(SgAsmPEFileHeader *fhdr, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx);
    void *encode(SgAsmCoffSymbol::COFFSymbol_disk*) const;
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const SgUnsignedCharList& get_aux_data() const ROSE_DEPRECATED("use get_auxiliaryData");
    void set_aux_data(const SgUnsignedCharList&) ROSE_DEPRECATED("use set_auxiliaryData");


};
