#ifndef Rose_MultiSemantics2_H
#define Rose_MultiSemantics2_H

#include "BaseSemantics2.h"

namespace Rose {                                // documented elsewhere
namespace BinaryAnalysis {                      // documented elsewhere
namespace InstructionSemantics2 {               // documented elsewhere

/** Semantic domain composed of subdomains.
 *
 * The MultiSemantics semantic domain is a pseudo domain composed of one or more subdomains.  Each of the RISC operations
 * implemented in this policy invokes the operation in each of the enabled subdomains.  The type of values manipulated by the
 * MultiSemantics domain are the union of the values from each of the subdomains. One could accomplish the same thing by
 * instantiating multiple instruction semantics objects and calling each one for each instruction.  However, using this
 * MultiSemantics domain is cleaner and easier to specialize.
 *
 * A multi-domain is created by instantiating all the subdomain RiscOperators and adding them one by one to the multi-domain
 * RiscOperators object via its add_subdomain() method.  Each call to add_subdomain() returns an ID number for the subdomain,
 * by which the subdomain's RiscOperators and SValue objects can be accessed given a multi-domain RiscOperators or SValue
 * object.
 *
 * A sub-domain can be marked as active or inactive.  When the a multi-domain RISC operation is called, the same
 * operation will be invoked in each of the active sub-domains (provided the operation's inputs are valid for that
 * sub-domain).  If the operation returns a multi-value (as most do), then the sub-values corresponding to called sub-domains
 * will be marked valid and the other sub-values are marked invalid.
 *
 * Using a multi-domain directly is not all that interesting.  Where the real convenience comes is in specializing the
 * multi-domain to do things like convert values from one domain to another.  For example, consider two semantic domains called
 * Alpha and Beta implemented operating on values of type AlphaValue and BetaValue. Assume that for some reason, and ADD
 * operation in Beta is expensive and that a BetaValue can be constructed from an AlphaValue.  Therefore, it is more efficient
 * to skip the ADD operation in Beta and instead compute the Beta sum from the Alpha sum.  One does that by subclassing the
 * MultiDomain::RiscOperators and overriding its add() method.  Here's the basic idea, sans error handling, etc.:
 *
 * @code
 *  class MyDomain: public MultiSemantics::RiscOperators {
 *  public:
 *      // the usual virtual constructors go here...
 *  public:
 *      BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
 *          SValuePtr a = SValue::promote(a_); //promote from BaseSemantics to MultiSemantics SValue
 *          SValuePtr b = SValue::promote(b_);
 *          const size_t AlphaID = 0, BetaID = 1; //probably part of the constructor
 *          if (is_active(AlphaID) && is_active(BetaID)) {
 *              clear_active(BetaID);
 *              SValuePtr result = SValue::promote(MultiSemantics::RiscOperators::add(a_, b_));
 *              set_active(BetaID);
 *              Beta::SValuePtr beta_result = compute_from_alpha(retval.get_subvalue(AlphaID));
 *              result.set_subvalue(BetaID, beta_result);
 *              return result;
 *          }
 *          return MultiSemantics::RiscOperators(a_, b_);
 *      }
 *  };
 * @endcode
 */
namespace MultiSemantics {

/** Helps printing multidomain values by allowing the user to specify a name for each subdomain. */
class Formatter: public BaseSemantics::Formatter {
public:
    std::vector<std::string> subdomain_names;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to a multi-semantic value. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Type of values manipulated by the MultiSemantics domain.
 *
 * A multi-semantic value is a set of values, one from each of the subdomains, and a bit vector that indicates which values
 * are valid.  The bit vector is accessed by the RiscOperators and new values are created by those operators with appropriate
 * validities, but the MultiSemantics domain doesn't otherwise modify the bit vector--that's up to the user-defined,
 * inter-operation callbacks.
 *
 * Individual sub-domain values can be queried from a multi-domain value with get_subvalue() using the ID returned by
 * add_subdomain() when the sub-domain's RiscOperators were added to the multi-domain's RiscOperators. */
class SValue: public BaseSemantics::SValue {
protected:
    typedef std::vector<BaseSemantics::SValuePtr> Subvalues;
    Subvalues subvalues;

protected:
    // Protected constructors
    explicit SValue(size_t nbits)
        : BaseSemantics::SValue(nbits) {}

