#ifndef ROSE_DISASSEMBLER_H
#define ROSE_DISASSEMBLER_H

/** Virtual base class for instruction disassemblers.
 *
 *  The Disassembler class is a virtual class providing all non-architecture-specific functionality for the recursive
 *  disassembly of instructions; architecture-specific components are in subclasses such as DisassemblerArm,
 *  DisassemblerPowerpc, and DisassemblerX86. In general, there is no need to explicitly instantiate or call functions in any
 *  of these subclasses.
 *
 *  All details of the instructions, and the operands and operator expression trees, etc. are stored in the binary AST as
 *  separate IR nodes.  The SgAsmInstruction class and its architecture-specific subclasses represent individual instructions.
 *  The arguments for those instructions are represented by the SgAsmExpression class and subclasses thereof.
 *
 *  Disassembly normally happens automatically unless the -rose:read_executable_file_format_only switch is specified.
 *  Alternatively, this Disassembler class can be used to explicitly disassemble parts of a file. The Disassembler class
 *  handles all non-architecture-specific details of disassembly, such as where to search for instructions in the address
 *  space and how instructions are concatenated into basic blocks.  The Disassembler has a pure virtual method,
 *  disassembleOne(), that is implemented by architecture-specific subclasses and whose purpose is to disassemble one
 *  instruction.
 *
 *  New architectures can be added to ROSE without modifying any ROSE source code. One does this by subclassing Disassembler
 *  and providing an implementation for the virtual can_disassemble() method.  An instance of the new class is registered with
 *  ROSE by calling the Disassembler's register_subclass() class method. When ROSE needs to disassemble something, it calls the
 *  can_disassemble() methods for all known disassemblers, and the first one that returns a new disassembler object will be
 *  used for the disassembly.
 *
 *  If an error occurs during the disassembly of a single instruction, the disassembler will throw an exception. When
 *  disassembling multiple instructions the exceptions are saved in a map, by virtual address, and the map is returned to the
 *  caller along with the instructions that were successfully disassembled.
 *
 *  The main interface to the disassembler is the disassembleBuffer() method. It searches for instructions based on the
 *  heuristics specified in the set_search() method, reading instruction bytes from a supplied buffer.  An RvaFileMap object is
 *  supplied in order to specify a mapping from virtual address space to offsets in the supplied buffer. The
 *  disassembleBuffer() method is used by methods that disassemble whole sections, whole interpretations, or whole files; in
 *  turn, it calls disassembleBlock() which disassembles sequential instructions until a control flow branch is encountered.
 *
 *  An RvaFileMap object can be built that describes the entire virtual address space and how it relates to offsets in the
 *  executable file.  This object, together with the entire contents of the file, can be passed to the disassembleBuffer()
 *  method in order to disassemble the entire executable in one call.  However, if the executable contains multiple
 *  independent interpretations (like a PE file that contains a Windows executable and a DOS executable) then the best
 *  practice is to disassemble each interpretation individually.  The disassemble() method is convenient for this.
 *
 *  While the main purpose of the Disassembler class is to disassemble instructions, it also needs to be able to group those
 *  instructions into basic blocks (SgAsmBlock) and functions (SgAsmFunctionDeclaration). It uses an instance of the
 *  Partitioner class to do so.  The user can supply a partitioner to the disassembler or have the disassembler create a
 *  default partitioner on the fly.  The user is also free to call the partitioner directly on the InstructionMap object
 *  returned by most of the disassembler methods. */
class Disassembler {
public:
    /** Exception thrown by the disassemblers. */
    class Exception {
    public:
        /** A bare exception not bound to any particular instruction. */
        Exception(const std::string &reason)
            : mesg(reason), ip(0), bit(0)
            {}
        /** An exception bound to a particular instruction. */
        Exception(const std::string &reason, rose_addr_t ip, const SgUnsignedCharList &raw_data, size_t bit)
            : mesg(reason), ip(ip), bytes(raw_data), bit(bit)
            {}

        std::string mesg;               /* Reason that disassembly failed. */
        rose_addr_t ip;                 /* Virtual address where failure occurred. */
        SgUnsignedCharList bytes;       /* Bytes (partial) of failed instruction, including byte at failure. */
        size_t bit;                     /* Index of bit in instruction byte sequence where disassembly failed. */
    };

