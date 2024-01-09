#ifndef ROSE_BinaryAnalysis_ModelChecker_UninitializedVariableTag_H
#define ROSE_BinaryAnalysis_ModelChecker_UninitializedVariableTag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Variables.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Tag that describes an out-of-bounds memory access. */
class UninitializedVariableTag: public Tag {
public:
    using Ptr = UninitializedVariableTagPtr;

private:
    const TestMode testMode_;                                   // may or must, but not off
    const SgAsmInstruction *insn_;                              // instruction where the oob access occurs (optional)
    const InstructionSemantics::BaseSemantics::SValuePtr addr_; // memory address that is accessed
    const FoundVariable variable_;                              // variable that was read before being written

protected:
    UninitializedVariableTag() = delete;
    UninitializedVariableTag(size_t nodeStep, TestMode, SgAsmInstruction*,
                             const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                             const FoundVariable &variable);

    UninitializedVariableTag(const UninitializedVariableTag&) = delete;

public:
    ~UninitializedVariableTag();

    /** Allocating constructor.
     *
     *  Constructs a tag with information about access to an uninitialized stack variable.  The @p variable is the variable
     *  that was accessed when uninitialized along with its @p variableLocation in memory.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, TestMode, SgAsmInstruction*,
                        const InstructionSemantics::BaseSemantics::SValuePtr &addr, const FoundVariable &variable);

public:
    virtual std::string name() const override;
    virtual std::string printableName() const override;
    virtual void print(std::ostream&, const std::string &prefix) const override;
    virtual void toYaml(std::ostream&, const std::string &prefix) const override;
    Sarif::ResultPtr toSarif() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