    SValue(const SValue &other)
        : BaseSemantics::SValue(other.get_width()) {
        init(other);
    }

    void init(const SValue &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Construct a prototypical value.  Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Promote a base value to a MultiSemantics value.  The value @p v must have a MultiSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) { // hot
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors
public:

    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const ROSE_OVERRIDE;

    /** Create a new undefined MultiSemantics value.  The returned value is constructed by calling the virtual undefined_()
     *  for each subdomain value in "this".  If you want a multidomain value that has no valid subvalues, then use
     *  the create_empty() method instead. */
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE;

    /** Create a new unspecified MultiSemantics value.  The returned value is constructed by calling the virtual unspecified_()
     *  for each subdomain value in "this".  If you want a multidomain value that has no valid subvalues, then use
     *  the create_empty() method instead. */
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const ROSE_OVERRIDE;

    /** Create a MultiSemantics value holding a concrete value.  The returned value is constructed by calling the virtual
     *  number_() method for each subdomain value in "this". */
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t number) const ROSE_OVERRIDE;

    /** Create a new MultiSemantics value with no valid subvalues.  The caller will probably construct a value iteratively by
     *  invoking set_subvalue() one or more times. */
    virtual SValuePtr create_empty(size_t nbits) const {
        return SValuePtr(new SValue(nbits));
    }
    
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        return BaseSemantics::SValuePtr(new SValue(*this));
    }
    
    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods
public:
    /** Determines if two multidomain values might be equal.  Two multidomain values are equal if, for any subdomain for which
     *  both values are valid, they are equal in the subdomain. */
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;

    /** Determines if two multidomain values must be equal.  Two multidomain values are equal if and only if there is at least
     *  one subdomain where both values are valid, and for all subdomains where both values are valid, their must_equal()
     *  relationship is satisfied. */
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;

    virtual void set_width(size_t nbits) ROSE_OVERRIDE;

    virtual bool isBottom() const ROSE_OVERRIDE;

    /** Determines if the value is a concrete number.  In the MultiSemantics domain, a value is a concrete number if and only
     *  if it has at least one valid subdomain value and all valid subdomain values are concrete numbers, and all are the same
     *  concrete number. */
    virtual bool is_number() const ROSE_OVERRIDE;

    virtual uint64_t get_number() const ROSE_OVERRIDE;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared at this level of the class hierarchy
public:
    /** Returns true if a subdomain value is valid.  A subdomain value is valid if the specified index has a non-null SValue
     *  pointer. It is permissible to call this with an index that is out of range (false is returned in that case). */
    virtual bool is_valid(size_t idx) const { // hot
        return idx<subvalues.size() && subvalues[idx]!=NULL;
    }

    /** Removes a subdomain value and marks it as invalid.  It is permissible to call this with an index that does not
     *  correspond to a valid subdomain value. */
    virtual void invalidate(size_t idx);

    /** Return a subdomain value.  The subdomain must be valid according to is_valid(). */
    virtual BaseSemantics::SValuePtr get_subvalue(size_t idx) const { // hot
        ASSERT_require(idx<subvalues.size() && subvalues[idx]!=NULL); // you should have called is_valid() first
        return subvalues[idx];
    }

