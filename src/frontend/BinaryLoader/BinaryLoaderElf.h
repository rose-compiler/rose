#ifndef ROSE_BINARYLOADERELF_H
#define ROSE_BINARYLOADERELF_H

#include "BinaryLoader.h"

class BinaryLoaderElf: public BinaryLoader {
public:
    BinaryLoaderElf() {}

    BinaryLoaderElf(const BinaryLoaderElf &other)
        : BinaryLoader(other)
        {}

    virtual ~BinaryLoaderElf() {}

    /** Copy constructor. See super class. */
    virtual BinaryLoaderElf *clone() const {
        return new BinaryLoaderElf(*this);
    }

    /** Capability query. See super class. */
    virtual bool can_load(SgAsmGenericHeader*) const;

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
    void add_lib_defaults(SgAsmGenericHeader *header=NULL);

    /** Returns the strings associated with certain variables in the ".dynamic" section. */
    static void get_dynamic_vars(SgAsmGenericHeader*, std::string &rpath/*out*/, std::string &runpath/*out*/);

    /** Perform relocation fixups. See super class. */
    virtual void fixup(SgAsmInterpretation *interp);

    /* FIXME: These should probably be in SgAsmElfSymver* classes instead. [RPM 2010-09-14] */
    /** Flags for version definitions and requirements. */
    enum {
        VER_FLG_BASE=0x1,
        VER_FLG_WEAK=0x2,
        VERSYM_HIDDEN=0x8000
    };

    /** Symbol from .dynsym combined with additional information.  The additional information is:
     *  <ul>
     *    <li>A pointer to the .dynsym section in which this symbol appeared.  For some reason, the section in which the symbol
     *        was defined is not a parent of the symbol in the AST. [FIXME RPM 2010-09-13]</li>
     *    <li>The symbol's entry in the GNU Symbol Version Table (.gnu.version section)</li>
     *    <li>The symbol's entry in the GNU Symbol Version Definition Table (.gnu.version_d section), if any.</li>
     *    <li>The symbol's auxiliary information (and thus, indirectly, the table entry) from the GNU Symbol Version
     *        Requirements Table (.gnu.version_r section), if any.</li>
     *  </ul> */
    class VersionedSymbol {
    private:
        SgAsmElfSymbol* p_symbol;
        SgAsmElfSymbolSection* p_parent;
        SgAsmElfSymverEntry* p_version_entry;
        SgAsmElfSymverDefinedEntry* p_version_def;
        SgAsmElfSymverNeededAux* p_version_need;
    public:
        explicit VersionedSymbol(SgAsmElfSymbol* symbol=NULL, SgAsmElfSymbolSection* parent=NULL)
            : p_symbol(symbol), p_parent(parent), p_version_entry(NULL), p_version_def(NULL), p_version_need(NULL)
            {}

        /** (Re)initializes this symbol and symbol section pointers. */
        void set_symbol(SgAsmElfSymbol *symbol, SgAsmElfSymbolSection *parent) {
            p_symbol = symbol;
            p_parent = parent;
        }
        
        /** Returns true if this symbol is visible only locally. */
        bool is_local() const;

        /** Returns true if this symbol is hidden. */
        bool is_hidden() const;

        /** Returns true if this symbol is a reference to an object rather than the definition of the object. */
        bool is_reference() const;
        
        /** Returns tru if this symbol is a base definition.  A base definition is either an unversioned symbol or a version
         *  definition with the VER_FLG_BASE flag set. */
        bool is_base_definition() const;

        /** Returns the symbol part of this versioned symbol. */
        SgAsmElfSymbol *get_symbol() const {
            return p_symbol;
        }

        /** Returns the symbol section (.dynsym) where this symbol was defined. */
        SgAsmElfSymbolSection *get_parent() const {
            return p_parent;
        }
        
        /** Returns the version string of this symbol. The empty string is returned if the symbol has no associated version. */
        std::string get_version() const;
        
        /** Returns the name of this symbol. */
        std::string get_name() const {
            return p_symbol->get_name()->get_string();
        }
        
        /** Returns the full, versionioned name of this symbol. Used for debugging. */
        std::string dump_versioned_name() const;
        
