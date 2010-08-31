#ifndef ROSE_BINARYLOADERGENERIC_H
#define ROSE_BINARYLOADERGENERIC_H

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
          process address space or fixing up relocation information.</li>
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
class BinaryLoader {
    /*========================================================================================================================
     * Exceptions
     *======================================================================================================================== */
public:
    /** Base class for exceptions thrown by loaders. */
    class Exception {
    public:
        Exception(const std::string &reason)
            : mesg(reason)
            {}
        void print(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream&, const Exception&);
        std::string mesg;
    };

    /*========================================================================================================================
     * Constructors, etc.
     *======================================================================================================================== */
public:
    BinaryLoader()
        : p_verbose(0), p_perform_dynamic_linking(false), p_perform_relocations(false) {
        set_verbose(SgProject::get_verbose());
    }

    BinaryLoader(const BinaryLoader &other)
        : p_verbose(other.p_verbose), p_perform_dynamic_linking(other.p_perform_dynamic_linking), 
          p_perform_relocations(other.p_perform_relocations) {
    }

    virtual ~BinaryLoader(){}

private:
    /** Initialize the class. Register built-in loaders. */
    static void initclass();

    /*==========================================================================================================================
     * Registration and lookup methods
     *========================================================================================================================== */
public:
    /** Register a loader instance.  More specific loader instances should be registered after more general loaders since the
     *  lookup() method will inspect loaders in reverse order of their registration. */
    static void register_subclass(BinaryLoader*);

    /** Predicate determining the suitability of a loader for a specific file header.  If this loader is capable of loading
     *  the specified file header, then this predicate returns true, otherwise it returns false.  The implementation in
     *  BinaryLoader always returns true because BinaryLoader is able to generically load all types of files, albeit with
     *  limited functionality. Subclasses should certainly redefine this method so it returns true only for certain headers. */
    virtual bool can_load(SgAsmGenericHeader*) const {
        return true;
    }

    /** Finds a suitable loader. Looks through the list of registered loader instances (from most recently registered to
     *  earliest registered) and returns the first one whose can_load() predicate returns true.  Throws an exception if no
     *  suitable loader can be found. */
    static BinaryLoader *lookup(SgAsmGenericHeader*);
    
    /** Finds a suitable loader. Looks through the list of registered loader instances (from most recently registered to
     *  earliest registered) and returns the first one whose can_load() predicate returns true. This is done for each header
     *  contained in the interpretation and the loader for each header must match the other headers. An exception is thrown if
     *  no suitable loader can be found. */
    static BinaryLoader *lookup(SgAsmInterpretation*);

    /** Creates a new copy of a loader. The new copy has all the same settings as the original. Subclasses that define data
     *  methods should certainly provide an implementation of this method, although all they'll need to change is the data
     *  type for the 'new' operator. */
    virtual BinaryLoader *clone() const {
        return new BinaryLoader(*this);
    }

    /*========================================================================================================================
     * Accessors for properties.
     *======================================================================================================================== */
public:
    void set_perform_dynamic_linking(bool b) { p_perform_dynamic_linking = b; }
    bool get_perform_dynamic_linking() const { return p_perform_dynamic_linking; }

    void set_perform_layout(bool b) { p_perform_layout = b; }
    bool get_perform_layout() const { return p_perform_layout; }

    void set_perform_relocations(bool b) { p_perform_relocations = b; }
    bool get_perform_relocations() const { return p_perform_relocations; }

    /* FIXME: Rename this to get_debug() and set_debug() to match Disassembler and Partitioner. [RPM 2010-08-31] */
    void set_verbose(int verbose) { p_verbose = verbose; }
    int get_verbose(SgBinaryComposite* composite=NULL) const {
        return std::max(composite?composite->get_verbose():0, p_verbose);
    }

    /*========================================================================================================================
     * Searching for shared objects
     *======================================================================================================================== */
public:
    /** Adds a library to the list of pre-loaded libraries. These libraries are linked into the AST in the order they were
     *  added to the preload list. The @p libname should be either the base name of a library, such as "libm.so" (in which
     *  case the search directories are consulted) or a path-qualified name like "/usr/lib/libm.so". */
    void add_preload(const std::string &libname);
    std::vector<std::string>& get_preloads();
    const std::vector<std::string>& get_preloads() const;

