#ifndef ROSE_BinaryAnalysis_ModelChecker_UninitVarTag_H
#define ROSE_BinaryAnalysis_ModelChecker_UninitVarTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes an out-of-bounds memory access. */
class UninitVarTag: public Tag {
public:
    using Ptr = UninitVarTagPtr;

private:
    const TestMode testMode_;                                    // may or must, but not off
    const SgAsmInstruction *insn_;                               // instruction where the oob access occurs (optional)
    const InstructionSemantics2::BaseSemantics::SValuePtr addr_; // memory address that is accessed
    const Variables::StackVariable variable_;                    // variable that was read before being written
    const AddressInterval variableLocation_;                     // location and size of stack variable in memory

protected:
    UninitVarTag() = delete;
    UninitVarTag(size_t nodeStep, TestMode, SgAsmInstruction*,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                 const Variables::StackVariable &variable, const AddressInterval &variableLocation);

    UninitVarTag(const UninitVarTag&) = delete;

public:
    ~UninitVarTag();

    /** Allocating constructor.
     *
     *  Constructs a tag with information about access to an uninitialized stack variable.  The @p variable is the variable
     *  that was accessed when uninitialized along with its @p variableLocation in memory.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, TestMode, SgAsmInstruction*,
                        const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                        const Variables::StackVariable &variable, const AddressInterval &variableLocation);

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
