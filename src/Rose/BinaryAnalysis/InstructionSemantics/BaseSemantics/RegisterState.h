#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_RegisterState_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_RegisterState_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/AddressSpace.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/version.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register States
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** The set of all registers and their values. RegisterState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::RegisterState is an abstract class that defines the interface.  See the
 *  Rose::BinaryAnalysis::InstructionSemantics namespace for an overview of how the parts fit together.*/
class RegisterState: public AddressSpace {
public:
    /** Shared-ownership pointer. */
    using Ptr = RegisterStatePtr;

private:
    MergerPtr merger_;
    SValuePtr protoval_;                                /**< Prototypical value for virtual constructors. */

protected:
    RegisterDictionaryPtr regdict;                      /**< Registers that are able to be stored by this state. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        //s & merger_; -- not saved
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AddressSpace);
        s & BOOST_SERIALIZATION_NVP(protoval_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(regdict);
    }
#endif

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RegisterState();                                    // for serialization

    RegisterState(const SValuePtr &protoval, const RegisterDictionaryPtr &regdict);

public:
    virtual ~RegisterState();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None are needed--this class is abstract.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.
public:
    /** Virtual constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be used only
     *  to create additional instances of the value via its virtual constructors.  The prototypical value is normally of the
     *  same type for all parts of a semantic analysis. The register state must be compatible with the rest of the binary
     *  analysis objects in use. */
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionaryPtr &regdict) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static RegisterStatePtr promote(const AddressSpacePtr&);

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

    /** Property: Register dictionary.
     *
     *  The register dictionary should be compatible with the register dictionary used for other parts of binary analysis. At
     *  this time (May 2013) the dictionary is only used when printing.
     *
     * @{ */
    RegisterDictionaryPtr registerDictionary() const /*final*/;
    void registerDictionary(const RegisterDictionaryPtr&) /*final*/;
    /** @} */

    /** Removes stored values from the register state.
     *
     *  Depending on the register state implementation, this could either store new, distinct undefined values in each
     *  register, or it could simply erase all information about stored values leaving the register state truly empty. For
     *  instance RegisterStateGeneric, which uses variable length arrays to store information about a dynamically changing set
     *  of registers, clears its arrays to zero length.
     *
     *  Register states can also be initialized by clearing them or by explicitly writing new values into each desired register
     *  (or both). See @ref BaseSemantics::RegisterStateGeneric::initialize_nonoverlapping for one way to initialize that
     *  register state. */
    virtual void clear() = 0;

    /** Set all registers to the zero. */
    virtual void zero() = 0;

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
     *  See @ref BaseSemantics::RiscOperators::readRegister for more details. */
    virtual SValuePtr readRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) = 0;

    /** Read a register without side effects.
     *
     *  This is similar to @ref readRegister except it doesn't modify the register state in any way. */
    virtual SValuePtr peekRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) = 0;

    /** Update register properties after reading a register.
     *
     *  This should be called by all implementations of @ref BaseSemantics::RiscOperators::readRegister. Depending on the
     *  semantic domain, it usually adds the READ property to all bits of the register, and conditionally adds READ_BEFORE_WRITE
     *  and/or READ_UNINITIALIZED properties to parts of the register. */
    virtual void updateReadProperties(RegisterDescriptor) = 0;

    /** Write a value to a register.
     *
     *  The register descriptor, @p reg, not only describes which register, but also which bits of that register (e.g., "al",
     *  "ah", "ax", "eax", and "rax" are all the same hardware register on an amd64, but refer to different parts of that
     *  register). The RISC operations are provided so that they can be used to insert the @p value bits into a wider the
     *  hardware register if necessary. See @ref BaseSemantics::RiscOperators::readRegister for more details. */
    virtual void writeRegister(RegisterDescriptor reg, const SValuePtr &value, RiscOperators *ops) = 0;

    /** Update register properties after writing to a register.
     *
     *  This should be called by all implementations of @ref BaseSemantics::RiscOperators::writeRegister. Depending on the
     *  domain, it usually adds the WRITE or INIT property to the bits of the rgister. */
    virtual void updateWriteProperties(RegisterDescriptor, InputOutputProperty) = 0;
};

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_VERSION(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterState, 1);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterState);

#endif
#endif
