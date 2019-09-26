#ifndef ROSE_BinaryAnalysis_BinaryLoader_H
#define ROSE_BinaryAnalysis_BinaryLoader_H

#include <RoseException.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {

/** Reference counting pointer to @ref BinaryLoader. */
typedef Sawyer::SharedPointer<class BinaryLoader> BinaryLoaderPtr;

/** Base class for loading a static or dynamic object.
 *
 *  The BinaryLoader class is the base class that defines the public interface and provides generic implementations for
 *  loading a static or dynamic object.  "Loading" means parsing, linking, mapping, and relocating, which are defined as
 *  follows:
 *
 *  <ul>
 *    <li>Parsing: reading the contents of a binary file and parsing the container format (ELF, PE, COFF, Dwarf, etc) to
 *        produce an abstract syntax tree (AST).  Although the container is parsed, the actual machine instructions are
 *        not (that is, disassembly is not part of loading).</li>
 *    <li>Linking: recursively parsing all shared object dependencies. This step does not include mapping memory into a
 *        process address space or fixing up relocation information.</li>
 *    <li>Mapping: choosing virtual addresses for parts of the binary file as if ROSE were creating a new OS process. For
 *        instance, mapping an ELF file will cause ROSE to choose virtual addresses for all the ELF Segments.  This has
 *        two side effects. First, the SgAsmGenericSection::get_mapped_actual_rva() method will now return the address that
 *        ROSE chose for the section rather than the same value as get_mapped_preferred_rva(), although they will often still
 *        be the same after loading. Second, the MemoryMap object returned by SgAsmInterpretation::get_map() will be updated
 *        to include a mapping from virtual addresses to the SgAsmGenericSection content.</li>
 *    <li>Relocation: applying relocation fixups to patch pointers and offsets in various parts of the virtual address
 *        space.</li>
 *  </ul>
 *
 *  Some goals of this design are:
 *  <ul>
 *    <li>to load an entire binary into memory in a manner similar to how the operating system would do so.</li>
 *    <li>to generically handle both static and dynamic executables, treating static as a special case of dynamic.</li>
 *    <li>to handle a variety of binary container formats (ELF, PE, etc) and loaders (Linux and Windows).</li>
 *    <li>to be able to control the locations of shared object dependencies without affecting how ROSE itself is loaded.</li>
 *    <li>to be able to emulate linking of one operating system while ROSE is running in another operating system.</li>
 *    <li>to load an entire binary at once or one part at a time. This is useful when emulating ldopen().</li>
 *    <li>to be able to partially load an object. Useful when not all libraries are available.</li>
 *    <li>to allow a user to register a new loader subclass at runtime and have ROSE use it when appropriate.</li>
 *  </ul>
 *
 *  The general design is similar to the Disassembler class.  The BinaryLoader has class methods to register user-defined
 *  loaders with the library and each loader is able to answer whether it is capable of loading a particular kind of binary.
 *  ROSE (or any user of this class) can obtain a suitable loader for a particular SgAsmInterpretation, clone it (if desired),
 *  modify properties that control its behavior, and use it to load a binary.
 */
class BinaryLoader: public Sawyer::SharedObject {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Referenc counting pointer to @ref BinaryLoader. */
    typedef Sawyer::SharedPointer<BinaryLoader> Ptr;

    /** Describes how a section contributes to the overall memory map. */
    enum MappingContribution
        {
        CONTRIBUTE_NONE,                /**< Section does not contribute to final mapping. */
        CONTRIBUTE_ADD,                 /**< Section is added to the mapping. */
        CONTRIBUTE_SUB                  /**< Section is subtracted from the mapping. */
    };

    /** Describes how conflicts are resolved when mapping a section. */
    enum ConflictResolution
        {
        RESOLVE_THROW,                  /**< Throw an exception such as MemoryMap::Inconsistent. */
        RESOLVE_OVERMAP,                /**< Free the part of the original mapping that is in conflict. */
        RESOLVE_REMAP,                  /**< Move the section to any unused part of the address space. */
        RESOLVE_REMAP_ABOVE,            /**< Move the section to a higher unused part of the address space. */
    };

