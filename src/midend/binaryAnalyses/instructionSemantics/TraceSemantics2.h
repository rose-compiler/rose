#ifndef Rose_TraceSemantics2_H
#define Rose_TraceSemantics2_H

#include "BaseSemantics2.h"
#include "Diagnostics.h"

namespace rose {
namespace BinaryAnalysis {                      // documented elsewhere
namespace InstructionSemantics2 {               // documented elsewhere

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
typedef boost::shared_ptr<void> SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void RegisterState;
typedef boost::shared_ptr<void> RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void MemoryState;
typedef boost::shared_ptr<void> MemoryStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Wraps RISC operators so they can be traced. */
class RiscOperators: public BaseSemantics::RiscOperators {
    BaseSemantics::RiscOperatorsPtr subdomain_;         // Domain to which all our RISC operators chain
    Sawyer::Message::Stream stream_;                    // stream to which output is emitted
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors.
protected:
    // use the version that takes a subdomain instead of this c'tor
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver), stream_(mlog[Diagnostics::INFO]) {
        set_name("Trace");
    }

    // use the version that takes a subdomain instead of this c'tor.
    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver), stream_(mlog[Diagnostics::INFO]) {
        set_name("Trace");
    }

    explicit RiscOperators(const BaseSemantics::RiscOperatorsPtr &subdomain)
        : BaseSemantics::RiscOperators(subdomain->currentState(), subdomain->solver()),
          subdomain_(subdomain), stream_(mlog[Diagnostics::INFO]) {
        set_name("Trace");
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
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object.  This domain does not manage any state--it only wraps another domains RISC
     *  operators. Therefore, the supplied protoval and solver are not actually used.  It is probably better to construct the
     *  TraceSemantics' RISC operators with the constructor that takes the subdomain's RISC operators. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
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
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
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
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Obtain a pointer to the subdomain's RISC operators.  The subdomain is the one that is being traced. */
    virtual BaseSemantics::RiscOperatorsPtr get_subdomain() const ROSE_DEPRECATED("use subdomain instead") {
        return subdomain();
    }


    /** Set the subdomain that is to be traced. All our RISC operators will simply chain to the subdomain operators. */
    void set_subdomain(const BaseSemantics::RiscOperatorsPtr &sd) ROSE_DEPRECATED("use subdomain instead") {
        subdomain(sd);
    }

    /** Property: Subdomain to which operations are forwarded.
     *
     * @{ */
    const BaseSemantics::RiscOperatorsPtr& subdomain() const { return subdomain_; }
    void subdomain(const BaseSemantics::RiscOperatorsPtr &subdomain) { subdomain_ = subdomain; }
    /** @} */

    /** Check that we have a valid subdomain.  If the subdomain isn't value (hasn't been set) then throw an exception.
     *
     *  @{ */
    void check_subdomain() const ROSE_DEPRECATED("use checkSubodmain instead") {
        checkSubdomain();
    }
    void checkSubdomain() const {
        if (subdomain_==NULL)
            throw BaseSemantics::Exception("subdomain is not set; nothing to trace", NULL);
    }
    /** @} */

    /** Property: output stream to which tracing is emitted.  The default is the INFO stream of the
     * rose::BinaryAnalysis::InstructionSemantics2 message facility. Output will only show up when this stream is enabled.
     *
     * @{ */
    Sawyer::Message::Stream& stream() { return stream_; }
    void stream(Sawyer::Message::Stream &s) { stream_ = s; }
    /** @} */

protected:
    void linePrefix();
    std::string toString(const BaseSemantics::SValuePtr&);
    std::string toString(SgAsmFloatType*);
    void check_equal_widths(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr &check_width(const BaseSemantics::SValuePtr &a, size_t nbits,
                                                const std::string &what="result");
    std::string register_name(const RegisterDescriptor&);

    void before(const std::string&);
    void before(const std::string&, const RegisterDescriptor&);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                size_t);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
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
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    void after(const BaseSemantics::Exception&);
    void after_exception();
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we override from our super class
public:
    virtual BaseSemantics::SValuePtr protoval() const ROSE_OVERRIDE;
    virtual void solver(SMTSolver*) ROSE_OVERRIDE;
    virtual SMTSolver *solver() const ROSE_OVERRIDE;
    virtual BaseSemantics::StatePtr currentState() const ROSE_OVERRIDE;
    virtual void currentState(const BaseSemantics::StatePtr&) ROSE_OVERRIDE;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;
    virtual size_t nInsns() const ROSE_OVERRIDE;
    virtual void nInsns(size_t n) ROSE_OVERRIDE;
    virtual SgAsmInstruction* currentInstruction() const ROSE_OVERRIDE;
    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
    virtual void finishInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
    
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr boolean_(bool value) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual void hlt() ROSE_OVERRIDE;
    virtual void cpuid() ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rdtsc() ROSE_OVERRIDE;

    // The actual RISC operators. These are pure virtual in the base class
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr&, size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr&,
                                                          const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                         const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr&, size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr&, size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&,
                                                    BaseSemantics::SValuePtr&/*out*/) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr&,
                                                      const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;

    virtual void interrupt(int majr, int minr);

    virtual BaseSemantics::SValuePtr fpFromInteger(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpToInteger(const BaseSemantics::SValuePtr&, SgAsmFloatType*,
                                                 const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpConvert(const BaseSemantics::SValuePtr&, SgAsmFloatType*, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpIsNan(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpIsDenormalized(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpIsZero(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpIsInfinity(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpSign(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpEffectiveExponent(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpAdd(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                           SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpSubtract(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpMultiply(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                              SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpSquareRoot(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr fpRoundTowardZero(const BaseSemantics::SValuePtr&, SgAsmFloatType*) ROSE_OVERRIDE;
    
    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor&) ROSE_OVERRIDE;
    virtual void writeRegister(const RegisterDescriptor&, const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual void writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
