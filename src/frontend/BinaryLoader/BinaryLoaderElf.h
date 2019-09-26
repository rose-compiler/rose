#ifndef ROSE_BinaryAnalysis_BinaryLoaderElf_H
#define ROSE_BinaryAnalysis_BinaryLoaderElf_H

#include "BinaryLoader.h"

namespace Rose {
namespace BinaryAnalysis {

/** Reference counting pointer to @ref BinaryLoaderElf. */
typedef Sawyer::SharedPointer<class BinaryLoaderElf> BinaryLoaderElfPtr;

/** Loader for ELF files. */
class BinaryLoaderElf: public BinaryLoader {
public:
    /** Reference counting pointer to @ref BinaryLoaderElf. */
    typedef Sawyer::SharedPointer<class BinaryLoaderElf> Ptr;

public:
    /* FIXME: These should probably be in SgAsmElfSymver* classes instead. [RPM 2010-09-14] */
    /** Flags for version definitions and requirements. */
    enum {
        VER_FLG_BASE=0x1,
        VER_FLG_WEAK=0x2,
        VERSYM_HIDDEN=0x8000
    };

    /** Symbol from .dynsym combined with additional information.  The additional information is:
     *  <ul>
     *    <li>The symbol's entry in the GNU Symbol Version Table (.gnu.version section)</li>
     *    <li>The symbol's entry in the GNU Symbol Version Definition Table (.gnu.version_d section), if any.</li>
     *    <li>The symbol's auxiliary information (and thus, indirectly, the table entry) from the GNU Symbol Version
     *        Requirements Table (.gnu.version_r section), if any.</li>
     *  </ul> */
    class VersionedSymbol {
    private:
        SgAsmElfSymbol* symbol_;
        SgAsmElfSymverEntry* versionEntry_;
        SgAsmElfSymverDefinedEntry* versionDef_;
        SgAsmElfSymverNeededAux* versionNeed_;
    public:
        explicit VersionedSymbol(SgAsmElfSymbol* symbol)
            : symbol_(symbol), versionEntry_(NULL), versionDef_(NULL), versionNeed_(NULL)
            {}

        /** Returns true if this symbol is visible only locally. */
        bool isLocal() const;

        /** Returns true if this symbol is hidden. */
        bool isHidden() const;

        /** Returns true if this symbol is a reference to an object rather than the definition of the object. */
        bool isReference() const;

        /** Returns tru if this symbol is a base definition.  A base definition is either an unversioned symbol or a version
         *  definition with the VER_FLG_BASE flag set. */
        bool isBaseDefinition() const;

        /** Property: The symbol.
         *
         * @{ */
        void symbol(SgAsmElfSymbol *symbol) { symbol_ = symbol; }
        SgAsmElfSymbol* symbol() const { return symbol_; }
        /** @} */

        /** Returns the symbol section (.dynsym) where this symbol was defined. */
        SgAsmElfSymbolSection* getSection() const {
            SgAsmElfSymbolSection *retval = SageInterface::getEnclosingNode<SgAsmElfSymbolSection>(symbol_);
            ASSERT_not_null(retval);
            return retval;
        }

        /** Returns the version string of this symbol. The empty string is returned if the symbol has no associated version. */
        std::string getVersion() const;

        /** Returns the name of this symbol. */
        std::string getName() const {
            return symbol_->get_name()->get_string();
        }

        /** Returns the full, versionioned name of this symbol. Used for debugging. */
        std::string getVersionedName() const;

        /** Property: Version pointer for this symbol. */
        void versionEntry(SgAsmElfSymverEntry *entry) {
            versionEntry_ = entry;
        }

        /** Property: Version definition of this symbol.
         *
         *  The definition flags must be zero or VER_FLG_BASE.
         *
         * @{ */
        SgAsmElfSymverDefinedEntry* versionDef() const {
            return versionDef_;
        }
        void versionDef(SgAsmElfSymverDefinedEntry* def) {
            ROSE_ASSERT(def->get_flags() == 0 || def->get_flags() == VER_FLG_BASE);
            versionDef_ = def;
        }
        /** @} */

