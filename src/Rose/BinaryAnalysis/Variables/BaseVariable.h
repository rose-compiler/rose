#ifndef ROSE_BinaryAnalysis_Variables_BaseVariable_H
#define ROSE_BinaryAnalysis_Variables_BaseVariable_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/InstructionAccess.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

/** Base class describing a source-level variable. */
class BaseVariable {
private:
    rose_addr_t maxSizeBytes_ = 0;                      // maximum possible size of this variable in bytes
    std::vector<InstructionAccess> insns_;              // instructions accessing the variable
    std::string name_;                                  // optional variable name

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(maxSizeBytes_);
        s & BOOST_SERIALIZATION_NVP(insns_);
        s & BOOST_SERIALIZATION_NVP(name_);
    }
#endif

protected:
    /** Default constructor.
     *
     *  Constructs a descriptor for a variable whose maximum size is zero. */
    BaseVariable();

    /** Construct a variable with a given maximum size. */
    BaseVariable(size_t maxSizeBytes, const std::vector<InstructionAccess> &definingInstructions, const std::string &name);

public:
    BaseVariable(const BaseVariable&);
    ~BaseVariable();

public:
    /** Property: Maximum variable size in bytes.
     *
     *  This is the maximum size that the variable could be, measured in bytes. A default constructed object will have a size
     *  of zero.
     *
     * @{ */
    rose_addr_t maxSizeBytes() const;
    void maxSizeBytes(rose_addr_t size);
    /** @} */

    /** Property: Defining instructions.
     *
     *  This is information about each instruction that accesses a variable, such as whether the instruction reads or writes to
     *  that variable.
     *
     * @{ */
    const std::vector<InstructionAccess>& instructionsAccessing() const;
    void instructionsAccessing(const std::vector<InstructionAccess>&);
    /** @} */

    /** Read/write status.
     *
     *  Returns bit flags describing whether the variable was read and/or written. */
    AccessFlags accessFlags() const;

    /** Property: I/O properties.
     *
     *  This property is a set of flags that describe how the variable is accessed. */
    InstructionSemantics::BaseSemantics::InputOutputPropertySet ioProperties() const;

    /** Property: Optional variable name.
     *
     *  There is no constraint on what the variable name may be. At this time it's used mainly for debugging. Therefore it
     *  should always be printed assuming it contains special characters.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string &s);
    /** @} */

    /** Insert information about how an instruction accesses this variable. */
    void insertAccess(Address insnAddr, AccessFlags);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