        /** Set the version pointer for this symbol. */
        void set_version_entry(SgAsmElfSymverEntry *entry) {
            p_version_entry = entry;
        }

        /** Set the version definition of this symbol.  The definition flags must be zero or VER_FLG_BASE. */
        void set_version_def(SgAsmElfSymverDefinedEntry* def) {
            ROSE_ASSERT(def->get_flags() == 0 || def->get_flags() == VER_FLG_BASE);
            p_version_def = def;
        }

        /** Set the version requirement of this symbol. The requirement flags must be zero or VER_FLG_WEAK. */
        void set_version_need(SgAsmElfSymverNeededAux* need) {
            ROSE_ASSERT(need->get_flags() == 0 || need->get_flags() == VER_FLG_WEAK);
            p_version_need = need;
        }

        /** Get the version requirement of this symbol. */
        SgAsmElfSymverNeededAux* get_version_need() const {
            return p_version_need;
        }

        /** Get the version definition of this symbol. */
        SgAsmElfSymverDefinedEntry* get_version_def() const {
            return p_version_def;
        }
    };

    /** An entry for a SymbolMap.  Each entry holds a list of related versioned symbols, the first of which is the base
     *  definition. */
    struct SymbolMapEntry {
    private:
        /* Base version will be at the front if we have one; other entries are unsorted. */
        std::vector<VersionedSymbol> p_versions;
    public:
        /** Returns the base version. */
        const VersionedSymbol &get_vsymbol() const {
            return get_base_version();
        }

        /** Returns the ELF symbol from the base version. */
        SgAsmElfSymbol *get_symbol() const {
            return get_vsymbol().get_symbol();
        }

        VersionedSymbol get_vsymbol(const VersionedSymbol &version) const;

        /** Returns the section where the base version symbol was defined. */
        SgAsmElfSymbolSection *get_parent() const {
            return get_vsymbol().get_parent();
        }

        /** Add an additional versioned symbol to this entry.  An entry can have only one base definition and an assertion
         *  will if an attempt is made to add a second base definition. */
        void addVersion(const VersionedSymbol &vsymbol);

        /** Merge the versions from the specified entry into this entry. */
        void merge(const SymbolMapEntry&);

    private:
        const VersionedSymbol& get_base_version() const {
            ROSE_ASSERT(!p_versions.empty());
            return p_versions.front();
        }
    };

    /* FIXME: We should probably make this a subclass of std::map<>, in which case we wouldn't need the extra level of
     *        indirection to get the BaseMap. [RPM 2010-09-14] */
    /** A mapping from symbol name (with optional version in parentheses) to SymbolMapEntry. */
    class SymbolMap {
    public:
        typedef std::map<std::string, SymbolMapEntry> BaseMap;

        /* FIXME: This should be renamed to lookup() since find() generally returns an iterator. [RPM 2010-09-14] */
        /** Finds and returns the entry having the specified name.  Returns the null pointer if the name cannot be found. */
        const SymbolMapEntry *find(std::string name) const;

        /* FIXME: This should be renamed to lookup() since find() generally returns an iterator. [RPM 2010-09-14] */
        /** Finds and returns the entry having the specified name and version.  Returns the null pointer if the name cannot be
         *  found.  The lookup is performed by enclosing the version string in parentheses (if the version is not empty) and
         *  appending it to the symbol name. */
        const SymbolMapEntry *find(std::string name, std::string version) const;

        BaseMap& get_base_map() {
            return p_base_map;
        }

    private:
        BaseMap p_base_map;
    };
    
    class SymverResolver {
    public:
        SymverResolver(SgAsmGenericHeader *header) { ctor(header); }

        /** Returns the VersionedSymbol corresponding to the specified symbol. The specified symbol must be a member of the
         *  versioned symbol map (or an assertion fails). */
        VersionedSymbol get_versioned_symbol(SgAsmElfSymbol *symbol) const;

    private:
        /** Helper for constructors. */
        void ctor(SgAsmGenericHeader*);

        /** Initialize the p_symbolVersionDefMap from the ELF Symbol Version Definition Table.  This mapping is from each entry's
         *  get_index() to the entry itself. */
        void makeSymbolVersionDefMap(SgAsmElfSymverDefinedSection*);