        /** Property: The version requirement of this symbol.
         *
         *  The requirement flags must be zero or VER_FLG_WEAK.
         *
         * @{ */
        SgAsmElfSymverNeededAux* versionNeed() const {
            return versionNeed_;
        }
        void versionNeed(SgAsmElfSymverNeededAux* need) {
            ROSE_ASSERT(need->get_flags() == 0 || need->get_flags() == VER_FLG_WEAK);
            versionNeed_ = need;
        }
        /** @} */

        /** Print used by operator<<. */
        void print(std::ostream&) const;

        /** Dump info like for SgAsm* objects. */
        void dump(FILE*, const char *prefix, ssize_t idx) const;
    };

    /** An entry for a SymbolMap.  Each entry holds a list of related versioned symbols, the first of which is the base
     *  definition. */
    struct SymbolMapEntry {
    private:
        /* Base version will be at the front if we have one; other entries are unsorted. */
        std::vector<VersionedSymbol> versions_;
    public:
        /** Returns the base version. */
        const VersionedSymbol &getVSymbol() const {
            return getBaseVersion();
        }

        /** Returns the ELF symbol from the base version. */
        SgAsmElfSymbol *getSymbol() const {
            return getVSymbol().symbol();
        }

        /** Find definition of symbol. The specified versioned symbol is probably a symbol referenced by a relocation. This
         *  method will scan the list of definitions in this SymbolEntryMap and return the first (only) symbol that has the
         *  same name as the supplied symbol's needed version. */
        VersionedSymbol getVSymbol(const VersionedSymbol &version) const;

        /** Returns the section where the base version symbol was defined. */
        SgAsmElfSymbolSection *getSection() const {
            return getVSymbol().getSection();
        }

        /** Add an additional versioned symbol to this entry.  An entry can have only one base definition and an assertion
         *  will if an attempt is made to add a second base definition. */
        void addVersion(const VersionedSymbol &vsymbol);

        /** Merge the versions from the specified entry into this entry. */
        void merge(const SymbolMapEntry&);

        /** Print info about this symbol map entry. */
        void dump(FILE*, const char *prefix) const ;

    private:
        const VersionedSymbol& getBaseVersion() const {
            ASSERT_forbid(versions_.empty());
            return versions_.front();
        }
    };

    /** A mapping from symbol name (with optional version in parentheses) to SymbolMapEntry. */
    class SymbolMap: public std::map<std::string/*symbol name*/, SymbolMapEntry> {
    public:
        /** Finds and returns the entry having the specified name.  Returns the null pointer if the name cannot be found. */
        const SymbolMapEntry *lookup(std::string name) const;

        /** Finds and returns the entry having the specified name and version.  Returns the null pointer if the name cannot be
         *  found.  The lookup is performed by enclosing the version string in parentheses (if the version is not empty) and
         *  appending it to the symbol name. */
        const SymbolMapEntry *lookup(std::string name, std::string version) const;

        /** Print debugging information about this SymbolMap. */
        void dump(FILE*, const char *prefix) const;
    };

    class SymverResolver {
    public:
        typedef std::map<SgAsmElfSymbol*, VersionedSymbol*> VersionedSymbolMap;
        typedef std::map<uint16_t, SgAsmElfSymverDefinedEntry*> SymbolVersionDefinitionMap;
        typedef std::map<uint16_t, SgAsmElfSymverNeededAux*> SymbolVersionNeededMap;

    private:
        // Map from each ELF Symbol Version Definition Table entry's get_index() to the entry itself.
        SymbolVersionDefinitionMap symbolVersionDefMap_;

        // Map from each auxiliary's get_other() to the auxiliary itself. The auxiliaries come from the GNU Symbol Version
        // Requirements Table, each entry of which points to a list of auxiliaries.  The parent of each auxiliary is the table
        // entry that contained the auxiliary, thus this mapping also maps get_other() to GNU Symbol Version Requirements Table
        // entries.
        SymbolVersionNeededMap symbolVersionNeedMap_;