    /** Heuristics used to find instructions to disassemble. The set of heuristics to try can be set by calling the
     *  set_search() method prior to disassembling. Unless noted otherwise, the bit flags affect the disassembleBuffer methods,
     *  which call the basic block and individual instruction disassembler methods, but which are called by the methods that
     *  disassemble sections, headers, and entire files. */
    enum SearchHeuristic {
        SEARCH_FOLLOWING = 0x0001,      /**< Disassemble at the address that follows each disassembled instruction, regardless
                                         *   of whether the following address is a successor. */
        SEARCH_IMMEDIATE = 0x0002,      /**< Disassemble at the immediate operands of other instructions.  This is a
                                         *   rudimentary form of constant propagation in order to better detect branch targets
                                         *   on RISC architectures where the target is first loaded into a register and then
                                         *   the branch is based on that register. */
        SEARCH_WORDS     = 0x0004,      /**< Like IMMEDIATE, but look at all word-aligned words in the disassembly regions.
                                         *   The word size, alignment, and byte order can be set explicitly; defaults are
                                         *   based on values contained in the executable file header supplied during
                                         *   construction. */
        SEARCH_ALLBYTES  = 0x0008,      /**< Disassemble starting at every possible address, even if that address is inside
                                         *   some other instruction. */
        SEARCH_UNUSED    = 0x0010,      /**< Disassemble starting at every address not already part of an instruction. The way
                                         *   this works is that the disassembly progresses as usual, and then this heuristic
                                         *   kicks in at the end in order to find addresses that haven't been disassembled.
                                         *   The search progresses from low to high addresses and is influenced by most of the
                                         *   other search heuristics as it progresses.  For instance, if SEARCH_DEADEND is not
                                         *   set then disassembly at address V is considered a failure if the basic block at
                                         *   V leads to an invalid instruction. */
        SEARCH_NONEXE    = 0x0020,      /**< Disassemble in sections that are not mapped executable. This heuristic only
                                         *   applies to disassembly methods that choose which sections to disassemble (for
                                         *   instance, disassembleInterp()).  The normal behavior is to disassemble sections
                                         *   that are known to contain code or are mapped with execute permission; specifying
                                         *   this heuristic also includes sections that are mapped but don't have execute
                                         *   permission.  It is not possible to disassemble non-mapped sections automatically
                                         *   since they don't have virtual addresses, but it is possible to supply your own
                                         *   virtual address mapping to one of the lower-level disassembler methods in order
                                         *   to accomplish this. */
        SEARCH_DEADEND   = 0x0040,      /**< Include basic blocks that end with a bad instruction. This is used by the methods
                                         *   that disassemble basic blocks (namely, any disassembly method other than
                                         *   disassembleOne()).  Normally, if disassembly of a basic block at virtual address
                                         *   V leads to a bad instruction, the entire basic block is discarded and address V
                                         *   is added to the bad address list (address V+1 is still available for the other
                                         *   heuristics). */
        SEARCH_UNKNOWN   = 0x0080,      /**< Rather than generating exceptions when an instruction cannot be disassembled,
                                         *   create a pseudo-instruction that occupies one byte. These will generally have
                                         *   opcodes like x86_unknown_instruction. */
        SEARCH_DEFAULT   = 0x0001       /**< Default set of heuristics to use. The default is to be moderately timid. */
    };

    typedef std::set<rose_addr_t> AddressSet;
    typedef std::map<rose_addr_t, SgAsmInstruction*> InstructionMap;
    typedef std::map<rose_addr_t, Exception> BadMap;

    Disassembler()
        : p_partitioner(NULL), p_search(SEARCH_DEFAULT), p_debug(NULL),
        p_wordsize(4), p_sex(SgAsmExecutableFileFormat::ORDER_LSB), p_alignment(4), p_ndisassembled(0)
        {ctor();}
    virtual ~Disassembler() {}




    /*==========================================================================================================================
     * Factory methods
     *========================================================================================================================== */
public:
    /** Register a disassembler with the factory.  The disassembler should provide a can_disassemble() method (virtual in
     *  Disassembler) which will return a new instance if the class has the capability to disassemble the specified header. The
     *  create() method will try subclasses in the opposite order they are registered (so register the most specific classes
     *  last). */
    static void register_subclass(Disassembler*);