    /** Insert a subdomain value.  The specified value is inserted at the specified index.  No attempt is made to validate
     *  whether the value has a valid dynamic type for that slot.  If the value is not a null pointer, then is_valid() will
     *  return true after this call. */
    virtual void set_subvalue(size_t idx, const BaseSemantics::SValuePtr &value) { // hot
        ASSERT_require(value==NULL || value->get_width()==get_width());
        if (idx>=subvalues.size())
            subvalues.resize(idx+1);
        subvalues[idx] = value;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register states
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void RegisterState;

/** Shared-ownership pointer to a multi-semantics register state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<void> RegisterStatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory states
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void MemoryState;

/** Shared-ownership pointer to a multi-semantics memory state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<void> MemoryStatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void State;

/** Shared-ownership pointer to a multi-semantics state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<void> StatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to multi-semantics RISC operators. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for the MultiSemantics domain.
 *
 *  Invoking a RISC operation in the MultiSemantics domain causes the same operation to be invoked for each of its subdomains
 *  provided all arguments are valid in those subdomains (i.e., SValue::is_valid() returns true). */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    typedef std::vector<BaseSemantics::RiscOperatorsPtr> Subdomains;
    Subdomains subdomains;
    std::vector<bool> active;
    Formatter formatter;                // contains names for the subdomains

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("Multi");
        (void) SValue::promote(protoval); // check that its dynamic type is a MultiSemantics::SValue
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(state, solver) {
        name("Multi");
        (void) SValue::promote(state->protoval());      // dynamic type must be a MultiSemantics::SValue
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Static allocating constructor. This constructor creates a new MultiDomain RiscOperators object that does't have any
     *  subdomains.  The subdomains should be added before using this object. The @p regdict argument is not used in this
     *  class and is only present for consistency with other classes and for subclasses. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        return RiscOperatorsPtr(new RiscOperators(protoval));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &ops);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hiearchy
public:
    /** Add a subdomain to this MultiSemantics domain.  Returns the identifier (index) used for this subdomain.  The @p name is
     *  optional and used mostly for debugging; it is also added to the formatter and can be used when printing a semantic
     *  value. The @p activate argument indicates whether this subdomain is activated (default is true).  Activated subdomains
     *  will participate in RISC operations if their arguments are defined.  See also, is_active(), set_active(), and
     *  clear_active(). */
    virtual size_t add_subdomain(const BaseSemantics::RiscOperatorsPtr &subdomain, const std::string &name, bool activate=true);

    /** Returns a formatter containing the names of the subdomains. */
    virtual Formatter& get_formatter() {
        return formatter;
    }
    
    /** Returns the number of subdomains added to this MultiDomain. */
    virtual size_t nsubdomains() const {
        return subdomains.size();
    }

    /** Returns the RiscOperators for a subdomain. The @p idx need not be valid. */
    virtual BaseSemantics::RiscOperatorsPtr get_subdomain(size_t idx) const;

    /** Returns true if a subdomain is active. Active subdomains participate in MultiSemantics RISC operations, provided all
     *  their operands are valid.  This method returns false if @p idx is out of range or the subdomain's RiscOperators are
     *  null. */
    virtual bool is_active(size_t idx) const {
        return idx<subdomains.size() && subdomains[idx]!=NULL && active[idx];
    }

    /** Makes a subdomain inactive.  The subdomain's RiscOperators object is not removed or deleted by this method; the
     *  subdomain can be made active again at a later time. */
    virtual void clear_active(size_t idx) {
        set_active(idx, false);
    }

    /** Makes a subdomain active or inactive.  When making a subdomain inactive, its RiscOperators object is not removed or
     *  deleted, and it can be reactivated later.  An invalid @p idx is ignored when deactivating; it must refer to a valid
     *  subdomain when activating. */
    virtual void set_active(size_t idx, bool status);

    /** Called before each subdomain RISC operation.  The default implementation does nothing, but subclasses can override this
     *  to do interesting things. The @p idx is the index of the subdomain that's about to be called. */
    virtual void before(size_t idx) {}

    /** Called after each subdomain RISC operation.  The default implementation does nothing, but subclasses can override this
     *  to do interesting things. The @p idx is the index of the subdomain that was just called. */
    virtual void after(size_t idx) {}

    /** Convenience function for SValue::create_empty(). */
    virtual SValuePtr svalue_empty(size_t nbits) {
        return SValue::promote(protoval())->create_empty(nbits);
    }
    
    /** Iterates over valid subdomains whose inputs are valid. This is intended to be used in a "for" loop inside a RISC
     * operation, such as:
     *
     * @code
     *  BaseSemantics::SValuePtr
     *  RiscOperators::add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
     *      SValuePtr retval(a->get_width());
     *      for (Cursor c(this, a, b); !c.at_end(); c.next())
     *          retval->set_subvalue(c->idx(), c->add(a, b));
     *      }
     *  }
     * @endcode
     *
     * This RiscOperator's before() and after() methods are automatically invoked around the body of the "for" loop
     * by the Cursor constructor and next() methods. */
    class Cursor {
    public:
        typedef std::vector<SValuePtr> Inputs;
    protected:
        RiscOperators *ops_;
        Inputs inputs_;
        size_t idx_;
    public:
        Cursor(RiscOperators *ops, const SValuePtr &arg1=SValuePtr(), const SValuePtr &arg2=SValuePtr(),
               const SValuePtr &arg3=SValuePtr())
            : ops_(ops), idx_(0) {
            init(arg1, arg2, arg3);
        }
        Cursor(RiscOperators *ops, const Inputs &inputs)
            : ops_(ops), inputs_(inputs), idx_(0) {
            init();
        }

        /** Class method to construct the array of inputs from a variable number of arguments.  This is used only by the
         *  SUBDOMAINS macro in the MultiSemantics source code so that the input values can be passed as a parenthesized
         *  group as the macro's second argument. */
        static Inputs inputs(const BaseSemantics::SValuePtr &arg1=BaseSemantics::SValuePtr(),
                             const BaseSemantics::SValuePtr &arg2=BaseSemantics::SValuePtr(),
                             const BaseSemantics::SValuePtr &arg3=BaseSemantics::SValuePtr());
                             
        bool at_end() const;                    /**< Returns true when the cursor has gone past the last valid subdomain. */
        void next();                            /**< Advance to the next valid subdomain. */
        size_t idx() const;                     /**< Return the subdomain index for the current cursor position. */
        BaseSemantics::RiscOperatorsPtr operator->() const;   /**< Return the subdomain for the current cursor position. */
        BaseSemantics::RiscOperatorsPtr operator*() const;    /**< Return the subdomain for the current cursor position. */

        /** Returns subdomain value of its multidomain argument. */
        BaseSemantics::SValuePtr operator()(const BaseSemantics::SValuePtr&) const;

    protected:
        void init(const SValuePtr &arg1, const SValuePtr &arg2, const SValuePtr &arg3);
        void init();
        void skip_invalid();
        bool inputs_are_valid() const;
    };
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // RISC operations and other overrides
public:
    virtual void print(std::ostream &o, BaseSemantics::Formatter&) const ROSE_OVERRIDE;
    virtual void startInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;
    virtual void finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr boolean_(bool) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr&) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a, size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a,
                                                const BaseSemantics::SValuePtr &nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a,
                                                 const BaseSemantics::SValuePtr &nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a,
                                               const BaseSemantics::SValuePtr &nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a,
                                                const BaseSemantics::SValuePtr &nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a,
                                                          const BaseSemantics::SValuePtr &nbits) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &cond, const BaseSemantics::SValuePtr &a,
                                         const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    const BaseSemantics::SValuePtr &c,
                                                    BaseSemantics::SValuePtr &carry_out/*output*/) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a,
                                                  const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a,
                                                  const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a,
                                                      const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE;
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
    virtual BaseSemantics::SValuePtr reinterpret(const BaseSemantics::SValuePtr&, SgAsmType*) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