        // Map from an SgAsmElfSymbol to a VersionedSymbol.
        VersionedSymbolMap versionedSymbolMap_;
    public:
        SymverResolver(SgAsmGenericHeader *header) {
            ctor(header);
        }

        /** Returns the VersionedSymbol corresponding to the specified symbol. The specified symbol must be a member of the
         *  versioned symbol map (or an assertion fails). */
        VersionedSymbol getVersionedSymbol(SgAsmElfSymbol *symbol) const;

        /** Print some info about the resolver. */
        void dump(FILE*, const char *prefix, ssize_t idx) const;

    private:
        /** Helper for constructors. */
        void ctor(SgAsmGenericHeader*);

        /** Initialize the symbolVersionDefMap_ from the ELF Symbol Version Definition Table.  This mapping is from each entry's
         *  get_index() to the entry itself. */
        void makeSymbolVersionDefMap(SgAsmElfSymverDefinedSection*);

        /** Initialize the symbolVersionNeedMap_ from the ELF Symbol Version Requirements Table auxiliary information.  The
         *  mapping is from each auxiliary's get_other() to the auxiliary. The table entries are available indirectly since an
         *  SgAsmElfSymverNeededEntry is the parent of each SgAsmElfSymverNeededAux. */
        void makeSymbolVersionNeedMap(SgAsmElfSymverNeededSection*);

        /** Create a map from from each SgAsmElfSymbol* to a VersionedSymbol. The symbolVersionDefMap_ must be initialized
         *  before calling this. The SgAsmElfSymverSection argument may be null, in which case VersionedSymbols are basically
         *  just a wrapper to their SgAsmElfSymbol. */
        void makeVersionedSymbolMap(SgAsmElfSymbolSection*, SgAsmElfSymverSection*);
    };

protected:
    /** Symbol table for an entire interpretation.
     *
     *  This symbol table is created by the fixup() method via build_master_symbol_table() and used by various relocation
     *  fixups. */
    SymbolMap symbols_;

protected:
    BinaryLoaderElf() {}

    BinaryLoaderElf(const BinaryLoaderElf &other)
        : BinaryLoader(other)
        {}

public:
    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new BinaryLoaderElf);
    }

    virtual ~BinaryLoaderElf() {}

    /** Copy constructor. See super class. */
    virtual BinaryLoaderPtr clone() const ROSE_OVERRIDE {
        return BinaryLoaderPtr(new BinaryLoaderElf(*this));
    }

    /** Capability query. See super class. */
    virtual bool canLoad(SgAsmGenericHeader*) const ROSE_OVERRIDE;

    /** Sets up library search paths and preloads from the environment.  The search paths and preloads are added to the end of
     *  the lists.  If an ELF file header is provided, then the DT_RPATH and DT_RUNPATH from the ".dynamic" section are also
     *  used.
     *
     *  Caveats:
     *  <ul>
     *    <li>The LD_PRELOAD and LD_LIBRARY_PATH environment variables are always consulted, even if the specimen is
     *        setuid.</li>
     *    <li>The library cache files (/etc/ld.so.*) are never consulted.</li>
     *    <li>No special behavior for specimens linked with "-z nodeflib" (not sure how to detect this.)</li>
     *    <li>The virtual dynamic shared object (vdso, linux-gate.so, etc) is not loaded.</li>
     *    <li>Since the environment variables that are consulted by this method are the very same ones used by the
     *        real loader-linker, it's not possible to fully control this method without also affecting the loading
     *        of ROSE itself.</li>
     *  </ul> */
    void addLibDefaults(SgAsmGenericHeader *header=NULL);

    /** Returns the strings associated with certain variables in the ".dynamic" section. */
    static void getDynamicVars(SgAsmGenericHeader*, std::string &rpath/*out*/, std::string &runpath/*out*/);

    // documented in superclass
    virtual void fixup(SgAsmInterpretation *interp, FixupErrors *errors=NULL) ROSE_OVERRIDE;