    /*NOTE: Keep this pure virtual. We cannot have Disassembler serving as a real disassembler--only subclasses can actually
     *      disassemble anything. Keeping at least one pure virtual method will prevent anyone from even trying to do that! */
    /** Method provided by subclasses and used by create().  It should return a new instance of the subclass if the subclass
     *  has the capability to disassemble the specified header. */
    virtual Disassembler *can_disassemble(SgAsmGenericHeader*) const = 0;

    /** Factory method to create a disassembler based on the architecture represented by the file header. */
    static Disassembler *create(SgAsmGenericHeader*);

    /** Factory method to create a disassembler based on the architecture represented by the interpretation. */
    static Disassembler *create(SgAsmInterpretation*);




    /*==========================================================================================================================
     * Main public disassembly methods
     *========================================================================================================================== */
public:
    /** This high-level method disassembles instructions belonging to part of a file described by an executable file header as
     *  indicated by the specified interpretation.  The instructions are partitioned into a block (SgAsmBlock) of functions
     *  (SgAsmFunctionDeclaration) containing basic blocks (SgAsmBlock) of instructions (SgAsmInstruction). The top-level
     *  block is then added to the interpretation.  The heuristics used to partition instructions into functions, and the
     *  aggressiveness of the disassembler in finding instructions can be controlled by setting properties of this Disassembler
     *  object.
     *
     *  Addresses containing instructions that could not be disassembled are added to the optional @p bad map.  Successor
     *  addresses where no disassembly was attempted are added to the optional successors set.
     *
     *  In essence, this method replaces the old disassembleInterpreation function in the old Disassembler name space, and
     *  will probably be the method most often called by other parts of ROSE.  All of its functionality is based on the other
     *  lower-level methods of this class. */
    void disassemble(SgAsmInterpretation*, AddressSet *successors=NULL, BadMap *bad=NULL);

    /** This class method is for backward compatibility with the disassembleInterpretation() function in the old Disassembler
     *  namespace. It just creates a default Disassembler object and invokes its disassemble method. */
    static void disassembleInterpretation(SgAsmInterpretation*);

    /** This class method is for backward compatibility with the disassembleFile() function in the old Disassembler namespace.
     *  It calls disassembleInterpretation for each interpretation in the file. */
    static void disassembleFile(SgAsmFile*);




    /*==========================================================================================================================
     * Disassembler properties and settings
     *========================================================================================================================== */
public:
    /** Specifies the instruction partitioner to use when partitioning instructions into functions.  If none is specified then
     *  a default partitioner will be constructed when necessary. */
    void set_partitioner(class Partitioner *p) {
        p_partitioner = p;
    }

    /** Returns the partitioner object set by set_partitioner(). */
    class Partitioner *get_partitioner() const {
        return p_partitioner;
    }

    /** Specifies the heuristics used when searching for instructions. The @p bits argument should be a bit mask of
     *  SearchHeuristic bits. */
    void set_search(unsigned bits) {
        p_search = bits;
    }

    /** Returns a bit mask of SearchHeuristic bits representing which heuristics would be used when searching for
     *  instructions. */
    unsigned get_search() const {
        return p_search;
    }

    /** Specifies the word size for the SEARCH_WORDS heuristic. The default is based on the word size of the file header used
     *  when the disassembler is constructed. */
    void set_wordsize(size_t);

    /** Returns the word size used by the SEARCH_WORDS heuristic. */
    size_t get_wordsize() const {
        return p_wordsize;
    }

    /** Specifies the alignment for the SEARCH_WORDS heuristic. The value must be a natural, postive power of two. The default
     *  is determined by the subclass (e.g., x86 would probably set this to one since instructions are not aligned, while ARM
     *  would set this to four. */
    void set_alignment(size_t);

    /** Returns the alignment used by the SEARCH_WORDS heuristic. */
    size_t get_alignment() const {
        return p_alignment;
    }

    /** Specifies the byte order for the SEARCH_WORDS heuristic. The default is based on the byte order of the file header
     *  used when the disassembler is constructed. */
    void set_sex(SgAsmExecutableFileFormat::ByteOrder sex) {
        p_sex = sex;
    }

    /** Returns the byte order used by the SEARCH_WORDS heuristic. */
    SgAsmExecutableFileFormat::ByteOrder get_sex() const {
        return p_sex;
    }

    /** Sends disassembler diagnostics to the specified output stream. Null (the default) turns off debugging. */
    void set_debug(FILE *f) {
        p_debug = f;
    }

