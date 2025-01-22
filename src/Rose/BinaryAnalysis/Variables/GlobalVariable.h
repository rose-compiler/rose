#ifndef ROSE_BinaryAnalysis_Variables_GlobalVariable_H
#define ROSE_BinaryAnalysis_Variables_GlobalVariable_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/BaseVariable.h>

#include <Sawyer/IntervalMap.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

/** Description of a global variable. */
class GlobalVariable: public BaseVariable {
    Address address_ = 0;                               // starting (lowest) virtual address

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseVariable);
        s & BOOST_SERIALIZATION_NVP(address_);
    }
#endif

public:
    /** Default constructor.
     *
     *  Constructs a descriptor for an invalid global variable whose maximum size is zero. */
    GlobalVariable();

    /** Constructor.
     *
     *  Creates a global variable descriptor for a variable whose lowest address is @p startingAddress and whose maximum size
     *  is @p maxSizeBytes bytes. The optional @p definingInstruction are the addresses of the instructions from which this
     *  variable was discerned and are usually memory dereferencing instructions. */
    GlobalVariable(Address startingAddress, rose_addr_t maxSizeBytes,
                   const std::vector<InstructionAccess> &definingInstructions = std::vector<InstructionAccess>(),
                   const std::string &name = "");

    ~GlobalVariable();

    /** Property: Starting address.
     *
     *  This property is the lowest address for the variable.
     *
     * @{ */
    Address address() const;
    void address(Address va);
    /** @} */

    /** Give variable a defult name.
     *
     *  This variable's name is replaced by a generated name and the name is returned. */
    const std::string& setDefaultName();

    /** Compare two global variable descriptors.
     *
     *  Two global variable descriptors are equal if and only if they have the same starting address and maximum size.
     *
     * @{ */
    bool operator==(const GlobalVariable &other) const;
    bool operator!=(const GlobalVariable &other) const;
    /** @} */

    /** Location of variable in memory. */
    AddressInterval interval() const;

    /** Printing global variable.
     *
     * @{ */
    void print(std::ostream&) const;
    std::string toString() const;
    /** @} */

    /** Predicate to test whether variable is valid.
     *
     *  Returns true if the variable is valid, i.e., if it has a non-zero size. Default constructed variables have a zero
     *  size. */
    explicit operator bool() const {
        return !interval().isEmpty();
    }

    /** Predicate to test whether variable is invalid.
     *
     *  Returns true if the variable is invalid, i.e., if it has a zero size. */
    bool operator!() const {
        return interval().isEmpty();
    }

    /** Print global variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&);
};

/** Maps virtual addresses to global variables.
 *
 *  The global variable will be represented in the map at all addresses that the global variable could occupy.  E.g., if global
 *  variable "g1" starts at 0x4000 and can be up to 1k 4-byte integers, then a record will exist in the map at all addresses
 *  from 0x4000 (inclusive) to 0x4100 (exclusive). */
using GlobalVariables = Sawyer::Container::IntervalMap<AddressInterval, GlobalVariable>;

/** Erase some global variables.
 *
 *  Erases global variables from the specified memory region. It is not sufficient to only remove addresses from the map; the
 *  addresses stored in the variables themselves (values in the map) may need to be adjusted so they don't overlap with the erased
 *  range. */
void erase(GlobalVariables&, const AddressInterval &toErase);

/** Print info about multiple global variables.
 *
 *  This output includes such things as their addresses, sizes, and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const GlobalVariables&,const Partitioner2::PartitionerConstPtr&, std::ostream &out, const std::string &prefix = "");

/** Check that the map is consistent.
 *
 *  Test that the keys of the map match up with the variables contained therein. If a map node is found where the interval key for
 *  the node doesn't match the addresses of the global variable stored in that node, then the map contains an inconsistency.  When
 *  an inconsistency is found, and the output stream is enabled then the map is printed and all inconsistencies are highlighted.
 *
 *  Returns the first address interval (key) where an inconsistency is detected, or an empty interval if there are no
 *  inconsistencies. */
AddressInterval isInconsistent(const GlobalVariables&, Sawyer::Message::Stream&);

} // namespace
} // namespace
} // namespace

#endif
#endif