    /** Find the section containing the specified virtual address.  Only ELF Sections of the specified header are searched,
     *  and we search based on the preferred mapping location of the section (not the actual mapping location).  The null
     *  pointer is returned if no suitable section can be found. */
    virtual SgAsmGenericSection *findSectionByPreferredVa(SgAsmGenericHeader*, rose_addr_t va);

protected:
    /** Returns mappable sections in a particular order.  Returns ELF Segments in the order they are defined in the segment
     *  table, followed by ELF Sections in the order they are defined in the section table but excluding those sections that
     *  were already added to the list as ELF Segments. */
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader*) ROSE_OVERRIDE;

public:
    /** Returns a new, temporary base address which is greater than everything that's been mapped already. */
    virtual rose_addr_t rebase(const MemoryMap::Ptr&, SgAsmGenericHeader*, const SgAsmGenericSectionPtrList&) ROSE_OVERRIDE;

protected:
    /** Linux-specific ELF Segment and Section alignment. */
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                            rose_addr_t *va, rose_addr_t *mem_size,
                                            rose_addr_t *offset, rose_addr_t *file_size, bool *map_private,
                                            rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve) ROSE_OVERRIDE;

    /** Builds the master symbol table. This table is built just before relocations are fixed up and contains information
     *  about all the symbols that might be necessary during that process.  The symbol table describes one entire
     *  interpretation. */
    void buildMasterSymbolTable(SgAsmInterpretation*);

    /*========================================================================================================================
     * Methods returning prerequisite information for fixups.  These names all begin with "fixup_info_".
     *======================================================================================================================== */
protected:

    /** Returns the defining symbol for a relocation, if any.  The relocation optionally points to a symbol under the same
     *  file header as the relocation.  We then consult the supplied @p resolver to find a defining symbol under a possibly
     *  different file header of the same interpretation.  The defining symbol is returned.
     *
     *  If the relocation does not refer to a symbol, or the symbol is weak with no definition then a null pointer is
     *  returned. Otherwise, if a defining symbol cannot be located via the resolver, then an Exception is thrown.
     *
     *  Debugging information is conditionally output and indented four spaces. */
    SgAsmElfSymbol *fixupInfoRelocSymbol(SgAsmElfRelocEntry*, const SymverResolver&);

    /** Returns the virtual address where a relocation should be supplied.  The relocation address is computed by treating the
     *  relocation offset as a virtual address, finding the section that would have contained that virtual address had all
     *  sections of the relocation's header been mapped at their preferred addresses, and returning the sum of the relocation
     *  offset with the difference between the section's actual and preferred mapping addresses.  The section and adjustment
     *  are optionally returned through the @p section_p and @p adj_p pointer arguments.
     *
     *  If no section can be found for the relocation offset then an Exception is thrown.
     *
     *  Debugging information is conditionally output and indented four spaces. */
    rose_addr_t fixupInfoTargetVa(SgAsmElfRelocEntry*, SgAsmGenericSection **section_p=NULL, rose_addr_t *adj_p=NULL);

    /** Returns the virtual address of a symbol adjusted for remapping.  The return value is computed by treating the symbol
     *  value as a virtual address, finding the section that would have contained that virtual address had all sections of the
     *  symbol's header been mapped at their preferred addresses, and returning the sum of the symbol value with the
     *  difference between the section's actual and preferred mapping addresses.  The section and adjustment are optionally
     *  returned through the @p section_p and @p adj_p pointer arguments.
     *
     *  If no section can be found for the relocation offset then an Exception is thrown.
     *
     *  Debugging information is conditionally output and indented four spaces. */
    rose_addr_t fixupInfoSymbolVa(SgAsmElfSymbol*, SgAsmGenericSection **section_p=NULL, rose_addr_t *adj_p=NULL);

    /** Returns the addend associated with a relocation.  If the relocation appears in a RELA relocation section then the
     *  addend is that which is specified in the relocation entry itself.  Otherwise the supplied relocation target virtual
     *  address and memory map are used to read the addend from specimen memory.
     *
     *  The @p nbytes arguments indicates how many bytes are read from the specimen's memory if the relocation is not of the
     *  RELA variety.  If the size is zero (the default) then it is obtained from the relocation's file header.  The byte sex
     *  is always obtained from information in the relocation's file header.
     *
     *  An Exception is thrown if an attempt is made to read from memory which is not mapped or not readable. */
    rose_addr_t fixupInfoAddend(SgAsmElfRelocEntry*, rose_addr_t target_va, const MemoryMap::Ptr&, size_t nbytes=0);

    /** Evaluates a simple postfix expression and returns the result.  The expression consists of terms, operators, and
     *  settings each consisting of a single character. They are defined as follows, and for the most part match various
     *  linker documentation from Sun Microsystems "Linker and Libraries Guide", April 2008, page 239:
     *
     *  <ul>
     *    <li>"0", "4", and "8" are settings for the size (in bytes) of memory accesses when performing operations
     *        such as reading addend values from specimen memory.  The default "0" indicates that the size should
     *        be determined from the word size specified by the relocation entrie's file header.</li>
     *    <li>"A" is the addend used to compute the value of the relocatable field. See fixup_info_addend(). The size
     *        of the addend is determined from the current nbytes setting.</li>
     *    <li>"B" is the base address at which a shared object is loaded into memory during execution. Generally,
     *        a shared object file is built with a base virtual address of zero. However, the execution address of
     *        the shared object is different. See the adjustment argument of fixup_info_symbol_va().</li>
     *    <li>"S" is the value of the symbol whose index resides in the relocation entry.  Specifically, this is the
     *        value of the associated defining symbol adjusted for remapping. See fixup_info_symbol_va().</li>
     *    <li>"+" replaces the top two values of the stack with their sum.</li>
     *    <li>"-" replaces the top two values of the stack with their difference, subtracting the top value from the
     *        second-to-top value.</li>
     *  </ul>
     *
     *  In addition to the primary return value, these additional values are returned through pointers when the pointer is
     *  non-null:
     *
     *  <ul>
     *    <li>@p target_va_t is the virtual address where the relocation should be applied.
     *        See fixup_info_target_va().</li>
     *  </ul>
     *
     * Debugging information is conditionally emitted and indented four spaces.  Most debugging information comes from
     * the underlying fixup_info_* methods that are called.
     *
     * Exceptions are thrown when something goes wrong.  Most exceptions come from the underlying fixup_info_* methods. */
    rose_addr_t fixupInfoExpr(const std::string &expression, SgAsmElfRelocEntry *reloc, const SymverResolver &resolver,
                              const MemoryMap::Ptr &memmap, rose_addr_t *target_va_p=NULL);



    /*========================================================================================================================
     * Methods that apply a relocation fixup.  These names all begin with "fixup_apply_".
     *======================================================================================================================== */