    /** Returns the file currently used for debugging; null implies no debugging. */
    FILE *get_debug() const {
        return p_debug;
    }

    /** Returns the number of instructions successfully disassembled. The counter is updated by disassembleBlock(), which is
     *  generally called by all disassembly methods except for disassembleOne(). */
    size_t get_ndisassembled() const {
        return p_ndisassembled;
    }




    /*==========================================================================================================================
     * Low-level disassembly functions
     *========================================================================================================================== */
public:
    /** This is the lowest level disassembly function and is implemented in the architecture-specific subclasses. It
     *  disassembles one instruction at the specified virtual address. The @p map is a mapping from virtual addresses to
     *  offsets in the content buffer and enables instructions to span file segments that are mapped contiguously in virtual
     *  memory by the loader but which might not be contiguous in the file.  The instruction's successor virtual addresses are
     *  added to the optional successor set (note that successors of an individual instruction can also be obtained via
     *  SgAsmInstruction::get_successors). If the instruction cannot be disassembled then an exception is thrown and the
     *  successors set is not modified. */
    virtual SgAsmInstruction *disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                                             AddressSet *successors=NULL) = 0;

    /** Similar in functionality to the disassembleOne method that takes an RvaFileMap argument, except the content buffer is
     *  mapped 1:1 to virtual memory beginning at the specified address. */
    SgAsmInstruction *disassembleOne(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                     AddressSet *successors=NULL);




    /** Like the disassembleOne method except it disassembles a basic block's worth of instructions beginning at the specified
     *  virtual address.  For the purposes of this function, a basic block is defined as starting from the specified
     *  instruction and continuing until we reach a branch instruction (e.g., "jmp", "je", "call", "ret", etc.), or an
     *  instruction with no known successors (e.g., "hlt", "int", etc), or the end of the buffer, or an instruction that
     *  cannot be disassembled. The @p map is a mapping from virtual addresses to offsets in the content buffer. The
     *  successors of the last instruction of the basic block are added to the optional successor set. An exception is thrown
     *  if the first instruction cannot be disassembled (or some other major error occurs), in which case the successors set
     *  is not modified.
     *
     *  If the SEARCH_DEADEND bit is set and an instruction cannot be disassembled then the address of that instruction is
     *  added to the successors and the basic block ends at the previous instruction.  If the SEARCH_DEADEND bit is clear and
     *  an instruction cannot be disassembled then the entire basic block is discarded, an exception is thrown (the exception
     *  address is the instruction that could not be disassembled), and the successors list is not modified. */
    InstructionMap disassembleBlock(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                                    AddressSet *successors=NULL);

    /** Similar in functionality to the disassembleBlock method that takes an RvaFileMap argument, except the supplied buffer
     *  is mapped 1:1 to virtual memory beginning at the specified address. */
    InstructionMap disassembleBlock(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                    AddressSet *successors=NULL);




    /** Disassembles instructions from the content buffer beginning with at the specified virtual address and including all
     *  instructions that are direct or indirect successors of the first instruction.  The @p map is a mapping from virtual
     *  addresses to offsets in the content buffer.  Any successors of individual instructions that fall outside the buffer
     *  being disassembled will be added to the optional successors set.  If an address cannot be disassembled then the
     *  address and exception will be added to the optional @p bad map; any address which is already in the bad map upon
     *  function entry will not be disassembled. Note that bad instructions have no successors.  An exception is thrown if an
     *  error is detected before disassembly begins. */
    InstructionMap disassembleBuffer(const unsigned char *buf, const RvaFileMap &map, size_t start_va,
                                     AddressSet *successors=NULL, BadMap *bad=NULL);

    /** Similar in functionality to the disassembleBuffer methods that take an RvaFileMap argument, except the supplied buffer
     *  is mapped 1:1 to virtual memory beginning at the specified address. */
    InstructionMap disassembleBuffer(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                     AddressSet *successors=NULL, BadMap *bad=NULL);

    /** Similar in functionality to the disassembleBuffer methods that take a single starting virtual address, except this one
     *  tries to disassemble from all the addresses specified in the workset. */
    InstructionMap disassembleBuffer(const unsigned char *buf, const RvaFileMap &map, AddressSet workset,
                                     AddressSet *successors=NULL, BadMap *bad=NULL);




    /** Disassembles instructions in the specified mapped section beginning at the specified virtual address.  The section
     *  need not be marked as executable or as containing code. All other aspects of this method are similar to the
     *  disassembleBuffer method. */
    InstructionMap disassembleSection(SgAsmGenericSection *section, rose_addr_t start_va,
                                      AddressSet *successors=NULL, BadMap *bad=NULL);

    /** Disassembles instructions in all code-containing sections of a particular executable file header according to the
     *  aggressiveness set for this Disassembler object.  All other aspects of this method are similar to the
     *  disassembleBuffer method. */
    InstructionMap disassembleInterp(SgAsmGenericHeader *header,
                                     AddressSet *successors=NULL, BadMap *bad=NULL);
    InstructionMap disassembleInterp(SgAsmInterpretation *interp,
                                     AddressSet *successors=NULL, BadMap *bad=NULL) {
        return disassembleInterp(interp->get_header(), successors, bad);
    }




    /*==========================================================================================================================
     * Methods for searching for disassembly addresses.
     *========================================================================================================================== */
