#ifndef ROSE_ASSEMBLER_H
#define ROSE_ASSEMBLER_H

/** Virtual base class for instruction assemblers.
 *
 *  The Assembler class is a virtual class providing all non-architecture-specific functionality for the assembly of
 *  instructions; architecture-specific components are in subclasses such as AssemblerArm, AssemblerPowerpc, and AssemblerX86.
 */
class Assembler {
public:
    /** Exception thrown by the assemblers. */
    class Exception {
    public:
        /** An exception bound to a particular instruction being assembled. */
        Exception(const std::string &reason, SgAsmInstruction *insn)
            : mesg(reason), insn(insn)
            {}

        std::string mesg;               /**< Reason that disassembly failed. */
        SgAsmInstruction *insn;         /**< Instruction associated with an assembly error. */
    };

    /** Assemblers can often assemble a single instruction various ways. For instance, on x86 an immediate value -64 can be
     *  assembled into a single byte, or sign extended into 2, 4, or 8 bytes.  These enumeration constants control how the
     *  assembleOne() method determines which encoding to return. */
    enum EncodingType {
        ET_SHORTEST,            /**< Returns the shortest possible encoding. */
        ET_LONGEST,             /**< Returns the longest encoding. */
        ET_MATCHES,             /**< Returns an encoding that matches the SgAsmInstruction::p_raw_bytes. This is used mainly
                                 *   for testing that the assembler can produce the same encoding that was originally
                                 *   used by the disassembler when the instruction was created. */
    };

    Assembler()
        : p_debug(NULL), p_encoding_type(ET_SHORTEST)
        {}

    virtual ~Assembler() {}

    /** Creates an assembler that is appropriate for assembling instructions in the specified interpretation. */
    static Assembler *create(SgAsmInterpretation *interp) {
        return create(interp->get_header());
    }

    /** Creates an assembler that is appropriate for assembling instructions in the specified header. */
    static Assembler *create(SgAsmGenericHeader*);

    /*==========================================================================================================================
     * Main public assembly methods
     *========================================================================================================================== */
public:
    /** This is the lowest level architecture-independent assembly function and is implemented in the architecture-specific
     *  subclasses (there may be other architecture-dependent assembly methods also). It assembles one instruction and returns
     *  the encoding, throwing an exception if anything goes wrong. */
    virtual SgUnsignedCharList assembleOne(SgAsmInstruction *insn) = 0;

    /** Assembles a single basic block of instructions, packing them together and adjusting their virtual addresses. The
     *  virtual address of the first instruction of the block determines the starting address.  An exception is thrown
     *  if any of the instructions cannot be assembled. */
    SgUnsignedCharList assembleBlock(SgAsmBlock*);

    /** Assembles a single basic block of instructions like the version that takes an SgAsmBlock pointer. In this case, the
     *  instructions are stored in a vector instead. The will be treated like a single basic block: no control flow
     *  adjustments will be made. An exception is thrown if any of the instructions cannot be disassembled. */
    SgUnsignedCharList assembleBlock(const std::vector<SgAsmInstruction*> &insns, rose_addr_t starting_rva);


    
    /*==========================================================================================================================
     * Assembler properties and settings
     *========================================================================================================================== */
public:
    /** Controls how the assembleOne() method determines which encoding to return. */
    void set_encoding_type(EncodingType et) {
        p_encoding_type = et;
    }

    /** Returns the encoding type employed by this assembler. See set_encoding_type(). */
    EncodingType get_encoding_type() const {
        return p_encoding_type;
    }

    /** Sends assembler diagnostics to the specified output stream. Null (the default) turns off debugging. */
    void set_debug(FILE *f) {
        p_debug = f;
    }

    /** Returns the file currently used for debugging; null implies no debugging. */
    FILE *get_debug() const {
        return p_debug;
    }

    /*==========================================================================================================================
     * Data members
     *========================================================================================================================== */
protected:
    FILE *p_debug;                                      /**< Set to non-null to get debugging info. */
    EncodingType p_encoding_type;                       /**< Which encoding should be returned by assembleOne. */
};

#endif
