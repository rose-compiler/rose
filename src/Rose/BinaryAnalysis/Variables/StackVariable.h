#ifndef ROSE_BinaryAnalysis_Variables_StackVariable_H
#define ROSE_BinaryAnalysis_Variables_StackVariable_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/BaseVariable.h>

#include <Sawyer/IntervalMap.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

/** Description of a local stack variable within a function. */
class StackVariable: public BaseVariable {
public:
    /** Purpose of variable. */
    enum class Purpose {
        RETURN_ADDRESS,                                 /**< Possible or known return address. */
        FRAME_POINTER,                                  /**< Pointer to previous stack frame. */
        STACK_POINTER,                                  /**< Saved stack pointer. */
        SPILL_AREA,                                     /**< Callee-saved registers. */
        NORMAL,                                         /**< Normal source code level variable. */
        UNKNOWN,                                        /**< Purpose is unknown. */
        OTHER                                           /**< None of the above purposes. */
    };

    /** Boundary between stack variables.
     *
     *  This is the lowest address for a region of variables along with information about what instructions were used to define
     *  this boundary and what purpose the addresses immediately above this boundary serve. */
    struct Boundary {
        int64_t stackOffset = 0;                        /**< Offset from function's initial stack pointer. */
        std::vector<InstructionAccess> definingInsns;   /**< Instructions that define this boundary. */
        Purpose purpose = Purpose::UNKNOWN;             /**< Purpose of addresses above this boundary. */
    };

    /** List of boundaries. */
    using Boundaries = std::vector<Boundary>;

private:
    Partitioner2::FunctionPtr function_;                // Function in which local variable exists
    int64_t stackOffset_ = 0;                           // Offset w.r.t. function's initial stack pointer
    Purpose purpose_ = Purpose::UNKNOWN;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseVariable);
        s & BOOST_SERIALIZATION_NVP(function_);
        s & BOOST_SERIALIZATION_NVP(stackOffset_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(purpose_);
    }
#endif

public:
    /** Default constructor.
     *
     *  Creates an invalid (zero-sized) variable descriptor. */
    StackVariable();

    /** Construct a variable descriptor. */
    StackVariable(const Partitioner2::FunctionPtr&, int64_t stackOffset, Address maxSizeBytes, Purpose,
                  const std::vector<InstructionAccess> &definingInstructions = std::vector<InstructionAccess>(),
                  const std::string &name = "");

    StackVariable(const StackVariable&);
    ~StackVariable();

    /** Property: Function owning the variable.
     *
     *  A default constructed object will have a null function pointer. All other objects have a valid function pointer.
     *
     * @{ */
    Partitioner2::FunctionPtr function() const;
    void function(const Partitioner2::FunctionPtr&);
    /** @} */

    /** Property: Stack offset.
     *
     *  This is the address of the variable relative to the function's initial stack pointer. For multi-byte variables, this is the
     *  lowest address of the variable. Depending on the architecture, this offset might be positive or negative.
     *
     * @{ */
    int64_t stackOffset() const;
    void stackOffset(int64_t offset);
    /** @} */

    /** Property: Purpose.
     *
     *  Areas of a stack frame serve different purposes. This property describes the purpose.
     *
     *  @{ */
    Purpose purpose() const;
    void purpose(Purpose p);
    /** @} */

    /** Give variable a defult name.
     *
     *  This variable's name is replaced by a generated name and the name is returned. */
    const std::string& setDefaultName();

    /** Compare two local variables.
     *
     *  Local variables are equal if and only if they belong to the same function, have the same stack offset, and have the
     *  same maximum size or both are default constructed.
     *
     * @{ */
    bool operator==(const StackVariable &other) const;
    bool operator!=(const StackVariable &other) const;
    /** @} */

    /** Location of variable w.r.t. function's stack.
     *
     *  Returns the concrete location of this local stack variable w.r.t. this function's initial stack pointer. */
    OffsetInterval interval() const;

    /** Insert a new boundary or adjust an existing boundary.
     *
     *  The boundaries are assumed to be unsorted, and if a new boundary is inserted it is inserted at the end of the list.
     *
     * @{ */
    static Boundary& insertBoundary(Boundaries& /*in,out*/, int64_t stackOffset, const InstructionAccess&);
    static Boundary& insertBoundaryImplied(Boundaries&/*in,out*/, int64_t stackOffset);
    /** @} */

    /** Print information about a boundary. */
    static void printBoundary(std::ostream&, const Boundary&, const std::string &prefix = "");

    /** Printing local variable.
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

    /** Print local variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&);
};

/** Collection of local variables organized by stack offsets. */
using StackVariables = Sawyer::Container::IntervalMap<OffsetInterval, StackVariable>;

/** Print info about multiple local variables.
 *
 *  This output includes such things as the function to which they belong and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const StackVariables&, const Partitioner2::PartitionerConstPtr&, std::ostream &out, const std::string &prefix = "");

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Variables::StackVariable, 1);
#endif

#endif
#endif