    /** Base class for exceptions thrown by loaders. */
    class Exception: public Rose::Exception {
    public:
        /** Construcor that takes a message. */
        explicit Exception(const std::string &reason): Rose::Exception(reason) {}

        /** Emit the error message to an output stream. */
        void print(std::ostream&) const;

        friend std::ostream& operator<<(std::ostream&, const Exception&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static Sawyer::Message::Facility mlog;              /**< Logging facility initialized by initDiagnostics(). */

private:
    static std::vector<Ptr> loaders_;                   // List of loader subclasses.
    std::vector<std::string> preloads_;                 // Libraries that should be pre-loaded.
    std::vector<std::string> directories_;              // Directories to search for libraries with relative names.
    bool performingDynamicLinking_;
    bool performingRemap_;
    bool performingRelocations_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    BinaryLoader()
        : performingDynamicLinking_(false), performingRemap_(true), performingRelocations_(false)
        { init(); }

    /** Copy constructor. */
    BinaryLoader(const BinaryLoader &other)
        : performingDynamicLinking_(other.performingDynamicLinking_),
          performingRemap_(other.performingRemap_), performingRelocations_(other.performingRelocations_) {
        preloads_ = other.preloads_;
        directories_ = other.directories_;
    }

public:
    virtual ~BinaryLoader(){}

    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new BinaryLoader);
    }

    /** Creates a new copy of a loader.
     *
     *  The new copy has all the same settings as the original. Subclasses that define data methods should certainly provide an
     *  implementation of this method, although all they'll need to change is the data type for the 'new' operator. */
    virtual Ptr clone() const {
        return Ptr(new BinaryLoader(*this));
    }

    /** Initialize diagnostic streams for binary loaders. */
    static void initDiagnostics();

private:
    /** Initialize the class. Register built-in loaders. */
    static void initclass();



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Registration and lookup methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Register a loader instance.
     *
     *  More specific loader instances should be registered after more general loaders since the lookup() method will inspect
     *  loaders in reverse order of their registration. */
    static void registerSubclass(const Ptr&);

    /** Predicate determining the suitability of a loader for a specific file header.
     *
     *  If this loader is capable of loading the specified file header, then this predicate returns true, otherwise it returns
     *  false.  The implementation in BinaryLoader always returns true because BinaryLoader is able to generically load all
     *  types of files, albeit with limited functionality. Subclasses should certainly redefine this method so it returns true
     *  only for certain headers. */
    virtual bool canLoad(SgAsmGenericHeader*) const {
        return true;
    }

    /** Finds a suitable loader.
     *
     *  Looks through the list of registered loader instances (from most recently registered to earliest registered) and
     *  returns the first one whose can_load() predicate returns true.  Throws an exception if no suitable loader can be
     *  found. */
    static Ptr lookup(SgAsmGenericHeader*);

