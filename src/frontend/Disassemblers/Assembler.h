#ifndef ROSE_BinaryAnalysis_Assembler_H
#define ROSE_BinaryAnalysis_Assembler_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <RoseException.h>

namespace Rose {
namespace BinaryAnalysis {

/** Virtual base class for instruction assemblers.
 *
 *  The Assembler class is a virtual class providing all non-architecture-specific functionality for the assembly of
 *  instructions; architecture-specific components are in subclasses such as AssemblerArm64, AssemblerPowerpc, and AssemblerX86.
 *
 *  This example shows how to test the disassembler against the assembler by disassembling and then reassembling all
 *  instructions. Generate debugging output for instructions that cannot be reassembled into an encoding identical to the
 *  original bytes:
 *
 *  @code
 *  // Disassemble the interpretation (normally happens automatically)
 *  SgAsmInterpretation *interp = ....;
 *  Disassembler *d = Disassembler::lookup(interp);
 *  d->disassemble(interp);
 *
 *  // Create an assembler that can handle instructions in this interpretation.
 *  Assembler *asm = Assembler::create(interp);
 *
 *  // Cause the assembler to choose encodings that are identical to the
 *  // bytes originally disassebled.  The assembler still goes through all the
 *  // work of assembling, but then checks the result against the original
 *  // bytes.
 *  asm->set_encoding_type(Assembler::ET_MATCHES);
 *
 *  // Attempt to reassemble each instruction. If the assembly fails to produce
 *  // the original bytes then an exception is thrown and we try again for
 *  // debugging side effects.
 *  std::vector<SgNode*> insns = NodeQuery::querySubTree(interp, V_SgAsmInstruction);
 *  for (size_t i=0; i<insns.size(); i++) {
 *      SgAsmInstruction *insn = isSgAsmInstruction(insns[i]);
 *      try {
 *          asm->assembleOne(insn);
 *      } catch(const Assembler::Exception &e) {
 *          fprintf(stderr, "assembly failed at 0x%08llx: %s\n",
 *                  insn->get_address(), e.mesg.c_str());
 *          asm->set_debug(stderr);
 *          try {
 *              asm->assembleOne(insn);
 *          } catch(const Assembler::Exception&) {
 *          }
 *          asm->set_debug(NULL);
 *      }
 *  }
 *  @endcode
 */
class Assembler {
public:
    /** %Exception thrown by the assemblers. */
    class Exception: public Rose::Exception {
    public:
        /** An exception bound to a particular instruction being assembled. */
        Exception(const std::string &reason, SgAsmInstruction *insn)
            : Rose::Exception(reason), insn(insn)
            {}
        /** An exception not bound to a particular instruction. */
        Exception(const std::string &reason)
            : Rose::Exception(reason), insn(NULL)
            {}
        void print(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream&, const Exception&);

        SgAsmInstruction *insn;         /**< Instruction associated with an assembly error. */
    };

    /** Assemblers can often assemble a single instruction various ways. For instance, on x86 the immediate value -53 can be
     *  assembled into a single byte, or sign extended into 2, 4, or 8 bytes.  These enumeration constants control how the
     *  assembleOne() method determines which encoding to return. */
    enum EncodingType
        {
        ET_SHORTEST,            /**< Returns the shortest possible encoding. This is the default. */
        ET_LONGEST,             /**< Returns the longest encoding. */
        ET_MATCHES             /**< Returns an encoding that matches the SgAsmInstruction::p_raw_bytes. This is used mainly
                                 *   for testing that the assembler can produce the same encoding that was originally
                                 *   used by the disassembler when the instruction was created. */
    };

    Assembler()
        : p_debug(NULL), p_encoding_type(ET_SHORTEST)
        {}

    virtual ~Assembler() {}

    /** Creates an assembler that is appropriate for assembling instructions in the specified interpretation. */
    static Assembler *create(SgAsmInterpretation *interp);

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

    /** Assembles a program from an assembly listing.  This method may call an external assembler to do its work. */
    virtual SgUnsignedCharList assembleProgram(const std::string &source) = 0;

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

} // namespace
} // namespace

#endif
#endif