private:
    /** Adds the address following a basic block to the list of addresses that should be disassembled.  This search method is
     *  invoked automatically if the SEARCH_FOLLOWING bit is set (see set_search()). */
    void search_following(AddressSet *worklist, const InstructionMap &bb, const BadMap *bad);

    /** Adds values of immediate operands to the list of addresses that should be disassembled.  Such operands are often used
     *  in a closely following instruction as a jump target. E.g., "move 0x400600, reg1; ...; jump reg1". This search method
     *  is invoked automatically if the SEARCH_IMMEDIATE bit is set (see set_search()). */
    void search_immediate(AddressSet *worklist, const InstructionMap &bb,  const RvaFileMap &map, const BadMap *bad);

    /** Adds all word-aligned values to work list, provided they specify a virtual address in the @p map.  The @p wordsize
     *  must be a power of two. This search method is invoked automatically if the SEARCH_WORDS bit is set (see set_search()). */
    void search_words(AddressSet *worklist, const unsigned char *buf, const RvaFileMap &map, const BadMap *bad);

    /** Finds the lowest virtual address, greater than or equal to @p start_va, which does not correspond to a previous
     *  disassembly attempt as evidenced by its presence in the supplied instructions or bad map.  If @p avoid_overlaps is set
     *  then do not return an address if an already disassembled instruction's raw bytes include that address.  Only virtual
     *  addresses contained in the RvaFileMap will be considered.  The address is returned by adding it to the worklist;
     *  nothing is added if no qualifying address can be found. This method is invoked automatically if the SEARCH_ALLBYTES or
     *  SEARCH_UNUSED bits are set (see set_search()). */
    void search_next_address(AddressSet *worklist, rose_addr_t start_va, const RvaFileMap &map, const InstructionMap &insns,
                             const BadMap *bad, bool avoid_overlaps);




    /*==========================================================================================================================
     * Miscellaneous methods
     *========================================================================================================================== */
public:
    /** Makes an unknown instruction from an exception. */
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&) = 0;

private:
    /** Initialize class (e.g., register built-in disassemblers). */
    static void initclass();

    /** Called during construction. */
    void ctor();

    /** Finds the highest-address instruction that contains the byte at the specified virtual address. Returns null if no such
     *  instruction exists. */
    static SgAsmInstruction *find_instruction_containing(const InstructionMap &insns, rose_addr_t va);




    /*==========================================================================================================================
     * Data members
     *========================================================================================================================== */
private:
    class Partitioner *p_partitioner;                   /**< Partitioner used for placing instructions into blocks and functions.*/
    unsigned p_search;                                  /**< Mask of SearchHeuristic bits specifying instruction searching. */
    FILE *p_debug;                                      /**< Set to non-null to get debugging info. */
    size_t p_wordsize;                                  /**< Word size used by SEARCH_WORDS. */
    SgAsmExecutableFileFormat::ByteOrder p_sex;         /**< Byte order for SEARCH_WORDS. */
    size_t p_alignment;                                 /**< Word alignment constraint for SEARCH_WORDS (0 and 1 imply byte). */
    static std::vector<Disassembler*> disassemblers;    /**< List of disassembler subclasses. */
    size_t p_ndisassembled;                             /**< Total number of instructions disassembled by disassembleBlock() */
};

#endif
