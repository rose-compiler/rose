#ifndef ROSE_BinaryAnalysis_Variables_InstructionAccess_H
#define ROSE_BinaryAnalysis_Variables_InstructionAccess_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Variables/BasicTypes.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

/** Information about how an instruction accesses a variable. */
class InstructionAccess {
public:
    Sawyer::Optional<Address> address_;                 /**< Address of instruction accessing the variable. */
    AccessFlags access_;                                /**< How the instruction accesses the variable. */

private:
    InstructionAccess() = default;                  // needed for boost::serialization
public:
    /** Constructor for variables created by no instruction. */
    explicit InstructionAccess(const AccessFlags);

    /** Constructor for variables created by an instruction. */
    InstructionAccess(const Address insnAddr, const AccessFlags);

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(address_);
        s & BOOST_SERIALIZATION_NVP(access_);
    }
#endif

public:
    /** Property: Instruction address.
     *
     *  The address is optional. If the address is missing it means that the variable was discovered by some means other
     *  than looking at instructions. For instance, some variables might be created by the caller according to the calling
     *  convention. */
    Sawyer::Optional<Address> address() const;

    /** Property: Access type.
     *
     *  How this instruction accesses the variable, whether it reads from the variable, writes to the variable, or both.
     *
     * @{ */
    AccessFlags access() const;
    AccessFlags& access();
    /** @} */

    /** String describing access.
     *
     *  Returns "read", "write", "read/write", or "no access".
     *
     * @{ */
    std::string accessString() const;
    static std::string accessString(AccessFlags);
    /** @} */

    /** String representation of this object. */
    std::string toString() const;
};

std::ostream& operator<<(std::ostream&, const InstructionAccess&);

} // namespace
} // namespace
} // namespace

#endif
#endif