        /** Initialize the p_symbolVersionNeedMap from the ELF Symbol Version Requirements Table auxiliary information.  The
         *  mapping is from each auxiliary's get_other() to the auxiliary. The table entries are available indirectly since an
         *  SgAsmElfSymverNeededEntry is the parent of each SgAsmElfSymverNeededAux. */
        void makeSymbolVersionNeedMap(SgAsmElfSymverNeededSection*);

        /** Create a map from from each SgAsmElfSymbol* to a VersionedSymbol. The p_symbolVersionDefMap must be initialized
         *  before calling this. The SgAsmElfSymverSection argument may be null, in which case VersionedSymbols are basically
         *  just a wrapper to their SgAsmElfSymbol. */
        void makeVersionedSymbolMap(SgAsmElfSymbolSection*, SgAsmElfSymverSection*);

        typedef std::map<SgAsmElfSymbol*, VersionedSymbol*> VersionedSymbolMap;
        typedef std::map<uint16_t, SgAsmElfSymverDefinedEntry*> SymbolVersionDefinitionMap;
        typedef std::map<uint16_t, SgAsmElfSymverNeededAux*> SymbolVersionNeededMap;

        /** Map from each ELF Symbol Version Definition Table entry's get_index() to the entry itself. */
        SymbolVersionDefinitionMap p_symbolVersionDefMap;

        /** Map from each auxiliary's get_other() to the auxiliary itself. The auxiliaries come from the GNU Symbol Version
         *  Requirements Table, each entry of which points to a list of auxiliaries.  The parent of each auxiliary is the table
         *  entry that contained the auxiliary, thus this mapping also maps get_other() to GNU Symbol Version Requirements Table
         *  entries. */
        SymbolVersionNeededMap p_symbolVersionNeedMap;

        /** Map from an SgAsmElfSymbol to a VersionedSymbol. */
        VersionedSymbolMap p_versionedSymbolMap;
    };

protected:
    /** Returns mappable sections in a particular order.  Returns ELF Segments in the order they are defined in the segment
     *  table, followed by ELF Sections in the order they are defined in the section table but excluding those sections that
     *  were already added to the list as ELF Segments. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader*);

    /** Returns a new, temporary base address which is greater than everything that's been mapped already. */
    virtual rose_addr_t rebase(MemoryMap*, SgAsmGenericHeader*, const SgAsmGenericSectionPtrList&);

    /** Linux-specific ELF Segment and Section alignment. */
    virtual MappingContribution align_values(SgAsmGenericSection*, MemoryMap*,
                                             rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                             rose_addr_t *va, rose_addr_t *mem_size,
                                             rose_addr_t *offset, rose_addr_t *file_size,
                                             rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi, 
                                             ConflictResolution *resolve);

    /*========================================================================================================================
     * Functions moved here from the BinaryLoader_ElfSupport name space.
     *======================================================================================================================== */

    /* FIXME: Move this to src/ROSETTA where it belongs. [RPM 2010-08-31] */
    typedef Rose_STL_Container<SgAsmElfSection*> SgAsmElfSectionPtrList;
    static int get_verbose() {return 5;}
    static SgAsmGenericSection *find_mapped_section(SgAsmGenericHeader*, rose_addr_t va);
    static void relocate_X86_JMP_SLOT(SgAsmElfRelocEntry*, SgAsmElfRelocSection*, const SymbolMap&, const SymverResolver&,
                                      const size_t addrSize);
    static void relocate_X86_64_RELATIVE(SgAsmElfRelocEntry*, SgAsmElfRelocSection*, const SymbolMap&, const SymverResolver&,
                                         const size_t addrSize);
    static void relocate_X86_64_64(SgAsmElfRelocEntry*, SgAsmElfRelocSection*, const SymbolMap&, const SymverResolver&,
                                   const size_t addrSize);
    static void performRelocation(SgAsmElfRelocEntry*, SgAsmElfRelocSection*, const SgAsmElfSectionPtrList& extentSortedSections,
                                  const SymverResolver&, const SymbolMap&);
    static void performRelocations(SgAsmElfFileHeader*, const SgAsmElfSectionPtrList& extentSortedSections,
                                   const SymbolMap&);




};

#endif /*ROSE_BINARYLOADERELF_H*/
