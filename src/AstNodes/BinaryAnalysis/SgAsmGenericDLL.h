#include <sageContainer.h>

#ifdef ROSE_IMPL
#include <SgAsmBasicString.h>
#endif

/** Base class for dynamically linked library information. */
class SgAsmGenericDLL: public SgAsmExecutableFileFormat {
public:
    /** Property: Name.
     *
     * @{ */
    [[using Rosebud: rosetta, traverse, mutators()]]
    SgAsmGenericString* name = createAndParent<SgAsmBasicString>(this);
    void set_name(SgAsmGenericString *s);
    /** @} */

    /** Property: Symbols. */
    [[using Rosebud: rosetta]]
    SgStringList symbols;

public:
    /** Constructor that initializes the name. */
    explicit SgAsmGenericDLL(SgAsmGenericString *s);

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Add a needed symbol to the import list for this DLL. */
    void addSymbol(const std::string&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void add_symbol(const std::string&) ROSE_DEPRECATED("use addSymbol");
};