protected:
    /** Writes a value into memory at the relocation target. The target virtual address is either specified by a non-zero
     *  value for the @p target_va argument, or (re)computed from the supplied relocation entry.  This method is usually
     *  called by the othe fixup_apply_* methods. The value is truncated and/or byte-swapped if necessary according to the
     *  file header containing the relocation entry. If @p nbytes is zero (the default) then the size will be determined from
     *  the relocation's file header.
     *
     *  Debugging information is conditionally emitted and indented four spaces.
     *
     *  An Exception is thrown if the value cannot be written to the specimen memory due to memory not being mapped or not
     *  being writable. */
    void fixupApply(rose_addr_t value, SgAsmElfRelocEntry*, const MemoryMap::Ptr&, rose_addr_t target_va=0, size_t nbytes=0);

    /** Copies symbol memory to the relocation target.  This is usually used to copy initialized library data (initialized by
     *  the loader calling a constructor) into a common location in the executable's .bss. */
    void fixupApplySymbolCopy(SgAsmElfRelocEntry*, const SymverResolver&, const MemoryMap::Ptr&);

    /*========================================================================================================================
     * Functions moved here from the BinaryLoader_ElfSupport name space.
     *======================================================================================================================== */
protected:
    void performRelocation(SgAsmElfRelocEntry*, const SymverResolver&, const MemoryMap::Ptr&);
    void performRelocations(SgAsmElfFileHeader*, const MemoryMap::Ptr&);
};

std::ostream& operator<<(std::ostream&, const BinaryLoaderElf::VersionedSymbol&);

} // namespace
} // namespace

#endif
