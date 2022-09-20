#ifndef ROSE_BinaryAnalysis_ModelChecker_OutOfBoundsTag_H
#define ROSE_BinaryAnalysis_ModelChecker_OutOfBoundsTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes an out-of-bounds memory access. */
class OutOfBoundsTag: public Tag {
public:
    using Ptr = OutOfBoundsTagPtr;

private:
    const TestMode testMode_;                                    // may or must, but not off
    const IoMode ioMode_;                                        // read or write
    const SgAsmInstruction *insn_;                               // instruction where the oob access occurs (optional)
    const InstructionSemantics::BaseSemantics::SValuePtr addr_; // memory address that is accessed
    const Variables::StackVariable intendedVariable_;            // variable that was incorrectly accessed
    const AddressInterval intendedVariableLocation_;             // location and size of stack variable in memory
    const Variables::StackVariable accessedVariable_;            // optional info about variable actually accessed
    const AddressInterval accessedVariableLocation_;             // optional location and size of variable actually accessed

protected:
    OutOfBoundsTag() = delete;
    OutOfBoundsTag(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                   const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                   const Variables::StackVariable &intendedVariable, const AddressInterval &intendedVariableLocation,
                   const Variables::StackVariable &accessedVariable, const AddressInterval &accessedVariableLocation);

    OutOfBoundsTag(const OutOfBoundsTag&) = delete;

public:
    ~OutOfBoundsTag();

    /** Allocating constructor.
     *
     *  Constructs a tag with information about out-of-bounds access to a stack variable.  The @p intendedVariable is the
     *  variable that was apparently intended to be accessed along with its @p intendedVariableLocation in memory. If the
     *  memory I/O accessed some other known stack variable, this can be indicated by the @p accessedVariable argument and its
     *  @p accessedVariableLocation. The @p accessedVariable should be a default constructed object if the I/O didn't access
     *  any part of any other known variable (the stack often has areas that are reserved by the compiler that don't correspond
     *  to any source level variables).
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, TestMode, IoMode, SgAsmInstruction*,
                        const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                        const Variables::StackVariable &intendedVariable, const AddressInterval &intendedVariableLocation,
                        const Variables::StackVariable &accessedVariable, const AddressInterval &accessedVariableLocation);

public:
    virtual std::string name() const override;
    virtual std::string printableName() const override;
    virtual void print(std::ostream&, const std::string &prefix) const override;
    virtual void toYaml(std::ostream&, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
