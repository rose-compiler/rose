#ifndef ROSE_BinaryAnalysis_InstructionSemantics_TraceSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_TraceSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/Diagnostics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** A semantics domain wrapper that prints and checks all RISC operators as they occur.
 *
 *  This semantics domain provides only a RiscOperators class, which chains most methods to a subdomain specified either
 *  with its constructor or via set_subdomain().  In order to add tracing to any domain, simply wrap that domain's
 *  RiscOperators object inside a TraceSemantics' RiscOperators:
 *
 * @code
 *  BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(....);
 *  ops = TraceSemantics::RiscOperators::instance(ops); // this turns on tracing
 * @endcode
 *
 *  When an instruction is processed, it will emit traces on standard output (by default; see set_stream()).  The messages
 *  look something like this (the exact format depends on the subdomain being traced):
 *
 * @code
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: startInstruction(mov    al, BYTE PTR ss:[ebp + 0x10])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(32, 3) = 3[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(32, 134512800) = 0x080480a0[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: add(0x080480a0[32], 3[32]) = 0x080480a3[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: writeRegister(eip, 0x080480a3[32])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: readRegister(ebp) = v3284[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(8, 16) = 16[8]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: signExtend(16[8], 32) = 16[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: add(v3284[32], 16[32]) = (add[32] v3284[32] 16[32])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: boolean_(1) = 1[1]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: readMemory(ss, (add[32] v3284[32] 16[32]), v3286[8], 1[1]) = v3285[8]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: writeRegister(al, v3285[8])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: finishInstruction(mov    al, BYTE PTR ss:[ebp + 0x10])
 * @endcode
 *
 *  The TraceSemantics also checks for problems with operand and return value widths and reports them in the output
 *  also. Tracing can be turned off either by specifying a NULL file pointer for set_stream(), or by unwrapping the subdomain's
 *  RISC operators, something along these lines:
 *
 * @code
 *  ops = TraceSemantics::RiscOperators::promote(ops)->get_subdomain();
 *  dispatcher->set_operators(ops);
 * @endcode
 */
namespace TraceSemantics {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void SValue;

/** Shared-ownership pointer to trace-semantics values. */
typedef boost::shared_ptr<void> SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void RegisterState;

/** Shared-ownership pointer to trace-semantics register state. */
typedef boost::shared_ptr<void> RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void MemoryState;

/** Shared-ownership pointer to trace-semantics memory state. */
typedef boost::shared_ptr<void> MemoryStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to trace-semantics RISC operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Wraps RISC operators so they can be traced. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

private:
    BaseSemantics::RiscOperatorsPtr subdomain_;         // Domain to which all our RISC operators chain
    Sawyer::Message::Stream stream_;                    // stream to which output is emitted
    std::string indentation_;                           // string to print at start of each line
    bool showingSubdomain_ = true;                      // show subdomain name and address on each line of output?
    bool showingInstructionVa_ = true;                  // show instruction VA on each line of output?
    bool onlyInstructions_ = true;                      // trace only operations associated with an instruction.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors.
protected:
    // use the version that takes a subdomain instead of this c'tor
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver), stream_(mlog[Diagnostics::INFO]) {
        name("Trace");
    }

    // use the version that takes a subdomain instead of this c'tor.
    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(state, solver), stream_(mlog[Diagnostics::INFO]) {
        name("Trace");
    }

    explicit RiscOperators(const BaseSemantics::RiscOperatorsPtr &subdomain)
        : BaseSemantics::RiscOperators(subdomain->currentState(), subdomain->solver()),
          subdomain_(subdomain), stream_(mlog[Diagnostics::INFO]) {
        name("Trace");
    }

public:
    virtual ~RiscOperators() {
        linePrefix();
        stream_ <<"operators destroyed\n";
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.
public:
    /** Instantiates a new RiscOperators object.  This domain does not create any of its own values--it only wraps another
     *  domains RISC operators. Therefore, the supplied protoval and solver are not actually used.  It is probably better to
     *  construct the TraceSemantics' RISC operators with the constructor that takes the subdomain's RISC operators. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object.  This domain does not manage any state--it only wraps another domains RISC
     *  operators. Therefore, the supplied protoval and solver are not actually used.  It is probably better to construct the
     *  TraceSemantics' RISC operators with the constructor that takes the subdomain's RISC operators. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }
    
    /** Instantiate a new RiscOperators object. The @p subdomain argument should be the RISC operators that we want to
     * trace. */
    static RiscOperatorsPtr instance(const BaseSemantics::RiscOperatorsPtr &subdomain) {
        ASSERT_not_null(subdomain);
        RiscOperatorsPtr self = subdomain->currentState()!=NULL ?
                                RiscOperatorsPtr(new RiscOperators(subdomain->currentState(), subdomain->solver())) :
                                RiscOperatorsPtr(new RiscOperators(subdomain->protoval(), subdomain->solver()));
        self->subdomain_ = subdomain;
        return self;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(state, solver);
    }

    /** Wraps a subdomain's RISC operators to add tracing. */
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::RiscOperatorsPtr &subdomain) {
        return instance(subdomain);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to trace operators. This is a checked conversion--it
     *  will fail if @p from does not point to a TraceSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = as<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Property: Subdomain to which operations are forwarded.
     *
     * @{ */
    const BaseSemantics::RiscOperatorsPtr& subdomain() const { return subdomain_; }
    void subdomain(const BaseSemantics::RiscOperatorsPtr &subdomain) { subdomain_ = subdomain; }
    /** @} */

    /** Check that we have a valid subdomain.  If the subdomain isn't value (hasn't been set) then throw an exception.
     *
     *  @{ */
    void checkSubdomain() const {
        if (subdomain_==NULL)
            throw BaseSemantics::Exception("subdomain is not set; nothing to trace", NULL);
    }
    /** @} */

    /** Property: output stream to which tracing is emitted.  The default is the INFO stream of the
     * Rose::BinaryAnalysis::InstructionSemantics message facility. Output will only show up when this stream is enabled.
     *
     * @{ */
    Sawyer::Message::Stream& stream() { return stream_; }
    void stream(Sawyer::Message::Stream &s) { stream_ = s; }
    /** @} */

    /** Property: Line prefix string.
     *
     *  This string will be printed at the start of each line of output. It's usually used for indentation.
     *
     * @{ */
    const std::string& indentation() const { return indentation_; }
    void indentation(const std::string &s) { indentation_ = s; }
    /** @} */

    /** Property: Show subdomain name in output.
     *
     *  If true, then the subdomain name and object address is printed for each line of output.
     *
     * @{ */
    bool showingSubdomain() const { return showingSubdomain_; }
    void showingSubdomain(bool b) { showingSubdomain_ = b; }
    /** @} */

    /** Property: Show instruction in output.
     *
     *  If true, then each line of output will contain the instruction virtual address.
     *
     * @{ */
    bool showingInstructionVa() const { return showingInstructionVa_; }
    void showingInstructionVa(bool b) { showingInstructionVa_ = b; }
    /** @} */

    /** Property: Show only operations for instructions.
     *
     *  Operators are usually called to interpret an instruction, but they can also be called directly by analysis, such as
     *  when an analysis needs to read a register from a semantic state. If the @ref onlyInstructions property is set, then
     *  the output will show only operations that are associated with an instruction and will suppress operations that are
     *  invoked without a current instruction.
     *
     * @{ */
    bool onlyInstructions() const { return onlyInstructions_; }
    void onlyInstructions(bool b) { onlyInstructions_ = b; }
    /** @} */

protected:
    void linePrefix();
    std::string toString(const BaseSemantics::SValuePtr&);
    std::string toString(SgAsmFloatType*);
    void check_equal_widths(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr &check_width(const BaseSemantics::SValuePtr &a, size_t nbits,
                                                const std::string &what="result");
    std::string register_name(RegisterDescriptor);

    bool shouldPrint() const;
    bool shouldPrint(SgAsmInstruction*) const;

    void before(const std::string&);
    void before(const std::string&, RegisterDescriptor);
    void before(const std::string&, RegisterDescriptor, const BaseSemantics::SValuePtr&);
    void before(const std::string&, RegisterDescriptor, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, RegisterDescriptor, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                size_t);
    void before(const std::string&, RegisterDescriptor, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                const BaseSemantics::SValuePtr&);
    void before(const std::string&, SgAsmInstruction*, bool showAddress);
    void before(const std::string&, size_t);
    void before(const std::string&, size_t, uint64_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, size_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&, size_t, size_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, SgAsmFloatType*);
    void before(const std::string&, const BaseSemantics::SValuePtr&, SgAsmFloatType*, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, SgAsmFloatType*, SgAsmFloatType*);
    void before(const std::string&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&, SgAsmFloatType*);

    void after();
    void after(SgAsmInstruction*);
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&, IteStatus);
    void after(const BaseSemantics::Exception&);
    void after(const BaseSemantics::Exception&, SgAsmInstruction*);
    void after_exception();
    void after_exception(SgAsmInstruction*);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we override from our super class
public:
    virtual BaseSemantics::SValuePtr protoval() const override;
    virtual void solver(const SmtSolverPtr&) override;
    virtual SmtSolverPtr solver() const override;
    virtual BaseSemantics::StatePtr currentState() const override;
    virtual void currentState(const BaseSemantics::StatePtr&) override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;
    virtual size_t nInsns() const override;
    virtual void nInsns(size_t n) override;
    virtual SgAsmInstruction* currentInstruction() const override;
    virtual void currentInstruction(SgAsmInstruction*) override;
    virtual bool isNoopRead() const override;
    virtual void isNoopRead(bool) override;
    virtual void startInstruction(SgAsmInstruction*) override;
    virtual void finishInstruction(SgAsmInstruction*) override;
    virtual void comment(const std::string&) override;

    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) override;
    virtual BaseSemantics::SValuePtr boolean_(bool value) override;
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) override;

    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr&) override;
    virtual void hlt() override;
    virtual void cpuid() override;
    virtual BaseSemantics::SValuePtr rdtsc() override;

    // The actual RISC operators. These are pure virtual in the base class
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr&, size_t begin_bit, size_t end_bit) override;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr&,
                                                          const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr iteWithStatus(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                   const BaseSemantics::SValuePtr&, IteStatus&) override;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr&, size_t nbits) override;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr&, size_t nbits) override;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&,
                                                    BaseSemantics::SValuePtr&/*out*/) override;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr&,
                                                      const BaseSemantics::SValuePtr&) override;

    virtual void interrupt(int majr, int minr) override;

    virtual BaseSemantics::SValuePtr fpFromInteger(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpToInteger(const BaseSemantics::SValuePtr&, SgAsmFloatType*,
                                                 const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr fpConvert(const BaseSemantics::SValuePtr&, SgAsmFloatType*, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpIsNan(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpIsDenormalized(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpIsZero(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpIsInfinity(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpSign(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpEffectiveExponent(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpAdd(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                           SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpSubtract(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpMultiply(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                              SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpSquareRoot(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpRoundTowardZero(const BaseSemantics::SValuePtr&, SgAsmFloatType*) override;
    
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr&) override;
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) override;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
