#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_RegisterState_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_RegisterState_H

#include <BaseSemanticsMerger.h>
#include <BaseSemanticsSValue.h>
#include <Registers.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/version.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register States
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** The set of all registers and their values. RegisterState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::RegisterState is an abstract class that defines the interface.  See the
 *  Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.*/
class RegisterState: public boost::enable_shared_from_this<RegisterState> {
    MergerPtr merger_;
    SValuePtr protoval_;                                /**< Prototypical value for virtual constructors. */

protected:
    const RegisterDictionary *regdict;                  /**< Registers that are able to be stored by this state. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        //s & merger_; -- not saved
        s & BOOST_SERIALIZATION_NVP(protoval_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(regdict);
    }
#endif

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RegisterState()
        : regdict(NULL) {}                                // for serialization

    RegisterState(const SValuePtr &protoval, const RegisterDictionary *regdict)
        : protoval_(protoval), regdict(regdict) {
        ASSERT_not_null(protoval_);
    }

public:
    /** Shared-ownership pointer for a @ref RegisterState object. See @ref heap_object_shared_ownership. */
    typedef RegisterStatePtr Ptr;

public:
    virtual ~RegisterState() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None are needed--this class is abstract.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.
public:
    /** Virtual constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be used only
     *  to create additional instances of the value via its virtual constructors.  The prototypical value is normally of the
     *  same type for all parts of a semantic analysis. The register state must be compatible with the rest of the binary
     *  analysis objects in use. */
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const = 0;

    /** Make a copy of this register state. */
    virtual RegisterStatePtr clone() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static RegisterStatePtr promote(const RegisterStatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...

    /** Property: Merger.
     *
     *  This property is optional details about how to merge two states. It is passed down to the register and memory state
     *  merge operation and to the semantic value merge operation.  Users can subclass this to hold whatever information is
     *  necessary for merging.  Unless the user overrides merge functions to do something else, all merging will use the same
     *  merger object -- the one set for this property.
     *
     * @{ */
    MergerPtr merger() const { return merger_; }
    void merger(const MergerPtr &m) { merger_ = m; }
    /** @} */

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr protoval() const { return protoval_; }

    /** The register dictionary should be compatible with the register dictionary used for other parts of binary analysis. At
     *  this time (May 2013) the dictionary is only used when printing.
     * @{ */
    const RegisterDictionary *get_register_dictionary() const { return regdict; }
    void set_register_dictionary(const RegisterDictionary *rd) { regdict = rd; }
    /** @} */

    /** Removes stored values from the register state.
     *
     *  Depending on the register state implementation, this could either store new, distinct undefined values in each
     *  register, or it could simply erase all information about stored values leaving the register state truly empty. For
     *  instance RegisterStateGeneric, which uses variable length arrays to store information about a dynamically changing set
     *  of registers, clears its arrays to zero length.
     *
     *  Register states can also be initialized by clearing them or by explicitly writing new values into each desired
     *  register (or both). See @ref RegisterStateGeneric::initialize_nonoverlapping for one way to initialize that register
     *  state. */
    virtual void clear() = 0;

    /** Set all registers to the zero. */
    virtual void zero() = 0;

    /** Merge register states for data flow analysis.
     *
     *  Merges the @p other state into this state, returning true if this state changed. */
    virtual bool merge(const RegisterStatePtr &other, RiscOperators *ops) = 0;

    /** Read a value from a register.
     *
     *  The register descriptor, @p reg, not only describes which register, but also which bits of that register (e.g., "al",
     *  "ah", "ax", "eax", and "rax" are all the same hardware register on an amd64, but refer to different parts of that
     *  register). The RISC operations are provided so that they can be used to extract the correct bits from a wider hardware
     *  register if necessary.
     *
     *  The @p dflt value is written into the register state if the register was not defined in the state. By doing this, a
     *  subsequent read of the same register will return the same value. Some register states cannot distinguish between a
     *  register that was never accessed and a register that was only read, in which case @p dflt is not used since all
     *  registers are already initialized.
     *
     *  See @ref RiscOperators::readRegister for more details. */
    virtual SValuePtr readRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) = 0;

    /** Read a register without side effects.
     *
     *  This is similar to @ref readRegister except it doesn't modify the register state in any way. */
    virtual SValuePtr peekRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) = 0;

    /** Write a value to a register.
     *
     *  The register descriptor, @p reg, not only describes which register, but also which bits of that register (e.g., "al",
     *  "ah", "ax", "eax", and "rax" are all the same hardware register on an amd64, but refer to different parts of that
     *  register). The RISC operations are provided so that they can be used to insert the @p value bits into a wider the
     *  hardware register if necessary. See @ref RiscOperators::readRegister for more details. */
    virtual void writeRegister(RegisterDescriptor reg, const SValuePtr &value, RiscOperators *ops) = 0;

    /** Print the register contents. This emits one line per register and contains the register name and its value.
     *  @{ */
    void print(std::ostream &stream, const std::string prefix = "") const;
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** RegisterState with formatter. See with_formatter(). */
    class WithFormatter {
        RegisterStatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const RegisterStatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing register states with formatting. The usual way to use this is:
     * @code
     *  RegisterStatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     *
     * Since specifying a line prefix string for indentation purposes is such a common use case, the
     * indentation can be given instead of a format, as in the following code that indents the
     * prefixes each line of the expression with four spaces.
     *
     * @code
     *  std::cout <<"Register state:\n" <<*(obj + "    ");
     * @code
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */

};

std::ostream& operator<<(std::ostream&, const RegisterState&);
std::ostream& operator<<(std::ostream&, const RegisterState::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterState, 1);

#endif
