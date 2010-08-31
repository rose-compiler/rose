#ifndef ROSE_BINARYLOADERGENERIC_H
#define ROSE_BINARYLOADERGENERIC_H

class BinaryLoaderGeneric
{
public:
    BinaryLoaderGeneric()
        :p_verbose(0), p_perform_dynamic_linking(false), p_perform_relocations(false), p_perform_disassembly(true) {
        set_verbose(SgProject::get_verbose());
    }

    virtual ~BinaryLoaderGeneric(){}


    /*************************************************************************************************************************
     * Accessors for properties.
     *************************************************************************************************************************/
public:
    void set_perform_dynamic_linking(bool b) { p_perform_dynamic_linking = b; }
    bool get_perform_dynamic_linking() const { return p_perform_dynamic_linking; }

    void set_perform_layout(bool b) { p_perform_layout = b; }
    bool get_perform_layout() const { return p_perform_layout; }

    void set_perform_relocations(bool b) { p_perform_relocations = b; }
    bool get_perform_relocations() const { return p_perform_relocations; }

    void set_perform_disassembly(bool b) { p_perform_disassembly = b; }
    bool get_perform_disassembly() const { return p_perform_disassembly; }

    void set_verbose(int verbose) { p_verbose = verbose; }
    int get_verbose(SgBinaryComposite* composite=NULL) const {
        return std::max(composite?composite->get_verbose():0, p_verbose);
    }


    /*************************************************************************************************************************
     * The main part of the interface.
     *************************************************************************************************************************/
public:
    /** Conditionally loads, performs layout, relocates and disassembles the given file.  If @p executable is the null pointer
     *  then it will be created by loading the binary whose name is stored in @p binaryFile. */
    virtual bool load(SgBinaryComposite* binaryFile, SgAsmGenericFile* executable);
  
    /** Performs loading only.  Will populate binaryFile->p_binaryFileList with SgAsmFile pointers.  Returns false if any
     *  library cannot be loaded. */
    virtual bool loadAllLibraries(SgBinaryComposite* binaryFile);

    /** Performs layout on all interpretations by calling layoutInterpLibraries().  Returns false if a valid layout cannot
     *  be performed. */
    virtual bool layoutAllLibraries(SgBinaryComposite* binaryFile);

    virtual bool relocateAllLibraries(SgBinaryComposite* binaryFile);

    /** Run the disassembler across all interpretations one at a time.  The disassembler operates across the entire memory map
     *  of each interpretation by calling Disassembler::disassembleInterpretation(). */
    virtual bool disassembleAllLibraries(SgBinaryComposite* binaryFile);


    /*************************************************************************************************************************
     * Functions for internal use.
     *************************************************************************************************************************/
protected:
    /** Parses all shared objects required by the specified interpretation.  Recursively perform a breadth-first search of all
     *  headers, starting with the headers already in the SgBinaryComposite. For each header, obtain a list of necessary
     *  shared objects (pruning away those that we've already processed) and parse the shared object, adding it to the AST and
     *  adding its appropriate headers to the interpretation.  Parsing of the shared objects is performed by calling
     *  createAsmAST().
     *
     *  This process is recursive in nature. A dynamically linked executable has a list of libraries on which it depends, and
     *  those libraries also often have dependencies.  The recursion is breadth-first because ELF specifies a particular order
     *  that symbols should be resolved. Order is not important for a PE binary since its shared object symbols are scoped to
     *  a library.
     *
     *  The list of dependencies for a particular header is obtained by the getDLLs() method, which is also responsible for
     *  not returning any shared object that we've already parsed.
     *
     *  Returns false if any library cannot be loaded. FIXME: We should instead throw an exception here so that the caller is
     *  able to obtain information about why the loading failed. [RPM 2010-08-31] */
    virtual bool loadInterpLibraries(SgBinaryComposite* binaryFile, SgAsmInterpretation* interp);

    /** Parses a single binary file. The file may be an executable, core dump, or shared library.  The machine instructions in
     *  the file are not parsed--only the binary container is parsed.  The new SgAsmGenericFile is added to the supplied
     *  binary @p composite and a new interpretation is created if necessary.  Dwarf debugging information is also parsed and
     *  added to the AST if Dwarf support is enable and the information is present in the binary container. */
    virtual SgAsmGenericFile *createAsmAST(SgBinaryComposite *composite, std::string filePath);

    /** Performs layout on unmapped sections in all files referenced by the specified interpretation. Returns false if a valid
     *  layout cannot be performed. */
    virtual bool layoutInterpLibraries(SgBinaryComposite* binaryFile, SgAsmInterpretation* interp);

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
    int p_verbose;
    bool p_perform_dynamic_linking;
    bool p_perform_layout;
    bool p_perform_relocations;
    bool p_perform_disassembly;
};

#endif /* ROSE_BINARYLOADERGENERIC_H */