    /** Finds a suitable loader.
     *
     *  Looks through the list of registered loader instances (from most recently registered to earliest registered) and
     *  returns the first one whose can_load() predicate returns true. This is done for each header contained in the
     *  interpretation and the loader for each header must match the other headers. An exception is thrown if no suitable
     *  loader can be found. */
    static Ptr lookup(SgAsmInterpretation*);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Whether this loader will perform the linking step.
     *
     *  To "link" means to recursively determine what shared objects are dependencies of the objects already in the AST and to
     *  parse them, adding them to the AST also.
     *
     * @{ */
    bool performingDynamicLinking() const { return performingDynamicLinking_; }
    void performingDynamicLinking(bool b) { performingDynamicLinking_ = b; }
    /** @} */

    /** Property: Whether this loader will perform the mapping step.
     *
     *  To "map" a section means to make the section's content available through a MemoryMap object attached to the
     *  interpretation that holds the section.  This step also resolves conflicts between two or more sections that request
     *  overlapping areas of memory.
     *
     * @{ */
    bool performingRemap() const { return performingRemap_; }
    void performingRemap(bool b) { performingRemap_ = b; }
    /** @} */

    /** Property: Whether this loader will perform the relocation step.
     *
     *  To "relocate" means to process the relocation sections of an executable and apply "fixups" to parts of memory that have
     *  been mapped. The fixups are computed based on the original contents of that memory and the difference in locations from
     *  the section's preferred memory location and the location chosen by this loader during the mapping phase.  There are
     *  many kinds of relocation fixups.
     *
     * @{ */
    bool performingRelocations() const { return performingRelocations_; }
    void performingRelocations(bool b) { performingRelocations_ = b; }
    /** @} */




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Searching for shared objects
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: List of libraries that will be pre-loaded.
     *
     *  These libraries are linked into the AST in the order they were added to the preload list. The @p libname should be
     *  either the base name of a library, such as "libm.so" (in which case the search directories are consulted) or a
     *  path-qualified name like "/usr/lib/libm.so".
     *
     * @{ */
    const std::vector<std::string>& preloads() const { return preloads_; }
    std::vector<std::string>& preloads() { return preloads_; }
    void preloads(const std::vector<std::string> &v) { preloads_ = v; }
    /** @} */

    /** Property: List of directories searched for libraries.
     *
     *  This is similar to the LD_LIBRARY_PATH environment variable of the ld-linux.so dynamic loader (see the ld.so man
     *  page). ROSE searches for libraries in directories in the order that directories were added.
     *
     * @{ */
    const std::vector<std::string>& directories() const { return directories_; }
    std::vector<std::string>& directories() { return directories_; }
    void directories(const std::vector<std::string> &v) { directories_ = v; }
    /** @} */

    /** Appends directories to the list of directories searched for libraries.
     *
     *  This is similar to the LD_LIBRARY_PATH environment variable of the ld-linux.so dynamic loader (see the ld.so man
     *  page). ROSE searches for libraries in directories in the order that directories were added. */
    void appendDirectories(const std::vector<std::string> &dirnames) {
        directories_.insert(directories_.end(), dirnames.begin(), dirnames.end());
    }

    /** Convert name to fully qualified name.
     *
     *  Given the name of a shared object, return the fully qualified name where the library is located in the file system.
     *  Throws a BinaryLoader::Exception if the library cannot be found. */
    virtual std::string findSoFile(const std::string &libname) const;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The main interface.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Top-level method to do everything.
     *
     *  Class method to parse, map, link, and/or relocate all interpretations of the specified binary composite. This should
     *  only be called for an SgBinaryComposite object that has been created but for which no binary files have been parsed
     *  yet.  It's only called from sage_support.cpp by SgBinaryComposite::buildAST().  A BinaryLoader::Exception is thrown if
     *  there's an error of some sort. */
    static void load(SgBinaryComposite* composite, bool read_executable_file_format_only=false);

    /** Top-level method to do everything.
     *
     *  Conditionally parse, map, link, and/or relocate the interpretation according to properties of this loader. If an error
     *  occurs, a BinaryLoader::Exception will be thrown.  The interpretation must be one that can be loaded by this loader as
     *  returned by this loader's can_load() method. */
    virtual void load(SgAsmInterpretation*);

    /** Finds and parses all shared objects needed by an interpretation.
     *
     *  Links an interpretation by parsing all shared objects required by that interpretation.  In other words, all
     *  dependencies of the interpretation are parsed and added to the AST.  As mentioned in the BinaryLoader documentation,
     *  this process is referred to as "linking".
     *
     *  Recursively perform a breadth-first search of all headers, starting with the headers already in the interpretation.
     *  For each header, obtain a list of necessary shared objects (pruning away those that have already been processed) and
     *  parse the shared object, adding it to the AST and adding its appropriate headers to the interpretation.  Parsing of
     *  the shared objects is performed by calling createAsmAST().
     *
     *  This process is recursive in nature. A dynamically linked executable has a list of libraries on which it depends, and
     *  those libraries also often have dependencies.  The recursion is breadth-first because ELF specifies a particular order
     *  that symbols should be resolved. Order is not important for a PE binary since its shared object symbols are scoped to
     *  a library.
     *
     *  The list of dependencies for a particular header is obtained by the getDLLs() method, which is also responsible for
     *  not returning any shared object that we've already parsed.
     *
     *  Throws a BinaryLoader::Exception if any error occurs. */
    virtual void link(SgAsmInterpretation *interp);

    /** Maps sections of the interpretation into the virtual address space.
     *
     *  This function uses the existing MemoryMap attached to the interpretation if it is available, or creates and attaches a
     *  new, empty MemoryMap.  When using an existing MemoryMap, the map is not cleared. This enables the caller to reserve
     *  space in memory.
     *
     *  The mapping algorithms consult each section's preferred address (SgAsmGenericSection::get_mapped_preferred_rva()) and
     *  update the section's actual address (SgAsmGenericSection::get_mapped_actual_rva()), adjusting the interpretation's
     *  MemoryMap in the process.  The algorithms are applied to one binary file header at a time in the order the headers
     *  appear in the interpretation.  The algorithms may control the order that sections are mapped within any given header.
     *
     *  Subclasses normally don't override this method, but rather the remap() method that is called by this method, namely
     *  the remap() that takes a memory map and file header as arguments.
     *
     *  This method throws a BinaryLoader::Exception if an error occurs. */
    virtual void remap(SgAsmInterpretation *interp);

    typedef std::vector<Exception> FixupErrors;

    /** Performs relocation fixups on the specified interpretation.
     *
     *  This should be called after sections are mapped into memory by remap().  If an error occurs, then this function either
     *  throws the error (BinaryLoader::Exception) or appends it to the @p errors container (if @p errors is non-null). */
    virtual void fixup(SgAsmInterpretation *interp, FixupErrors *errors=NULL);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting types and functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns true if the specified file name is already linked into the AST.
     *
     * @{ */
    virtual bool isLinked(SgBinaryComposite *composite, const std::string &filename);
    virtual bool isLinked(SgAsmInterpretation *interp, const std::string &filename);
    /** @} */

    /** Parses a single binary file.
     *
     *  The file may be an executable, core dump, or shared library.  The machine instructions in the file are not parsed--only
     *  the binary container is parsed.  The new SgAsmGenericFile is added to the supplied binary @p composite and a new
     *  interpretation is created if necessary.  Dwarf debugging information is also parsed and added to the AST if Dwarf
     *  support is enable and the information is present in the binary container. */
    static SgAsmGenericFile *createAsmAST(SgBinaryComposite *composite, std::string filePath);

    /** Finds shared object dependencies of a single binary header.
     *
     *  Returns a list of dependencies, which are usually library names rather than actual files.  The library names can be
     *  turned into file names by calling find_so_file().  Only one header is inspected (i.e., this function is not recursive)
     *  and no attempt is made to remove names from the return value that have already been parsed into the AST. */
    virtual std::vector<std::string> dependencies(SgAsmGenericHeader*);

    /** Remaps the sections for a particular header.
     *
     *  This method is often replaced by subclasses since this is where decisions are made about alignment.  Different
     *  operating systems and their loaders have different alignment policies. */
    virtual void remap(Rose::BinaryAnalysis::MemoryMap::Ptr&/*in,out*/, SgAsmGenericHeader*);

    /** Selects those sections of a header that should be mapped.
     *
     *  Returns the sections in the order they should be mapped. */
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader *header) {
        return header->get_mapped_sections();
    }

    /** Returns an alternate base virtual address if necessary for remapping.
     *
     *  For instance, when mapping multiple ELF files, each file should be mapped above a particular address that doesn't
     *  conflict with anything that's already mapped.  If a new base address is returned, then the remapper will temporarily
     *  adjust the base address of the file header for the duration of the mapping operation.  The default is to return the
     *  current base address.
     *
     *  This method is called with a memory map that describes what has been mapped so far, a file header for the sections
     *  that are about to be mapped, and a list of sections about to be mapped. */
    virtual rose_addr_t rebase(const MemoryMap::Ptr&/*in,out*/, SgAsmGenericHeader *header, const SgAsmGenericSectionPtrList&) {
        return header->get_base_va();
    }

    /** Extended Euclid Algorithm.
     *
     *  The Euclid algorithm for finding the greatest common divisor (GCD) of two natural numbers, extended to also compute a
     *  solution to Bezout's Identity.  Bezout's Identity states that if @p a and @p b are two non-zero integers with greatest
     *  common divisor @p d, then there exist integers @p x and @p y (called Bezout Coefficients), such that \f$ a x + b y = d
     *  \f$. Additionally @p d is the smallest positive integer for which there are integer solutions to the preceding
     *  equation.
     *
     *  The returned value is the greatest common divisor of @p a and @p b.  If the caller supplies non-null pointers for @p x
     *  and @p y then a pair of Bezout Coefficients are returned through those pointers. */
    static int64_t gcd(int64_t a, int64_t b, int64_t *x=NULL/*out*/, int64_t *y=NULL/*out*/);

    /** Calculate adjustment to cause two values to be aligned to two different alignments.
     *
     *  Given two values and two alignments, return the smallest integer adjustment such that subtracting the adjustment from
     *  both values results in each value satisfying its alignment constraint.
     *
     *  Note that it is not always possible to find an adjustment. When no adjustment can be found an Exception is thrown. */
    rose_addr_t bialign(rose_addr_t val1, rose_addr_t align1,
                        rose_addr_t val2, rose_addr_t align2);

    /** For a given section, return information about how the section contributes to the memory map.
     *
     *  The algorithm may use information from the section itself and the memory map as defined up to this point in the
     *  mapping process.  The main return value is an indication of whether the section adds to the map, subtracts from the
     *  map, or does nothing to the map.  The remaining arguments are grouped according to their purpose:
     *
     *  <ul>
     *    <li>Memory alignment. The @p malign_lo and @p malign_hi indicate how the section must be aligned in memory.
     *        In particular, the @p va return value will be a multiple of @p malign_lo, and the sum of @p va and
     *        @p mem_size will be a multiple of @p malign_hi.</li>
     *    <li>Memory location: The @p va value is the low virtual address of the new mapping, and the @p mem_size is
     *        the number of bytes to map.</li>
     *    <li>File location: The @p offset value is the byte offset from the beginning of the file in which this
     *        section appears, and the @p file_size is the number of bytes to map from the file into memory. The
     *        @p file_size may be smaller than the @p mem_size (see below).</li>
     *    <li>Internal offset: The size of the memory mapping may be larger than the section itself. When this
     *        happens, the section will be located somewhere inside the mapped region.  The @p va_offset
     *        is the offset of the section with respect to @p va (it must be non-negative).  The boolean return
     *        values @p anon_lo and @p anon_hi determine whether ROSE will map file contents (false) or zeros (true)
     *        below and above the section when the section is smaller than the mapped region.  Zeros are always
     *        used for areas that are beyond the end of the file.</li>
     *    <li>Allocation: If map_private is true upon return, then the memory map will point to a copy of the file contents
     *        rather than directly into the file.  This simulates "private" mappings.  The default (if align_values() doesn't
     *        change it, is false.</li>
     *    <li>Conflicts: If an attempt is made to map a region into part of the address space that already has
     *        a mapping, then one of three things can happen: (1) a MemoryMap::Inconsistent exception is thrown,
     *        (2) a hole can be created by first unmapping the space in conflict, or (3) a new mapping can be chosen
     *        automatically.  The @p resolve result indicates what should happen.</li>
     *  </ul>
     *
     *  Subclasses often override this method since each operating system tends to have unique rules about how
     *  things are mapped and how conflicts are resolved.
     *
     *  If so desired, the conflict resolution may be handled by the align_values() method itself since the memory map is
     *  available there. However, a benefit of allowing a higher layer to resolve the conflict is that diagnostics will
     *  be produced automatically when debugging is enabled.
     *
     *  Likewise, this method is allowed to perform the mapping itself if the algorithm in the caller, remap(), cannot
     *  be sufficiently influenced by the values that are returned.  In this case, this method should perform the mapping
     *  and return CONTRIB_NONE to prevent the caller from doing any further mapping. */
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                            rose_addr_t *va, rose_addr_t *mem_size,
                                            rose_addr_t *offset, rose_addr_t *file_size, bool *map_private,
                                            rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve);

    /** Selects loadable sections.
     *
     *  Selects those sections which should be layed out by the Loader and inserts them into the @p allSections argument.  The
     *  default implementation (in this base class) is to add all sections to the list. Subclasses will likely restrict this to
     *  a subset of sections. */
    virtual void addSectionsForRemap(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections);

    /** Find headers similar to given header.
     *
     *  Find all headers in @p candidateHeaders that are similar to @p matchHeader.
     *
     *  This is used to determine whether two headers can be placed in the same SgAsmInterpretation. We make this determination
     *  by looking at whether the Disassembler for each header is the same.  In other words, an x86_64 header will not be
     *  similar to an i386 header even though they are both ELF headers and both x86 architectures. */
    static SgAsmGenericHeaderPtrList findSimilarHeaders(SgAsmGenericHeader *matchHeader,
                                                        SgAsmGenericHeaderPtrList &candidateHeaders);

    /** Determines whether two headers are similar.
     *
     *  Determines whether two headers are similar enough to be in the same interpretation.  Two headers are similar if
     *  disassembly would use the same Disassembler for both.  See findSimilarHeaders(). */
    static bool isHeaderSimilar(SgAsmGenericHeader*, SgAsmGenericHeader*);

    /** MemoryMap permissions.
     *
     *  Returns the permissions that should be used when mapping the specified section into memory. The default implementation
     *  in the base class uses the access permissions stored in the section. Subclasses can override this method to provide
     *  other permissions without resorting to changing the AST (which would be misleading for other analysis). */
    virtual unsigned mappingPermissions(SgAsmGenericSection*) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();                                        // Further initializations in a *.C file.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated members.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:    
    // [Robb Matzke 2019-01-30]: deprecated
    static void register_subclass(const Ptr &loader) ROSE_DEPRECATED("use registerSubclass") {
        return registerSubclass(loader);
    }
    void set_perform_dynamic_linking(bool b) ROSE_DEPRECATED("use performingDynamicLinking") {
        performingDynamicLinking(b);
    }
    bool get_perform_dynamic_linking() const ROSE_DEPRECATED("use performingDynamicLinking") {
        return performingDynamicLinking();
    }
    void set_perform_remap(bool b) ROSE_DEPRECATED("use performingRemap") {
        performingRemap(b);
    }
    bool get_perform_remap() const ROSE_DEPRECATED("use performingRemap") {
        return performingRemap();
    }
    void set_perform_relocations(bool b) ROSE_DEPRECATED("use performingRelocations") {
        performingRelocations(b);
    }
    bool get_perform_relocations() const ROSE_DEPRECATED("use performingRelocations") {
        return performingRelocations();
    }
    void add_preload(const std::string &libname) ROSE_DEPRECATED("use preloads") {
        preloads().push_back(libname);
    }
    const std::vector<std::string>& get_preloads() const ROSE_DEPRECATED("use preloads") {
        return preloads();
    }
    void add_directory(const std::string &dirname) ROSE_DEPRECATED("use directories") {
        directories().push_back(dirname);
    }
    void add_directories(const std::vector<std::string> &dirnames) ROSE_DEPRECATED("use appendDirectories") {
        appendDirectories(dirnames);
    }
    const std::vector<std::string>& get_directories() const ROSE_DEPRECATED("use directories") {
        return directories();
    }
};

} // namespace
} // namespace

#endif