    /** Adds a directory to the list of directories searched for libraries.  This is similar to the LD_LIBRARY_PATH
     *  environment variable of the ld-linux.so dynamic loader (see the ld.so man page). ROSE searches for libraries in
     *  directories in the order that directories were added. */
    void add_directory(const std::string &dirname);
    std::vector<std::string>& get_directories();
    const std::vector<std::string>& get_directories() const;

    /** Given the name of a library, return the fully qualified name where the library is located in the file system. Returns
     *  the empty string if the library cannot be found. */
    std::string find_library(const std::string &libname) const;

    /*************************************************************************************************************************
     * The main part of the interface.
     *************************************************************************************************************************/
public:
    /** Class method to parse, map, link, and/or relocate all interpretations of the specified binary composite. This should
     *  only be called for an SgBinaryComposite object that has been created but for which no binary files have been parsed
     *  yet.  It's only called from sageSupport.C by SgBinaryComposite::buildAST().  A BinaryLoader::Exception is thrown if
     *  there's an error of some sort. */
    static void load(SgBinaryComposite* composite);

    /** Conditionally parse, map, link, and/or relocate the interpretation according to properties of this loader. If an error
     *  occurs, a BinaryLoader::Exception will be thrown.  The interpretation must be one that can be loaded by this loader as
     *  returned by this loader's can_load() method. */
    virtual void load(SgAsmInterpretation*);

    /** Links an interpretation by parsing all shared objects required by that interpretation.  In other words, all
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
    virtual void linkDependencies(SgAsmInterpretation *interp);
    
    /** Maps sections of the interpretation into the virtual address space.  Throws a BinaryLoader::Exception if an error
     *  occurs. Ignores any mapping that might already have been defined, recalculating a new mapping from scratch. */
    virtual void remapSections(SgAsmInterpretation *interp);

    /** Performs relocation fixups on the specified interpretation. This should be called after sections are mapped into
     *  memory by remapSections(). Throws a BinaryLoader::Exception if an error occurs. */
    virtual void fixupSections(SgAsmInterpretation *interp);


    /*************************************************************************************************************************
     * Functions for internal use.
     *************************************************************************************************************************/
protected:
    /** Parses a single binary file. The file may be an executable, core dump, or shared library.  The machine instructions in
     *  the file are not parsed--only the binary container is parsed.  The new SgAsmGenericFile is added to the supplied
     *  binary @p composite and a new interpretation is created if necessary.  Dwarf debugging information is also parsed and
     *  added to the AST if Dwarf support is enable and the information is present in the binary container. */
    static SgAsmGenericFile *createAsmAST(SgBinaryComposite *composite, std::string filePath);


    /** Selects those sections which should be layed out by the Loader and inserts them into the @p allSections argument.  The
     *  default implementation (in this base class) is to add all sections to the list. Subclasses will likely restrict this
     *  to a subset of sections. */
    virtual void addSectionsForLayout(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections);

    virtual void handleSectionMapped(SgAsmGenericSection* section){} /* =0 TODO */; 

    /** Helper function to get raw dll list from a file. */
    virtual Rose_STL_Container<std::string> getDLLs(SgAsmGenericHeader* header,
                                                    const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded);

    /** Determine whether two headers are similar enough that they can be located in the same interpretation. */
    static bool isHeaderSimilar(const SgAsmGenericHeader *matchHeader, const SgAsmGenericHeader *candidate);

    /** Find all headers in @p candidateHeaders that are similar to @p matchHeader. */
    static SgAsmGenericHeaderPtrList findSimilarHeaders(const SgAsmGenericHeader *matchHeader,
                                                        const SgAsmGenericHeaderPtrList &candidateHeaders);

    /** Determines the likely word size of a binary.  Some executable formats allow multiple architectures to reside in
     *  the same file (fat binaries) and it is not clear what we should do in that case.  For now we're going to guess at the
     *  "most likely" size by returning the maximum word size across the file headers defined in the file. */
    static size_t wordSizeOfFile(SgAsmGenericFile *file);


    /*************************************************************************************************************************
     * Data members
     *************************************************************************************************************************/
private: 
    static std::vector<BinaryLoader*> loaders;          /**< List of loader subclasses. */

    int p_verbose;
    bool p_perform_dynamic_linking;
    bool p_perform_layout;
    bool p_perform_relocations;
};

#endif /* ROSE_BINARYLOADERGENERIC_H */
